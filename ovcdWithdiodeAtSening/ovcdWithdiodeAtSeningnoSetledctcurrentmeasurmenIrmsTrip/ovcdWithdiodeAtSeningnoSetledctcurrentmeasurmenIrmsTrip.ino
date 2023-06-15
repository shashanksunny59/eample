/*Begining of Auto generated code by Atmel studio */
// #include <Arduino.h>

/*End of auto generated code by Atmel studio */

//#include<Arduino.h>
//Beginning of Auto generated function prototypes by Atmel Studio

//End of Auto generated function prototypes by Atmel Studio


/*Hi_Low CUTOFF V.1
•  circuit
• Arduino & relay is powered by the USB. And sensing circuit is powered by a Capacitor base AC - DC power supply
• From the Fan regulator the sensing circuit is energized 
• The CT is connected to the rack chargers. With different light switching current measurement is calibrated.
• Program
• Using the ////////Serial. print if we remove, the RMS values are getting affected
• 
 */


// pins declaration
const unsigned short analogInPin[] = { A2, A1, A0 };  //A3 as Avcc
const unsigned short relay = 8, HighPt = 9;
// const unsigned short led[] = { 3, 4, 4, 3 };  // for va logger
const unsigned short led[] = { 2, 3, 3, 2 }; // OVCD v1
// variables for Voltage measurment
float outputValue[] = { 0, 0 }, peak[] = { 0, 0 };
float mx[] = { 0, 0 }, mi[] = { 0, 0 };
float mxTemp[] = { 0, 0 }, miTemp[] = { 5, 10 }, rms[] = { 0, 0 };
unsigned short select = 2;
unsigned short realIcount = 0;
float Irms =0;
double SumI = 0 ;
float vref = 4.98;
short resl = 1024;

// variables for OV & UV
unsigned short OCcount = 1;
unsigned long OCcleartime = 0;
unsigned long int mxTime[] = { 0, 0 }, miTime[] = { 0, 0 }, tme = 250, countTime = 0;
unsigned short ryRxCo = 1, ryRxTm = 100;
char tp = '1';
uint8_t HighPtFlag = 0;
boolean whileExc = false, condi = true;
// const float mxLt = 3.01811, miLt = 1.789, ctmxLt = 4.1,ctNorLt= 3.5; // max values for 270 and 170 volt

 const float mxLt = 2.6797, miLt = 1.685, ctmxLt = 4.1,ctNorLt= 3.5; // max values for 270 and 170 volt

// const float mxLt = 2.65049, miLt = 1.5563, ctmxLt = 4.1,ctNorLt= 3.5; // max values for 270 and 170 volt 3rd board

// double nu[] = { 89.462, 3.535 };
double nu[] = { 102.543, 3.535 }; //3rd board
//function
void measuremaxmin();
void whileCon(char op, unsigned short tm, unsigned short count);
void whileCurrent();



void setup() {
 Serial.begin(115200);
  Serial.println("In setup");
  pinMode(relay, OUTPUT);
  digitalWrite(relay, LOW);
  pinMode(HighPt, OUTPUT);
  digitalWrite(HighPt, LOW);
  pinMode(led[0], OUTPUT);
  digitalWrite(led[0], HIGH);
  pinMode(led[1], OUTPUT);
  digitalWrite(led[1], LOW);

  for (unsigned short idx = 0; idx < 5000; idx++) {
    measuremaxmin();
  }
  Serial.println("End Setup");
}

void loop() {

 // voltage check
  outputValue[0] = (analogRead(analogInPin[0]) * vref) / resl;
  if (((tp == 'N') || (tp == '1') || (HighPtFlag == 0)) && (outputValue[0] > mxLt))  // OV
  {
    Serial.println(" In OV if");
    countTime = millis();
    whileCon('O', 100, 1);
  } else if (((tp == 'N') || (tp == '1') || (HighPtFlag == 1)) && (peak[0] < miLt))  // UV
  {
    Serial.println("In UV if");
    countTime = millis();
    whileCon('U', 200, 1);
  } else if (((tp == 'Y') || (tp == '1')) && (mx[0] < (mxLt - 0.03)) && (peak[0] > (miLt + 0.04)) && (Irms < 4.8) && (OCcount < 4) )  // NV
  {
    tp = 'Y';
    digitalWrite(led[0], LOW);
    digitalWrite(led[1], LOW);
    Serial.println("In NV if");
    countTime = millis();
    whileCon('N', 10000, 6000);
  }
  //taking current & current valuex
 
  measuremaxmin();

  if(tp =='N')
  {
     whileCurrent();
  }


  switch (OCcount) {
  case 2:
  case 3:
        if ((millis() - OCcleartime) > 600000) {
            OCcount = 1;
            }
        break;
  }
  


}

