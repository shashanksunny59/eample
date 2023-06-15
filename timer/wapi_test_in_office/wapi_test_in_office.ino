#include <TimeLib.h>
#include <SPI.h>                        //SPI library used for SD card interfacing.
#include <SdFat.h>                      //SDFat.h library used for create file with long file name.
SdFat SD;                               //perform SD card operations like open file,close file, read file etc..

#define CS 53                             //define the Chip Select pin for SD card detection.
#define Size 30                           //define size of the file name.
#define bat_vol A0                        //define battery voltage pin number
#define device_id "504001WL002"                   //define the device id.
#define gsm_rst 5                       //define the relay pin for GSM Reset
#define pwPin1 3                        //define the Ultrasonic Sensor pin

int ck_server=0;
int cal_depth(int pwPin);
void Init_SD_Card(void);                  //Function declaration of initializing the sd card. 
void Write_SD_Card(void);                 //Function declaration of save data in sd card
void C(void);                              //Function declaration of post data to server.   
void Digital_Clock_Display();             //Function declaration of show time stamp.
void millis_Clock_Display();
void printDigits(int digits);
void set_time();
void set_filedate();                      //Function declaration of filename setting for date.
//void i2c_cnt();
void read_sd_card();
void msg_sel(int m);
void send_sms(String ms,String number);
void initGSM();
void initGPRS();
void connectGSM (String cmd, char *res);
void Daystartmsg();
void any_recv_msg_avail(void);
//void send_i2c_from_slave();


/***   Reset Function Start   ***/
void(*resetFunc)(void)=0;
/***   Reset Function End   ***/


String TextForSMS;
// cell numbers to which you want to send the security alert message
String operator1 = "+919866837704"; 
String operator2 = "+919441015266"; 
String operator3 = "+919550816114";
String operator4 = "+918186051757";
String operator5 = "+919849905885";

int v=0;
float vol=0;
unsigned long datalength;
unsigned short topiclength;
unsigned char topic[30]; 
char str[500];
String postdata=""; 
char postdata_array[500];    
String host = "AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"";     //"AT+CIPSTART=\"TCP\",\"broker.hivemq.com\",\"1883\""
unsigned char encodedByte;
int X;
unsigned short MQTTProtocolNameLength;
unsigned short MQTTClientIDLength;
unsigned short MQTTUsernameLength;
unsigned short MQTTPasswordLength;
const char MQTTHost[30] = "broker.hivemq.com";
const char MQTTPort[10] = "1883";
const char MQTTClientID[20] ="cditbjyz";                                      //"sdfuigh89srehgsf";
const char MQTTTopic[30] = "sensor/input/61388198718";                        //"fervid";
const char MQTTProtocolName[10] = "MQTT";                                    //MQIsdp
const char MQTTLVL = 0x04;
const char MQTTFlags = 0x02;
const unsigned int MQTTKeepAlive = 60;
//const char MQTTUsername[30] = "cditbjyz";
//const char MQTTPassword[35] = "wOVHItA1od6w";
const char MQTTQOS = 0x00;
const char MQTTPacketID = 0x0001;                                

char FileName[Size];
char FileDate[Size];
char PreDate[Size];
int FileNum=1,i2c=0,save=0,data_set = 0,server = 1,daysms=0,str_match=0,date_set_done=0;
unsigned long int Files=0;
unsigned long int days=0;
const int sets=(60/5);                  //((60 min)/(5min))
const int parameters=10;
int dataArray[sets][parameters];
String dataString="";

int oht_depth = 0,oht_water_height = 0,oht_height = 560,msg=0,owhs=1,owhs2=1;
int WL_H2_max = 440, WL_H2_min = 420,WL_H1_max=400,WL_H1_min=380,WL_L1_max=30,WL_L1_min=10,WL_L2_max=8,WL_L2_min=3;
unsigned long sensor, cm, inches,sum;
float oht_kl = 0,b_vol=0;
unsigned long RST_time_now=0;
unsigned long period = 290000,time_now = 0,cMillis = 0,mMillis = 0,time_now1 = 0,max_num = 4294967295;
int k=0,l=0;
String date = " ";

