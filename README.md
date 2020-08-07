# MQTT-Wake-Up-Light
An Arduino IDE sketch for MQTT-controlled LED fixture with stepped illumination levels and sunrise fade-up.

This is an update to my previous project, moving it from an Arduino nano to an ESP32 and setting it up for use 
in the Home Assistant Node-RED integration.
  
  Goals for this update:
 <br>   * Add flexibility in alarm programming
 <br>   * Make usable in lighting "scenes" 
 <br>   * Integrate with home automation controls such as Google Home and Amazon Alexa.


This project uses a modified branch of the jled library. In order to create a long fade-up sequence, you must use the modified
version.

<br>
<br>
<br>
<br>

# Options
If you find MQTT or your wLAN to be too unreliable for use as a morning alarm, please take a look at the [Fail-Safe Branch](https://github.com/Didgeridrew/MQTT-Wake-Up-Light/tree/fail-safe) for a version that can use MQTT, but does not rely on it.
