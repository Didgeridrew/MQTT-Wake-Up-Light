/*  This is an MQTT/ESP32 implementation of my previous wakeUpLight projects. A slowly brightening 
 *  LED light to help people wake up easier in windowless bedrooms or other situations where 
 *  circadian lighting is desired. The intended use is to be controlled by a
 *  home automation server, allowing more flexibility in alarm programming and adaptability of use in
 *  lighting "scenes" as well as integration with other home automation controls such as Google Home 
 *  and Amazon Alexa.
 *  
 *  This Fail-Safe implementation includes the ability to program alarms. 
 *  
 *  Many thanks to all the folks out there who make videos, create libraries, and post tutorials to help us "noobs".
 *  
 *  Some of the sites that made this project possible include:
 *  
 *  Playful Technology  https://www.youtube.com/watch?v=VSwu-ZYiTxg&t=1680s
 *  Rui Santos  https://randomnerdtutorials.com
 *  Jan Delgado  https://github.com/jandelgado
*/

#include <ButtonKing.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <jled.h>   // This sketch requires a modified branch of jLed (https://github.com/Didgeridrew/jled/tree/long_period)


//Define IO for Buttons
#define BUTTON_PIN_1 4
#define BUTTON_PIN_2 15

//Instantiate ButtonKing entities
ButtonKing button1(BUTTON_PIN_1, true);
ButtonKing button2(BUTTON_PIN_2, true);

//Variables for LED programs
int manualSetting = 0;
const byte led_gpioR = 19;    // the PWM pin the LED is attached to
const byte led_gpioB = 18;    // the PWM pin the LED is attached to
int fadePeriod = 900000;      // *** fadePeriod over 65536 requires use of a modified version of jled ***
int candlePeriod = 1800000;   // *** candlePeriod over 65536 requires use of a modified version of jled ***

const char* fadeSet = "false";
const char* candleSet = "false";

const char* host = "esp32";
WebServer server(80);

//Instantiate JLed entities
auto esp32LedR = JLed(led_gpioR).Off().DelayBefore(100);
auto esp32LedB = JLed(led_gpioB).Off().DelayBefore(100);

bool runLedR;
bool runLedB;
bool fadingLedB;
bool autoFlag;

/**********************************************  Setup  ************************************************/
/*******************************************************************************************************/

void setup() {
  Serial.begin(115200);
  delay(1000);
 
  versPrint();
  
  /*** Setup the Input pins and button actions ***/
  pinMode(BUTTON_PIN_1, INPUT_PULLUP);
  pinMode(BUTTON_PIN_2, INPUT_PULLUP);
  
  button1.setClick(manualPos);
  button1.setDoubleClick(allOn);
  button1.setLongClickStart(fadeUpR);
  button2.setClick(manualNeg);
  button2.setDoubleClick(allOff);
  button2.setLongClickStart(candleOn);
  
  wifiSetup();
  mqttSetup();
  setupOTA();
}

/*********************************************  MAIN LOOP  *********************************************/
/*******************************************************************************************************/

void loop() {
  server.handleClient();
  mqttLoop();
  esp32LedR.Update();
  esp32LedB.Update();
  runLedR = esp32LedR.IsRunning();
  runLedB = esp32LedB.IsRunning();
  delay(5);
  button1.isClick();
  delay(5);
  button2.isClick();
  delay(5);
  if ((autoFlag == 1)&&(fadeSet == "true")&&(runLedR == 0)&&(runLedB == 0)&&(fadingLedB == 0)) {
    runLedB = 1;
    fadingLedB = 1;
    fadeUpB();
  }
  else if ((autoFlag == 1)&&(fadeSet == "true")&&(runLedR == 0)&&(runLedB == 0)&&(fadingLedB == 1)) {
    fadeComplete();
  }
  else if ((autoFlag == 1)&&(candleSet == "true")&&(runLedR == 0)&&(runLedB == 0)) {
      candleComplete();
  }
}


/***************************************  Manual LED Functions  ****************************************/
/*******************************************************************************************************/
void manualNeg() {
  manualSetting--;
//  Serial.println("Lower Manual Setting");
  manualSet();
  delay(5);
}

