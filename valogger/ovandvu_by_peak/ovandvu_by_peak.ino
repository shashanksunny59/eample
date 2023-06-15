#include<Arduino>

#include "EEPROM.h"
#include <RTClib.h>
#include <SPI.h>
#include <SD.h>

RTC_DS3231 rtc;
File myFile; 

//pin 
const unsigned short analogInPin[] = {A2,A1,A3};          //A3 as Avcc/pins declaration
const unsigned short led[] = {2,3,3,2};
// variables
unsigned long int preValueTime = 0;                                         //setting vref
float vref = 5;

unsigned short  numOfSkipAd = 0, numOfSkip = 0;            // dataskip.
unsigned long tripTotal[2] = {0,0};                       // trip count
unsigned short tripCount[2] = {0,0}, tripad[2] = {0,0};

float  outputValue[] = {0,0},  minTemp[] = {5, 10}, peak[] ={0, 0};     // meassuring
float  mx[] = {0, 0}, mi[] = {0, 0}, maxTemp[] = {0, 0}, rms[] = {0,0};
unsigned long int maxTime[] = {0, 0}, minTime[]= {0, 0}, iAvgTime = 0, iAvgCount = 0; 
long double iAvg = 0;
char trip = 'S';

unsigned long int tme = 100, countTime = 0;                    // OU, VU, NV conditiontion 
boolean whileExc = false, condition = true;
const float mxLt= 4.29 , miLt= 1.168646, mxNormalLT = mxLt - 0.09, miNormalLT  = mxLt + 0.09; // max values for 240 and 128 volt
double  nu[] = {140.903,3.535}, resl = 1023;

                                                                //funtion
unsigned short select =2;
void measuremaxmin( );
void whileCon(char op, unsigned short tm, unsigned short count );
void setvref();
void setLED (unsigned short num, unsigned short action ); //0 OFF 1 On 2 ON --> OFF 3 OFF --> ON
void anCount(char op);
void valueSd(char *txt, unsigned long value);
void initi()
{
     if (! rtc.begin())                                // Initialization RTC 
     {
      Serial.println("Couldn't find RTC");
      Serial.flush();
      while (1) delay(10);
    }
//  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
                                                    // Initializing sd card
    Serial.print("Initializing SD card...");
  
    if (!SD.begin(4)) 
    {
      Serial.println("initialization failed!");
       //return;
    }
     else
      {Serial.println("initialization done.");} 
      
      }
      
void setup() {
  
  Serial.begin(115200);                                  // Initializing Serial

//   EEPROM.write(0,1);
    pinMode(led[0], OUTPUT);                        // Pin setup
    
    pinMode(led[1], OUTPUT);
  
    setLED(3,false); 


    initi();                    //Initializing RTC & SD 
     
    
                                       // writing MCU power ON the skiped data form eeprom to file
        datalogger('S');
   
}

void loop() {
                                                                    // setting vref  
  if( ((millis()-preValueTime) > 20000) )
 {
      Serial.println("IN set ");
      preValueTime = millis();
      for(unsigned short typ = 0; typ < 2; typ++)
     { 
       if(typ)
      {
      Serial.print("AMP_ ");
     
     }
      else
     {
     Serial.print("VOLT_ ");
   
     }
      Serial.print(" rms->");
     Serial.print(rms[typ],5); 
      Serial.print("output_");
     Serial.print(outputValue[typ],7);
     Serial.print(" max_");
     Serial.print(mx[typ],7);
     Serial.print(" mi_");
     Serial.print(mi[typ],7);
     Serial.print(" peak_");
    Serial.println(peak[typ],7); 
     }
  
  analogReference(INTERNAL);
  delay(1);
  float count = 400;
  unsigned long tmpvref = 0;

  for (int id = 1; id <= count; id++) //1000 count 85 msec
  {
   tmpvref += analogRead(A3); 
  }
  tmpvref /= count;
  Serial.print(tmpvref );
  count = tmpvref*0.00632;
  if((count > 4.7) && (count < 5.1))  //Alert : check the values
   {
    vref = count;
   }
  Serial.print(" vref ");
  Serial.println(vref,7);
//      Serial.print(" rms->");
//     Serial.println(rms[0],5);   
   analogReference(DEFAULT);
  }

 outputValue[0] = (analogRead(analogInPin[0])*vref)/resl;                            // OV
 
 if(   ( (trip == 'N') || (trip == 'S') ) && (outputValue[0]> mxLt) )                 
 {
   Serial.print(outputValue[0],7);
  Serial.println(" In OV if");
  countTime = millis();
  whileCon('O', 500, 10);
   
 }
 else if(  ( (trip == 'N') || (trip == 'S') )  && (peak[0] < miLt) )                 // UV
 {
  Serial.print(peak[0],7);
  Serial.println("In UV if");
  countTime = millis();
  whileCon('U', 1000, 500);

 }
 else if ( ( (trip == 'Y')||(trip == 'S') ) && ( mx[0] < mxNormalLT) && ( peak[0] > miNormalLT )  )                // NV
 { 
  trip = 'Y';
 Serial.print(peak[0],7);
 Serial.print(" ");
 Serial.print(mx[0],7);
 Serial.println(" In NV if");
 countTime = millis();
 whileCon('N', 10000, 2500);
 
  }                                          
 
   //taking current & voltage value
    select = 2;
    measuremaxmin();                                                            
}

