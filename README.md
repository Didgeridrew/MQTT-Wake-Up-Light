# MQTT-Wake-Up-Light Fail-Safe
An Arduino IDE sketch for MQTT-controlled LED fixture with stepped illumination levels and sunrise fade-up.

This is an update to my previous project, moving it from an Arduino nano to an ESP32 and setting it up for use 
in the Home Assistant Node-RED integration.

My main goal for this update was to allow more flexibility in alarm programming and adaptability of use in lighting
"scenes" as well as integration with other home automation controls such as Google Home and Amazon Alexa.

My secondary goal was to create a MQTT WiFi sketch that I (or others) could drop into future projects. 
The included WiFiMQTT.ino file does just that. It is highly commented in a way that I hope makes it useful as an 
explanatory tool for other MQTT noobs like me.


# FAIL-SAFE BRANCH
This branch is meant to cover an edge-case issue where MQTT is not a reliable means of control for someone who relies on this as an
important component of their morning alarm. It also solves some issues I had in my orignal Arduino Wake-Up-Light project.
Incorporating a real time clock module (RTC), network time protocol (NTP), the TimeAlarms library, 
and the OTA library add the following benefits:

  1. The light alarm function will continue to operate as long as it has power, even if the MQTT server or wLAN goes down.
  2. NTP will get an accurate time whenever the wLAN is up and RTC will maintain correct time if the wLAN is down.
  3. DST adjustments are handled automatically by NTP.
  4. OTA allows the user to reprogram the alarm times without removing the microcontroller from the fixture.
  
