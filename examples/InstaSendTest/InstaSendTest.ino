#include <Arduino.h>

#define SKETCH_NAME "Funkbus"
#define SKETCH_VERSION 1.00

#include "InstaFunkbus.h"

int snd = 4;
int led = 13;
InstaFunkbus Funkbus;

void setup() {
  Serial.begin(115200);
  Funkbus.enableSending(snd, "0010", "1100", "0101111101001110");
  Funkbus.setState("1010");
  pinMode(led, OUTPUT);
  Serial.println(" Setup ");
}

void loop() {
  for ( int i = 1; i < 8; i++) {
    delay(3000);
    digitalWrite(led, HIGH);
    Funkbus.SendSwitchOn('A', i);
    digitalWrite(led, LOW);
    delay(3000);
    digitalWrite(led, HIGH);
    Funkbus.SendSwitchOff('A', i);
    digitalWrite(led, LOW);
    delay(6000);
  }

  digitalWrite(led, HIGH);
  Funkbus.SendSwitchOnLong('B', 1);
  digitalWrite(led, LOW);
  delay(6000);
  digitalWrite(led, HIGH);
  Funkbus.SendSwitchOffLong('B', 1);
  digitalWrite(led, LOW);
  delay(6000);
}