void whileCon(char op, unsigned short tm, unsigned short count )
{
 Serial.println("IN while fun");
 condition = false;
 whileExc = true;
while (whileExc)
{
 select = 2;
 measuremaxmin();
 Serial.println(count);
// Serial.print(" _IN while loop_");
// Serial.print(op);
// Serial.print(" MAX ");
// Serial.print(mx[0],6);
// Serial.print(" PK ");
// Serial.print(peak[0],6);
// Serial.print(" OP");
// Serial.print(outputValue[0],6);
// Serial.print(" rms->");
//Serial.println(rms[0],5); 

  if ( ( outputValue[0] > mxLt) && (op !='O') && (trip == 'N'))                       //OV
 {
   op ='O';
   tm = 100 ;
   count = 1;
   Serial.println("OV");
    outputValue[0] = (analogRead(analogInPin[0])*vref)/resl;
   }

 switch (op)
 {
   case 'O':
   condition = outputValue[0] > mxLt;
   break;
   case 'U':
   condition = peak[0] < miLt;
   break;
   case 'N':
   condition = ( mx[0] < mxNormalLT)  && ( peak[0] > miNormalLT );
   break;
 }
  if(condition)
 {
   count--;
   if(count == 0)
   {
   countTime = 0;
//   Serial.println("The");
   if( (trip == 'N') || (trip == 'S') )              // trip = N means relay is ON,trip = Y means relay is OFF,
   {                                                 //trip = S means MCU is poweredON,  
   trip = 'Y';
   Serial.println("Realy trip");
   setLED(3,true);
   whileExc = false; 
   unsigned short num = 0, add = 0;
  
                                                          // wrinting file else eeprom

   datalogger(op);

  }
   else if( (trip == 'Y') || (trip == 'S') )
   {
   Serial.println("Realy ON");
  setLED(1,true);
   countTime = 0;
   trip = 'N';
   whileExc = false; 
   }
   }
   
 }
 else if ((millis()-countTime) > tm)
 {
   Serial.println("False value");
   countTime = 0;
   whileExc = false;
 }
}
}

void measuremaxmin ()                                 // setting the max(mx) & min(mi) value for measuring voltage & current 
{
  outputValue[0] = (analogRead(analogInPin[0])*vref)/resl;
  outputValue[1] = (analogRead(analogInPin[1])*vref)/resl;
  for(short int typ = 0; typ < select; typ++)
  {

   if ( ( outputValue[typ] > maxTemp[typ] ) || ( (millis()-maxTime[typ]) > tme ) )
   {
    if((millis()-maxTime[typ]) > tme)
   {
    mx[typ] = maxTemp[typ];

    } 
   maxTemp[typ] = outputValue[typ];
   maxTime[typ]  = millis();
   }
   else if((outputValue[typ] < minTemp[typ])||((millis()-minTime[typ]) > tme) )
   {
    if((millis()-minTime[typ]) > tme)
   {
    mi[typ] = minTemp[typ];
    }
   minTemp[typ] = outputValue[typ];
   minTime[typ]  = millis(); 
    }
    peak[typ] = (mx[typ]-mi[typ])/2.0;
    
    rms[typ] = peak[typ]*nu[typ];

      if( (mi[typ] == 0.0) )
    {
      rms[typ] = 0;
      }
      
//    if(typ)
//  {
//     Serial.print("AMP_ ");
//     
//     }
//  else
//  {
//     Serial.print("VOLT_ ");
//   
//     }
//     Serial.print(" vref ");
//     Serial.print(vref,5);
//      Serial.print(" rms->");
//     Serial.println(rms[typ],5); 
//      Serial.print("output_");
//     Serial.print(outputValue[typ],7);
//     Serial.print(" max_");
//     Serial.print(mx[typ],7);
//     Serial.print(" mi_");
//     Serial.print(mi[typ],7);
//     Serial.print(" peak_");
//    Serial.print(peak[typ],7); 
  } 

  if(rms[1] < 0.2)
  {
    rms[1] = 0;
    }
  
  //I avg
  iAvg += rms[1];
  iAvgCount++;
  if( (millis()-iAvgTime) > 50000 )
  {
    Serial.println("Avg");
    iAvg /= iAvgCount;
    rms[1]  = iAvg;
    iAvg = 0;
    iAvgCount = 0; 
    iAvgTime = millis();
    datalogger('P');
    Serial.println("Avgdone");
  }
}

