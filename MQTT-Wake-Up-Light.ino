/*  This is an MQTT/ESP32 implementation of my previous wakeUpLight projects. A slowly brightening 
 *  LED light to help people wake up easier in windowless bedrooms or other situations where 
 *  circadian lighting is desired. The intended use is for it to be controlled by a Home Assistant 
 *  home automation server, allowing more flexibility in alarm programming and adaptability of use in
 *  lighting "scenes" as well as integration with other home automation controls sucj as Google Home 
 *  and Amazon Alexa.
 *  This implementation also lowers the component count by removing the need for an onboard RTC. 
 *  Many thanks to all the folks out there who make videos, create libraries, and post tutorials to help us "noobs".
 *  
 *  Some of the sites that made this project possible include:
 *  
 *  Playful Technology  https://www.youtube.com/watch?v=VSwu-ZYiTxg&t=1680s
 *  Rui Santos  https://randomnerdtutorials.com
*/

#include <ButtonKing.h>

//Define IO for Buttons
#define BUTTON_PIN_1 4
#define BUTTON_PIN_2 15

//Define IO for LEDs/MOSFETS
#define pulsePinB 18
#define pulsePinR 19

//Instantiate ButtonKing entities
ButtonKing button1(BUTTON_PIN_1, true);
ButtonKing button2(BUTTON_PIN_2, true);

//Variables for LED programs
int manualSetting = 0;
const byte led_gpioR = 19; // the PWM pin the LED is attached to
int brightnessR = 0;    // how bright the LED is. This sets its initial value to 0
int fadeAmount = 1;    // how many points to fade the LED by
const byte led_gpioB = 18; // the PWM pin the LED is attached to
int brightnessB = 0;    // how bright the LED is. This sets its initial value to 0
int ledDelay = 3800;    // this delay controls the time between steps as the light increases


/**********************************************  Setup  ************************************************/
/*******************************************************************************************************/

void setup() {
  ledcAttachPin(led_gpioR, 0);  // assign a led pins to a channel
  ledcSetup(0, 40000, 8);       // 40 kHz PWM, 8-bit resolution
  ledcWrite(0, 0);              // Set initial LED state to "Off"
  
  ledcAttachPin(led_gpioB, 1);  // assign a led pins to a channel
  ledcSetup(1, 40000, 8);       // 40 kHz PWM, 8-bit resolution
  ledcWrite(1, 0);              // Set initial LED state to "Off"
  
  Serial.begin(115200);
  delay(2000);
 
  versPrint();

  wifiSetup();
  mqttSetup();
}

/*********************************************  MAIN LOOP  *********************************************/
/*******************************************************************************************************/

void loop() {
  button1.isClick();
  delay(10);
  button2.isClick();
  delay(10);
  mqttLoop();
  delay(200);
}


/***************************************  Manual LED Functions  ****************************************/
/*******************************************************************************************************/
void manualNeg() {
  manualSetting--;
  delay(10);
  manualSet();
  delay(10);
}

void manualPos() {
  manualSetting++;
  delay(10);
  manualSet();
  delay(10);
}

void manualSet() {
    switch (manualSetting) {
      case 0:
        ledcWrite(0, 0);
        ledcWrite(1, 0);
        break;
      case 1:
        ledcWrite(0, 50);
        ledcWrite(1, 0);
        break;
      case 2:
        ledcWrite(0, 100);
        ledcWrite(1, 0);
        break;
      case 3:
        ledcWrite(0, 250);
        ledcWrite(1, 0);
        break;
      case 4:
        ledcWrite(0, 250);
        ledcWrite(1, 100);
        break;
      case 5:
        ledcWrite(0, 250);
        ledcWrite(1, 250);
        break;
      default:    
        ledcWrite(0, 0);
        ledcWrite(1, 0);
        manualSetting = 0;
    }
    
}

/**************************************** LED Automation Functions  ******************************************/
/*************************************************************************************************************/
/*** Fade LED ON ***/
void fadeUpR(){
  Serial.println("Fading LEDs Up");
  while (brightnessR < 254) {
    ledcWrite(0, brightnessR);
    brightnessR = brightnessR + fadeAmount;
    delay(ledDelay);
    }
  if (brightnessR >= 254) {
    ledcWrite(0, 255);
    brightnessR = 255;
  }
manualSetting = 3;
manualSet();
fadeUpB();
}

void fadeUpB(){
  while (brightnessB < 254){
    ledcWrite(0, 255);   
    ledcWrite(1, brightnessB);
    brightnessB = brightnessB + fadeAmount;
    delay(ledDelay);
    }
  if (brightnessB >= 254){
    ledcWrite(0, 255);
    brightnessB = 255;
  }    
manualSetting = 5;
manualSet();
}

/*** Turn off the light ***/
void allOff(){
  manualSetting = 0;
  delay(10);
  manualSet();
}

/*** Turn On the light ***/
void allOn(){
  manualSetting = 5;
  delay(10);
  manualSet();
}


/******************************************** Version Information  *******************************************/
/*************************************************************************************************************/
//Announce Version
void versPrint(){
  Serial.println();
  Serial.println("MQTTWakeLight v0.1.1");
  Serial.println();
}
/* Version notes:
 v0.1.0   ESP32 & MQTT implementation of ESPWakeLight v1.0.3
     .1   Moved all Wifi and MQTT functions to seperate tab
          Moved secrets to seperate tab
            
            
            
*/