unsigned long TIME=0;
int day_complete=0,file_closed=0,Hour=0,Minu=0,Sec=0,Day=0,Month=0,Year=0;
File dataLog;


void ShowSerialData() {            //show the response of the AT command of GSM  module using serial monitor

  while(Serial1.available()!=0)
    Serial.write(Serial1.read());
}

void set_time(){
  Serial1.println("AT+CLTS=1");   //Enable auto network time sync.
  delay(1000);
  ShowSerialData();               //response of the GSM module
  Serial1.println("AT&W");        //Save the setting to permanent memory.
  delay(1000);
  ShowSerialData();               //response of the GSM module
  Serial1.println("AT+CCLK?");    //check the current time.
  delay(1000);
  ShowSerialData();               //response of the GSM module
}

void Digital_Clock_Display() {           // digital clock display of the time.  
  char Date[30];
  char Date2[30];
  String dt_str=" "; 
  int first,last,cnt=0; 
  do{
    set_time();                 
    Serial1.println("AT+CCLK?");                //?????
    while(Serial1.available()==0)
    {
     for(int i=0;i<10;i++);
    }
    while(Serial1.available()!=0)
    {
     dt_str=Serial1.readString();
    }
    first = dt_str.indexOf('"');
    last = dt_str.indexOf('+',first+1);
    str_match=dt_str.startsWith("+CCLK: ",first-7 );
    Serial.println(first);
    Serial.println(last);
    Serial.println(str_match);  
    cnt++; 
  }while(str_match<=0&&cnt<3); 
  
  for(int i=first+1,j=0;i<last;i++,j++)
  {
    Date[j]=dt_str[i];
    Date2[j]=dt_str[i];
  }  
    Date[last-(first+1)]='\0';
    Date2[last-(first+1)]='\0';
   Serial.println("dt_str==");
   Serial.println(dt_str);
   Serial.println(Date);
    Date[2]='-';
    Date[5]='-';   
    Date[0]=Date2[6];
    Date[1]=Date2[7];
    Date[3]=Date2[3]; 
    Date[4]=Date2[4];
    Date[6]=Date2[0];
    Date[7]=Date2[1];
    Date[8]=' ';
    date=Date;
    Serial.println(date);                                     
}

void set_filedate(){
  Digital_Clock_Display();
  if(str_match==1)
  {
    Serial.print(date);
    Serial.println("   set by GSM.");
  }
  else if(str_match<=0)      //If Date not set by GSM, Setting time Manually
  {
    date="";
    Serial.println(date);
    int x=day();
    printDigits(day());
    Serial.print("-");
    date += "-";
    printDigits(month());
    Serial.print("-");
    date += "-";
    int y=year();
    y=y%1000; // ????????
    printDigits(y);
    Serial.print(" ");
    date += " ";
    printDigits(hour());
    Serial.print(":");
    date += ":";
    printDigits(minute());
    Serial.print(":");
    date += ":";
    printDigits(second());
    Serial.println();
    Serial.print(date);
    Serial.println("   set manually.");
  } 
  Serial.println("modified date in filedate: ");
  for(int i=0;i<8;i++)
  {
    FileDate[i]=date[i];
  } 
  FileDate[8]='\0';
  Serial.println(FileDate); 
  char Time[3],DaTe[3];
  DaTe[0]=date[0];
  DaTe[1]=date[1];
  DaTe[2]='\0';
  Day=atoi(DaTe);
  DaTe[0]=date[3];
  DaTe[1]=date[4];
  DaTe[2]='\0';
  Month=atoi(DaTe);
  DaTe[0]=date[6];
  DaTe[1]=date[7];
  DaTe[2]='\0';
  Year=atoi(DaTe);
  Time[0]=date[9];
  Time[1]=date[10];
  Time[2]='\0';
  Hour=atoi(Time);
  Time[0]=date[12];
  Time[1]=date[13];
  Time[2]='\0';
  Minu=atoi(Time);
  Time[0]=date[15];
  Time[1]=date[16];
  Time[2]='\0';
  Sec=atoi(Time);
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
  Serial.println(Sec);
  setTime(Hour,Minu,Sec,Day,Month,Year);//????????????????????
}

