

#include <BluetoothSerial.h>
#include <BTAddress.h>
#include <BTAdvertisedDevice.h>
#include <BTScan.h>


//RTC_DS3231 RTC;


#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,20,4);

//#include "BluetoothSerial.h"
#include <BluetoothSerial.h>
#include <BTAddress.h>
#include <BTAdvertisedDevice.h>
#include <BTScan.h>
BluetoothSerial SerialBT;
#include <SPI.h>  // not used here, but needed to prevent a RTClib compile error

#include <Wire.h>

#include <RTClib.h>
//#include "RTClib.h"
RTC_DS3231 RTC;     // Setup an instance of DS1307 naming it RTC
//#include <EEPROM.h>

#include <EEPROM.h>

void display_time_date();
void date_set_func();
void time_set_func();
void confirmation_1();
void count();
void set_time(int count);
void days_copy(int List[], int Elements);
void days_selection();
void confirmation_2_3(int operation, int a);
void read_eeprom_time(int c);
void write_eeprom_time(int operation,int a);
void BT_ON_OFF_TIME(int operation,int a);
void check_BT_serial_avail();
void display_BT_menu();
void BT_days_option_menu();
void days_elements_count();
void menu_file();
void EEPROMWriteInt(int address, int value)
{Serial.println("In EEPROMWriteInt");
  byte one = (value & 0xFF);
  byte two = ((value >> 8) & 0xFF);

  EEPROM.write(address, one);
  EEPROM.write(address + 1, two);
  EEPROM.commit();
  Serial.println(" end EEPROMWriteInt");
}

int EEPROMReadInt(int address)
{
  Serial.println("In EEPROMReadInt");
  long one = EEPROM.read(address);
  long two = EEPROM.read(address + 1);
   Serial.println(" End EEPROMReadInt");
  return ((one << 0) & 0xFFFFFF) + ((two << 8) & 0xFFFFFFFF);
 
}

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#define TIMER_SW 18 //GPIO 18 
#define MENU 4
#define UP 16
#define DOWN 17
#define ENTER 5
#define EEPROM_SIZE 100 // me define for size
unsigned long interval = 1000, time_now = 0, cMillis = 0, max_num = 4294967295,ON_time[2]={0},OFF_time[2]={0},current_time=0;
int Dotw, Day, Month, Year, Hour, Minu, Sec, s_no = 1, on_cnt, cnt, days_option = 1, ELEMENTS, days_list[7],T_ON_1=0,T_ON_2=0,MODE=5;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
char shortdaysOfTheWeek[7][4] = {"Su ", "Mo ", "Tu ", "We ", "Th ", "Fr ", "Sa"};
int eeprom_cnt_addr[] = {0};
int eeprom_data_addr[] = {4, 6, 8, 10, 12, 14};
int on_data_start_addr[] = {0, 48};
int off_data_start_addr[] = {24, 72};
String Bt_Recv_str="";

struct TIME {
  int HOUR, MINUTE, SECOND, DAYS_OPTION, ELEMENTS;
  int DAYS_LIST[10];
};
struct TIME on_time[2], off_time[2];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);// Set serial port speed
  Serial.println("In setup");
  SerialBT.begin("ESP32test");                                                 //Bluetooth device name
  Wire.begin();                                                               // Start the I2C
  RTC.begin();  // Init RTC
  EEPROM.begin(EEPROM_SIZE);                                                      //intz RTc
  Serial.println("Start....");
  Serial.println("The device started, now you can pair it with bluetooth!");
  if (! RTC.begin()) {                                                                 //RTC error
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();                                                                            
  }
  Serial.println("Time and date set");
  //pin mode
  pinMode(MENU, INPUT_PULLUP);                                          //PIN
  pinMode(UP, INPUT_PULLUP);
  pinMode(DOWN, INPUT_PULLUP);
  pinMode(ENTER, INPUT_PULLUP);
  pinMode(TIMER_SW, OUTPUT);
  digitalWrite(TIMER_SW,HIGH);
  Serial.println("Real Time Clock");
  //lcd displaying 
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  lcd.setCursor(0, 0);                  //At first row first column
  lcd.print("    Fervid Smart");        //Print this
  lcd.setCursor(0, 2);                  //At secound row first column
  lcd.print(" Solutions Pvt.Ltd.");
  lcd.setCursor(0, 3);                  //At Third row first column
  lcd.print("       Timer");         //Print this
  
//  RTC.adjust(DateTime(2020, 12, 26, 13, 5, 00));                  RTC adjust

  Serial.println("------------------");
  delay(2000);
  lcd.clear();
  //reading from eeprom                                         read from eeprom
  on_cnt = EEPROMReadInt(eeprom_cnt_addr[0]);
  Serial.println(on_cnt);
  read_eeprom_time(on_cnt);
  Serial.println("end setup");
}

