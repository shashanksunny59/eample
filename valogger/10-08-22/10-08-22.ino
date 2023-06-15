//#include<Arduino>

#include "EEPROM.h"
#include <RTClib.h>
#include <SPI.h>
#include <SD.h>

RTC_DS3231 rtc;
File myFile; 

//pin 
const unsigned short analogInPin[] = {A2,A1,A3};          //A3 as Avcc/pins declaration
const unsigned short led[] = {3,2,2,3};
// variables
float tempValue = 0;
unsigned long int preValueTime = 0;                                         //setting vref
float vref = 5.01;

unsigned short  numOfSkipAd = 0, numOfSkip = 0, Date = 0,preDate =0;            // dataskip.
unsigned long tripTotal[2] = {0,0};                       // trip count
unsigned short tripCount[2] = {0,0}, tripad[2] = {0,0};

float  outputValue[] = {0,0},  minTemp[] = {5, 10}, peak[] ={0, 0};     // meassuring
float  mx[] = {0, 0}, mi[] = {0, 0}, maxTemp[] = {0, 0}, rms[] = {0,0};
unsigned long int maxTime[] = {0, 0}, minTime[]= {0, 0}, iAvgTime = 0, iAvgCount = 0; 
long double iAvg = 0;
char trip = 'S';

unsigned long int tme = 100, countTime = 0;                    // OU, VU, NV conditiontion 
boolean whileExc = false, condition = true;
const float mxLt= 230 , miLt= 157, mxNormalLT = 220, miNormalLT  = 160; // max values for 240 and 128 volt
double  nu[] = {137.393,3.18872}, resl = 1023;

                                                                //funtion
void(* resetFunc) (void) = 0;                                                                
unsigned short select =2;
void measuremaxmin( );
void whileCon(char op, unsigned short tm, unsigned short count );
void setvref();
void setLED (unsigned short num, unsigned short action ); //0 OFF 1 On 2 ON --> OFF 3 OFF --> ON
void anCount(char op);
void valueSd(char *txt, unsigned long value);
void datalogger( char op);
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
      
void setup() 
{
  
  Serial.begin(115200);                                  // Initializing Serial
//Serial.print("Setup start time");
//Serial.println(millis());
//   EEPROM.write(0,1);
    pinMode(led[0], OUTPUT);                        // Pin setup
    
    pinMode(led[1], OUTPUT);
  
    setLED(3,false); 


    initi();                    //Initializing RTC & SD 
     

    
      measuremaxmin( );
    
//     Serial.print(" rms->");
//     Serial.println(rms[0],5);
     datalogger('S');
      preDate = Date    ;
    Serial.println(" setup end ");
//  Serial.println(millis());
}

void loop() {

  if(preDate != Date)
  {
    resetFunc();
    }
  
//  
//  Serial.print("In loop ");
//  Serial.println(millis());// setting vref  
  if( (((preValueTime+20) < rms[0])||((preValueTime-20) > rms[0])) || (trip == 'S') )
 {
      Serial.println("IN set ");
      preValueTime = rms[0];
      
//      for(unsigned short typ = 0; typ < 2; typ++)
//     { 
//       if(typ)
//      {
//      Serial.print("AMP_ ");
//     
//     }
//      else
//     {
//     Serial.print("VOLT_ ");
//   
//     }
//      Serial.print(" rms->");
//     Serial.print(rms[typ],5); 
//      Serial.print("output_");
//     Serial.print(outputValue[typ],7);
//     Serial.print(" max_");
//     Serial.print(mx[typ],7);
//     Serial.print(" mi_");
//     Serial.print(mi[typ],7);
//     Serial.print(" peak_");
//    Serial.println(peak[typ],7); 
//     }
     
   analogReference(INTERNAL);
  delay(1);
  unsigned short count = 1000;
  unsigned long tmpvref = 0;
  for (unsigned int id = 1; id <= count; id++) //1000 count 85 msec
  {
    
  tmpvref += analogRead(A3); 
  delay(10);
  }
  tmpvref /= count;
  Serial.print(tmpvref );
//  tempValue = tmpvref*(1.1/1023);
  Serial.print(" vref ");
  tempValue = (tmpvref*5.02)/815;
  Serial.println(tempValue,7);
  if ((tempValue > 4.8 ) && (tempValue < 5.3))
  {
    Serial.print(" changing ");
    vref = tempValue;
  }
  Serial.print(" vref ");
  Serial.println(vref,7);
 // vref = 5.03;
// delay(3500);
analogReference(DEFAULT);
  }
//
// outputValue[0] = (analogRead(analogInPin[0])*vref)/resl;                            // OV
 
 if(   ( (trip == 'U') ||(trip == 'N') || (trip == 'S') ) && (rms[0]> mxLt) )                 
 {
  
//   Serial.print(outputValue[0],7);
  Serial.println(" In OV if ");
  countTime = millis();
  whileCon('O', 2500, 2);
   
 }
 else if(  ((trip == 'O') ||(trip == 'N') || (trip == 'S') )  && (rms[0] < miLt) )                 // UV
 {
//  Serial.print(peak[0],7);
  Serial.println("In UV if");
  countTime = millis();
  whileCon('U', 2500, 4);

 }
 else if ( ( (trip == 'U') ||(trip == 'O')||(trip == 'S') ) && ( rms[0] < mxNormalLT) && ( rms[0] > miNormalLT )  )                // NV
 { 
  
// Serial.print(peak[0],7);
// Serial.print(" ");
// Serial.print(mx[0],7);
 Serial.println(" In NV if");
 countTime = millis();
 whileCon('N',  5000, 6);
 
  }                                          
 
   //taking current & voltage value

   
      measuremaxmin( );

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
     
//    Serial.println("end loop ");
//  Serial.println(millis());                                                           
// for(short int typ = 0; typ < select; typ++)
//  { 
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
////           Serial.print("output_");
////     Serial.print(outputValue[typ],7);
////     Serial.print(" max_");
////     Serial.print(mx[typ],7);
////     Serial.print(" mi_");
////     Serial.print(mi[typ],7);
////     Serial.print(" peak_");
////    Serial.print(peak[typ],7);
//     Serial.print(" vref ");
//     Serial.print(vref,5);
//      Serial.print(" rms->");
//     Serial.println(rms[typ],5); 
//
//  }
}

