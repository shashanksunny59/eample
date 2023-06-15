/* Timestamp functions using a DS1307 RTC connected via I2C and Wire lib
**
** Useful for file name
**		` SD.open(time.timestamp()+".log", FILE_WRITE) `
**
**
** Created: 2015-06-01 by AxelTB
** Last Edit:
*/

#include "RTClib.h"

RTC_DS3231 rtc;

void setup () {
  Serial.begin(9600);



  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }


  
   rtc.adjust(DateTime(2022, 06, 07, 11, 24, 0));
}

void loop() {
 DateTime time = rtc.now();

 //Full Timestamp
 Serial.println(String("DateTime::TIMESTAMP_FULL:\t")+time.timestamp(DateTime::TIMESTAMP_FULL));

 //Date Only
 Serial.println(String("DateTime::TIMESTAMP_DATE:\t")+time.timestamp(DateTime::TIMESTAMP_DATE));

 //Full Timestamp
 Serial.println(String("DateTime::TIMESTAMP_TIME:\t")+time.timestamp(DateTime::TIMESTAMP_TIME));

 Serial.println("\n");

 //Delay 5s
 delay(5000);
}