void millis_Clock_Display() {            // digital clock display of the time
  date=FileDate;
  date += " ";
  printDigits(hour());
  Serial.print(":");
  date += ":";
  printDigits(minute());
  Serial.print(":");
  date += ":";
  printDigits(second());
  Serial.println();
}

void printDigits(int digits) {
                                                 // utility function for digital clock display: prints preceding colon and leading 0
  if (digits < 10)
  {
    Serial.print('0');
    date += '0';
  }
  Serial.print(digits);
  date += String(digits);
}

void initGSM()                                  // initialize the GSM module using AT commands
{
  connectGSM("AT", "OK");
  connectGSM("ATE1", "OK");
  connectGSM("AT&W", "OK"); ///??????????????
  connectGSM("AT+CPIN?", "READY");
}

void initGPRS()                                 // initialize the GPRS mode
{
  connectGSM("AT+CIPSHUT", "OK");
  connectGSM("AT+CGATT=1", "OK");
  connectGSM("AT+CSTT=\"\",\"\",\"\"", "OK");
  connectGSM("AT+CIPMODE=0", "OK");
  connectGSM("AT+CIICR", "OK");
  delay(5000);
  Serial1.println("AT+CIFSR");
  Serial.println("AT+CIFSR");
  delay(5000);
  connectGSM(host, "CONNECT");
}

void connectGSM (String cmd, char *res)
{
  int c=0;
  while (c<3)
  {
    //Serial.println(cmd);
    Serial1.println(cmd);
    Serial.println(cmd);
    // send command to gsm module
    delay(1000);
    while (Serial1.available() > 0)      // gsm module is give the response of the command
    {
      if (Serial1.find(res))             // if the response is matched to res string then go to next command else untill wait for matched response
      {
        delay(1000);
        return;
      }
    }
    c++;
  }
}

void SendConnectPacket(void)
{
  Serial1.print("\r\nAT+CIPSEND\r\n");
  Serial.print("\r\nAT+CIPSEND\r\n");
  delay(3000);
  Serial1.write(0x10);
  Serial.write(0x10);
  MQTTProtocolNameLength = strlen(MQTTProtocolName);
  MQTTClientIDLength = strlen(MQTTClientID);
 // MQTTUsernameLength = strlen(MQTTUsername);
 // MQTTPasswordLength = strlen(MQTTPassword);
  datalength = MQTTProtocolNameLength + 2 + 4 + MQTTClientIDLength + 2 ;//+ MQTTUsernameLength + 2 + MQTTPasswordLength + 2;
  X = datalength;
  do
  {
    encodedByte = X % 128;
    X = X / 128;
    if ( X > 0 ) {
      encodedByte |= 128;
    }
    Serial1.write(encodedByte);
    Serial.write(encodedByte);
  }
  while ( X > 0 );
  Serial1.write(MQTTProtocolNameLength >> 8);
  Serial.write(MQTTProtocolNameLength >> 8);
  Serial1.write(MQTTProtocolNameLength & 0xFF);
  Serial.write(MQTTProtocolNameLength & 0xFF);
  Serial1.print(MQTTProtocolName);
  Serial.print(MQTTProtocolName);
  Serial1.write(MQTTLVL); // LVL
  Serial.write(MQTTLVL); // LVL
  Serial1.write(MQTTFlags); // Flags
  Serial.write(MQTTFlags); // Flags
  Serial1.write(MQTTKeepAlive >> 8);
  Serial.write(MQTTKeepAlive >> 8);
  Serial1.write(MQTTKeepAlive & 0xFF);
  Serial.write(MQTTKeepAlive & 0xFF);
  Serial1.write(MQTTClientIDLength >> 8);
  Serial.write(MQTTClientIDLength >> 8);
  Serial1.write(MQTTClientIDLength & 0xFF);
    Serial.write(MQTTClientIDLength & 0xFF);
  Serial1.print(MQTTClientID);
  Serial.print(MQTTClientID);
/*  Serial1.write(MQTTUsernameLength >> 8);
  Serial.write(MQTTUsernameLength >> 8);
  Serial1.write(MQTTUsernameLength & 0xFF);
  Serial.write(MQTTUsernameLength & 0xFF);
  Serial1.print(MQTTUsername);
  Serial.print(MQTTUsername);
  Serial1.write(MQTTPasswordLength >> 8);
  Serial.write(MQTTPasswordLength >> 8);
  Serial1.write(MQTTPasswordLength & 0xFF);
  Serial.write(MQTTPasswordLength & 0xFF);
  Serial1.print(MQTTPassword);
  Serial.print(MQTTPassword);
  */
  Serial1.write(0x1A);
  Serial.write(0x1A);
}

