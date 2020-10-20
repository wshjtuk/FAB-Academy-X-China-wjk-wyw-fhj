#define _FRONT_DISTANCE_SENSOR_PIN A3
#define _DIRECTION_INPUT A0
#define _FIRE_PIN 2
#define _BREAK_PIN 3
#define _BREAK_LED 12
#define _DASHBOARD_POINTER 9
#define _SOUND_SPEAKER 6
#define _WHEELL 11
#include <Servo.h>
Servo dashboard;
Servo wheel;
int _speed = 0;
int _angle = 0;
void setup() {
  dashboard.attach(9);
  wheel.attach(11);
  pinMode(A3, INPUT );
  pinMode(A0, INPUT );
  pinMode(2,  INPUT );
  pinMode(3,  INPUT );
  pinMode(12, OUTPUT);
  pinMode(6,  OUTPUT);
  pinMode(9,  OUTPUT);
  pinMode(11, OUTPUT);
  //Serial.begin(115200);
}

void loop() {
  digitalWrite(12, digitalRead(3));
  _speed+=digitalRead(2)*6-3-digitalRead(3)*10;
  if(_speed < 1) {_speed = 1;}
  if(_speed > 179) {_speed = 179;}
  dashboard.write(180-_speed);
  tone(6, 1022, 100 * (int)(analogRead(A3)<100));
  int inverse = -1;
  for(int i=0; i<50;++i) {
    wheel.write(digitalRead(2)-digitalRead(3)+(int)(inverse*_speed/15));
    inverse *= -1; delay(2);
  }
}
