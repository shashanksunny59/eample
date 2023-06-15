String op(int i);
void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
/*for(int i=0;i <=15;i++)
{Serial.println(i);
String temp = op(i);
  Serial.println(temp);
  Serial.println(temp.length());
  }*/
  String temp = op(1);
  String Day ="Sun";
  int rept = 0;
  Serial.println(temp);
  //Serial.println(String(" length ")+String(temp.length()));
 Serial.println( temp.indexOf(Day));
 if(( temp.indexOf(Day) >= 0)&& (23 >temp.indexOf(Day)))
    {
      Serial.println(Day);
      }
/*for (int x = 0; x <= temp.length()/3;x++)
 {
  Serial.println(temp.indexOf((0+3),(3+x)));
  }
 */
  
}

void loop() {
  // put your main code here, to run repeatedly:

  }

 String op (int num)
  {Serial.println("in");
String list[15] ;
list[0] = {"SunMonTueWedThuFriSat"};
list[1] = {"MonTueWedThuFriSat"};
list[2] = {"MonTueWedThuFri"};
list[3] = {"SunMonWedThuFri"};
list[4] = {"SunTueThuSat"};
list[5] = {"SunMonWedFri"};
list[6] = {"MonTueWed"};
list[7] = {"ThuFriSat"};
list[8] = {"SunSat"};
list[9] = {"Sun"};
list[10] = {"Mon"};
list[11] = {"Tue"};
list[12] = {"Wed"};
list[13] = {"Thu"};
list[14] = {"Fri"};
list[15] = {"Sat"};
return list[num]; 
   
  }
