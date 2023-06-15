// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"

RTC_DS3231 rtc;
LiquidCrystal_I2C lcd(0X27,20,4);
// variables
//RTC
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
int rtcTime[3],text;
int rtcDate[3];
//function
//rtc 
void readRtc();// read & save in rtcTime[]
//
void lcdDisp();
void setup () {
  Serial.begin(57600);
lcd.init();lcd.clear();
#ifndef ESP8266
  while (!Serial); // wait for serial port to connect. Needed for native USB
#endif

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  lcd.setCursor(3,0);
  lcd.print(" Fervid Smart ");
  lcd.setCursor(6,1);
  lcd.print("Solutions");
  lcd.setCursor(4,2);
  lcd.print("Timer_testing");
  lcd.setCursor(0,3);
  lcd.print("With_Esp32_RTCDS3231");
  delay(2500);
  
}

void loop () {
  //Read from RTC and store in rtcData;rtcTime,rtcDay
  Serial.println("read from RTC");
  DateTime now = rtc.now();
 //store day in rtcDay
// rtcDay = daysOfTheWeek[now.dayOfTheWeek()];
 // Serial.println(rtcDay);
 //Storing date from RTC to rtcDate[]
 
rtcDate[0] = now.year();
/*rtcDate[1] = now.month();
rtcDate[2] = now.day();
rtcDate[3] =( rtcDate[2]*100000)+(rtcDate[1]*10000)+rtcDate[0];
Serial.println(rtcDate[3]);
// storing time from RTC to rtcTime[]
rtcTime[0] = now.second();
rtcTime[1] = now.minute();
rtcTime[2] = now.hour();
rtcTime[3] = (rtcTime[2]*10000)+(rtcTime[1]*100)+(rtcTime[0]);
 Serial.println(rtcTime[3]);
 //printing LCD
  delay(5000);*/
lcdDisp();
}

void readRtc()
{ 
  }
 
  void lcdDsip()
 {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(String(rtcDate[0]));lcd.print("/");//lcd.print(rtcDate[1]);lcd.print("/");lcd.print(rtcDate[0]);lcd.print(" ");
  //lcd.print(rtcTime[2]);lcd.print(":");lcd.print(rtcTime[1]);lcd.print(":");lcd.print(rtcTime[0]);lcd.print(" ");

  }
