
RTC_DS3231 rtc;

time_t syncProvider(){
  return rtc.now().unixtime();
}

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec =  -18000;
const int   daylightOffset_sec = 3600;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-SET ALARM TIMES HERE-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
int onHour = 5;
int onMinute = 45;
int offHour = 10;
int offMinute = 0;
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/


void timeSetup() {
  Wire.begin();
  rtc.begin();
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
    }
     
  setSyncProvider(syncProvider);
  if(timeStatus() != timeSet)
      Serial.println("Unable to sync with the RTC");
  else
      Serial.println("RTC has set the system time");     
    Serial.println();
}

void instantNTP() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
}

void instantAlarms() {
  Alarm.alarmRepeat((onHour),onMinute,0, fadeUpR);              /*** Turn on the light ***/
  Alarm.alarmRepeat((offHour),offMinute,0, allOff);             /*** Turn off the light ***/
}


/*** Setup bi-hourly call for NTP/RTC print and adjust ***/
void hourlyTimer(){
  Alarm.timerRepeat( 2,0,0, instantNTP);
  Serial.println("NTP will Update RTC every 2 hours at this minute");  
}

/*** Get time from NTP and adjust RTC to match ***/
void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  rtc.adjust(DateTime(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec));
  Serial.print("NTP Time: ");
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  rtcTimeRead();
}

/*** Print RTC time ***/
void rtcTimeRead(){
  DateTime now = rtc.now();
  Serial.print("RTC Time: ");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(", ");
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print('/');
  Serial.print(now.year(), DEC);
  Serial.print(" ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
}