void SendPublishPacket(void)
{
  Serial1.print("\r\nAT+CIPSEND\r\n");//??????????
  Serial.print("\r\nAT+CIPSEND\r\n");
  delay(3000);
  memset(str, 0, 500);
  topiclength = sprintf((char*)topic, MQTTTopic);///?????
  postdata.toCharArray(postdata_array, 500);
  datalength = sprintf((char*)str, "%s%s", topic, postdata_array);
  delay(1000);
  Serial1.write(0x30);
  Serial.write(0x30);
  X = datalength + 2;
  do
  {
    encodedByte = X % 128;
    X = X / 128;
    if ( X > 0 ) {
      encodedByte |= 128;
    }
    Serial1.write(encodedByte);
    Serial.write(encodedByte);
  }
  while ( X > 0 );
  Serial1.write(topiclength >> 8);
  Serial.write(topiclength >> 8);
  Serial1.write(topiclength & 0xFF);
  Serial.write(topiclength & 0xFF);
  Serial1.print(str);
  Serial.print(str);
  Serial1.write(0x1A);
  Serial.write(0x1A);
}

void setup()
{
  pinMode(gsm_rst,OUTPUT);
  digitalWrite(gsm_rst,HIGH);
  delay(15000);
  Serial.begin(9600);                                   //set baud rate for serial monitor
  Serial1.begin(9600);                                  //set the baud rate for gsm module
  pinMode(38, OUTPUT);
  digitalWrite(38,HIGH);
  pinMode(pwPin1, INPUT);
  pinMode(bat_vol, INPUT);
  Serial.println("Setup begins ");
  initGSM();                                        //check network signal strength.  
  set_time();                                           //set the system time using GSM network time
  connectGSM("AT+CMGF=1\r", "OK");
  connectGSM("AT+CNMI=2,2,0,0,0\r", "OK");
  pinMode(CS, OUTPUT);
  Init_SD_Card();                                       //initializing the sd card
  set_filedate();
  Serial.println ("set_filedate");
  for(int i=0;FileDate[i]!='\0';i++)
  {
    PreDate[i]=FileDate[i];
  }
  PreDate[8]='\0';
  Serial.print("PreDate is ");
  Serial.println(PreDate);
  Serial.print("FileDate is ");
  Serial.println(FileDate);
  sprintf((char*)FileName,"%s_Data%d.txt",FileDate,FileNum);//set the file name///????
  Serial.println(FileName);                          //print the file name
  millis_Clock_Display();
  Digital_Clock_Display();
  TextForSMS=""+date+"--> Alert: SETUP BODY ";
  send_sms(TextForSMS,operator1);
  delay(5000);
  send_sms(TextForSMS,operator4);
  delay(3000);
  cMillis = mMillis = millis();
}

