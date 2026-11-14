// Stage 1: bang-bang (on/off) balancing
// ESP32 + MPU6050 + TB6612 motor driver
// Drives the motors at a fixed speed whenever the tilt leaves a +/-5 degree band.

#include <Wire.h>
#include <MPU6050_light.h>

MPU6050 mpu(Wire);

// TB6612
#define PWMA 25
#define AIN1 27
#define AIN2 26

#define PWMB 14
#define BIN1 33
#define BIN2 32

#define STBY 4

void setup() {

  Serial.begin(115200);

  Wire.begin(21,22);

  byte status = mpu.begin();

  if(status != 0){
    Serial.println("MPU ERROR");
    while(1);
  }

  delay(1000);

  Serial.println("Keep robot upright...");
  delay(2000);

  mpu.calcOffsets();

  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);

  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);

  pinMode(STBY, OUTPUT);
  digitalWrite(STBY, HIGH);

  ledcAttach(PWMA, 1000, 8);
  ledcAttach(PWMB, 1000, 8);

  Serial.println("Ready");
}

void loop() {

  mpu.update();

  float angle = mpu.getAngleX();   // X-axis balancing

  Serial.println(angle);

  if(angle > 5) {

    // Forward

    digitalWrite(AIN1,HIGH);
    digitalWrite(AIN2,LOW);

    digitalWrite(BIN1,HIGH);
    digitalWrite(BIN2,LOW);

    ledcWrite(PWMA,150);
    ledcWrite(PWMB,150);
  }

  else if(angle < -5) {

    // Reverse

    digitalWrite(AIN1,LOW);
    digitalWrite(AIN2,HIGH);

    digitalWrite(BIN1,LOW);
    digitalWrite(BIN2,HIGH);

    ledcWrite(PWMA,150);
    ledcWrite(PWMB,150);
  }

  else {

    ledcWrite(PWMA,0);
    ledcWrite(PWMB,0);
  }

  delay(10);
}
