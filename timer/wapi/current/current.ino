#include <SD.h>
#include <SPI.h>
#include "RTClib.h"
RTC_DS3231 rtc;
File sdcard_file;

int CS_pin = 10; // Pin 10 on Arduino Uno
int sensor_pin = A0;


void setup() {
  Serial.begin(9600);
  pinMode(sensor_pin,INPUT);
  pinMode(CS_pin, OUTPUT);
 //RTC
 if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
  
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); 
  // SD Card Initialization
  if (SD.begin())
  {
    Serial.println("SD card is ready to use.");
  } else
  {
    Serial.println("SD card initialization failed");
    return;
  }
  
  Serial.print("Date  ");   
  Serial.print("      ");
  Serial.print("   Time  ");
  Serial.print("     ");
  Serial.print("   current   ");
  Serial.println("     ");
  sdcard_file = SD.open("data.txt", FILE_WRITE);
  if (sdcard_file) { 
    sdcard_file.print("Date  ");   
    sdcard_file.print("      ");
    sdcard_file.print("   Time  ");
    sdcard_file.print("     ");
    sdcard_file.print("   current   ");
    sdcard_file.println("     ");
    sdcard_file.close(); // close the file
  }
  // if the file didn't open, print an error:
  else {
    Serial.println("error opening test.txt");
  }
}

void loop() {
   DateTime now = rtc.now();
  unsigned int x=0;
float AcsValue=0.0,Samples=0.0,AvgAcs=0.0,AcsValueF=0.0;

  for (int x = 0; x < 150; x++){ //Get 150 samples
  AcsValue = analogRead(A0);     //Read current sensor values   
  Samples = Samples + AcsValue;  //Add samples together
  delay (3); // let ADC settle before next sample 3ms
}
AvgAcs=Samples/150.0;//Taking Average of Samples
AcsValueF = (2.5 - (AvgAcs * (5.0 / 1024.0)) )/0.100;

delay(50);
//**************//
String text = " ";
   int one , two, three;
   one =now.day(); two =now.month(); three = now.year();
text = " "+String(one)+"/"+String(two)+"/"+String(three);
  Serial.print(text);
  Serial.print("     ");
  one =now.hour(); two =now.minute(); three = now.second();
text = " "+String(one)+"/"+String(two)+"/"+String(three);
  Serial.print(text);
  Serial.print("      ");
  Serial.println(AcsValueF);
 
  sdcard_file = SD.open("data.txt", FILE_WRITE);
  if (sdcard_file) {
    
    
    one =now.day(); two =now.month(); three = now.year();
text = " "+String(one)+"/"+String(two)+"/"+String(three);
    sdcard_file.print(text);
    sdcard_file.print("     ");  
    one =now.hour(); two =now.minute(); three = now.second();
text = " "+String(one)+"/"+String(two)+"/"+String(three);
    sdcard_file.print(text);
    sdcard_file.print("     ");
    sdcard_file.println(AcsValueF);
    sdcard_file.close(); // close the file
  }
  // if the file didn't open, print an error:
  else {
    Serial.println("error opening test.txt");
  }
  delay(3000);
}
    