int cal_depth(int pwPin) 
{
  bool Func_DEBUG=false;
  int Loop=20,discard=7,buf[Loop],temp=0;
  unsigned long sum =0,cm=0;
  for(int i=0;i<Loop;i++)
  {
    int distanceCM = 0;
    sensor = pulseIn(pwPin, HIGH);
    distanceCM += sensor/10;
    delay(10);
    buf[i] = distanceCM;
    if(Func_DEBUG){
      Serial.println(buf[i]);
    }    
  }
  for(int i=0;i<Loop-1;i++)
   {
    for(int j=i+1;j<Loop;j++)
    {
       if(buf[i]>buf[j])
       {
        temp=buf[i];
        buf[i]=buf[j];
        buf[j]=temp;
       }
    }
    if(Func_DEBUG){
      Serial.println(buf[i]);
    }
   }
  sum=0;
  for(int i=discard;i<Loop-discard;i++)
  {
    sum+=buf[i];
    if(Func_DEBUG){
      Serial.println(buf[i]);
    }
  }   
  cm =(sum/(Loop-(2*discard)));
  if(Func_DEBUG){
      Serial.println(F("###########"));
      Serial.println(cm);
    }
  return cm;
}

float cal_bat_vol()
{
  float sum1=0;
  for (int i=0; i<10; i++)
  {
    v = analogRead(bat_vol);
    vol = (v * 5.0)/1024;
    vol = vol*2;
    sum1 +=vol;
    delay(100);
  }
  sum1 = sum1/10;
  return(sum1);
}

void loop()
{
  mMillis= millis();
  cMillis = millis();
  b_vol = cal_bat_vol();
  oht_depth = cal_depth(pwPin1);
  oht_water_height  = oht_height - oht_depth;
  if(oht_water_height<=0)
  {
    oht_water_height=1;
  }
//  oht_kl = (0.4347 * oht_water_height);

/*Serial Data for Maintance*/

Serial.println("-----------------");
Serial.print("oht_water_height:");
Serial.println(oht_water_height);
   Serial.print("oht_depth=");
    Serial.println(oht_depth);
    Serial.println(oht_kl);//????
    Serial.print("Battery_Voltage=");
    Serial.println(b_vol);
    delay(2000);

if((oht_water_height < WL_H2_max &&oht_water_height > WL_H2_min)&& owhs2==1)
{
      msg=4;
      server=1;
      save=1;
      Digital_Clock_Display();
      Write_SD_Card();
      msg_sel(msg);
      Send_To_Server();
      msg=0;
      owhs2=0;
      Serial.println("success Once 4...... :)");
}

else if((oht_water_height <=WL_L2_max &&oht_water_height>=WL_L2_min)&& owhs2==1)
{
      msg=1;
      msg_sel(msg);
      server=1;
      save=1;
      Digital_Clock_Display();
      Write_SD_Card();
      Send_To_Server();
      msg=0;
      owhs2=0;
      Serial.println("success Once 1...... :)");
}
      
else if((oht_water_height < WL_H2_min-15 && oht_water_height >WL_L2_max+15)&&owhs2==0)
{
      owhs2=1;
}

if((oht_water_height < WL_H1_max&&oht_water_height >WL_H1_min)&& owhs==1)
{
      msg=3;
      msg_sel(msg);
      server=1;
      save=1;
      Digital_Clock_Display();
      Write_SD_Card();
      Send_To_Server();
      msg=0;
      owhs=0;
      Serial.println("success Once 3...... :)");
}

else if((oht_water_height <=WL_L1_max&&oht_water_height>=WL_L1_min)&& owhs==1)
{
      msg=2;
      msg_sel(msg);
      server=1;
      save=1;
      Digital_Clock_Display();
      Write_SD_Card();
      Send_To_Server();
      msg=0;
      owhs=0;
      Serial.println("success Once 2...... :)");
}      

else if((oht_water_height < WL_H1_min-15 && oht_water_height >WL_L1_max+15)&&owhs==0)
{
      owhs=1;
}

if (time_now + period >= max_num && cMillis <= 60000)
  {
    time_now = millis();
  }
  
if (cMillis - time_now >= period)
{   
    Serial.println("success every 5 min ...... :)");  
    time_now = cMillis;
    server=1;
    save=1;
    Digital_Clock_Display();
    Write_SD_Card();
    Send_To_Server();   
}

if(ck_server==0)
{
  server=1;
  Digital_Clock_Display();
  Send_To_Server();
  ck_server=1;
} 

if((hour()==23)&&((minute())==59)&&((second())>=0))
  {
    if(day_complete==0&&file_closed==0)
    {
        dataLog = SD.open(FileName, FILE_WRITE);  //??? dataLog       //If file created/opened successfully (dataLog==1) 
        Serial.print("datalog==");
        Serial.println(dataLog);
        if(dataLog)
        {
          String dataString = " ";
          dataString = "]}"; 
          dataLog.print(dataString);                        //prints total one hour data in json formate in a file of sd card
          Serial.println(dataString);
        }
      dataLog.close();                                    //close the file
      read_sd_card();
      data_set=0;
      day_complete=1;
      Serial.println("loop body ");
      Daystartmsg();
      Serial.println(day_complete);
      date_set_done=0;
      Serial.println("loop body ");
    }
}
  
if(hour()==0&&minute()<1&&second()<60)
{
    TextForSMS="Alert: System  Restart in few minutes/seconds...";
    send_sms(TextForSMS,operator1);
    delay(5000);
    digitalWrite(gsm_rst,LOW);
    delay(3000);
    digitalWrite(gsm_rst,HIGH);
    delay(100);
    resetFunc();     
}  
if(data_set>=sets)      
{ 
    data_set=0;
    i2c=0;
}
any_recv_msg_avail();
if(mMillis - time_now1 >= 60000)
{
  time_now1 = mMillis;
  set_filedate();
  millis_Clock_Display();
}
}

