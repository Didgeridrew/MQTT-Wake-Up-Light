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

//WiFi user config. Change these to fit your home network.
const char* ssid = "";                               // Wifi SSID
const char* wifiPassword = "";                        // Wifi Password, if required.

//MQTT broker config
const IPAddress mqttServerIP(192, 168, 0, 99);        //Change to the IP address of the device running your broker
const char* mqttServerName = "";                    
const char* username = "";                            // If you have set your broker to require a user and password
const char* mqttPassword = "";                        // set them here. Otherwise, leave the "" empty.
unsigned int mqttPort = 1883;                         // Port 1883 is the default used by many MQTT apps, but you may need to change
                                                      // this if your network already uses this port for something else.
//MQTT client config
const char* deviceID = "wakeUpLight";                  // Unique device name for this client. For connection to broker
                                                       // and as a topic name for messages bound for this device. Example: "ESP32_humidity_sensor"
const IPAddress deviceIP(192, 168, 0, 131);            // Set a static IP address for the client device

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

  Serial.print("Message received in topic [");          // These print to the Arduino serial monitor to let you know if things are 
  Serial.print(topic);                                  // working properly.
  Serial.print("] ");
  Serial.print(msg);

  /*  In this section we accomplish three important things:
   *   - Define the messages we want our device to recognize 
   *   - Check any messages the device received against our definitions 
   *   - Tell which function from the main sketch to run if we've received a recognizable message
   *  
   *  We do that by comparing the string of a received message to a second string, if they are the same 
   *  (watch out for spelling and capitalization) the strcmp function will output
   *  a 0 and follow then the commands to jump over to the main sketch function, otherwise it will check the 
   *  message against the next "if" statment... 
   *  It is usually advisable to have the last statement (the "else") run the command to turn things off.
   */
   
  if (strcmp(msg, "on") == 0) {
    allOn();
    delay(30);
  }
  else if (strcmp(msg, "fade") == 0) {
    fadeUpR();
    delay(30);
  }
  else if (strcmp(msg, "down") == 0) {
    publish("Light lowered");
    delay(30);
    manualNeg();
    delay(30);
  }
  else if (strcmp(msg, "up") == 0) {
    publish("Light raised");
    delay(30);
    manualPos();
    delay(30);
  }
  else if (strcmp(msg, "off") == 0) {
    allOff();
    delay(30);
  }
}

/**********************************************  WiFiSetup  ************************************************/
/***********************************************************************************************************/

void wifiSetup() {
  if(!Serial) {                                           // Check if the serial connection is off. If it is,
    Serial.begin(115200);                                 // start it now.
  }
    
  delay(30);
  
  Serial.print("Connecting to ");                         //Attempt to connect to your WiFi network
  Serial.println(ssid);
  WiFi.begin(ssid, wifiPassword);

  while (WiFi.status() != WL_CONNECTED) {                 //Show a basic progress couter while waiting for connection
    delay(500);
    Serial.print(".");
  }

  delay(2000);                                            //Wait for the connection for complete
  
  Serial.println("Connected to IP address: ");
  Serial.println(WiFi.localIP());
}
  
/**********************************************  mqttSetup  ************************************************/
/***********************************************************************************************************/

/* Unless you changed the names of the global variables or constants you shouldn't need to change anything here
*/

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
  

  /* This section offers a lot of configurization flexibility. Which of these settings you need to use will depend
   * on how your broker is set up. I have added numerical annotations for each of the following configuration options.
   * In order for messages to flow properly both the broker and clients need to be looking at the same topics and subtopics. 
   * 
   * 
   * #1. Uncomment this line if you do not have your broker doesn't require user and password.
   * #2. Uncomment this line if your broker is set to only allow connection of specific users with a password
   * 
   * !!! You must uncomment one of the lines mentioned above !!!
   * 
   * #3. Your broker must be set up to listen to this topic. For ease of use, set your broker to subscribe to "ToBroker/#" 
   *     this will make it listen to all subtopics of ToBroker. If you don't like this topic name, you can change it to 
   *     whatever meets your needs. Just make sure the broker and client topics match.
   * #4. This is optional, but can be useful if you want to send the same message to all your devices.  
   *     For example, if you wanted to turn all your devices on or off at once or 
   *     you need to poll all of your sensors at the same time.
*/
  
// Attempt to connect to MQTT broker
  
//#1 if (MQTTclient.connect(deviceID)) {  

 if (MQTTclient.connect(mqttServerName, username, mqttPassword)) {

    Serial.print("MQTT client connected to broker");

    snprintf(topic, 32, "ToBroker/%s", deviceID);             // #3 This sets up a topic with the subtopic of the device ID for the client to publish to.
    snprintf(msg, 64, "CONNECT", deviceID);                   // Once connected, this publishes a message to the broker at the specified topic 
    MQTTclient.publish(topic, msg);                           // with the subtopic of the device ID (for example ToBroker/ESP32_humidity_sensor). 
                                                              
    snprintf(topic, 32, "ToDevice/%s", deviceID);             // This sets up a topic with the subtopic of the device ID for the client to subscribe to.
    MQTTclient.subscribe(topic);                              // This subscribes to the "ToDevice/" topic with the subtopic of the device ID.

    MQTTclient.subscribe("ToDevice/All");                     // #4 This subscribes to the "ToDevice/All" topic.
 
   }  
   else {
    Serial.print("Connection to broker failed, rc=");
    Serial.print(MQTTclient.state());
    Serial.println("try again in 5 seconds");
    
    delay(5000);                                              // Wait 5 seconds before retrying
    }
  }

  MQTTclient.loop();
}

/**************************************  Publish Helper function  ******************************************/
/***********************************************************************************************************/

void publish(char* message) {
  snprintf(topic, 32, "ToHost/%s", deviceID);
  MQTTclient.publish(topic, message);
}

/***********************************************************************************************************/
void publishMan() {
  snprintf(topic, 32, "ToBroker/%s/Manual", deviceID);         // This function sets up a sub-subtopic called "Manual" of the previous subtopic
  snprintf(msg, 16, "%i", manualSetting);                      // This sets the message to the value of the variable manualSetting 
  MQTTclient.publish(topic, msg);                              // Here we publish the message to the sub-subtopic
}
