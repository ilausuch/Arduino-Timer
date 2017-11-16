#include <ilTimer.h>

void onTimer1(byte t){
  Serial.print("End timeout ");
  Serial.println(t);
}

void onTimer2(byte t){
  static unsigned long last=millis();
  Serial.print("End interval ");
  Serial.print(t);
  Serial.print(" elapsed ");
  Serial.println(millis()-last);
  last=millis();
}

void onTimer3(byte t){
  static int ms=500;
  Serial.print("End manual ");
  Serial.print(t);

  ms=ms-100;
  if (ms==0){
    Serial.println(" end ");
  }
  else{
    Serial.print(" reset ");
    Serial.println(ilTimerControl.reset(t,ms));
  }

}

void setup() {
  Serial.begin(9600);
  Serial.println("Init");

  Serial.print("Start timeout ");
  Serial.println(ilTimerControl.timeout(onTimer1,1000));
  Serial.print("Start interval ");
  Serial.println(ilTimerControl.interval(onTimer2,1000));
  Serial.print("Start manual ");
  Serial.println(ilTimerControl.manual(onTimer3,2500));
}


void loop() {
   ilTimerControl.check();
}
