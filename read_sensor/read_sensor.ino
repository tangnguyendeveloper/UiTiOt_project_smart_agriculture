
#include <Wire.h>
#include <Adafruit_AM2315.h>

#define SOIL_PIN A0

Adafruit_AM2315 am2315;

float read_soil(){
  float soil = 0;
  for (int i = 0; i < 100; i++) {
    soil += analogRead(SOIL_PIN);
    delay(2);
  }
  return soil/100.0;
}

void setup(){

  Serial.begin(9600);
  while (!Serial) {
    delay(10);
  }
  pinMode(SOIL_PIN, INPUT);

  // Initialize the I2C bus (BH1750 library doesn't do this automatically)
  Wire.begin();

  // begin() does a test read, so need to wait 2secs before first read
  delay(2000);

}

void loop() {

  float soil_value = read_soil();
  soil_value = 100.0 - map(soil_value, 0, 1024, 0, 100);

  float temperature = 0, humidity = 0;

  if (! am2315.readTemperatureAndHumidity(&temperature, &humidity)) {
    return;
  }
  Serial.print("S:"); Serial.print(soil_value);Serial.print("|");
  Serial.print("T:"); Serial.print(temperature);Serial.print("|");
  Serial.print("H:"); Serial.print(humidity);Serial.print("e");
  delay(4000);
}
