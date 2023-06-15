
#include "RTClib.h"
#include "EEPROM.h"
#include <LiquidCrystal_I2C.h>
#include "BluetoothSerial.h"
// set up
RTC_DS3231 rtc;// set up RTC_DS3231
LiquidCrystal_I2C lcd(0x27, 20, 4); // setLCD
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
unsigned long present; int ir = 0;char SW, manualStatus ='0';
//*******************rt*******************
char daysOfTheWeek[7][12] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
struct realTimeClock {
  unsigned int Temp, Hr, Min, Sec = 0, Date, Mon, Year;
  String Day;
} rt;
//*******************BT*******************
//c015424052022tue/t009301130030/m0
struct Timer
{ unsigned int onTimeMin, onTimeHr, offTimeMin, offTimeHr, dayOption, en;
  char st = 'f';
} timer[3];

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
  Serial.println("**********In Setup**********");
  //*******************BT****************************
  SerialBT.begin("24052022"); //Bluetooth device name
  //*******************RTC****************************
#ifndef ESP32
  while (!Serial); // wait for serial port to connect. Needed for native USB
#endif

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");SerialBT.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  //*******************LCD****************************
  lcd.init();// initialize the lcd
  lcd.backlight();
  lcd.setCursor(3, 0);
  lcd.print(" Fervid Smart ");
  lcd.setCursor(6, 1);
  lcd.print("Solutions");
  lcd.setCursor(4, 2);
  lcd.print("Timer_V.0.1");
  lcd.setCursor(0, 3);
  lcd.print("With_Esp32_RTCDS3231");
  //resetting app
  SerialBT.println("reset");
  delay(2500);
  //*******************PIN****************************
  pinMode(TIMER_SW, OUTPUT);
  digitalWrite(TIMER_SW, HIGH);SW='H';
  //*******************READ FROM EEPROM****************************
  if (!EEPROM.begin(EEPROM_SIZE))
  {
    Serial.println("failed to initialise EEPROM"); delay(1000000);
  }
  for (int numTimer = 0; numTimer <= 3; numTimer++  )
  { int add = numTimer * 10;
    timer[numTimer].onTimeHr = EEPROM.read(add);
    timer[numTimer].onTimeMin = EEPROM.read(add + 1);
    timer[numTimer].offTimeHr = EEPROM.read(add + 2);
    timer[numTimer].offTimeMin = EEPROM.read(add + 3);
    timer[numTimer].dayOption = EEPROM.read(add + 4);
    timer[numTimer].en = EEPROM.read(add + 5);
    Serial.println(String("tmer ") + String(numTimer));
    Serial.println(String("T_on ") + String(timer[numTimer].onTimeHr) + String(":") + String(timer[numTimer].onTimeMin));
    Serial.println(String("T_off ") + String(timer[numTimer].offTimeHr) + String(":") + String(timer[numTimer].offTimeMin));
    Serial.println(String("DayOp ") + String(timer[numTimer].dayOption));
    Serial.println(String("en ") + String(timer[numTimer].en));
    if (numTimer == 2) {
      break;
    }
  }

  //*******************function call's****************************
  read_Rtc();
  lcdDsip();
Serial.println("**********END SETUP**********");
}

