/* ESP32 WiFi MQTT
 * 
 * Functions for connecting, subscribing, and publishing to an MQTT broker.
 * For use with ESP32-based microcontrollers.
 * Made to use with Home Assistant and Mosquitto, but usable with a stand-alone MQTT systems.
 * 
 * This sketch is meant to be used along with a main sketch that contains the non-WiFi and non-MQTT functions. This is done
 * to make it easier to "drop in" Wifi and MQTT functionality in a modular way.
 */

#include <WiFi.h>
#include <PubSubClient.h>

//Constants

//WiFi user config. Change these to fit your home network. (I moved this to a secrets file)
//const char* ssid = "";                              // Wifi SSID
//const char* wifiPassword = "";                      // Wifi Password, if required.

//MQTT broker config
const IPAddress mqttServerIP(10, 0, 0, 99);           //IP address of the device running your broker
//const char* mqttServerName = "";                    // (I moved all this to a secrets file)
//const char* username = "";                          // If you have set your broker to require a user and password
//const char* mqttPassword = "";                      // set them here. Otherwise, leave the "" empty.
unsigned int mqttPort = 1883;                         // Port 1883 is the default used by many MQTT apps, but you may need to change
                                                      // this if your network already uses this port for something else.
//MQTT client config
const char* deviceID = "wakeUpLight";                  // Unique device name for this client. For connection to broker 
                                                       // and as a topic name for messages bound for this device. Example: "ESP32_humidity_sensor"
const IPAddress deviceIP(10, 0, 0, 131);               // Set a static IP address for the client device

// Global Variables

WiFiClient wifiClient;                                // Instantiate WiFi Client
PubSubClient MQTTclient(wifiClient);                  // Instantiate MQTT Client based on Wifi client

long lastMsgTime = 0;                                 // Track the time from millis() since the last message was published.
char msg[64];                                         // A memory buffer to hold messages that have been sent/ are to be sent.
char topic[32];                                       // The topic in which to publish a message.
int pulseCounter = 0;                                 // Counter for heartbeat pulse sent. This is helpful for debugging disconnection.


/*******************************************  MQTT Callback  *******************************************/
/*******************************************************************************************************/

//This function is called whenever a message is published in any topic that this device is subscribed to.
//The meassage "payload" passed to this function is a byte*

void mqttCallback(char* topic, byte* payload, unsigned int length) {

  memcpy(msg, payload, length);                        // Copy the message contents to the msg char[] array
  msg[length] = '\0';                                  // This adds a NULL terminator to the message for correct string formatting

  Serial.println();
  Serial.println("Message received in topic [");          // These print to the Arduino serial monitor to let you know if things are 
  Serial.print(topic);                                  // working properly.
  Serial.print("] ");
  Serial.print(msg);
  Serial.println();
   
  if (strcmp(msg, "on") == 0) {
    allOn();
    Alarm.delay(10);
    publish("All On");
  }
  else if (strcmp(msg, "fade") == 0) {
    fadeUpR();
    Alarm.delay(10);
    publish("Start Fade");
  }
  else if (strcmp(msg, "candle") == 0) {
    candleOn();
    Alarm.delay(10);
    publish("Start Candle");
  }
  else if (strcmp(msg, "down") == 0) {
    manualNeg();
    Alarm.delay(10);
    publish("Light lowered");
  }
  else if (strcmp(msg, "up") == 0) {
    manualPos();
    Alarm.delay(10);
    publish("Light raised");
  }
  else if (strcmp(msg, "off") == 0) {
    allOff();
    Alarm.delay(10);
    publish("All Off");
  }
  else if (strcmp(msg, "0") == 0) {
    manualSetting = 0;
    Alarm.delay(10);
    manualSet();
  }
  else if (strcmp(msg, "1") == 0) {
    manualSetting = 1;
    Alarm.delay(10);
    manualSet();
  }
  else if (strcmp(msg, "2") == 0) {
    manualSetting = 2;
    Alarm.delay(10);
    manualSet();
  }  
  else if (strcmp(msg, "3") == 0) {
    manualSetting = 3;
    Alarm.delay(10);
    manualSet();
  }      
  else if (strcmp(msg, "4") == 0) {
    manualSetting = 4;
    Alarm.delay(10);
    manualSet();
  }
  else if (strcmp(msg, "5") == 0) {
    manualSetting = 5;
    Alarm.delay(10);
    manualSet();
  }
}

