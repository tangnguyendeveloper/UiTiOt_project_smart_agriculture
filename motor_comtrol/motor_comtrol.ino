
#include <SoftwareSerial.h>

#define RELAY_PIN 4

SoftwareSerial LoRa(12, 13);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  LoRa.begin(9600);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (LoRa.available()){
    String frame = LoRa.readStringUntil('\n');
    if (frame == "motor|1") digitalWrite(RELAY_PIN, LOW);
    if (frame == "motor|0") digitalWrite(RELAY_PIN, HIGH);
    if (frame != "") Serial.println(frame);
  }
}