void loop() {

  if((millis()-present)>1000){   Serial.println(String(present)+"___"+String(ir));
  //sending relay status

    present = millis();
    ir++;
    
    }
    if(ir>10)
    {ir=0;  read_Rtc(); comp();lcdDsip();}

  if (SerialBT.available())
  {
    btRead();lcdDsip();
  }


}
void btRead() {
  Serial.println("***********In btRead***********");
  String serialData = " ";
  serialData = " ";
  while (SerialBT.available()) {
    serialData += char( SerialBT.read());
  }                           
  delay(250);
   Serial.println(serialData);
   /* Serial.print("Serial value = ");*/
    Serial.print(serialData.length());
  if ((serialData[1] == 'c') && (serialData.length() == 17))
  {
    Serial.println(" in loop ");
    String value = String(serialData[2]) + String(serialData[3]); rt.Hr = value.toInt();
    value = String(serialData[4]) + String(serialData[5]); rt.Min = value.toInt();
    value = String(serialData[6]) + String(serialData[7]); rt.Date = value.toInt();
    value = String(serialData[8]) + String(serialData[9]); rt.Mon = value.toInt();
    value = String(serialData[10]) + String(serialData[11]) + String(serialData[12]) + String(serialData[13]);
    rt.Year = value.toInt();
    value = String(serialData[14]) + String(serialData[15]) + String(serialData[16]); rt.Day = value;
    Serial.println(String(rt.Date) + String("/") + String(rt.Mon) + String("/") + String(rt.Year));
    Serial.println(String(rt.Hr) + String(":") + String(rt.Min));
    Serial.println(rt.Day);
    SerialBT.println("RECIVED CURRENT TIME");
    rtc.adjust(DateTime(rt.Year, rt.Mon, rt.Date, rt.Hr, rt.Min, 0));
  }
  else if ((serialData[1] == 't') && (serialData.length() == 14))
  {
    int numTimer = String(serialData[2]).toInt();
    int add = numTimer * 10;
    int value = (String(serialData[3]) + String(serialData[4])).toInt();
    if (timer[numTimer].onTimeHr != value) {
      timer[numTimer].onTimeHr = value;
      EEPROM.write(add, timer[numTimer].onTimeHr); EEPROM.commit();
    }
    value = (String(serialData[5]) + String(serialData[6])).toInt();
    if (timer[numTimer].onTimeMin != value) {
      timer[numTimer].onTimeMin = value;
      EEPROM.write(add + 1, timer[numTimer].onTimeMin); EEPROM.commit();
    }

    value = (String(serialData[7]) + String(serialData[8])).toInt();
    if (timer[numTimer].offTimeHr != value) {
      timer[numTimer].offTimeHr = value;
      EEPROM.write(add + 2, timer[numTimer].offTimeHr); EEPROM.commit();
    }

    value = (String(serialData[9]) + String(serialData[10])).toInt();
    if (timer[numTimer].offTimeMin != value) {
      timer[numTimer].offTimeMin = value;
      EEPROM.write(add + 3, timer[numTimer].offTimeMin); EEPROM.commit();
    }

    value = (String(serialData[11]) + String(serialData[12])).toInt();
    if (timer[numTimer].dayOption != value) {
      timer[numTimer].dayOption = value;
      EEPROM.write(add + 4, timer[numTimer].dayOption); EEPROM.commit();
      if (timer[numTimer].dayOption) {}
    }

    value = String(serialData[13]).toInt();
    if (timer[numTimer].en != value) {
      timer[numTimer].en = value;
      EEPROM.write(add + 5, timer[numTimer].en); EEPROM.commit();
    }

    Serial.println(String(" tmer ") + String(serialData[2]));
    Serial.println(String("T_on ") + String(timer[numTimer].onTimeHr) + String(":") + String(timer[numTimer].onTimeMin));
    Serial.println(String("T_off ") + String(timer[numTimer].offTimeHr) + String(":") + String(timer[numTimer].offTimeMin));
    Serial.println(String("DayOp ") + String(timer[numTimer].dayOption));
    Serial.println(String("en ") + String(timer[numTimer].en));
//    SerialBT.print(String(" tmer ") + String(serialData[2]));
//    SerialBT.print(String("T_on ") + String(timer[numTimer].onTimeHr) + String(":") + String(timer[numTimer].onTimeMin));
//    SerialBT.print(String("T_off ") + String(timer[numTimer].offTimeHr) + String(":") + String(timer[numTimer].offTimeMin));
//    SerialBT.print(String("DayOp ") + String(timer[numTimer].dayOption));
//    SerialBT.println(String("en ") + String(timer[numTimer].en));
SerialBT.println("Recived TIMER VALUE");
  }
  
  else if ((serialData[1] == 'm') && (serialData.length() == 3))
  {
    if (serialData[2] == '0')
    { digitalWrite(TIMER_SW, HIGH);SW='H'; Serial.println(" Realy is OFF ");SerialBT.println("m-0");manualStatus='0';
    }
    else if (serialData[2] == '1')
    { digitalWrite(TIMER_SW, LOW);SW='L'; Serial.println(" Realy is ON ");SerialBT.println("m-1");manualStatus='1';
    }
    
  }
  else
  {
   SerialBT.println(" Not In Syntax");
    Serial.println("Recived Data Is Not In Syntax");
  }
  
  Serial.println("***********END btRead***********");
}