/**********************************************  WiFiSetup  ************************************************/
/***********************************************************************************************************/

void wifiSetup() {
  if(!Serial) {                                           // Check if the serial connection is off. If it is,
    Serial.begin(115200);                                 // start it now.
  }
    
  Alarm.delay(30);
  
  Serial.print("Connecting to ");                         //Attempt to connect to your WiFi network
  Serial.println(ssid);
  WiFi.begin(ssid, wifiPassword);

  while (WiFi.status() != WL_CONNECTED) {                 //Show a basic progress couter while waiting for connection
    Alarm.delay(500);
    Serial.print(".");
  }

  Alarm.delay(2000);                                            //Wait for the connection for complete
  
  Serial.println("Connected to IP address: ");
  Serial.println(WiFi.localIP());
}
  
/**********************************************  mqttSetup  ************************************************/
/***********************************************************************************************************/

void mqttSetup() {
  // Here we define some important settings for the MQTT client
  MQTTclient.setServer(mqttServerIP, mqttPort);
  MQTTclient.setCallback(mqttCallback);
}

/***********************************************  mqttLoop  ************************************************/
/***********************************************************************************************************/

void mqttLoop() {
  while (!MQTTclient.connected()) {                         // Check whether or not the MQTT server is connected
  Serial.print("Attempting MQTT connection to broker at ");
  Serial.println(mqttServerIP);
  
// Attempt to connect to MQTT broker

 if (MQTTclient.connect(mqttServerName, username, mqttPassword)) {

    Serial.print("MQTT client connected to broker");

    snprintf(topic, 32, "ToBroker/wakeUpLight");             // #3 This sets up a topic with the subtopic of the device ID for the client to publish to.
    snprintf(msg, 64, "wakeUpLight CONNECTED to Broker");    // Once connected, this publishes a message to the broker at the specified topic 
    MQTTclient.publish(topic, msg);                          // with the subtopic of the device ID (for example ToBroker/ESP32_humidity_sensor). 

    MQTTclient.subscribe("ToDevice/wakeUpLight");
    
    MQTTclient.subscribe("ToDevice/wakeUpLight/Manual");     // #4 This subscribes to the a specific topic for manual control topic.
 
   }  
   else {
    Serial.print("Connection to broker failed, rc=");
    Serial.print(MQTTclient.state());
    Serial.println("try again in 5 seconds");
    
    Alarm.delay(5000);                                              // Wait 5 seconds before retrying
    }
  }

  MQTTclient.loop();
}

/**************************************  Publish Helper functions  ******************************************/
/***********************************************************************************************************/

void publish(char* message) {
  snprintf(topic, 32, "ToBroker/%s", deviceID);
  MQTTclient.publish(topic, message);
}

void publishMan() {
  snprintf(topic, 32, "ToBroker/%s/Manual", deviceID);
  snprintf(msg, 64, "%i", manualSetting);
  MQTTclient.publish(topic, msg);
}

void publishFade() {
  snprintf(topic, 32, "ToBroker/%s/fadeSet", deviceID);
  snprintf(msg, 64, fadeSet);
  MQTTclient.publish(topic, msg);
}

void publishCandle() {
  snprintf(topic, 32, "ToBroker/%s/candleSet", deviceID);
  snprintf(msg, 64, candleSet);
  MQTTclient.publish(topic, msg);
}