void msg_sel(int msg){ 
    int var=msg;
    Serial.print("\nmsg no testing var:");
    Serial.println(var);
   if(var==1)
   {
    Serial.println("sending Msg..... 1"); 
    TextForSMS="Alert: Balancing Tank is empty.";
    delay(2);
   }
   else if(var==2)
   {
    Serial.println("sending Msg..... 2"); 
    TextForSMS="Alert: Water level is low(10%) in Balancing Tank ";
    delay(2);
   }
   else if(var==3)
   {
    Serial.println("sending Msg..... 3"); 
    TextForSMS="Alert: Water level is high(90%) in Balancing Tank ";
    delay(2);
   }
   else if(var==4)
   {
    Serial.println("sending Msg..... 4"); 
    TextForSMS="Alert: Balancing Tank is at overflow state.";
    delay(2);
   }
   else if(var==5)
   {
    Serial.println("sending Msg..... 5"); 
    TextForSMS="Alert: Surge occur in Phase";
    delay(2);
   }
   else if(var==6)
   {
    Serial.println("sending Msg..... 6"); 
    TextForSMS="Alert: Surge occur in Neutral.";
    delay(2);
   }
   
   send_sms(TextForSMS,operator1);
   delay(5000);
   send_sms(TextForSMS,operator2);
   delay(5000);
   send_sms(TextForSMS,operator3);
   delay(5000);
   send_sms(TextForSMS,operator4);
   delay(5000);
   send_sms(TextForSMS,operator5);
   delay(5000);
}

void Daystartmsg()
{
  if(daysms==0)
  {
    millis_Clock_Display();    
    Serial.println(date);                      //date is used for formate of date(19(Y)/11(M)/09(D)).
    Serial.println("sending DayStartMsg......"); 
    TextForSMS=""+date+"--> Alert: Day Started in few minutes/seconds ";
    send_sms(TextForSMS,operator1);
    daysms=1;
  }
}