void whileCon(char op, unsigned short tm, unsigned short count )
{
 Serial.println("IN while fun start time ");
//  Serial.println(millis());
 condition = false;
 whileExc = true;
while (whileExc)
{
 select = 2;
 measuremaxmin();
 Serial.print(count);
 Serial.print(" rms->");
     Serial.println(rms[0],5);
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

  if ( ( rms[0] > mxLt) && (op !='O') && (trip == 'N'))                       //OV
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
   condition = rms[0] > mxLt;
   break;
   case 'U':
   condition = rms[0] < miLt;
   break;
   case 'N':
   condition = ( rms[0] < mxNormalLT)  && ( rms[0] > miNormalLT );
   break;
 }
  if(condition)
 {
   count--;
   if(count == 0)
   {
   countTime = 0;
//   Serial.println("The");
   if( ((trip == 'N') || (trip == 'S')  || (trip != op)) && (op != 'N'))              // trip = N means relay is ON,trip = Y means relay is OFF,
   {                                                 //trip = S means MCU is poweredON,  
   
   trip = op;
   Serial.println("Realy trip");
   setLED(3,true);
   whileExc = false; 
   unsigned short num = 0, add = 0;
                                                          // wrinting file else eeprom
   datalogger(op);

  }
   else if((trip == 'U') || (trip == 'O') || (trip == 'S') )
   {
   Serial.println("Realy ON");
  setLED(1,true);
   countTime = 0;
   trip = 'N';
   whileExc = false; 
   datalogger(op);
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
Serial.println("emd while loop ");
//  Serial.println(millis());
}

void measuremaxmin ()                                 // setting the max(mx) & min(mi) value for measuring voltage & current 
{
//  Serial.println("In measuremaxmin ");
//  Serial.println(millis());
 for(unsigned short id = 1; id <50; id++ )                             // writing MCU power ON the skiped data form eeprom to file
    {
      select = 2;
  unsigned long tempTime = millis();
  while( (millis() -tempTime) < 40)
  {
    delay(3);
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
      
  }
  }   
  
  if(rms[1] < 0.15)
  {
    rms[1] = 0;
    }
    
//  for(short int typ = 0; typ < select; typ++)
//  { 
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
////      Serial.print("output_");
////     Serial.print(outputValue[typ],7);
////     Serial.print(" max_");
////     Serial.print(mx[typ],7);
////     Serial.print(" mi_");
////     Serial.print(mi[typ],7);
////     Serial.print(" peak_");
////    Serial.print(peak[typ],7); 
//  }
  
  
  //I avg
  
//  Serial.println("end measuremaxmin ");
//  Serial.println(millis());
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
  unsigned short temp = 0;
   Serial.println("In datalogger ");
//  Serial.println(millis());
   myFile = SD.open("sk.txt", FILE_WRITE);                               //opening file
  wirteInFile:
      DateTime now = rtc.now();
     Date = now.day();
  if (myFile) 
  {
   Serial.print("file open ");                                                                         // data skied 
  numOfSkip = EEPROM.read(numOfSkipAd );
  delay(150);

  if (numOfSkip != 1)
   {
    Serial.print("EEPROM to file ");
    Serial.println(numOfSkip);
    for(unsigned short id = 1; id < numOfSkip ;id = id+9)
   {
    Serial.print(" ID ");
    Serial.print(id);
    Serial.print("  ");
    
    temp = EEPROM.read(id);                           //op
    Serial.print((char)temp);
    Serial.print(";");
    myFile.print((char)temp);

    delay(150);
    temp = EEPROM.read(id+1);                         //date
    Serial.print(temp);
    Serial.print(";");
    valueSd(";",temp);

    delay(150);                                     //month
    temp = EEPROM.read(id+2);
    Serial.print(temp);
    Serial.print(";");
   valueSd("/", temp);

   delay(150);                                    // year
   temp = EEPROM.read(id+3)+2000;
   Serial.print(temp);
   Serial.print(";");
   valueSd("/",temp); 

   delay(150);                                        //hr
   temp = EEPROM.read(id+4);
   Serial.print(temp);
    Serial.print(";");
   valueSd(";",temp);

   delay(150);                                      //min
   temp = EEPROM.read(id+5);
   Serial.print(temp);
    Serial.print(";");
   valueSd(":", temp);
   myFile.print(";");

   delay(150);                       //vref
   Serial.print(EEPROM.read(id+6)/10.0);
    Serial.print(";");
   myFile.print(EEPROM.read(id+6)/10.0);
   myFile.print("v;");

   delay(150);                                       //vrms
   temp = EEPROM.read(id+7);
   Serial.print(temp);
    Serial.print(";");
   myFile.print(temp);
   myFile.print("V;");

   delay(150);                                      //amp
   Serial.print(EEPROM.read(id+8)/10.0);
    Serial.println(";");
   myFile.print(EEPROM.read(id+8)/10.0);
   myFile.println("A.Skiped");
   }

   delay(150);
   EEPROM.write(numOfSkipAd,1);
   Serial.println(".done");
   }
                                                //prasent data entering 

   Serial.print("Writing to sk.txt...");
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
    myFile = SD.open("sk.txt", FILE_WRITE);                               //opening file
  
     if (myFile) 
    {
    goto wirteInFile;
    }
    else
    {
    Serial.println("error opening sk.txt");
    Serial.print("To EEPROM ");
    numOfSkip = EEPROM.read(numOfSkipAd) ;
    
    delay(250);
    Serial.print(numOfSkip);
    Serial.print("  ");

    temp =(int) op; 
    Serial.print(temp);
    Serial.print(";");
    EEPROM.write(numOfSkip,temp);

    delay(150);
    temp = now.day();
    Serial.print(temp);
    Serial.print("/");
    EEPROM.write((numOfSkip+1),temp);

    delay(150);
    temp = now.month();
    Serial.print(temp);
    Serial.print("/");
    EEPROM.write((numOfSkip+2),temp);

    delay(150);
    temp = now.year() - 2000;
    Serial.print(temp);
    Serial.print(";");
    EEPROM.write((numOfSkip+3),temp);

    delay(150);
    temp = now.hour();
    Serial.print(temp);
    Serial.print(":");
    EEPROM.write((numOfSkip+4),temp);

    delay(150);
    temp = now.minute();
    Serial.print(temp);
    Serial.print(";");
    EEPROM.write((numOfSkip+5),temp);

    delay(150);
    temp = vref*10;
    Serial.print(temp);
    Serial.print(";");
    EEPROM.write((numOfSkip+6),temp);

    delay(150);
    if(rms[0] >= 254)
    {
      temp = round(rms[0]*0.034);
        Serial.print(temp);
        Serial.print(";");
        EEPROM.write((numOfSkip+7),temp);
    }
    else
    {
        Serial.print(round(rms[0]));
        Serial.print(";");
        EEPROM.write((numOfSkip+7),round(rms[0]));
    }
    
    delay(150);
    temp  = round(rms[1]*10);
    Serial.print(temp);
    EEPROM.write((numOfSkip+8),temp);

    delay(150);
    if(numOfSkip > 1016)
    {
      EEPROM.write(numOfSkipAd,1);
      }
    else
    {
    EEPROM.write(numOfSkipAd,numOfSkip+9);  
      }
      Serial.print(" .done ");

     delay(150);
    Serial.println(EEPROM.read(numOfSkipAd));
    
  }
  }
  Serial.println("end datalogger ");
//  Serial.println(millis());
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
