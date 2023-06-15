char* onesToTens(unsigned short* num);

void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
String tx;
while(Serial.available()){
 tx = Serial.readString();
  }
  Serial.println(tx);
  unsigned short a = tx.toInt();
  //char* rt;
  //rt = onesToTens(&a);
 Serial.print("retuned value is ");
 
  Serial.println(onesToTens(&a));
  delay(1000);
}

char* onesToTens(unsigned short* num)
{// Serial.print(" in onesToTens ");
  char* temp;
  temp=(char*)malloc(3);
  if(*num < 10){
    
    temp[0]='0';
    temp[1]= *num+'0';
    temp[2]='\0';
   // Serial.println(*num);
    Serial.println(temp);
    return  temp;
    } 
itoa(*num, temp, 10);
  return (char*)temp ;
    }
 
