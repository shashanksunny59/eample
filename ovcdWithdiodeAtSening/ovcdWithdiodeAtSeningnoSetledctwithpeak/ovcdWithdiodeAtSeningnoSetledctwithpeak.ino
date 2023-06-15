//#include<Arduino.h>
/*Hi_Low CUTOFF V.1
•  circuit
• Arduino & relay is powered by the USB. And sensing circuit is powered by a Capacitor base AC - DC power supply
• From the Fan regulator the sensing circuit is energized 
• The CT is connected to the rack chargers. With different light switching current measurement is calibrated.
• Program
• Using the //////Serial. print if we remove, the RMS values are getting affected
• 
 */


// pins declaration
const unsigned short analogInPin[] = { A2, A1, A0 };  //A3 as Avcc
const unsigned short relay = 8;
const unsigned short led[] = { 3, 4, 4, 3 };  //
// variables for Voltage measurment
float outputValue[] = { 0, 0 }, peak[] = { 0, 0 };
float mx[] = { 0, 0 }, mi[] = { 0, 0 };
float mxTemp[] = { 0, 0 }, miTemp[] = { 5, 10 }, rms[] = { 0, 0 };
unsigned short select = 2;
float vref = 5;
short resl = 1023;

// variables for OV & UV
unsigned long int mxTime[] = { 0, 0 }, miTime[] = { 0, 0 }, tme = 200, countTime = 0;
unsigned short ryRxCo = 1, ryRxTm = 100;
char tp = '1';
boolean whileExc = false, condi = true;
const float mxLt = 2.7956988, miLt = 1.7497556, ctmxLt = 1.92;  // max values for 270 and 170 volt

double nu[] = { 97.00497, 3.535 };
//function
void measuremaxmin(short int typ = 0);
void whileCon(char op, unsigned short tm, unsigned short count, unsigned short whileselect = 1);



void setup() {

  pinMode(relay, OUTPUT);
  digitalWrite(relay, LOW);
  pinMode(led[0], OUTPUT);
  digitalWrite(led[0], HIGH);
  pinMode(led[1], OUTPUT);
  digitalWrite(led[1], LOW);

  Serial.begin(115200);
  //////Serial.println("In setup");

  for (unsigned short idx = 0; idx < 5000; idx++) {
    select = 1;
    measuremaxmin();
  }
  digitalWrite(led[0], LOW);
  //////Serial.println("End Setup");
}

void loop() {

 // voltage check
  outputValue[0] = (analogRead(analogInPin[0]) * vref) / resl;
  if (((tp == 'N') || (tp == '1')) && (outputValue[0] > mxLt))  // OV
  {
    //////Serial.println(" In OV if");
    countTime = millis();
    whileCon('O', 100, 1);
  } else if (((tp == 'N') || (tp == '1')) && (peak[0] < miLt))  // UV
  {
    //////Serial.println("In UV if");
    countTime = millis();
    whileCon('U', 200, 1);
  } else if (((tp == 'Y') || (tp == '1')) && (mx[0] < (mxLt - 0.04)) && (peak[0] > (miLt + 0.04)) && (peak[1] < (ctNorLt)) )  // NV
  {
    tp = 'Y';
    digitalWrite(led[0], LOW);
    digitalWrite(led[1], LOW);
    Serial.println("In NV if");
    countTime = millis();
    whileCon('N', 5000, 2500,2);
  }
  //taking current & current value
  select = 2;
  measuremaxmin();

  if(tp =='N')
  {
     whileCurrent();
  }
}

void whileCon(char op, unsigned short tm, unsigned short count, unsigned short whileselect = 1) 
{
  //////Serial.println("IN while fun");
  condi = false;
  whileExc = true;
  unsigned short blinkLed=0,preblinkLed=0;
  bool preLed = false;

  while (whileExc) {
    select = whileselect;
    measuremaxmin();
    // //////Serial.println(count);
    // //////Serial.print(" _IN while loop_");
    // //////Serial.print(op);
    // //////Serial.print(" MAX ");
    // //////Serial.print(mx[0], 6);
    // //////Serial.print(" PK ");
    // //////Serial.print(peak[0], 6);
    // //////Serial.print(" RMS");
    // //////Serial.println(rms[0], 6);
    //OV
    if ( (tp == 'N')&&(op != 'O') && (outputValue[0] > mxLt) ) {
      op = 'O';
      tm = 100;
      count = 1;
      //////Serial.println("OV");
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
        condi = (mx[0] < (mxLt - 0.04)) && (peak[0] > (miLt + 0.04) && ((peak[1] < ctNorLt )));
        if ((preblinkLed-blinkLed) > 420) {
          //////Serial.println("kpk");
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
        //   //////Serial.println("The");
        if ((tp == 'N') || (tp == '1')) {
          tp = 'Y';
          digitalWrite(relay, LOW);
          //////Serial.println("Realy trip");
          digitalWrite(led[1], LOW);
          digitalWrite(led[0], HIGH);
          delay(1000);      
          whileExc = false;
        } else if ((tp == 'Y') || (tp == '1')) {
          //////Serial.println("Realy ON");
          digitalWrite(led[0], LOW);
          digitalWrite(led[1], HIGH);   
          countTime = 0;
          tp = 'N';
          digitalWrite(relay, HIGH);
          whileExc = false;
        }
      }

    } else if ((millis() - countTime) > tm) {
      //////Serial.println("False value");
      if (op == 'N') {
        digitalWrite(led[1], LOW);
        digitalWrite(led[0], HIGH);
      }
      countTime = 0;
      whileExc = false;
    }
  }

}



