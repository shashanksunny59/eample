
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>

// set up 
RTC_DS3231 rtc;// set up RTC_DS3231 
LiquidCrystal_I2C lcd(0x27,20,4);// setLCD

// variables
//RTC
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
int rtcTemp, rtcTime[3], rtcDate[3]; String rtcDay =" ";
//function
//rtc 
void read_Rtc();// read & save in rtcTime[]
//
void lcdDisp();
void setup() {
   Serial.begin(57600);
   
   // for RTC
   #ifndef ESP8266
  while (!Serial); // wait for serial port to connect. Needed for native USB
#endif

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
  
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));    
 // rtc
    //LCD 
    lcd.init();// initialize the lcd
  lcd.backlight();
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

void loop() {
read_Rtc();  
  delay(5000);
lcdDsip();
}
void read_Rtc()
{ //Read from RTC and store in rtcData;rtcTime,rtcDay
  Serial.println("read from RTC");
  DateTime now = rtc.now();
 //store day in rtcDay
 rtcDay = daysOfTheWeek[now.dayOfTheWeek()];
  Serial.println(rtcDay);
 //Storing date from RTC to rtcDate[]
rtcDate[2] = now.day();rtcDate[1] = now.month();rtcDate[0] = now.year();
rtcDate[3] = rtcDate[2]*100000+rtcDate[1]*10000+rtcDate[0];
Serial.println(rtcDate[3]);
// storing time from RTC to rtcTime[]
rtcTime[2] = now.hour();rtcTime[1] = now.minute();rtcTime[0] = now.second();
rtcTime[3] = (rtcTime[2]*10000)+(rtcTime[1]*100)+(rtcTime[0]);
 Serial.println(rtcTime[3]);
 // temp
 rtcTemp = rtc.getTemperature();Serial.println(rtcTemp);
 Serial.println("END");
  }
 
  void lcdDsip()
 {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(String(rtcDate[2])+"/"+String(rtcDate[1])+"/"+String(rtcDate[0]));
 lcd.print(" "+String(rtcTime[2])+":"+String(rtcTime[1])+":"+String(rtcTime[0]));
  lcd.setCursor(0,1);
  lcd.print(String(rtcTemp)+"C");
  }
