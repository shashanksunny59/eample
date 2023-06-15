//ERFINDER CODE
#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // RX, TX
void readGSM();
void setup()
{

Serial.begin(9600);mySerial.begin(9600);
delay(2000);
mySerial.println("AT");
readGSM();

mySerial.println("AT+CMGF=1");
readGSM();
mySerial.println("AT+CLTS?");
readGSM();
mySerial.println("AT+CLTS=1;&W");
readGSM();
mySerial.println("AT+CFUN=1,1");
readGSM();
mySerial.println("AT+CLTS?");
readGSM();
mySerial.println("AT+CCLK?");
readGSM();
}

void loop() // run over and over
{
 mySerial.println("AT+CCLK?");
readGSM();
delay(1000);
}
 void readGSM(){
  delay(200);
 while(mySerial.available())
{Serial.write(mySerial.read());}
  Serial.println("");
}
