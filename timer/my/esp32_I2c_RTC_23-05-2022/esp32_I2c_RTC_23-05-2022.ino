
#include "RTClib.h"
#include "EEPROM.h"
#include <LiquidCrystal_I2C.h>
#include "BluetoothSerial.h"
// set up 
RTC_DS3231 rtc;// set up RTC_DS3231 
LiquidCrystal_I2C lcd(0x27,16,2);// setLCD
//bL
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

#define EEPROM_SIZE 512

BluetoothSerial SerialBT;

//*******************pin declaration*******************
#define TIMER_SW 18
//*******************variables*******************
//*******************Gen*******************
unsigned int itTime = 0, itCount = 999;
//*******************rt*******************
char daysOfTheWeek[7][12] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
struct realTimeClock{
  unsigned int Temp, Hr, Min, Sec = 0, Date, Mon, Year;
  String Day;}rt;
//*******************BT*******************
//c015424052022tue/t009301130030/m0
struct Timer
{unsigned int onTimeMin,onTimeHr,offTimeMin,offTimeHr, dayOption, en; } timer[3];

//*******************function*******************
String op(int i);

//*****************rtc *****************
void read_Rtc();// read & save in rtcTime[]
//*****************bt*****************
void btRead();
//*******************LCD*******************
void lcdDisp();
void comp();
void setup() {
   Serial.begin(115200);
//*******************BT****************************
SerialBT.begin("24052022"); //Bluetooth device name
//*******************RTC****************************
  #ifndef ESP32
  while (!Serial); // wait for serial port to connect. Needed for native USB
#endif

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
  
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));    
 //*******************LCD**************************** 
    lcd.init();// initialize the lcd
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print(" Fervid Smart ");
  lcd.setCursor(0,1);
  lcd.print("Solutions");
  delay(1000);
  lcd.setCursor(0,0);
  lcd.print("Timer_testing");
  lcd.setCursor(0,1);
  lcd.print("With_Esp32_RTCDS3231");
  delay(2500);
  pinMode(TIMER_SW, OUTPUT);
  digitalWrite(TIMER_SW,HIGH);
  //*******************READ FROM EEPROM****************************
  if (!EEPROM.begin(EEPROM_SIZE))
  {
    Serial.println("failed to initialise EEPROM"); delay(1000000);
  }
  for(int numTimer = 0; numTimer <= 3; numTimer++  )
  { int add = numTimer*10;
    timer[numTimer].onTimeHr = EEPROM.read(add);
    timer[numTimer].onTimeMin = EEPROM.read(add+1);
    timer[numTimer].offTimeHr = EEPROM.read(add+2);
    timer[numTimer].offTimeMin = EEPROM.read(add+3);
    timer[numTimer].dayOption = EEPROM.read(add+4);
    timer[numTimer].en = EEPROM.read(add+5);
    Serial.println(String("tmer ")+String(numTimer));
    Serial.println(String("T_on ")+String(timer[numTimer].onTimeHr)+String(":")+String(timer[numTimer].onTimeMin));
    Serial.println(String("T_off ")+String(timer[numTimer].offTimeHr)+String(":")+String(timer[numTimer].offTimeMin));
    Serial.println(String("DayOp ")+String(timer[numTimer].dayOption));
    Serial.println(String("en ")+String(timer[numTimer].en));
    if(numTimer == 2){break;}
    }

//*******************function call's****************************
read_Rtc();
lcdDsip();
itCount = millis();
}  