void manualPos() {
  manualSetting++;
//  Serial.println("Raise Manual Setting");
  manualSet();
  delay(5);
}

void manualSet() {
  manualSetting = constrain(manualSetting, 0, 5);
    switch (manualSetting) {
      case 0:
        esp32LedR.Set(0).MaxBrightness(254);
        esp32LedB.Set(0).MaxBrightness(254);
        break;
      case 1:
        esp32LedR.Set(50).MaxBrightness(254);
        esp32LedB.Set(0).MaxBrightness(254);
        break;
      case 2:
        esp32LedR.Set(100).MaxBrightness(254);
        esp32LedB.Set(0).MaxBrightness(254);
        break;
      case 3:
        esp32LedR.Set(250).MaxBrightness(254);
        esp32LedB.Set(0).MaxBrightness(254);
        break;
      case 4:
        esp32LedR.Set(250).MaxBrightness(254);
        esp32LedB.Set(100).MaxBrightness(254);
        break;
      case 5:
        esp32LedR.Set(254).MaxBrightness(254);
        esp32LedB.Set(254).MaxBrightness(254);
        break;
      default:
        manualSetting = 0;
        esp32LedR.Set(0).MaxBrightness(254);
        esp32LedB.Set(0).MaxBrightness(254);
        break;
    delay(5);
    }
  publishMan();
}

/**************************************** LED Automation Functions  ******************************************/
/*************************************************************************************************************/
/*** Fade LED ON ***/
void fadeUpR() {
  runLedR = 1;
  runLedB = 1;
  autoFlag = 1;
  fadeSet = "true";
  fadingLedB = 0;
  delay(5);
  Serial.println("FadeUpR Start");
  publishFade();
  esp32LedR.FadeOn(fadePeriod).MaxBrightness(254);
}

void fadeUpB() {
  runLedB = 1;
  delay(5);
  Serial.println("FadeUpB Start");
  esp32LedB.FadeOn(fadePeriod).MaxBrightness(254);
}

void fadeComplete() {
  Serial.println("Fade Complete");
  fadeSet = "false";
  autoFlag = 0;
  publishFade();
  allOn();
}

void candleOn() {
  runLedR = 1;
  runLedB = 1;
  autoFlag = 1;
  candleSet = "true";
  delay(5);
  Serial.println("Candle Start");
  publishCandle();
  esp32LedR.Candle(7, 15, candlePeriod).MaxBrightness(100);
  esp32LedB.Candle(7, 5, candlePeriod).MaxBrightness(50);
  delay(5);
}

void candleComplete() {
  Serial.println("Candle Complete");
  candleSet = "false";
  autoFlag = 0;
  publishCandle();
  allOff();
}

/*** Turn off the light ***/
void allOff() {
  manualSetting = 0;
  Serial.println("All Off");
  delay(5);
  manualSet();
}

/*** Turn On the light ***/
void allOn() {
  manualSetting = 5;
  Serial.println("All On");
  delay(5);
  manualSet();
}


/******************************************** Version Information  *******************************************/
/*************************************************************************************************************/
//Announce Version
void versPrint() {
  Serial.println();
  Serial.println("MQTTWakeLight v0.1.6");
  Serial.println();
}
/* Version notes:
 v0.1.0   ESP32 & MQTT implementation of ESPWakeLight v1.0.3
     .1   Moved all Wifi and MQTT functions to seperate tab
     .2   Added publishing of manual setting and constrained manualSetting var
     .3   Fail-Safe branch: Reintegrated RTC and alarm functions for fail-safe operation. Non-blocking delay() fixed issues with fade function failure.
     .4   Added fadeOn/Off publish to avoid feedback loop in HA/Node-RED
     .5   Added OTA in case I want to reprogram alarm times and added a manualset() to mqttLoop() to fix Home Assistant(HA) 
          frontend not refecting setting on reboot.
          ISSUES: Fadeup function blocks code, causes issues with MQTT and HA frontend
     .6   Convert all led functions to use modified, non-blocking, jled library.         
*/