void measuremaxmin(short int typ = 0) 
{
 outputValue[0] = (analogRead(analogInPin[0]) * vref) / resl;
 outputValue[1] = (analogRead(analogInPin[1]) * vref) / resl;

  for (typ ; typ < select; typ++) {
    if ((outputValue[typ] > mxTemp[typ]) || ((millis() - mxTime[typ]) > tme)) {
      if ((millis() - mxTime[typ]) > tme) {
        mx[typ] = mxTemp[typ];
      }
      mxTemp[typ] = outputValue[typ];
      mxTime[typ] = millis();
    } else if ((typ) && ((outputValue[typ] < miTemp[typ]) || ((millis() - miTime[typ]) > tme))) {
      if ((millis() - miTime[typ]) > tme) {
        mi[typ] = miTemp[typ];
      }
      miTemp[typ] = outputValue[typ];
      miTime[typ] = millis();
    }
    peak[0] = mx[0];
    peak[1] = (mx[1] - mi[1]) / 2.0;

    rms[typ] = peak[typ] * nu[typ];

    if ((mi[1] == 0.0)) {
      rms[1] = 0;
    }

    if (typ) {
    // //////Serial.print("AMP_ ");
    //  Serial.print("output:");
      //  Serial.println(outputValue[typ],7);
      //  Serial.println(",");
    // //////Serial.print("max:");
    // //////Serial.print(mx[typ], 7);
    // //////Serial.print(",");
    //     //////Serial.print("mi_");
    //     //////Serial.print(mi[typ],7);
    // Serial.print("peak:");
    // Serial.println(peak[typ], 7);
    // Serial.println(",");


    // Serial.print("rms:");
    // Serial.println(rms[typ], 5);

    // //////Serial.println();
    } 
    // else {
  //      //////Serial.print("VOLT_ ");
  //   }

  //        //////Serial.print("output_");
  //       //////Serial.print(outputValue[typ],7);
  //   //////Serial.print("max:");
  //   //////Serial.print(mx[typ], 7);
  //   //////Serial.print(",");
  //       //////Serial.print(" mi_");
  //       //////Serial.print(mi[typ],7);
  //   //////Serial.print("peak:");
  //   //////Serial.print(peak[typ], 7);//////Serial.print(",");


  //   //////Serial.print("rms:");
  //   //////Serial.println(rms[typ], 5);
  }
}

void whileCurrent()
{
  select =2;
  measuremaxmin();
  if  (outputValue[1] > ctmxLt)  // Oc
  {
    Serial.println(" In OC if");
    countTime = millis();
    bool whileCurrentst =true;
    unsigned short currentripcount = 10;
    unsigned long  octime = millis();
    unsigned short    octimeout= 30;
    while (whileCurrentst) 
    {
      select =2;
      measuremaxmin();

      //      voltage check
          if (((tp == 'N') || (tp == '1')) && (outputValue[0] > mxLt))  // OV
      {
        //////Serial.println(" In OV if");
        countTime = millis();
        whileCon('O', 100, 1);
      } else if (((tp == 'N') || (tp == '1')) && (peak[0] < miLt))  // UV
      {
        //////Serial.println("In UV if");
        countTime = millis();
        whileCon('U', 200, 1);
      }

      //current check
      if (outputValue[1] > ctmxLt) {
      currentripcount--;
      // Serial.println(outputValue[1]);
      Serial.println(currentripcount);
      }

      if(!currentripcount)
      {
        whileCurrentst = false;
        digitalWrite(relay, LOW);
        digitalWrite(led[1], LOW);
          digitalWrite(led[0], HIGH);
        //////Serial.println("Realy trip");
        tp = 'Y';
        delay(500);
      }
      else if((millis() - octime ) > octimeout)
      {
        whileCurrentst = false;
       
      }

    }
  } 
  
}



