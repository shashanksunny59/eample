#include "RTClib.h"
RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
struct realTimeClock{
  unsigned int Temp, Hr, Min, Date, Mon, Year;
  String Day;}rt;
void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
#ifndef ESP32
  while (!Serial); // wait for serial port to connect. Needed for native USB
#endif

if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
  
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); 
}

void loop() {
  //Read from RTC and store in rtcData;rtcTime,rtcDay
  Serial.println("read from RTC");
  DateTime now = rtc.now();
 //store day in rtcDay
 rt.Day = daysOfTheWeek[now.dayOfTheWeek()];
  Serial.println(rt.Day);
 //Storing date from RTC to rtcDate[]
rt.Date = now.day();rt.Mon = now.month();rt.Year = now.year();
Serial.println(String(rt.Date)+String("/")+String(rt.Mon)+String("/")+String(rt.Year));
// storing time from RTC to rtcTime[]
rt.Hr = now.hour();rt.Min = now.minute();
Serial.println(String(rt.Hr)+String(":")+String(rt.Min));
 // temp
 rt.Temp = rtc.getTemperature();Serial.println(rt.Temp);
 Serial.println("END");
 delay(5000);
}