void valueSd(char* txt, unsigned long value)          // used in the datalogger function purpose is to reduce the code line 
    {
      myFile.print(txt);
      if(value < 9)
      {
        myFile.print('0');
      }
      myFile.print(value);
    }

void datalogger( char op)                                                   // write in file else in eeprom upto continous 112 times
{ 
   Serial.print("In datalogger ");
   
   myFile = SD.open("lpd.txt", FILE_WRITE);                               //opening file
  wirteInFile:
      DateTime now = rtc.now();
  if (myFile) 
  {
   Serial.print("file open ");                                                                         // data skied 
  numOfSkip = EEPROM.read(numOfSkipAd );

  if (numOfSkip != 1)
   {
    Serial.print("EEPROM to file ");
    Serial.print(numOfSkip);
    for(unsigned short id = 1; id < numOfSkip ;id = id+9)
   {
    Serial.print(" ID ");
    Serial.print(id);
    myFile.print((char)EEPROM.read(id));
    valueSd(";",EEPROM.read(id+1));
   valueSd("/", EEPROM.read(id+2));
   valueSd("/",(EEPROM.read(id+3)+2000)); 
   valueSd(";",EEPROM.read(id+4));
   valueSd(":", EEPROM.read(id+5));
   myFile.print(";");
   myFile.print((EEPROM.read(id+6))/10);
   myFile.print("v;");
   myFile.print(EEPROM.read(id+7));
   myFile.print("V;");
   myFile.print(EEPROM.read(id+8));
   myFile.println("A.Skiped");
   }
   
   EEPROM.write(numOfSkipAd,1);
   Serial.println(".done");
   }
                                                //prasent data entering 

   Serial.print("Writing to lpd.txt...");
   myFile.print(op);
   valueSd(";", now.day());
   valueSd("/", now.month());
   valueSd("/",now.year()); 
   valueSd(";",now.hour());
   valueSd(":", now.minute());
   myFile.print(";");
   myFile.print(vref);
   myFile.print("v;");
   myFile.print(rms[0]);
   myFile.print("V;");
   myFile.print(rms[1]);
   myFile.println("A.");
   myFile.close();
   Serial.println("done.");
  } 
  else {
    // if the file didn't open, print an error:
    initi();
    myFile = SD.open("lpd.txt", FILE_WRITE);                               //opening file
  
     if (myFile) 
    {
    goto wirteInFile;
    }
    else
    {
    Serial.println("error opening test.txt");
    Serial.print("To EEPROM ");
    numOfSkip = EEPROM.read(numOfSkipAd) ;
    EEPROM.update(numOfSkip,op);
    EEPROM.update((numOfSkip+1),now.day());
    EEPROM.update((numOfSkip+2),now.month());
    EEPROM.update((numOfSkip+3),(now.year()-2000));
    EEPROM.update((numOfSkip+4),now.hour());
    EEPROM.update((numOfSkip+5),now.minute());
    EEPROM.update((numOfSkip+6),(int)(vref*10));
    if(rms[0] >= 254)
    {
        EEPROM.update((numOfSkip+7),(round((rms[0]*0.034))));
    }
    else
    {
        EEPROM.update((numOfSkip+7),rms[0]);
    }
    EEPROM.update((numOfSkip+8),(rms[1]*10));
    numOfSkip = numOfSkip + 9;
    if(numOfSkip > 1016)
    {
      EEPROM.update(numOfSkipAd,1);
      }
    else
    {
    EEPROM.update(numOfSkipAd,numOfSkip+9);  
      }
    
    Serial.println(".done ");
  }
  }
  
}
 
 void setLED (unsigned short num, boolean action )
{
  unsigned short id  = 0, count = 3, gp = 150;
  digitalWrite(led[num  - 1], LOW);
  for( id = 0; id < count; id++ )
     {
      digitalWrite(led[num], !action);
      delay(gp);
      digitalWrite(led[num], action);
      delay(gp);
     }
      // digitalWrite(led[num  - 1], !action);
}
