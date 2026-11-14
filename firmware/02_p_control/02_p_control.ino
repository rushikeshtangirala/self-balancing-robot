// Stage 2: proportional (P) control with motor dead-zone compensation
// ESP32 + MPU6050 + TB6612 motor driver

#include <Wire.h>
#include <MPU6050_light.h>

MPU6050 mpu(Wire);

// TB6612 Pins
#define PWMA 25
#define AIN1 27
#define AIN2 26

#define PWMB 14
#define BIN1 33
#define BIN2 32

#define STBY 4

// Adjust after checking upright angle
float setPoint = 0.0;

// Start with this
float Kp = 15.0;

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

  float angle = mpu.getAngleY();

  float error = setPoint - angle;

  int pwm = abs(error) * Kp;

  pwm = constrain(pwm, 0, 255);

  // Motor dead zone compensation
  if(pwm > 0 && pwm < 90)
    pwm = 90;

  // Stop near balance point
  if(abs(error) < 1.5)
    pwm = 0;

  if(error > 0)
  {
    // Direction 1
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);

    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
  }
  else
  {
    // Direction 2
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);

    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
  }

  ledcWrite(PWMA, pwm);
  ledcWrite(PWMB, pwm);

  Serial.print("Angle: ");
  Serial.print(angle);
  Serial.print(" PWM: ");
  Serial.println(pwm);

  delay(5);
}
