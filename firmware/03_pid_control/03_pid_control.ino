// Stage 3: full PID control
// ESP32 + MPU6050 (tockn library, complementary-filtered angle)
// Pin names follow the IN1-IN4 / ENA-ENB convention (see README wiring table).

#include <Wire.h>
#include <MPU6050_tockn.h>

MPU6050 mpu(Wire);

float Kp = 25;
float Ki = 0.8;
float Kd = 1.2;

float error;
float previousError;
float integral;
float derivative;

float setpoint = 0;

#define IN1 25
#define IN2 26
#define IN3 27
#define IN4 14

#define ENA 32
#define ENB 33

void setup()
{
  Serial.begin(115200);

  Wire.begin(21,22);

  mpu.begin();
  mpu.calcGyroOffsets(true);

  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);

  ledcAttach(ENA,1000,8);
  ledcAttach(ENB,1000,8);
}

void loop()
{
  mpu.update();

  float angle = mpu.getAngleY();

  error = setpoint - angle;

  integral += error;

  derivative = error - previousError;

  float output =
      Kp*error +
      Ki*integral +
      Kd*derivative;

  previousError = error;

  int speed = constrain(abs(output),0,255);

  if(output > 0)
  {
      digitalWrite(IN1,HIGH);
      digitalWrite(IN2,LOW);

      digitalWrite(IN3,HIGH);
      digitalWrite(IN4,LOW);
  }
  else
  {
      digitalWrite(IN1,LOW);
      digitalWrite(IN2,HIGH);

      digitalWrite(IN3,LOW);
      digitalWrite(IN4,HIGH);
  }

  ledcWrite(ENA,speed);
  ledcWrite(ENB,speed);

  delay(5);
}
