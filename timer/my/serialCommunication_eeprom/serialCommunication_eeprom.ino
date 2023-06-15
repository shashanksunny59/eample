#include <EEPROM.h>

#define TIMER_SW 18
//variables

//rt
struct realTimeClock{
  unsigned int Temp, Hr, Min, Date, Mon, Year;
  String Day;}rt;
  //bt/c015424052022tue/t009301130030/m0
  String serialData =" ";
  struct TIMER
{unsigned int onTimeMin,onTimeHr,offTimeMin,offTimeHr, dayOption, en; } timer[3];
  
void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
pinMode(TIMER_SW, OUTPUT);
  digitalWrite(TIMER_SW,HIGH);
  for(int numTimer = 0; numTimer <= 3; numTimer++  )
  { int add = numTimer*100;
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
serialData = " ";
while (Serial.available()) {
    serialData +=char( Serial.read());
  }
  delay(1000);
  Serial.println(serialData.length());
  Serial.print("Serial value = ");
  Serial.print(serialData[1]);
  if((serialData[1]=='c')&&(serialData.length()==18))
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
    //RTC.adjust(DateTime(rt.Year, rt.Mon, rt.Date, rt.Hr, rtc.Min, 00));
  }
  else if((serialData[1]=='t')&&(serialData.length()==15))
  {
    int numTimer= String(serialData[2]).toInt();
    int add = numTimer*100;
    String value = (String(serialData[3])+String(serialData[4]));
    timer[numTimer].onTimeHr=value.toInt();
    EEPROM.update(add, timer[numTimer].onTimeHr);
    timer[numTimer].onTimeMin= (String(serialData[5])+String(serialData[6])).toInt();
    EEPROM.update(add+1, timer[numTimer].onTimeMin);
    timer[numTimer].offTimeHr= (String(serialData[7])+String(serialData[8])).toInt();
    EEPROM.update(add+2, timer[numTimer].offTimeHr);
    timer[numTimer].offTimeMin= (String(serialData[9])+String(serialData[10])).toInt();
    EEPROM.update(add+3, timer[numTimer].offTimeMin);
    timer[numTimer].dayOption= (String(serialData[11])+String(serialData[12])).toInt();
    EEPROM.update(add+4, timer[numTimer].dayOption);
    timer[numTimer].en= String(serialData[13]).toInt();
    EEPROM.update(add+5, timer[numTimer].en);
    Serial.println(String("tmer ")+String(serialData[2]));
    Serial.println(String("T_on ")+String(timer[numTimer].onTimeHr)+String(":")+String(timer[numTimer].onTimeMin));
    Serial.println(String("T_off ")+String(timer[numTimer].offTimeHr)+String(":")+String(timer[numTimer].offTimeMin));
    Serial.println(String("DayOp ")+String(timer[numTimer].dayOption));
    Serial.println(String("en ")+String(timer[numTimer].en));
    }
    else if((serialData[1]=='m')&&(serialData.length()==4))
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