void whileCon(char op, unsigned short tm, unsigned short count ) 
{
  Serial.println("IN while fun");
  condi = false;
  whileExc = true;
  unsigned short blinkLed=0,preblinkLed=0;
  bool preLed = false;

  while (whileExc) {
    measuremaxmin();
    // Serial.println(count);
    // ////////Serial.print(" _IN while loop_");
    // ////////Serial.print(op);
    // ////////Serial.print(" MAX ");
    // ////////Serial.print(mx[0], 6);
    // ////////Serial.print(" PK ");
    // ////////Serial.print(peak[0], 6);
    // ////////Serial.print(" RMS");
    // ////////Serial.println(rms[0], 6);
    //OV
    if ( (tp == 'N')&&(op != 'O') && (outputValue[0] > mxLt) ) {
      op = 'O';
      tm = 100;
      count = 1;
      Serial.println("OV");
      outputValue[0] = (analogRead(analogInPin[0]) * vref) / resl;
    }

    switch (op) {
      case 'O':
        condi = outputValue[0] > mxLt;
        break;
      case 'U':
        condi = peak[0] < miLt;
        break;
      case 'N':
        condi = (mx[0] < (mxLt - 0.03)) && (peak[0] > (miLt + 0.04) && ((peak[1] < ctNorLt )) && (Irms < 5.5));
        if ((preblinkLed-blinkLed) > 420) {
          ////////Serial.println("kpk");
          preblinkLed=blinkLed;
          preLed = preLed ? false : true;
          digitalWrite(led[1], preLed);   
        }
        preblinkLed++;

        break;
    }
    if (condi) {
      count--;
      if (count == 0) {
        countTime = 0;
        if ((tp == 'N') || (tp == '1') ||(op == 'O') ||(op == 'U') ) {
          tp = 'Y';
          digitalWrite(relay, LOW);
          Serial.println("Realy trip");
          switch (op) {
          case 'O':
          HighPtFlag = 1;
          digitalWrite(HighPt, HIGH);
          break;
          case 'U':
          HighPtFlag = 0;
          digitalWrite(HighPt, LOW);
          }
          
          digitalWrite(led[1], LOW);
          digitalWrite(led[0], HIGH);
          delay(1000);      
          whileExc = false;
        } else if ((tp == 'Y') || (tp == '1')) {
          Serial.println("Realy ON");
          digitalWrite(HighPt, LOW);
          HighPtFlag = 0;
          digitalWrite(led[0], LOW);
          digitalWrite(led[1], HIGH);   
          countTime = 0;
          tp = 'N';
          digitalWrite(relay, HIGH);
          whileExc = false;
        }
      }

    } else if ((millis() - countTime) > tm) {
      Serial.println("False value");
      if (op == 'N') {
        digitalWrite(led[1], LOW);
        digitalWrite(led[0], HIGH);
      }
      countTime = 0;
      whileExc = false;
    }
  }

}



void measuremaxmin() 
{
 outputValue[0] = (analogRead(analogInPin[0]) * vref) / resl;
 outputValue[1] = (analogRead(analogInPin[1]) * vref) / resl;


  // for (short int typ=0 ; typ < 1; typ++) {
    if ((outputValue[0] > mxTemp[0]) || ((millis() - mxTime[0]) > tme)) {
      if ((millis() - mxTime[0]) > tme) {
        mx[0] = mxTemp[0];
      }
      mxTemp[0] = outputValue[0];
      mxTime[0] = millis();
    } else if ((0) && ((outputValue[0] < miTemp[0]) || ((millis() - miTime[0]) > tme))) {
      if ((millis() - miTime[0]) > tme) {
        mi[0] = miTemp[0];
      }
      miTemp[0] = outputValue[0];
      miTime[0] = millis();
    }
    peak[0] = mx[0];
    peak[1] = (mx[1] - mi[1]) / 2.0;

    rms[0] = peak[0] * nu[0];

    // if ((mi[1] == 0.0)) {
    //   rms[1] = 0;
    // }

  // }

  float realI = outputValue[1]-2.5;
  
  
    realIcount++;
    SumI += (realI*realI);
  
  if(realIcount > 100) {
  Irms =sqrt(SumI/realIcount);
  // Irms *= 4.908761048;
  Irms *= 5;
  // Serial.print("Irm =");
  // Serial.println(Irms,7);
  realIcount =0;;
  SumI = 0;
   }
  
  //  Serial.print(peak[0],7); Serial.print(",");
  // Serial.println(rms[0],7);

}

void whileCurrent()
{
  for (unsigned short idx = 0; idx < 101; idx++) {
    measuremaxmin();
 delayMicroseconds(100);    
  }
  if  (Irms > ctmxLt)  // Oc
  {
    Serial.println(" In OC if");
    countTime = millis();
    bool whileCurrentst =true;
    unsigned short currentripcount = 6;
    unsigned long  octime = millis();
    unsigned short  octimeout= 250;
    while (whileCurrentst) 
    {
      for (unsigned short idx = 0; idx < 101; idx++) {
    measuremaxmin();
    delayMicroseconds(100);
  }

      // outputValue[1] = (analogRead(analogInPin[1]) * vref) / resl;

      //      voltage check
          if (((tp == 'N') || (tp == '1')) && (outputValue[0] > mxLt))  // OV
      {
        ////////Serial.println(" In OV if");
        countTime = millis();
        whileCon('O', 100, 1);
      } else if (((tp == 'N') || (tp == '1')) && (peak[0] < miLt))  // UV
      {
        ////////Serial.println("In UV if");
        countTime = millis();
        whileCon('U', 200, 1);
      }

      //current check
      if (Irms > ctmxLt) {
      currentripcount--;
      Serial.println(Irms);
      // Serial.println(currentripcount);
      }

      if(!currentripcount)
      {
        whileCurrentst = false;
        digitalWrite(relay, LOW);
        digitalWrite(led[1], LOW);
          digitalWrite(led[0], HIGH);
        Serial.println("Realy trip");
        tp = 'Y';
        delay(5000*OCcount);
        OCcount++;
        OCcleartime = millis();
      }
      else if((millis() - octime ) > octimeout)
      {
        whileCurrentst = false;
       Serial.println(" In OC if faled");
      }

    }
  } 
  
}



