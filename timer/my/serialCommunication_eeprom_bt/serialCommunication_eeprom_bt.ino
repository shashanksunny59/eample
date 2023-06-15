#include "EEPROM.h"
#include "BluetoothSerial.h"

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

//*******************rt*******************
struct realTimeClock{
  unsigned int Temp, Hr, Min, Date, Mon, Year;
  String Day;}rt;
  //bt/c015424052022tue/t009301130030/m0
  
  struct TIMER
{unsigned int onTimeMin,onTimeHr,offTimeMin,offTimeHr, dayOption, en; } timer[3];
  
void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
//*******************BT****************************
SerialBT.begin("24052022"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
//*******************PIN****************************
pinMode(TIMER_SW, OUTPUT);
  digitalWrite(TIMER_SW,HIGH);
  //*******************READ form eeprom*******************
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
    Serial.println("end");
}

void loop() {   
  // put your main code here, to run repeatedly:
  String serialData =" ";
while (SerialBT.available()) {
    serialData +=char( SerialBT.read());
  }
  delay(1000);
  Serial.println(serialData.length());
  Serial.print("Serial value = ");
  Serial.print(serialData[1]);
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
   // rtc.adjust(DateTime(rt.Year, rt.Mon, rt.Date, rt.Hr, rt.Min, 0));
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
  Serial.println();
}