void loop() {
//itCount = millis();Serial.println(millis());
//  Serial.println(String("itCount ")+String(itCount));
if((millis()-itCount) > 1000)
{ 
  read_Rtc();lcdDsip();
  }
if(SerialBT.available())
{btRead(); } 
  

  
}
void btRead() {
  Serial.println("In btRead");
  String serialData = " ";
serialData = " ";
while (SerialBT.available()) {
    serialData +=char( SerialBT.read());
  }
  delay(1000);
/*  Serial.println(serialData.length());
  Serial.print("Serial value = ");
  Serial.print(serialData[1]);*/
  if((serialData[1]=='c')&&(serialData.length()==19))
  {
    Serial.println(" in loop ");
    String value = String(serialData[2])+String(serialData[3]);rt.Hr = value.toInt();
    value = String(serialData[4])+String(serialData[5]);rt.Min = value.toInt();
    value = String(serialData[6])+String(serialData[7]);rt.Date = value.toInt();
    value = String(serialData[8])+String(serialData[9]);rt.Mon = value.toInt();
    value = String(serialData[10])+String(serialData[11])+String(serialData[12])+String(serialData[13]);
    rt.Year = value.toInt();
    value = String(serialData[14])+String(serialData[15])+String(serialData[16]);rt.Day = value;
    Serial.println(String(rt.Date)+String("/")+String(rt.Mon)+String("/")+String(rt.Year));
    Serial.println(String(rt.Hr)+String(":")+String(rt.Min));
    Serial.println(rt.Day);
    rtc.adjust(DateTime(rt.Year, rt.Mon, rt.Date, rt.Hr, rt.Min, 0));
  }
  else if((serialData[1]=='t')&&(serialData.length()==16))
  {
    int numTimer= String(serialData[2]).toInt();
    int add = numTimer*10;
    int value = (String(serialData[3])+String(serialData[4])).toInt();
    if(timer[numTimer].onTimeHr != value){
      timer[numTimer].onTimeHr=value;
    EEPROM.write(add, timer[numTimer].onTimeHr);EEPROM.commit();}   
    value = (String(serialData[5])+String(serialData[6])).toInt();
   if(timer[numTimer].onTimeMin != value){
    timer[numTimer].onTimeMin =value;
    EEPROM.write(add+1, timer[numTimer].onTimeMin);EEPROM.commit();
   }
    
    value = (String(serialData[7])+String(serialData[8])).toInt();
   if(timer[numTimer].offTimeHr != value){
    timer[numTimer].offTimeHr =value;
    EEPROM.write(add+2, timer[numTimer].offTimeHr);EEPROM.commit();
   }
    
    value = (String(serialData[9])+String(serialData[10])).toInt();
   if(timer[numTimer].offTimeMin != value){
    timer[numTimer].offTimeMin =value;
    EEPROM.write(add+3, timer[numTimer].offTimeMin);EEPROM.commit();
   }

   value = (String(serialData[11])+String(serialData[12])).toInt(); 
   if(timer[numTimer].dayOption != value){
    timer[numTimer].dayOption =value;
    EEPROM.write(add+4, timer[numTimer].dayOption);EEPROM.commit();
    if(timer[numTimer].dayOption){}
   }

   value = String(serialData[13]).toInt(); 
   if(timer[numTimer].en != value){
    timer[numTimer].en =value;
    EEPROM.write(add+5, timer[numTimer].en);EEPROM.commit();
   }
    
    Serial.println(String(" tmer ")+String(serialData[2]));
    Serial.println(String("T_on ")+String(timer[numTimer].onTimeHr)+String(":")+String(timer[numTimer].onTimeMin));
    Serial.println(String("T_off ")+String(timer[numTimer].offTimeHr)+String(":")+String(timer[numTimer].offTimeMin));
    Serial.println(String("DayOp ")+String(timer[numTimer].dayOption));
    Serial.println(String("en ")+String(timer[numTimer].en));
    }
    else if((serialData[1]=='m')&&(serialData.length()==5))
  {
    if(serialData[2]=='0')
    {digitalWrite(TIMER_SW,HIGH);Serial.println(" Realy is OFF ");
    }
    else if(serialData[2]=='1')
    {digitalWrite(TIMER_SW,LOW);Serial.println(" Realy is ON ");
    }
  }
  else 
  {
    Serial.println("Recived Data Is Not In Syntax");
    }
  Serial.println("end btRead");  
  }
  
void read_Rtc()
{ //Read from RTC and store in rtcData;rtcTime,rtcDay
  Serial.println("read from RTC");
  DateTime now = rtc.now();
 //store day in rtcDay
 rt.Day = daysOfTheWeek[now.dayOfTheWeek()];
  Serial.println(rt.Day);
 //Storing date from RTC to rtcDate[]
rt.Date = now.day();rt.Mon = now.month();rt.Year = now.year();
Serial.println(String(rt.Date)+String("/")+String(rt.Mon)+String("/")+String(rt.Year));
SerialBT.println("d"+String(rt.Date)+String("/")+String(rt.Mon)+String("/")+String(rt.Year));

// storing time from RTC to rtcTime[]
rt.Hr = now.hour();rt.Min = now.minute();rt.Sec = now.second();
Serial.println(String(rt.Hr)+String(":")+String(rt.Min));
SerialBT.println("t"+String(rt.Hr)+String(":")+String(rt.Min));
 // temp
 rt.Temp = rtc.getTemperature();
SerialBT.print(rt.Temp);
 Serial.println("END");
 delay(5000);
  }
 
void lcdDsip()
 {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(String(rt.Date)+String("/")+String(rt.Mon)+String("/")+String(rt.Year));
 lcd.print(" "+String(rt.Hr)+String(":")+String(rt.Min));
  lcd.setCursor(0,1);
  lcd.print(String(rt.Temp)+"C");
  }
/* void comp()
 {Serial.print("In Comp");
  for (int index = 0; index <= 3; index++ )
  {Serial.println("timer "+ String(index))
 String text = op(timer[index].dayOption);
 for()
    if(timer[intex].en == 1)
    {
      for(int i = 0; ;!="/0";)
      rt.Day == 
      if()
      }
    }
  }*/

  String op (int i)
  {
String list[15];
list[0] = {"SunMonTueWedThuFriSat"};
list[1] = {"MonTueWedThuFriSat"};
list[2] = {"MonTueWedThuFri"};
list[3] = {"SunMonWedThuFri"};
list[4] = {"SunTueThuSat"};
list[5] = {"SunMonWedFri"};
list[6] = {"MonTueWed"};
list[7] = {"ThuFriSat"};
list[8] = {"SunSat"};
list[9] = {"Sun"};
list[10] = {"Mon"};
list[11] = {"Tue"};
list[12] = {"Wed"};
list[13] = {"Thu"};
list[14] = {"Fri"};
list[15] = {"Sat"};
for(int i = 0; i<=15; i++)
{
  Serial.println("length of list_"+String(i)+" is "+list[i].length());
  }
return list[i];
    }