void loop() {
  // put your main code here, to run repeatedly:
  cMillis = millis();
  Serial.println("In loop");
  DateTime now = RTC.now();
  check_BT_serial_avail();
  if(Bt_Recv_str.startsWith("cardguy007"))
  {
    display_BT_menu(); 
  }
  if (time_now + interval >= max_num && cMillis <= 5000)
  {
    time_now = millis();
  }
  if (cMillis - time_now >= interval)
  {
    display_time_date();
    time_now = millis();
  }
  if (digitalRead(MENU) == LOW)
  {
    s_no = 1;
    while (1)
    {
      //     display_time_date();
      delay(250);
      if (s_no == 1)
      {
        Serial.println("1.Change Date  and  Time.");
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("1.Change Date  and  Time.");
        if (digitalRead(ENTER) == LOW)
        {
          delay(250);
          lcd.clear();
          display_time_date();
          date_set_func();
          lcd.setCursor(0, 0);
          lcd.print("Clock Set-up...");
          time_set_func();
          confirmation_1();
        }
      }
      else if (s_no == 2)
      {
        Serial.println("2.Set ON Time  and  Set OFF Time.");
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("2.Set ON Time  and  Set OFF Time.");
        if (digitalRead(ENTER) == LOW)
        {
          delay(250);
          //write in setup body first... read on count from eeprom.
          //         on_cnt=EEPROMReadInt(eeprom_cnt_addr[0]);
          cnt = on_cnt;
          count();
          on_cnt = cnt;
          set_time(on_cnt);
          
        }
      }
      else if (s_no == 3)
      {
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("3.Manual ON.");
        if (digitalRead(ENTER) == LOW)
        {
          MODE=s_no;
          break;
        }
      }
      else if (s_no == 4)
      {
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("4.Manual OFF.");
        if (digitalRead(ENTER) == LOW)
        { 
          MODE=s_no;
          break;
        }      
      }
      else if (s_no ==5)
      {
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("5.Auto Mode.");
        if (digitalRead(ENTER) == LOW)
        { 
          MODE=s_no;
          digitalWrite(TIMER_SW, HIGH);
          break;
        }       
      }
      if (digitalRead(UP) == LOW)
      {
        s_no++;
        if (s_no >= 6)
        {
          s_no = 1;
        }
      }
      else if (digitalRead(DOWN) == LOW)
      {
        s_no--;
        if (s_no == 0)
        {
          s_no = 5;
        }
      }
      else if (digitalRead(MENU) == LOW)
      {
        break;
      }
    }
    delay(1000);
  }
  if(MODE==3)
  {
    digitalWrite(TIMER_SW, LOW);
    lcd.setCursor(0, 0);
    Serial.println("Timer ON by User.");
    lcd.print("Timer ON by User.");
  }
  else if(MODE==4)
  {
    digitalWrite(TIMER_SW, HIGH);
    lcd.setCursor(0, 0);
    Serial.println("Timer OFF by User");
    lcd.print("Timer OFF by User.");
  }
  if(MODE==5)
  {
    lcd.setCursor(0, 0);
    Serial.println("Timer in Auto mode.");
    lcd.print("Timer in Auto mode.");
    if(((current_time==ON_time[0])||(current_time>=ON_time[0]&&current_time<OFF_time[0]))&&on_cnt>0&&T_ON_1==0)
    {
      for(int index = 0; index <on_time[0].ELEMENTS ; index++) 
      { 
        if(on_time[0].DAYS_LIST[index] == Dotw)
        {
          digitalWrite(TIMER_SW, LOW);
          T_ON_1=1;
          Serial.println("match Success 1");
          Serial.println("Timer ON");
          Serial.println(on_time[0].DAYS_LIST[index]);
          break;
        }
      }
      delay(2000);
      Serial.println();   
    }
    if(((current_time==OFF_time[0])||(current_time<ON_time[0]||current_time>=OFF_time[0]))&&on_cnt>0&&T_ON_1==1)
    {
      for(int index = 0; index <off_time[0].ELEMENTS ; index++) 
      { 
        if(off_time[0].DAYS_LIST[index] == Dotw)
        {
          digitalWrite(TIMER_SW, HIGH);
          T_ON_1=0;
          Serial.println("match Success 2");
          Serial.println("Timer OFF");
          Serial.println(off_time[0].DAYS_LIST[index]);
          break;
        }
      }
      delay(2000);
      Serial.println();  
    }      
    if(((current_time==ON_time[1])||(current_time>=ON_time[1]&&current_time<OFF_time[1]))&&on_cnt>1&&T_ON_2==0)
    {
      for(int index = 0; index <on_time[1].ELEMENTS ; index++) 
      { 
        if(on_time[1].DAYS_LIST[index] == Dotw)
        {
          digitalWrite(TIMER_SW, LOW);
          T_ON_2=1;
          Serial.println("match Success 3");
          Serial.println("Timer ON");
          Serial.println(on_time[1].DAYS_LIST[index]);
          break;
        }
      }
      delay(2000);
      Serial.println();   
    }  
    if(((current_time==OFF_time[1])||(current_time<ON_time[1]||current_time>=OFF_time[1]))&&on_cnt>1&&T_ON_2==1)
    {
      for(int index = 0; index <off_time[1].ELEMENTS ; index++) 
      { 
        if(off_time[1].DAYS_LIST[index] == Dotw)
        {
          digitalWrite(TIMER_SW, HIGH);
          T_ON_2=0;
          Serial.println("match Success 4");
          Serial.println("Timer OFF");
          Serial.println(off_time[1].DAYS_LIST[index]);
          break;
        }
      }
      delay(2000);
      Serial.println();   
    }
  }
    
}

void display_time_date() {
  Serial.print("In display_time_date");
  lcd.noCursor();
  lcd.clear();
  DateTime now = RTC.now();
  lcd.setCursor(7, 0);
  lcd.print("Clock ");
  lcd.setCursor(0, 1);
  lcd.print("Day : ");
  Dotw = now.dayOfTheWeek();
  Serial.print("(");
  Serial.print(daysOfTheWeek[Dotw]);
  Serial.print(") ");
  lcd.print(daysOfTheWeek[Dotw]);
  lcd.setCursor(0, 3);
  lcd.print("Date: ");
  Day = now.day();
  if (Day <= 9)                                   //
  {
    Serial.print("0");
    lcd.print("0");
  }
  Serial.print(Day, DEC);
  Serial.print('-');
  lcd.print(Day, DEC);
  lcd.print("-");
  Month = now.month();
  if (Month <= 9)
  {
    Serial.print("0");
    lcd.print("0");
  }
  Serial.print(Month, DEC);
  Serial.print('-');
  lcd.print(Month, DEC);
  lcd.print("-");
  Year = now.year();
  Serial.print(Year, DEC);
  lcd.print(Year, DEC);
  Serial.print(" ");
  
  lcd.setCursor(0, 2);
  lcd.print("Time: ");
  Hour = now.hour();
  if (Hour <= 9)
  {
    lcd.print("0");
    Serial.print("0");
  }
  Serial.print(Hour, DEC);
  Serial.print(':');
  lcd.print(Hour, DEC);
  lcd.print(":");
  Minu = now.minute();
  if (Minu <= 9)
  {
    lcd.print("0");
    Serial.print("0");
  }
  Serial.print(Minu, DEC);
  Serial.print(':');
  lcd.print(Minu, DEC);
  lcd.print(":");
  Sec = now.second();
  if (Sec <= 9)
  {
    lcd.print("0");
    Serial.print("0");
  }
  Serial.print(Sec, DEC);
  lcd.print(Sec, DEC);
  Serial.println();
  String BT_INFO="("+String(daysOfTheWeek[Dotw])+")  "+String(Day)+"/"+String(Month)+"/"+String(Year)+" "+String(Hour)+":"+String(Minu)+":"+String(Sec);
  SerialBT.println(BT_INFO); 
  current_time=((Hour*10000)+(Minu*100)+(Sec));  
  Serial.println(current_time);
  Serial.println("End  display_time_date"); 
} 

