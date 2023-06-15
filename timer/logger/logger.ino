
#include <SD.h>
#include <SPI.h>
#include "RTClib.h"

RTC_DS3231 rtc;
File sdcard_file;

int CS_pin = 10; // Pin 10 on Arduino Uno
const int sensor_pin = A0;


void setup() {
  Serial.begin(9600);
  pinMode(sensor_pin,INPUT);
  pinMode(CS_pin, OUTPUT);
  /**********rtc*******/
    if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
     rtc.adjust(DateTime(2022, 06, 07, 11, 24, 0));

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
  Serial.print("   Current   ");
  Serial.println("     ");
  sdcard_file = SD.open("data.txt", FILE_WRITE);
  if (sdcard_file) { 
    sdcard_file.print("Date  ");   
    sdcard_file.print("      ");
    sdcard_file.print("   Time  ");
    sdcard_file.print("     ");
    sdcard_file.print("   Current   ");
    sdcard_file.println("     ");
    sdcard_file.close(); // close the file
  }
  // if the file didn't open, print an error:
  else {
    Serial.println("error opening test.txt");
  }
}

void loop() {
 
unsigned int x=0;
float AcsValue=0.0,Samples=0.0,AvgAcs=0.0,AcsValueF=0.0;

  for (int x = 0; x < 150; x++){ //Get 150 samples
  AcsValue = analogRead(A0);     //Read current sensor values   
  Samples = Samples + AcsValue;  //Add samples together
  delay (3); // let ADC settle before next sample 3ms
}
AvgAcs=Samples/150.0;//Taking Average of Samples

//((AvgAcs * (5.0 / 1024.0)) is converitng the read voltage in 0-5 volts
//2.5 is offset(I assumed that arduino is working on 5v so the viout at no current comes
//out to be 2.5 which is out offset. If your arduino is working on different voltage than 
//you must change the offset according to the input voltage)
//0.185v(185mV) is rise in output voltage when 1A current flows at input
AcsValueF = (2.5 - (AvgAcs * (5.0 / 1024.0)) )/0.185;
 /**********RTC********/
  DateTime time = rtc.now();

  Serial.print(time.timestamp(DateTime::TIMESTAMP_DATE));
  Serial.print("     ");
  Serial.print(time.timestamp(DateTime::TIMESTAMP_TIME));
  Serial.print("      ");
  Serial.println(AcsValueF);
 
  sdcard_file = SD.open("data.txt", FILE_WRITE);
  if (sdcard_file) {    
    sdcard_file.print(time.timestamp(DateTime::TIMESTAMP_DATE));
    sdcard_file.print("     ");   
    sdcard_file.print(time.timestamp(DateTime::TIMESTAMP_TIME));
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
