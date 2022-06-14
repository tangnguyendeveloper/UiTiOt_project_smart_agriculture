
#include <SoftwareSerial.h>
#include <BH1750.h>
#include <Wire.h>
#include <TimerEvent.h>

//Line Tracking IO define
#define LT_R !digitalRead(10)
#define LT_M !digitalRead(4)
#define LT_L !digitalRead(2)
//motor contol define
#define ENA 5
#define ENB 6
#define IN1 7
#define IN2 8
#define IN3 9
#define IN4 11
//motor speed define
#define carSpeed 250
#define CarLow 150

//rx 6  tx 13
SoftwareSerial LoRa(12, 13);
int lux_value = 0;
BH1750 lightMeter(0x23);
int is_stop = 0;

const unsigned int timer_line_tracking = 50;
const unsigned int timer_line_send_data = 5000;

TimerEvent thread_line_tracking;
TimerEvent thread_line_send_data;

float get_lux();
void forward();
void back();
void left();
void right();
void stop();
void line_tracking();
void send_data();



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  LoRa.begin(9600);
  Wire.begin();

  pinMode(10,INPUT);
  pinMode(4,INPUT);
  pinMode(2,INPUT);

  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println(F("BH1750 Advanced begin"));
  } else {
    Serial.println(F("Error initialising BH1750"));
  }

  randomSeed( (unsigned long)( micros()%millis() ) );
  thread_line_tracking.set(timer_line_tracking, line_tracking);
  thread_line_send_data.set(timer_line_send_data, send_data);

}

void loop() {
  // put your main code here, to run repeatedly
  thread_line_tracking.update();
  thread_line_send_data.update();
  
}

float get_lux(){
  float lux = 0;
  if (lightMeter.measurementReady()) {
    lux = lightMeter.readLightLevel();
  }
  return lux;
}

void forward(){
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void back(){
  analogWrite(ENA, CarLow);
  analogWrite(ENB, CarLow);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void left(){
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void right(){
  analogWrite(ENA, CarLow);
  analogWrite(ENB, CarLow);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW); 
} 

void stop(){
   digitalWrite(ENA, LOW);
   digitalWrite(ENB, LOW);
}

void line_tracking(){
  if (is_stop) { stop(); return;} 

  if(LT_M){
    forward();
  }
  else if(LT_R) { 
    right();                          
  }   
  else if(LT_L) {
    left();
  }
}

void send_data(){
  is_stop = 1; 
  stop();

  String msg = "";
  lux_value = get_lux();

  String frame = Serial.readStringUntil('e');
  if (frame != "") {
    msg = "L:"+String(lux_value)+"|"+frame;
    LoRa.println(msg.c_str());
  }

  delay(3000);
  is_stop = 0; 
  line_tracking();
}