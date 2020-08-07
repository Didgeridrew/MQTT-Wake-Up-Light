# MQTT-Wake-Up-Light
An Arduino IDE sketch for MQTT-controlled LED fixture with stepped illumination levels and sunrise fade-up.

This is an update to my previous project, moving it from an Arduino nano to an ESP32 and setting it up for use 
in the Home Assistant Node-RED integration.
  
  Goals for this update:
 <br>   * Add flexibility in alarm programming
 <br>   * Make usable in lighting "scenes" 
 <br>   * Integrate with home automation controls such as Google Home and Amazon Alexa.


This project uses a [modified branch](https://github.com/Didgeridrew/jled/tree/long_period) of the jled library. In order to create a long fade-up sequence, you must use the modified
version.

<br>
<br>
<br>
<br>

# Options
If you find MQTT or your wLAN to be too unreliable for use as a morning alarm, please take a look at the [Fail-Safe Branch](https://github.com/Didgeridrew/MQTT-Wake-Up-Light/tree/fail-safe) for a version that can use MQTT, but does not rely on it.
# FAIL-SAFE BRANCH
This branch is meant to cover an edge-case issue where MQTT is not a reliable means of control for someone who relies on this as an
important component of their morning alarm. It also solves some issues I had in my orignal Arduino Wake-Up-Light project.
Incorporating a real time clock module (RTC), network time protocol (NTP), the TimeAlarms library, 
and the OTA library add the following benefits:

  1. The light alarm function will continue to operate as long as it has power, even if the MQTT server or wLAN goes down.
  2. NTP will get an accurate time whenever the wLAN is up and RTC will maintain correct time if the wLAN is down.
  3. DST adjustments are handled automatically by NTP.
  4. OTA allows the user to reprogram the alarm times without removing the microcontroller from the fixture.
  
