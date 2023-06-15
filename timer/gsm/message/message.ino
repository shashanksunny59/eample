#include <HardwareSerial.h>
HardwareSerial mySerial(2);


//ERFINDER CODE



float a=0,b=0;
void setup()
{
  mySerial.begin(9600,SERIAL_8N1,15,2);   // Setting the baud rate of GSM Module  
  Serial.begin(9600);    // Setting the baud rate of Serial Monitor (Arduino)
  delay(100);
  mySerial.println("AT");
  delay(1000);
    mySerial.println("ATE0");
  delay(1000);
  mySerial.println("AT+CPIN?");
  delay(1000);
  mySerial.println("AT+CREG?");
  delay(1000);
  mySerial.println("AT+CGATT?");
  delay(1000);
  mySerial.println("AT+CIPSHUT");
  delay(1000);
  mySerial.println("AT+CIPSTATUS");
  delay(1000);
  mySerial.println("AT+CIPMUX=0");
  delay(1000);
  
  ShowSerialData();
 
  mySerial.println("AT+CSTT=\"airtelgprs.com\"");//start task and setting the APN,
  delay(1000);
 
  ShowSerialData();
 
  mySerial.println("AT+CIICR");//bring up wireless connection
  delay(3000);
 
  ShowSerialData();
 
  mySerial.println("AT+CIFSR");//get local IP adress
  delay(2000);
 
  ShowSerialData();
 
  mySerial.println("AT+CIPSPRT=0");
  delay(3000);
 
  ShowSerialData();
  
}


void loop()
{
  Serial.println("In loop");
//  a = analogRead(A0);     
//a = (a * 5.0) / 1024.0;   
//a = a *((96.6+45)/45); //4.08
////a = a*0.9932;// calibrator factor
//unsigned int x=0;
//float AcsValue=0.0,Samples=0.0,AvgAcs=0.0;
//
//  for (int x = 0; x < 150; x++){ //Get 150 samples
//  AcsValue = analogRead(A1);     //Read current sensor values   
//  Samples = Samples + AcsValue;  //Add samples together
//  delay (3); // let ADC settle before next sample 3ms
//}
//AvgAcs=(Samples/150.0);//Taking Average of Samples
//
////((AvgAcs * (5.0 / 1024.0)) is converitng the read voltage in 0-5 volts
////2.5 is offset(I assumed that arduino is working on 5v so the viout at no current comes
////out to be 2.5 which is out offset. If your arduino is working on different voltage than 
////you must change the offset according to the input voltage)
////0.185v(185mV) is rise in output voltage when 1A current flows at input
//b = (2.5 - (AvgAcs * (5.0 / 1024.0)) )/0.185;
//b= b*1.625; // calibrator factor
//Serial.print("voltage_");  
//Serial.println(a);
//Serial.print("Current_");  
//Serial.println(b);
  if (Serial.available()>0)
   switch(Serial.read())
  {
    case 's':
      SendMessage();
      break;
    case 'r':
      RecieveMessage();
      break;
      case 't':
      RecieveMessage();
      break;
      case 'b':
      RecieveMessage();
      break;
      case 'd':
      thingSpeak();
      break;
  }
//  thingSpeak();

 ShowSerialData();delay(500);
}
void thingSpeak()
{
  mySerial.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"");//start up the connection
  delay(6000);
 
  ShowSerialData();
  mySerial.println("AT+CIPSEND");//begin send data to remote server
  delay(4000);
  ShowSerialData();
  
  String str="GET https://api.thingspeak.com/update?api_key=XFXKLSUA2DDOYVAU&field3=" + String(a) +"&field4="+String(b);
  b++;
  Serial.println(str);
  mySerial.println(str);//begin send data to remote server
  
  delay(4000);
  ShowSerialData();
 
  mySerial.println((char)26);//sending
  delay(5000);//waitting for reply, important! the time is base on the condition of internet 
  mySerial.println();
 
  ShowSerialData();
 
  mySerial.println("AT+CIPSHUT");//close the connection
  delay(100);
  ShowSerialData();
  }
void ShowSerialData()
{
  while(mySerial.available()!=0)
  Serial.write(mySerial.read());
  delay(5000); 
  
}
void TimeGSM()
{
  mySerial.println("AT+CMGF=1");
delay(1000);
mySerial.println("AT+CLTS?");
delay(1000);
mySerial.println("AT+CLTS=1;&W");
delay(1000);
mySerial.println("AT+CFUN=1,1");
delay(1000);
mySerial.println("AT+CLTS?");
delay(1000);
mySerial.println("AT+CCLK?");
delay(1000);
  }
void BatteryGSM()
{
mySerial.println("AT+CBC=?\r");
delay(1000);
  }
 void SendMessage()
{
  Serial.println("SEnd");
  mySerial.println("AT+CMGF=?");
  delay(1000);
  mySerial.println("AT+CMGF?");
  delay(1000);
  mySerial.println("AT+CREG?");
  delay(1000);
  mySerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  mySerial.println("AT+CMGS=\"+918074638788\"\r"); // Replace x with mobile number
  delay(2000);
  mySerial.println("I am SMS from GSM Module");// The SMS text you want to send
  delay(100);
   mySerial.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
}


 void RecieveMessage()
{
  mySerial.println("AT+CNMI=2,2,0,0,0"); // AT Command to recieve a live SMS
  delay(1000);
 }
 