void check_BT_serial_avail(){
  Serial.println("In  check_BT_serial_avail"); 
  if (SerialBT.available()) {
    Bt_Recv_str="";
    while(SerialBT.available())
    {
      Bt_Recv_str+=char(SerialBT.read());
    }
    Serial.println("########################");
    Serial.println(Bt_Recv_str);
    Serial.println("########################");
  }
  Serial.println("End  check_BT_serial_avail"); 
}
void menu_file(){
  Serial.println("In  menu file"); 
  SerialBT.println("Please choose any one(Valid) option.");
  SerialBT.println("--------------   MENU   -----------------");
  SerialBT.println(" 1 .Change Current Date and Time.");
  SerialBT.println(" 2 .Change ON Time and OFF Time.");
  SerialBT.println(" 3 .MANUAL ON.");
  SerialBT.println(" 4 .MANUAL OFF.");
  SerialBT.println(" 5 .Auto mode.");
  SerialBT.println(" 6 .EXIT.");
  SerialBT.println("-------------------------------------------");
  Serial.println("Please choose any one(Valid) option.");
  Serial.println("--------------   MENU   -----------------");
  Serial.println(" 1 .Change Current Date and Time.");
  Serial.println(" 2 .Change ON Time and OFF Time.");
  Serial.println(" 3 .MANUAL ON.");
  Serial.println(" 4 .MANUAL OFF.");
  Serial.println(" 5 .Auto mode.");
  Serial.println(" 6 .EXIT.");
  Serial.println("-----------------------------------------");
  Serial.println("End  menu file"); 
}
void display_BT_menu(){
  Serial.println("In  display_BT_menu"); 
  int choice=0;
  int Exit=0;
  menu_file();
  while(1)
  {  
    if (SerialBT.available()) {
      Bt_Recv_str="";
      while(SerialBT.available())
      {
        Bt_Recv_str+=char(SerialBT.read());
      }
      Serial.println(Bt_Recv_str);
      
      choice=Bt_Recv_str.toInt();
      switch (choice) { 
      case 1: { 
          SerialBT.println("Enter Valid Date and time.");
          Serial.println("Enter Valid Date and time.");
          SerialBT.println("Example Formate. SET YYYY-MM-DD HH:mm:SS(SET 2021-01-01 09:00:00).");
          Serial.println("Example Formate. SET YYYY-MM-DD HH:mm:SS(SET 2021-01-01 09:00:00).");
          Serial.println("----------------------------------(OR)------------------------------");
          SerialBT.println("----------------------------------(OR)------------------------------");
          SerialBT.println("If You Want Exit...");
          Serial.println("If You Want Exit...");
          SerialBT.println("Example Formate. EXIT.");
          Serial.println("Example Formate. EXIT.");
          while(1){
            check_BT_serial_avail();
            if(Bt_Recv_str.startsWith("SET")){
              char TIME[5];
              TIME[0]=Bt_Recv_str[4];
              TIME[1]=Bt_Recv_str[5];
              TIME[2]=Bt_Recv_str[6];
              TIME[3]=Bt_Recv_str[7];
              TIME[4]='\0';
              Year=atoi(TIME);
              Serial.println(Year);
              SerialBT.println(Year);
              TIME[0]=Bt_Recv_str[9];
              TIME[1]=Bt_Recv_str[10];
              TIME[2]='\0';
              Month=atoi(TIME);
              Serial.println(Month);
              SerialBT.println(Month);
              TIME[0]=Bt_Recv_str[12];
              TIME[1]=Bt_Recv_str[13];
              TIME[2]='\0';
              Day=atoi(TIME);
              Serial.println(Day);
              SerialBT.println(Day);
              TIME[0]=Bt_Recv_str[15];
              TIME[1]=Bt_Recv_str[16];
              TIME[2]='\0';
              Hour=atoi(TIME);
              Serial.println(Hour);
              SerialBT.println(Hour);
              TIME[0]=Bt_Recv_str[18];
              TIME[1]=Bt_Recv_str[19];
              TIME[2]='\0';
              Minu=atoi(TIME);
              Serial.println(Minu);
              SerialBT.println(Minu);
              TIME[0]=Bt_Recv_str[21];
              TIME[1]=Bt_Recv_str[22];
              TIME[2]='\0';
              Sec=atoi(TIME);
              Serial.println(Sec);
              SerialBT.println(Sec);
              break;
            }
            if(Bt_Recv_str.startsWith("EXIT")){
              Exit=1;
              break;
            }
          }
          if(Exit==1){
            SerialBT.println("Exit...");
            Serial.println("Exit...");
            Exit=0;
          }
          else if(Year>=1900&&Month<=12&&Day<=31&&Hour<=23&&Minu<=59&&Sec<=59)
          {
            RTC.adjust(DateTime(Year, Month, Day, Hour, Minu, Sec));
            SerialBT.println("Set Date and Time Successfully.");
            Serial.println("Set Date and Time Successfully.");
          }else{
            SerialBT.println("You Enter Invalid date / Wrong formate..");
            Serial.println("You Enter Invalid date / Wrong formate..");            
          }
          menu_file();
          break;          
      } 
      case 2: { 
        /*   SerialBT.println("Current Sets");
          Serial.println("Current Sets");
          SerialBT.println(on_cnt);
          Serial.println(on_cnt);
          SerialBT.println("Current ON times and OFF Times");
          Serial.println("Current ON times and OFF Times");
          for(int x=0;x<on_cnt;x++){
            SerialBT.println("Current ON time");
            Serial.println("Current ON time");
            SerialBT.println(ON_time[x]);
            Serial.println(ON_time[x]);
            SerialBT.println("Current OFF time");
            Serial.println("Current OFF time");
            SerialBT.println(OFF_time[x]);
            Serial.println(OFF_time[x]);
          }
         */
          SerialBT.println("Current ON & OFF Times.");
          Serial.println("Current ON & OFF Times.");
          read_eeprom_time(on_cnt); 
          SerialBT.println("Enter How Many Sets Required?(1 Set = 1 ON Time + 1 OFF Time) .");
          Serial.println("Enter How Many Sets Required?(1 Set = 1 ON Time + 1 OFF Time) ."); 
          SerialBT.println("Please Enter only 1 or 2 only.");
          Serial.println("Please Enter only 1 or 2 only.");
          Serial.println("----------------------------------(OR)------------------------------");
          SerialBT.println("----------------------------------(OR)------------------------------");
          SerialBT.println("If You Want Exit...");
          Serial.println("If You Want Exit...");
          SerialBT.println("Example Formate. EXIT.");
          Serial.println("Example Formate. EXIT.");
          Bt_Recv_str="";
          while(1){
            check_BT_serial_avail();
            if(Bt_Recv_str.startsWith("EXIT"))
            {
              break;
            }           
            choice=Bt_Recv_str.toInt();
            if(choice==1||choice==2){
             SerialBT.println(" Enter 1 or 2 .");
             Serial.println(" Enter  1 or 2 .");
             EEPROMWriteInt(eeprom_cnt_addr[0], choice);
             int operation=0; 
             for(int c=0;c<choice;c++)
             {
               operation=1;
               BT_days_option_menu();
               SerialBT.print("Enter Valid ON Time_");
               Serial.print("Enter Valid ON Time_");
               SerialBT.println(c+1);
               Serial.println(c+1);
               SerialBT.println("Example Formate. SET HH:mm:SS D_NUM(SET 09:00:00 D01).");
               Serial.println("Example Formate. SET HH:mm:SS D_NUM(SET 09:00:00 D01).");
               while(1){
                check_BT_serial_avail();
                if(Bt_Recv_str.startsWith("SET"))
                {
                  BT_ON_OFF_TIME(operation,c);
                  break;
                }
               }
               operation=0;
               BT_days_option_menu();
               SerialBT.print("Enter Valid OFF Time_");
               Serial.print("Enter Valid OFF Time_");
               SerialBT.println(c+1);
               Serial.println(c+1);
               SerialBT.println("Example Formate. SET HH:mm:SS D_NUM(SET 09:00:00 D01).");
               Serial.println("Example Formate. SET HH:mm:SS D_NUM(SET 09:00:00 D01).");
               while(1){
                check_BT_serial_avail();
                if(Bt_Recv_str.startsWith("SET"))
                {
                  BT_ON_OFF_TIME(operation,c);
                  break;
                }
               }
             }
             SerialBT.println("Updated ON & OFF Times.");
             Serial.println("Updated ON & OFF Times.");
             on_cnt = EEPROMReadInt(eeprom_cnt_addr[0]);
             read_eeprom_time(on_cnt);
             break; 
            }
          }
          menu_file();
          break;
      } 
      case 3: { 
          digitalWrite(TIMER_SW, LOW);
          SerialBT.println(" Timer ON by User.");
          Serial.println("Timer ON by User."); 
          MODE=3;
          menu_file();
          break;
      } 
      case 4: { 
          digitalWrite(TIMER_SW, HIGH);
          SerialBT.println(" Timer OFF by User.");
          Serial.println("Timer OFF by User.");
          MODE=4; 
          menu_file();
          break;
      } 
      case 5: { 
          digitalWrite(TIMER_SW, HIGH);
          SerialBT.println(" Timer Set in Auto Mode.");
          Serial.println("Timer Set in Auto Mode."); 
          MODE=5;
          menu_file();
          break;
      } 
      case 6: { 
          SerialBT.println("Bye...I'm exit from bluetooth Serial not bluetooth connection....");
          Serial.println("Bye...I'm exit from bluetooth Serial not bluetooth connection...."); 
          T_ON_1=0;
          T_ON_2=0;
          return; 
      } 
      default: 
          SerialBT.println("Enter Valid Option.");
          Serial.println("Enter Valid Option.");
          menu_file();          
     }
    }   
  }
  Serial.println("End  display_BT_menu");
  
}
void date_set_func() {
  //        Day=1;
  Serial.println("In date_set_func");
  lcd.cursor();
  lcd.setCursor(0, 0);
  lcd.print("Date  Set-up...");
  lcd.setCursor(0, 1);
  lcd.print("                    ");
  while (1)
  {
    Serial.println("Day Set up...");    
    delay(250);
    if (digitalRead(UP) == LOW)
    {
      Day++;
      if (Day >= 32)
      {
        Day = 1;
      }
    }
    else if (digitalRead(DOWN) == LOW)
    {
      Day--;
      if (Day == 0)
      {
        Day = 31;
      }
    }
    else if (digitalRead(ENTER) == LOW)
    {
      delay(250);
      break;
    }
    else if (digitalRead(MENU) == LOW)
    {
      delay(250);
      return;
    }
    Serial.print("Day=");
    Serial.println(Day);
    lcd.setCursor(6,3);
    if(Day<10)
    { 
     lcd.print("0");
    } 
    lcd.print(Day);
  }
  //        Month=1;
  while (1)
  {
    Serial.println("Month Set up...");
    delay(250);
    if (digitalRead(UP) == LOW)
    {
      Month++;
      if (Month >= 13)
      {
        Month = 1;
      }
    }
    else if (digitalRead(DOWN) == LOW)
    {
      Month--;
      if (Month == 0)
      {
        Month = 12;
      }
    }
    else if (digitalRead(ENTER) == LOW)
    {
      delay(250);
      break;
    }
    else if (digitalRead(MENU) == LOW)
    {
      delay(250);
      return;
    }
    Serial.print("Month=");
    Serial.println(Month);
    lcd.setCursor(9,3); 
    if(Month<10)
    {
     lcd.print("0");
    } 
    lcd.print(Month);
  }
  //        Year=1990;
  while (1)
  {
    Serial.println("Year Set up...");
    delay(250);
    if (digitalRead(UP) == LOW)
    {
      Year++;
      if (Year >= 2101)
      {
        Year = 1990;
      }
    }
    else if (digitalRead(DOWN) == LOW)
    {
      Year--;
      if (Year == 1989)
      {
        Year = 2100;
      }
    }
    else if (digitalRead(ENTER) == LOW)
    {
      delay(250);
      break;
    }
    else if (digitalRead(MENU) == LOW)
    {
      delay(250);
      return;
    }
    Serial.print("Year=");
    Serial.println(Year);
    lcd.setCursor(12,3); 
    lcd.print(Year);
  }
  lcd.noCursor();
  Serial.println("end date_set_func");
}
void time_set_func() 
{   Serial.println("In time_set_func");
  
  //        Hour=0;
  lcd.cursor();
  while (1)
  {
    Serial.println("Hour Set up...");
    delay(250);
    if (digitalRead(UP) == LOW)
    {
      Hour++;
      if (Hour >= 24)
      {
        Hour = 0;
      }
    }
    else if (digitalRead(DOWN) == LOW)
    {
      Hour--;
      if (Hour == -1)
      {
        Hour = 23;
      }
    }
    else if (digitalRead(ENTER) == LOW)
    {
      delay(250);
      break;
    }
    else if (digitalRead(MENU) == LOW)
    {
      delay(250);
      return;
    }
    Serial.print("Hour=");
    Serial.println(Hour);
    lcd.setCursor(6,2); 
    if(Hour<10)
    {
     lcd.print("0");
    } 
    lcd.print(Hour);
  }
  //        Minu=0;
  while (1)
  {
    Serial.println("Minu Set up...");
    delay(250);
    if (digitalRead(UP) == LOW)
    {
      Minu++;
      if (Minu >= 60)
      {
        Minu = 0;
      }
    }
    else if (digitalRead(DOWN) == LOW)
    {
      Minu--;
      if (Minu == -1)
      {
        Minu = 59;
      }
    }
    else if (digitalRead(ENTER) == LOW)
    {
      delay(250);
      break;
    }
    else if (digitalRead(MENU) == LOW)
    {
      delay(250);
      return;
    }
    Serial.print("Minu=");
    Serial.println(Minu);
    lcd.setCursor(9,2); 
    if(Minu<10)
    {
     lcd.print("0");
    } 
    lcd.print(Minu);
  }
  if(s_no==1)
  {
    Sec = 0;
  }   
  while (1)
  {
    Serial.println("Sec Set up...");
    delay(250);
    if (digitalRead(UP) == LOW)
    {
      Sec++;
      if (Sec >= 60)
      {
        Sec = 0;
      }
    }
    else if (digitalRead(DOWN) == LOW)
    {
      Sec--;
      if (Sec == -1)
      {
        Sec = 59;
      }
    }
    else if (digitalRead(ENTER) == LOW)
    {
      delay(250);
      break;
    }
    else if (digitalRead(MENU) == LOW)
    {
      delay(250);
      return;
    }
    Serial.print("Sec=");
    Serial.println(Sec);
    lcd.setCursor(12,2); 
    if(Sec<10)
    {
     lcd.print("0");
    } 
    lcd.print(Sec);
  }
  lcd.noCursor();
   Serial.println("End time_set_func");
}
void confirmation_1() {
   Serial.println("In confirmation_1");
  lcd.setCursor(0,0);
  lcd.print("Please Confirm???");
  while (1)
  {
    delay(250);
    Serial.println();
    Serial.println("If You Choose this Date and Time. Please Check & Confirm...");
    Serial.print(Day);
    Serial.print("/");
    Serial.print(Month);
    Serial.print("/");
    Serial.print(Year);
    Serial.print(" ");
    Serial.print(Hour);
    Serial.print(":");
    Serial.print(Minu);
    Serial.print(":");
    Serial.print(Sec);
    Serial.println();

    if (digitalRead(ENTER) == LOW)
    {
      RTC.adjust(DateTime(Year, Month, Day, Hour, Minu, Sec));
      Serial.println("Set Date and Time Successfully.");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("OK! Set Date & Time.");
      break;
    }
    else if (digitalRead(MENU) == LOW)
    {
      Serial.println("Go to Menu Successfully.");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.noCursor();
      lcd.print("Back to Menu...");
      break;
    }
  }
  delay(1000);
  Serial.println("end confirmation_1");
}
void count() {
  Serial.println("In count");
  lcd.clear();
  while (1)
  {
    delay(200);
    Serial.println("How many ON Times Required??.");
    
    lcd.setCursor(0,0);
    lcd.print("How many ON Times ??");
    if (digitalRead(UP) == LOW)
    {
      cnt++;
      if (cnt >= 3)
      {
        cnt = 0;
      }
    }
    else if (digitalRead(DOWN) == LOW)
    {
      cnt--;
      if (cnt == -1)
      {
        cnt = 2;
      }
    }
    else if (digitalRead(MENU) == LOW)
    {
      delay(250);
      break;
    }
    else if (digitalRead(ENTER) == LOW)
    {
      delay(250);
      EEPROMWriteInt(eeprom_cnt_addr[0], cnt);
      break;
    }
    Serial.print("ON Times = ");
    Serial.println(cnt);
    lcd.setCursor(2,2);
    lcd.print(cnt);   
  }
  Serial.println("End count");
}
void set_time(int count) {
  Serial.println("In set_time");
  int operation = 0;
  for (int a = 0; a < count; a++)
  {
    Serial.println("Set ON Time.");
    Hour = on_time[a].HOUR;
    Minu = on_time[a].MINUTE;
    Sec = on_time[a].SECOND;
    days_option = on_time[a].DAYS_OPTION;
    operation = 1;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Set T_ON_");
    lcd.print(a+1);
    lcd.setCursor(0, 2);
    lcd.print("Time: ");
    if(Hour<10)
    {
      lcd.print("0");
    }
    lcd.print(Hour);
    lcd.print(":");
    if(Minu<10)
    {
      lcd.print("0");
    }
    lcd.print(Minu);
    lcd.print(":");
    if(Sec<10)
    {
      lcd.print("0");
    }
    lcd.print(Sec);
    time_set_func();
    days_selection();
    confirmation_2_3(operation, a);
    delay(250);
    Serial.println("Set OFF Time.");
    Hour = off_time[a].HOUR;
    Minu = off_time[a].MINUTE;
    Sec = off_time[a].SECOND;
    days_option = off_time[a].DAYS_OPTION;
    operation = 0;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Set T_OFF_");
    lcd.print(a+1);
    lcd.setCursor(0, 2);
    lcd.print("Time: ");
    if(Hour<10)
    {
      lcd.print("0");
    }
    lcd.print(Hour);
    lcd.print(":");
    if(Minu<10)
    {
      lcd.print("0");
    }
    lcd.print(Minu);
    lcd.print(":");
    if(Sec<10)
    {
      lcd.print("0");
    }
    lcd.print(Sec);
    time_set_func();
    days_selection();
    confirmation_2_3(operation, a);
  }
  Serial.println("end set_time");
}
void days_selection() {
  Serial.println("in days_selection");
  lcd.setCursor(0, 1);
  lcd.print("                    ");
  lcd.setCursor(0, 1);
  lcd.print("Select Days_Option  ");
  
  while (1)
  {
    Serial.println("days_Selection Set up...");
    delay(250);
    
    if (digitalRead(UP) == LOW)
    {
      days_option++;
      if (days_option >= 16)
      {
        days_option = 1;
      }
      lcd.setCursor(0, 3);
      lcd.print("                    ");
    }
    else if (digitalRead(DOWN) == LOW)
    {
      days_option--;
      if (days_option == 0)
      {
        days_option = 15;
      }
      lcd.setCursor(0, 3);
      lcd.print("                    ");
    }
    else if (digitalRead(ENTER) == LOW)
    {
      delay(250);
      break;
    }
    else if (digitalRead(MENU) == LOW)
    {
      delay(250);
      return;
    }
    Serial.print("days_option = ");
    Serial.println(days_option);
    days_elements_count();
  }
   Serial.println("end days_selection");
}
void days_copy(int List[], int Elements) {
   Serial.println("in days_copy");
  Serial.print("Elements ==");
  Serial.println(Elements);
  ELEMENTS = Elements;
  Serial.println(ELEMENTS);
  for (int index = 0; index < Elements ; index++)
  {
    days_list[index] = List[index];
    if(days_list[index]==0)
    {
      lcd.setCursor(0, 3);
    }
    else if(days_list[index]==1)
    {
      lcd.setCursor(3, 3);
    }
    else if(days_list[index]==2)
    {
      lcd.setCursor(6, 3);
    }
    else if(days_list[index]==3)
    {
      lcd.setCursor(9, 3);
    }
    else if(days_list[index]==4)
    {
      lcd.setCursor(12, 3);
    }
    else if(days_list[index]==5)
    {
      lcd.setCursor(15, 3);
    }
    else if(days_list[index]==6)
    {
      lcd.setCursor(18, 3);
    }
    lcd.print(shortdaysOfTheWeek[days_list[index]]);
    Serial.print(shortdaysOfTheWeek[days_list[index]]);
    Serial.print("\t");
  }
  Serial.println();
  Serial.println("End days_copy");
}
void days_elements_count(){
  Serial.println("in days_elements_count");
  if (days_option == 1)
    {
      Serial.println("days_option==1");
      int list[] = {0, 1, 2, 3, 4, 5, 6};
      int elements = (sizeof(list) / sizeof(int));
      days_copy(list, elements);
    }
    else if (days_option == 2)
    {
      Serial.println("days_option==2");
      int list[] = {1, 2, 3, 4, 5, 6};
      int elements = (sizeof(list) / sizeof(int));
      days_copy(list, elements);
    }
    else if (days_option == 3)
    {
      Serial.println("days_option==3");
      int list[] = {1, 2, 3, 4, 5};
      int elements = (sizeof(list) / sizeof(int));
      days_copy(list, elements);
    }
    else if (days_option == 4)
    {
      Serial.println("days_option==4");
      int list[] = {1, 3, 5, 0};
      int elements = (sizeof(list) / sizeof(int));
      days_copy(list, elements);
    }
    else if (days_option == 5)
    {
      Serial.println("days_option==5");
      int list[] = {0, 2, 4, 6};
      int elements = (sizeof(list) / sizeof(int));
      days_copy(list, elements);
    }
    else if (days_option == 6)
    {
      Serial.println("days_option==6");
      int list[] = {1, 2, 3};
      int elements = (sizeof(list) / sizeof(int));
      days_copy(list, elements);
    }
    else if (days_option == 7)
    {
      Serial.println("days_option==7");
      int list[] = {4, 5, 6};
      int elements = (sizeof(list) / sizeof(int));
      days_copy(list, elements);
    }
    else if (days_option == 8)
    {
      Serial.println("days_option==8");
      int list[] = {6, 0};
      int elements = (sizeof(list) / sizeof(int));
      days_copy(list, elements);
    }
    else if (days_option == 9)
    {
      Serial.println("days_option==9");
      int list[] = {0};
      int elements = (sizeof(list) / sizeof(int));
      days_copy(list, elements);
    }
    else if (days_option == 10)
    {
      Serial.println("days_option==10");
      int list[] = {1};
      int elements = (sizeof(list) / sizeof(int));
      days_copy(list, elements);
    }
    else if (days_option == 11)
    {
      Serial.println("days_option==11");
      int list[] = {2};
      int elements = (sizeof(list) / sizeof(int));
      days_copy(list, elements);
    }
    else if (days_option == 12)
    {
      Serial.println("days_option==12");
      int list[] = {3};
      int elements = (sizeof(list) / sizeof(int));
      days_copy(list, elements);
    }
    else if (days_option == 13)
    {
      Serial.println("days_option==13");
      int list[] = {4};
      int elements = (sizeof(list) / sizeof(int));
      days_copy(list, elements);
    }
    else if (days_option == 14)
    {
      Serial.println("days_option==14");
      int list[] = {5};
      int elements = (sizeof(list) / sizeof(int));
      days_copy(list, elements);
    }
    else if (days_option == 15)
    {
      Serial.println("days_option==15");
      int list[] = {6};
      int elements = (sizeof(list) / sizeof(int));
      days_copy(list, elements);
    }
    Serial.println("end days_elements_count");
}
void confirmation_2_3(int operation, int a) {
  
  Serial.println("in confirmation_2_3");
  while (1)
  {
    delay(250);
    Serial.println();
    if (operation == 1)
    {
      Serial.println("If You Choose this Time and Days for ON Time Of the Timer. Please Check & Confirm...");
    }
    else
    {
      Serial.println("If You Choose this Time and Days for OFF Time Of the Timer. Please Check & Confirm...");
    }
    Serial.print(Hour);
    Serial.print(":");
    Serial.print(Minu);
    Serial.print(":");
    Serial.print(Sec);
    Serial.println();
    Serial.print("days_option=");
    Serial.println(days_option);
    Serial.print("ELEMENTS=");
    Serial.println(ELEMENTS);
    for (int index = 0; index < ELEMENTS; index++)
    {
      Serial.print(shortdaysOfTheWeek[days_list[index]]);
      Serial.print("\t");
    }
    Serial.println();
    lcd.setCursor(0,0);
    lcd.print("                    ");
    lcd.setCursor(0,0);
    lcd.print("Please Confirm???");
    if (digitalRead(ENTER) == LOW)
    {
      write_eeprom_time(operation,a);
      Serial.println("Set Days and Time Successfully.");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Set Successfully.");
      break;
    }
    else if (digitalRead(MENU) == LOW)
    {
      Serial.println("Go to Back Successfully.");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Back to Menu...");
      break;
    }
  }
  delay(1000);
  Serial.println("end confirmation_2_3");
}
void write_eeprom_time(int operation,int a){
  Serial.println("in write_eeprom_time");
  if (operation == 1 && a == 0)
      {
        on_time[a].HOUR = Hour;
        EEPROMWriteInt((on_data_start_addr[a] + eeprom_data_addr[0]), Hour);
        on_time[a].MINUTE = Minu;
        EEPROMWriteInt((on_data_start_addr[a] + eeprom_data_addr[1]), Minu);
        on_time[a].SECOND = Sec;
        EEPROMWriteInt((on_data_start_addr[a] + eeprom_data_addr[2]), Sec);
        on_time[a].DAYS_OPTION = days_option;
        EEPROMWriteInt((on_data_start_addr[a] + eeprom_data_addr[3]), days_option);
        on_time[a].ELEMENTS = ELEMENTS;
        EEPROMWriteInt((on_data_start_addr[a] + eeprom_data_addr[4]), ELEMENTS);
        for (int index = 0; index < on_time[a].ELEMENTS; index++)
        {
          on_time[a].DAYS_LIST[index] = days_list[index];
          EEPROMWriteInt((on_data_start_addr[a] + (eeprom_data_addr[5] + (index * 2))), days_list[index]);
        }
        ON_time[a]=((on_time[a].HOUR*10000)+(on_time[a].MINUTE*100)+(on_time[a].SECOND));
      } else if (operation == 1 && a == 1)
      {
        on_time[a].HOUR = Hour;
        EEPROMWriteInt((on_data_start_addr[a] + eeprom_data_addr[0]), Hour);
        on_time[a].MINUTE = Minu;
        EEPROMWriteInt((on_data_start_addr[a] + eeprom_data_addr[1]), Minu);
        on_time[a].SECOND = Sec;
        EEPROMWriteInt((on_data_start_addr[a] + eeprom_data_addr[2]), Sec);
        on_time[a].DAYS_OPTION = days_option;
        EEPROMWriteInt((on_data_start_addr[a] + eeprom_data_addr[3]), days_option);
        on_time[a].ELEMENTS = ELEMENTS;
        EEPROMWriteInt((on_data_start_addr[a] + eeprom_data_addr[4]), ELEMENTS);
        for (int index = 0; index < on_time[a].ELEMENTS; index++)
        {
          on_time[a].DAYS_LIST[index] = days_list[index];
          EEPROMWriteInt((on_data_start_addr[a] + (eeprom_data_addr[5] + (index * 2))), days_list[index]);
        }
        ON_time[a]=((on_time[a].HOUR*10000)+(on_time[a].MINUTE*100)+(on_time[a].SECOND));
      } else if (operation == 0 && a == 0)
      {
        off_time[a].HOUR = Hour;
        EEPROMWriteInt((off_data_start_addr[a] + eeprom_data_addr[0]), Hour);
        off_time[a].MINUTE = Minu;
        EEPROMWriteInt((off_data_start_addr[a] + eeprom_data_addr[1]), Minu);
        off_time[a].SECOND = Sec;
        EEPROMWriteInt((off_data_start_addr[a] + eeprom_data_addr[2]), Sec);
        off_time[a].DAYS_OPTION = days_option;
        EEPROMWriteInt((off_data_start_addr[a] + eeprom_data_addr[3]), days_option);
        off_time[a].ELEMENTS = ELEMENTS;
        EEPROMWriteInt((off_data_start_addr[a] + eeprom_data_addr[4]), ELEMENTS);
        for (int index = 0; index < off_time[a].ELEMENTS; index++)
        {
          off_time[a].DAYS_LIST[index] = days_list[index];
          EEPROMWriteInt((off_data_start_addr[a] + (eeprom_data_addr[5] + (index * 2))), days_list[index]);
        }
        OFF_time[a]=((off_time[a].HOUR*10000)+(off_time[a].MINUTE*100)+(off_time[a].SECOND));
      } else if (operation == 0 && a == 1)
      {
        off_time[a].HOUR = Hour;
        EEPROMWriteInt((off_data_start_addr[a] + eeprom_data_addr[0]), Hour);
        off_time[a].MINUTE = Minu;
        EEPROMWriteInt((off_data_start_addr[a] + eeprom_data_addr[1]), Minu);
        off_time[a].SECOND = Sec;
        EEPROMWriteInt((off_data_start_addr[a] + eeprom_data_addr[2]), Sec);
        off_time[a].DAYS_OPTION = days_option;
        EEPROMWriteInt((off_data_start_addr[a] + eeprom_data_addr[3]), days_option);
        off_time[a].ELEMENTS = ELEMENTS;
        EEPROMWriteInt((off_data_start_addr[a] + eeprom_data_addr[4]), ELEMENTS);
        for (int index = 0; index < off_time[a].ELEMENTS; index++)
        {
          off_time[a].DAYS_LIST[index] = days_list[index];
          EEPROMWriteInt((off_data_start_addr[a] + (eeprom_data_addr[5] + (index * 2))), days_list[index]);
        }
        OFF_time[a]=((off_time[a].HOUR*10000)+(off_time[a].MINUTE*100)+(off_time[a].SECOND));
      }
      Serial.println("end write_eeprom_time");
}
void read_eeprom_time(int c) {
  int num =0;
  Serial.println("in read_eeprom_time");
  
  for (int a = 0; a < c; a++)
  {num++;
  Serial.print("no of = ");Serial.println(num);
    Serial.println("-----1------");
    on_time[a].HOUR = EEPROMReadInt(on_data_start_addr[a] + eeprom_data_addr[0]);
    on_time[a].MINUTE = EEPROMReadInt(on_data_start_addr[a] + eeprom_data_addr[1]);
    on_time[a].SECOND = EEPROMReadInt(on_data_start_addr[a] + eeprom_data_addr[2]);
    on_time[a].DAYS_OPTION = EEPROMReadInt(on_data_start_addr[a] + eeprom_data_addr[3]);
    on_time[a].ELEMENTS = EEPROMReadInt(on_data_start_addr[a] + eeprom_data_addr[4]);
    Serial.println();
    Serial.print(on_time[a].HOUR);
    Serial.print(":");
    Serial.print(on_time[a].MINUTE);
    Serial.print(":");
    Serial.println(on_time[a].SECOND);
    Serial.print("DAYS_OPTION=");
    Serial.println(on_time[a].DAYS_OPTION);
    Serial.print("ELEMENTS=");
    Serial.println(on_time[a].ELEMENTS);
    Serial.println("-----2------");
    for (int index = 0; index < on_time[a].ELEMENTS; index++)
    {
      Serial.print(" loop in index = ");Serial.print( index);
     Serial.print(" a ="); Serial.println(a);
      on_time[a].DAYS_LIST[index] = EEPROMReadInt(on_data_start_addr[a] + (eeprom_data_addr[5] + (index * 2)));
      Serial.print(shortdaysOfTheWeek[on_time[a].DAYS_LIST[index]]);
      Serial.print("\t");
     
    }
    Serial.print("---2.5----");
    ON_time[a]=((on_time[a].HOUR*10000)+(on_time[a].MINUTE*100)+(on_time[a].SECOND));
    Serial.println(ON_time[a]);
    Serial.println();
    
    SerialBT.println();
    SerialBT.print(on_time[a].HOUR);
    SerialBT.print(":");
    SerialBT.print(on_time[a].MINUTE);
    SerialBT.print(":");
    SerialBT.println(on_time[a].SECOND);
    SerialBT.print("DAYS_OPTION=");
    SerialBT.println(on_time[a].DAYS_OPTION);
    SerialBT.print("ELEMENTS=");
    SerialBT.println(on_time[a].ELEMENTS);
    Serial.println("-----3------");
    for (int index = 0; index < on_time[a].ELEMENTS; index++)
    {
      on_time[a].DAYS_LIST[index] = EEPROMReadInt(on_data_start_addr[a] + (eeprom_data_addr[5] + (index * 2)));
      SerialBT.print(shortdaysOfTheWeek[on_time[a].DAYS_LIST[index]]);
      SerialBT.print("\t");
    }
    ON_time[a]=((on_time[a].HOUR*10000)+(on_time[a].MINUTE*100)+(on_time[a].SECOND));
    SerialBT.println(ON_time[a]);
    SerialBT.println();
    off_time[a].HOUR = EEPROMReadInt(off_data_start_addr[a] + eeprom_data_addr[0]);
    off_time[a].MINUTE = EEPROMReadInt(off_data_start_addr[a] + eeprom_data_addr[1]);
    off_time[a].SECOND = EEPROMReadInt(off_data_start_addr[a] + eeprom_data_addr[2]);
    off_time[a].DAYS_OPTION = EEPROMReadInt(off_data_start_addr[a] + eeprom_data_addr[3]);
    off_time[a].ELEMENTS = EEPROMReadInt(off_data_start_addr[a] + eeprom_data_addr[4]);
    Serial.println();
    Serial.print(off_time[a].HOUR);
    Serial.print(":");
    Serial.print(off_time[a].MINUTE);
    Serial.print(":");
    Serial.println(off_time[a].SECOND);
    Serial.print("DAYS_OPTION=");
    Serial.println(off_time[a].DAYS_OPTION);
    Serial.print("ELEMENTS=");
    Serial.println(off_time[a].ELEMENTS);
    Serial.println("-----4------");
    for (int index = 0; index < off_time[a].ELEMENTS; index++)
    {
      off_time[a].DAYS_LIST[index] = EEPROMReadInt(off_data_start_addr[a] + (eeprom_data_addr[5] + (index * 2)));
      Serial.print(shortdaysOfTheWeek[off_time[a].DAYS_LIST[index]]);
      Serial.print("\t");
    }
    OFF_time[a]=((off_time[a].HOUR*10000)+(off_time[a].MINUTE*100)+(off_time[a].SECOND));
    Serial.println(OFF_time[a]);
    Serial.println();

    SerialBT.println();
    SerialBT.print(off_time[a].HOUR);
    SerialBT.print(":");
    SerialBT.print(off_time[a].MINUTE);
    SerialBT.print(":");
    SerialBT.println(off_time[a].SECOND);
    SerialBT.print("DAYS_OPTION=");
    SerialBT.println(off_time[a].DAYS_OPTION);
    SerialBT.print("ELEMENTS=");
    SerialBT.println(off_time[a].ELEMENTS);
    Serial.println("-----5------");
    for (int index = 0; index < off_time[a].ELEMENTS; index++)
    {
      off_time[a].DAYS_LIST[index] = EEPROMReadInt(off_data_start_addr[a] + (eeprom_data_addr[5] + (index * 2)));
      SerialBT.print(shortdaysOfTheWeek[off_time[a].DAYS_LIST[index]]);
      SerialBT.print("\t");
    }
    OFF_time[a]=((off_time[a].HOUR*10000)+(off_time[a].MINUTE*100)+(off_time[a].SECOND));
    SerialBT.println(OFF_time[a]);
    SerialBT.println();
  }
  Serial.println("-----6------");
  Serial.println("end read_eeprom_time");
}
void BT_days_option_menu(){
  Serial.println("in BT_days_option_menu");
  Serial.println("-------------Days Option Menu--------------");
  Serial.println("D01--->Su Mo Tu We Th Fr Sa");
  Serial.println("D02--->   Mo Tu We Th Fr Sa");
  Serial.println("D03--->   Mo Tu We Th Fr   ");
  Serial.println("D04--->Su Mo    We    Fr   ");
  Serial.println("D05--->Su    Tu    Th    Sa");
  Serial.println("D04--->Su Mo    We    Fr   ");
  Serial.println("D06--->   Mo Tu We");
  Serial.println("D07--->            Th Fr Sa");
  Serial.println("D08--->Su                Sa");
  Serial.println("D09--->Su                  ");
  Serial.println("D10--->   Mo               ");
  Serial.println("D11--->      Tu            ");
  Serial.println("D12--->         We         ");
  Serial.println("D13--->            Th      ");
  Serial.println("D14--->               Fr   ");
  Serial.println("D15--->                  Sa");
  Serial.println("-------------------------------------------");
  SerialBT.println("-------------Days Option Menu--------------");
  SerialBT.println("D01--->Su Mo Tu We Th Fr Sa");
  SerialBT.println("D02--->   Mo Tu We Th Fr Sa");
  SerialBT.println("D03--->   Mo Tu We Th Fr   ");
  SerialBT.println("D04--->Su Mo    We    Fr   ");
  SerialBT.println("D05--->Su    Tu    Th    Sa");
  SerialBT.println("D04--->Su Mo    We    Fr   ");
  SerialBT.println("D06--->   Mo Tu We");
  SerialBT.println("D07--->            Th Fr Sa");
  SerialBT.println("D08--->Su                Sa");
  SerialBT.println("D09--->Su                  ");
  SerialBT.println("D10--->   Mo               ");
  SerialBT.println("D11--->      Tu            ");
  SerialBT.println("D12--->         We         ");
  SerialBT.println("D13--->            Th      ");
  SerialBT.println("D14--->               Fr   ");
  SerialBT.println("D15--->                  Sa");
  SerialBT.println("-------------------------------------------");
  Serial.println("end BT_days_option_menu");
  
}
void BT_ON_OFF_TIME(int operation,int a){
  Serial.println("in BT_ON_OFF_TIME");
  char TIME[3];
  TIME[0]=Bt_Recv_str[4];
  TIME[1]=Bt_Recv_str[5];
  TIME[2]='\0';
  Hour=atoi(TIME);
  Serial.println(Hour);
  SerialBT.println(Hour);
  TIME[0]=Bt_Recv_str[7];
  TIME[1]=Bt_Recv_str[8];
  TIME[2]='\0';
  Minu=atoi(TIME);
  Serial.println(Minu);
  SerialBT.println(Minu);
  TIME[0]=Bt_Recv_str[10];
  TIME[1]=Bt_Recv_str[11];
  TIME[2]='\0';
  Sec=atoi(TIME);
  Serial.println(Sec);
  SerialBT.println(Sec);
  TIME[0]=Bt_Recv_str[14];
  TIME[1]=Bt_Recv_str[15];
  TIME[2]='\0';
  days_option=atoi(TIME);
  Serial.println(days_option);
  SerialBT.println(days_option);
  days_elements_count();  
  write_eeprom_time(operation,a);
  Bt_Recv_str="";
  Serial.println("end BT_ON_OFF_TIME");
}
