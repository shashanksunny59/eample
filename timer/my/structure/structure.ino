struct timer
{int onTimeMin,onTimeHr,offTimeMin,offTimeHr, dayOption; } timer_[3];
struct realTimeClock{
  int Temp, Hr, Min, Date, Mon, Year;
  String Day;}rt;
void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
timer_[0].onTimeMin = 50;timer_[0].onTimeHr = 50;
timer_[0].offTimeMin = 50;timer_[0].offTimeHr = 50;
timer_[1].onTimeMin = 51;timer_[1].onTimeHr = 50;
timer_[1].offTimeMin = 51;timer_[1].offTimeHr = 50;
//
rt.Temp = 24;
rt.Day = "Monday";
rt.Hr = 22; rt.Min = 50; 
rt.Date = 23; rt.Mon = 05; rt.Year = 2020;
Serial.println(timer_[0].onTimeMin);
Serial.println(timer_[1].onTimeMin);
Serial.println(String(rt.Date)+String("/")+String(rt.Mon)+String("/")+String(rt.Year));
Serial.println(String(rt.Hr)+String(":")+String(rt.Min));
Serial.println(rt.Day);
}

void loop() {
  // put your main code here, to run repeatedly:
}