void read_Rtc()
{ //Read from RTC and store in rtcData;rtcTime,rtcDay
  Serial.println("***********IN read_RTC***********");
  DateTime now = rtc.now();
  //store day in rtcDay
  rt.Day = daysOfTheWeek[now.dayOfTheWeek()];
  Serial.println(rt.Day);
  //Storing date from RTC to rtcDate[]
  rt.Date = now.day(); rt.Mon = now.month(); rt.Year = now.year();
  Serial.println(String(rt.Date) + String(",") + String(rt.Mon) + String(",") + String(rt.Year));
  delay(1500);

  // storing time from RTC to rtcTime[]
  rt.Hr = now.hour(); rt.Min = now.minute(); rt.Sec = now.second();
  Serial.println(String(rt.Hr) + String(",") + String(rt.Min));

// temp
  rt.Temp = rtc.getTemperature();
  Serial.println(String(rt.Temp)+"°C");

SerialBT.println( String(rt.Date) + String(",") + String(rt.Mon) + String(",") + String(rt.Year)+","+String(rt.Hr) + String(",") + String(rt.Min)+","+String(rt.Temp)+","+String(manualStatus)+","+String(timer[0].st)+","+String(timer[1].st));

delay(1000);
  Serial.println("***********END read_RTC***********");
}

void lcdDsip()
{Serial.println("***********IN lcdDsip***********");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print( String(rt.Hr) + String(":") + String(rt.Min));
  lcd.print(" " +String(rt.Date) + String("/") + String(rt.Mon) + String("/") + String(rt.Year));
  lcd.print(" " + String(rt.Day));
  lcd.setCursor(0, 1);
  lcd.print(String(rt.Temp) + "C");
  for(int i =0;i<2;i++)
  {lcd.setCursor(0, i+2);
  lcd.print("T"+String(i)+" "+String(timer[i].onTimeHr)+":"+String(timer[i].onTimeMin));
  lcd.print(" "+String(timer[i].offTimeHr)+":"+String(timer[i].offTimeMin));
  lcd.print(" "+String(timer[i].dayOption));
  lcd.print("e"+String(timer[i].en));
  lcd.print(String(timer[i].st));
  }
  Serial.println("***********END lcdDsip***********");
}
void comp()
{ Serial.println("***********IN Comp***********");
  for (int index = 0; index <  2; index++ )
  {if(index >= 4){break;} // err
  int x =timer[index].dayOption;Serial.println(" timer " + String(index));
    String temp = op(x);delay(250);
    if (((SW=='H')||(timer[index].st == 'f')) && (timer[index].en == 1) && (( temp.indexOf(rt.Day) >= 0) && (23 > temp.indexOf(rt.Day))))
    {

      if ((((rt.Hr * 100) + rt.Min) >= ((timer[index].onTimeHr * 100) + timer[index].onTimeMin)) && (((rt.Hr * 100) + rt.Min) < ((timer[index].offTimeHr * 100) + timer[index].offTimeMin)))
      {
        timer[index].st = 'o';
        digitalWrite(TIMER_SW, LOW);SW='L';
        Serial.println("------Relay is on------");SerialBT.println(String(index)+"/"+String(timer[index].st));
      }
    }
    else if (((SW=='L')||(timer[index].st == 'o')) && (timer[index].en == 1) && (( temp.indexOf(rt.Day) >= 0) && (23 > temp.indexOf(rt.Day))))
    {Serial.println("------IN off------");
      if (((timer[index].offTimeHr * 100) + timer[index].offTimeMin) <= ((rt.Hr * 100) + rt.Min))
      {
        timer[index].st = 'f';
        digitalWrite(TIMER_SW, HIGH);SW='H';
        Serial.println("------Relay is off------"); SerialBT.println(String(index)+"/"+String(timer[index].st));
      }
    }

  }

Serial.println("***********END Comp***********");
}


String op (int num)
{ Serial.println("***********In OP***********");
  String list[15] ;
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
  Serial.println("***********END op***********");
  return list[num];

}