void send_sms(String msg,String number){
     Serial1.println("AT+CMGF=1\r"); 
     Serial.println("AT+CMGF=1\r");   
     delay(1000); 
     String command_m_number ="AT+CMGS=\""+number+"\"\r"; 
     Serial1.println(command_m_number);
     Serial.println(command_m_number); 
     delay(1000);
     Serial1.println(msg);
     Serial.println(msg);
     delay(100);
     Serial1.println((char)26); 
     Serial.println((char)26); 
     delay(1000);
}

void i2c_cnt(){
  if(i2c==1)
  {
    if(day_complete==1&&date_set_done==0)                //if(hour()==0&&day_complete==1)
    {
      Serial.print("PreDate is ");
      Serial.println(PreDate);
      Serial.print("FileDate is ");
      Serial.println(FileDate);
      while(1)
      {
        if(strncmp(FileDate,PreDate,sizeof(FileDate)) == 0)
        {
            Serial.println("Predate and FileDate are same so update the Filedate.");
            Serial.println(FileDate);
            FileDate[0]='\0';
            Serial.println(FileDate);
            initGSM();                                        //check network signal strength.  
            set_time();
            set_filedate(); 
            Serial.println("file date is set.....Now");
            Serial.print("PreDate is ");
            Serial.println(PreDate);
            Serial.print("FileDate is ");
            Serial.println(FileDate);
            
        }
        else
        {
          Serial.println("Predate and FileDate are Different.");
          for(int i=0;FileDate[i]!='\0';i++)
          {
            PreDate[i]=FileDate[i];
          }
          PreDate[8]='\0';
          Serial.print("PreDate is ");
          Serial.println(PreDate);
          Serial.print("FileDate is ");
          Serial.println(FileDate);
          daysms=0;
          date_set_done=1;
          break;
        } 
      }
      Serial.println("successfully updated");
    }
  if(data_set<sets)                               // if condition is valid  then count the no.of data sets to be received and save the receiving time and date
  { 
    millis_Clock_Display();    
    Serial.println(date);                      //date is used for formate of date(19(Y)/11(M)/09(D)).
    i2c=0;
    save=1;
  }
  Serial.println(data_set);
  if(data_set>=sets)      
  { 
    data_set=0;
    i2c=0;
  }
  Serial.println("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
  }
}


void Send_To_Server(void)                       //post data to server using Post method
{
  if(server==1)
  {
   postdata="{\"projectId\":\"61388198718\",\"Timestamp\":\""+date+"\",\"Oht_water_height\":"+String(oht_water_height)+",\"Msg\":"+String(msg)+",\"VSYS\":"+String(b_vol)+",\"deviceId\":\""+String(device_id)+"\"}";      
   data_set=++data_set;
   initGSM();
   initGPRS();
   SendConnectPacket();
   delay(5000);
   SendPublishPacket();
   if(data_set==sets || day_complete==1)
   {
    data_set = 0;
   }
   if(data_set>=sets)      
   { 
    data_set=0;
   }
   server=0;
  }
}

void Init_SD_Card()                           //check the SD card is placed or not. 
{
  int count=0;
  while (!SD.begin(CS)&&count<5) {
    Serial.println("------------------------------");
    Serial.println("SD Card not found / responding");
    Serial.println("Insert a formatted SD card");
    Serial.println("------------------------------");
    count++;
  }
  if(count==5)
  {
    Serial.println ("SD Card initialisation \"NOT\" completed successfully");
    return;
  }
  Serial.println ("SD Card initialisation completed successfully");
}

void read_sd_card(){
       File myfile = SD.open(FileName);
       if (myfile)
       {
         Serial.println("$$$$$$$$$$$$$$$");
         Serial.print("read_sd_card===");
         while (myfile.available()) {
           Serial.print(char (myfile.read()));
         }
         myfile.close();
         Serial.println();
       }
       Serial.println("$$$$$$$$$$$$$$$");
}

void Write_SD_Card(void)
{
  if(save==1)
  { 
    String dataString = " ";
    if(data_set==0)
    {
      while (SD.exists(FileName))                          // if filename is already used then create new file name.
            {
              if(day_complete==1)                                //After creating 24 files then 25th file is 1st file of next day  
              {
 /*               Serial.println(FileDate);
                FileDate[0]='\0';
                Serial.println(FileDate);
                set_filedate();
                Serial.println("set");
                Serial.println(FileDate);*/
                FileNum=1;
                day_complete=0;
              }
              else
              {
               FileNum++; 
              }
              sprintf((char*)FileName,"%s_Data%d.txt",FileDate,FileNum);
              Serial.println(FileName);
            }
    }
    if((hour()==23)&&((minute()+5)>=60)&&((second())>=0))
      {
        day_complete=1;
        file_closed=1;
        Serial.println("SD body ");
        Daystartmsg();
        Serial.println(day_complete);
        Serial.println("SD body ");
        date_set_done=0;
      }
        dataLog = SD.open(FileName, FILE_WRITE);           //If file created/opened successfully (dataLog==1) 
        Serial.print("datalog==");
        Serial.println(dataLog);
    if(data_set==0)
    {
        if (dataLog) 
        {
          dataString = "{\"projectId\":\"61388198718\",\"Timestamp\":\""+date+"\",\"Oht_water_height\":"+String(oht_water_height)+",\"Msg\":"+String(msg)+",\"VSYS\":"+String(b_vol)+",\"Latitude\":"+String("19.664774")+",\"Longitude\":"+String("78.526718")+",\"deviceId\":\""+String(device_id)+"\"}";      
          dataLog.print(dataString);                        //prints total one hour data in json formate in a file of sd card
          Serial.println(dataString);
          Serial.println(day_complete);
        }
    }
   if(data_set<sets&&data_set!=0)                   //data_set<sets&&data_set!=0//data_set<sets
    {
      if (dataLog) 
      {
       dataString = "{\"projectId\":\"61388198718\",\"Timestamp\":\""+date+"\",\"Oht_water_height\":"+String(oht_water_height)+",\"Msg\":"+String(msg)+",\"VSYS\":"+String(b_vol)+",\"Latitude\":"+String("19.664774")+",\"Longitude\":"+String("78.526718")+",\"deviceId\":\""+String(device_id)+"\"}";      
   
      if(data_set!=sets-1&&day_complete==0)                                    
      {
        file_closed=0;
      }
      dataLog.print(dataString);                     
      Serial.println(dataString);
      Serial.println(day_complete);
      }
    }
    if(data_set==sets-1||day_complete==1)
    {
      if(dataLog)
      {
        dataString = "]}"; 
        dataLog.print(dataString);                        //prints total one hour data in json formate in a file of sd card
        Serial.println(dataString);
      }
      if (SD.exists(FileName)) 
          {
            Serial.print(FileName);
            Serial.println(" file created.");
            Files++;                                            // count the no.of files created
            if(day_complete==1)                      // count the days
              {
                  days++;
              }                                                 //prints the information of days and files
              Serial.print("days==");
              Serial.println(days);
              Serial.print("Files==");
              Serial.println(Files);
          }
          else
          {
            Serial.print(FileName);
            Serial.println(" file doesn't created.");
          }     
    }
   dataLog.close();                                    //close the file
   read_sd_card();
   server=1;    
   save=0;     
  }
}

void any_recv_msg_avail(void){
  TextForSMS="";
  if(Serial1.available()>0){
    TextForSMS = Serial1.readString();
    Serial.print(TextForSMS);   
    delay(10);
  } 
  if(TextForSMS.indexOf("R@S@T")>=0){
  TextForSMS="Your msg received. Alert: System  Restart in few minutes/seconds ";
  send_sms(TextForSMS,operator1);
  delay(5000);
  digitalWrite(gsm_rst,LOW);
  delay(3000);
  digitalWrite(gsm_rst,HIGH);
  delay(100);
  resetFunc();
  }
}
