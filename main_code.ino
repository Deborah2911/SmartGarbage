#include <Servo.h>

Servo myservo;
int pos_servo = 0;
int opened = 0;
int led_on = 0;

const int buzzer = 8;
const int trig_pin = 6;
const int echo_pin = 5;

int photomodule = A2;
int led = 12;
int threshold = 900;
int laser = 13;

unsigned long start_millis = 0;
unsigned long current_millis;
const unsigned long period = 3000;

void setup() {
  pinMode(echo_pin, INPUT);
  pinMode(trig_pin, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(photomodule, INPUT);
  pinMode(laser, OUTPUT);
  pinMode(led, OUTPUT);

  digitalWrite(laser, LOW);
  digitalWrite(trig_pin, LOW);
  digitalWrite(buzzer, LOW);
  digitalWrite(led, LOW);

  noTone(buzzer);

  myservo.attach(9);
  Serial.begin(9600);

  start_millis = millis();
  myservo.write(0);
}

void openServo() {
  if (pos_servo <= 0) {
    for (pos_servo = 0; pos_servo <= 90; pos_servo += 1) {
      myservo.write(pos_servo);
      delay(5);
    }
    delay(500);
  }
}

void closeServo() {
  if (pos_servo >= 90) {
    for (pos_servo = 90; pos_servo >= 0; pos_servo -= 1) {
      myservo.write(pos_servo);
      delay(5);
    }
    delay(500);
  }
}

float readDistance() {
  digitalWrite(trig_pin, LOW);
  delay(2);

  digitalWrite(trig_pin, HIGH);
  delay(10);
  digitalWrite(trig_pin, LOW);

  float timing = pulseIn(echo_pin, HIGH);
  float distance = (timing * 0.034) / 2;

  return distance;
}

void loop() {
  current_millis = millis();
  float distance = readDistance();

  if (distance > 0 && distance <= 30) {
    start_millis = current_millis;

    if (opened == 0) {
      digitalWrite(laser, HIGH);
      delay(500);
      int read = analogRead(photomodule);
      delay(100);
      digitalWrite(laser, LOW);

      if (read < threshold) {
        if (led_on == 0) {
          tone(buzzer, 440);
          delay(1000);
          noTone(buzzer);
        }
        digitalWrite(led, HIGH);
        led_on = 1;
      } else {
        digitalWrite(led, LOW);
        led_on = 0;
        openServo();
        opened = 1;
      }
    }
  } else {
    if (opened == 1) {
      if (current_millis - start_millis >= period) {
        float distance2 = readDistance();
        if (distance2 > 0 && distance2 <= 30) {
          start_millis = current_millis;
        } else {
          closeServo();
          opened = 0;
          digitalWrite(led, LOW);
          digitalWrite(laser, LOW);
        }
      }
    }
  }
}
