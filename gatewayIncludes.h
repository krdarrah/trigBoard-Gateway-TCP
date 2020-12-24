
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
AsyncWebServer server(80);
const int silentButtonPin = 12;
const int LEDpin = 0;//output

#include <SoftwareSerial.h>//for sending data to particle
const int particleRX = 33;
const int particleTX = 32;
//SoftwareSerial particle(particleRX, particleTX);//rx tx
SoftwareSerial particle;
//this compiled after installing EspSoftwareSerial library


//functions
void initAP();

//globals

int duplicateCount = 0;
