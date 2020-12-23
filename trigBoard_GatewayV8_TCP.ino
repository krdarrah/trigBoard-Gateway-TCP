//ESP32 Gateway System from UDP

//#include "includes.h"
#include "gatewayIncludes.h"//special stuff needed just for this setup, mp3 player and pins

const char fwVersion[] = "12/23/20 GATEWAY TCP";

void setup() {
  pinMode(LEDpin, OUTPUT);
  digitalWrite(LEDpin, HIGH);
  Serial.begin(115200);
  particle.begin(9600);
  delay(500);

  pinMode(0, OUTPUT);
  pinMode(silentButtonPin, INPUT_PULLUP);

  initAP();
}

void loop() {
}
