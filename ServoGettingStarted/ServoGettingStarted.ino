#include <Servo.h>
#define POT A0

Servo m1;

void setup() {
  m1.attach(6);
  Serial.begin(9600);
  delay(50);
  Serial.println("Serial ready:");
  arm(m1);
}

void loop() {
  int potValue = analogRead(POT);
  int s = map(potValue, 60, 930, 1200, 1700); //Map the pot values to motor values

  m1.write(s);
  Serial.println(s);
}

void arm(Servo m) {
  Serial.print("Arming.");
  m.write(0);
  delay(100);
  m.write(1224); //A value at which the motor starts turning
  delay(2000);
  m.write(1024); //A value at which the motor stands still
  delay(3000);
  Serial.println(" Armed!");
}
