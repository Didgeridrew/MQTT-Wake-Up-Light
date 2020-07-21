# MQTT-Wake-Up-Light
An Arduino IDE sketch for MQTT-controlled LED fixture with stepped illumination levels and sunrise fade-up.

This is an update to my previous project, moving it from an Arduino nano to an ESP32 and setting it up for use 
in the Home Assistant Node-RED integration.

My main goal for this update was to allow more flexibility in alarm programming and adaptability of use in lighting
"scenes" as well as integration with other home automation controls such as Google Home and Amazon Alexa.

My secondary goal was to create a MQTT WiFi sketch that I (or others) could drop into future projects. 
The included WiFiMQTT.ino file does just that.  It is highly commented in a way that I hope makes it useful as an explanatory tool for other MQTT noobs like me.
