# Self-Balancing Two-Wheel Robot

A two-wheel **inverted-pendulum robot** that stays upright using IMU feedback and closed-loop control on an **ESP32**.
I built it during my **Robotics Engineering internship at Makers Hive Innovations (2026)**.

The repo keeps all three controller versions, because how the controller evolved is the interesting part:

| Stage | Folder | Controller | What it taught me |
|---|---|---|---|
| 1 | `firmware/01_bang_bang` | On/off: full speed when tilt > ±5° | Proves the sensing and motor directions, but oscillates hard around upright |
| 2 | `firmware/02_p_control` | Proportional, Kp = 15, with motor dead-zone compensation | Speed now scales with tilt; small corrections stall unless PWM is lifted past the motor dead zone |
| 3 | `firmware/03_pid_control` | Full PID, Kp = 25, Ki = 0.8, Kd = 1.2 | Integral removes steady lean, derivative damps the overshoot |

![ESP32](https://img.shields.io/badge/ESP32-E7352C?logo=espressif&logoColor=white)
![Arduino](https://img.shields.io/badge/Arduino%20C%2B%2B-00878F?logo=arduino&logoColor=white)
![Control](https://img.shields.io/badge/Control-PID-1f3a68)

---

## Control loop

```
 setpoint 0°        error e            u = Kp·e + Ki·Σe + Kd·Δe
     ──────▶ ( Σ ) ───────▶ [   PID   ] ──── PWM + direction ───▶ [ motor driver ] ──▶ [ DC geared motors ]
               ▲ −                                                                         │
               │                                                                           │ body tilt
               │  tilt angle θ                                                             ▼
               └──────── [ complementary filter ] ◀──── accel + gyro ◀──── [ MPU6050 IMU ]
```

- **Sensing:** the MPU6050's accelerometer is noisy and its gyro drifts. The MPU6050 library fuses the two with a complementary filter, which gives a clean tilt angle every loop.
- **Loop rate:** about 5 ms per iteration (≈200 Hz) in stages 2–3.
- **Output:** the sign of the controller output picks the wheel direction, and |output| (clamped to 0–255) sets the PWM duty at 1 kHz, 8-bit.

### Stage 2 details: dead-zone compensation

DC geared motors don't turn at all below a certain PWM. Stage 2 lifts any non-zero command up to **PWM 90**, so small corrections actually move the wheels.
It also cuts output inside a **±1.5° band** so the robot doesn't chatter when it's already balanced.

## Hardware

| Part | Notes |
|---|---|
| ESP32 dev board | Controller, 1 kHz LEDC PWM |
| MPU6050 | 6-axis IMU over I²C |
| TB6612FNG motor driver | Dual H-bridge |
| 2 × DC geared motors + wheels | Differential drive, both driven together for balancing |
| Battery pack | Motor supply |

## Wiring

**MPU6050 (all stages)**

| ESP32 | MPU6050 |
|---|---|
| 3V3 | VCC |
| GND | GND |
| GPIO 21 | SDA |
| GPIO 22 | SCL |

**TB6612 (stages 1 and 2)**

| ESP32 | TB6612 | Function |
|---|---|---|
| GPIO 25 | PWMA | Left motor speed |
| GPIO 27 | AIN1 | Left motor direction |
| GPIO 26 | AIN2 | Left motor direction |
| GPIO 14 | PWMB | Right motor speed |
| GPIO 33 | BIN1 | Right motor direction |
| GPIO 32 | BIN2 | Right motor direction |
| GPIO 4 | STBY | Driver enable (held HIGH) |

**Stage 3 pin map:** the PID sketch uses IN1–IN4 / ENA–ENB names: IN1 = GPIO 25, IN2 = GPIO 26, IN3 = GPIO 27, IN4 = GPIO 14,
ENA = GPIO 32, ENB = GPIO 33. Match these to your driver's direction and enable inputs before flashing.

## Getting started

1. Install the **ESP32 board package** in the Arduino IDE (core 3.x, which provides `ledcAttach`).
2. Install the IMU library for the stage you're flashing: **MPU6050_light** (stages 1–2) or **MPU6050_tockn** (stage 3).
3. Flash the sketch and **hold the robot upright** during the start-up calibration.
4. Open the Serial Monitor at **115200 baud** to watch the angle and PWM while you tune.

### Tuning order

1. Ki = Kd = 0. Raise **Kp** until the robot catches itself but oscillates.
2. Add **Kd** until the oscillation damps out.
3. Add a small **Ki** to remove any steady lean caused by an off-centre centre of mass.

## Next improvements

- Clamp the integral term (anti-windup) so a long fall doesn't saturate the controller.
- Divide the derivative by the measured loop time (`dt`) so the gains don't depend on loop speed.
- Add wheel encoders for position hold, so the robot doesn't drift while it balances.

## Author

**Rushikesh Tangirala**, Mechanical Engineering, VIT-AP University
[LinkedIn](https://www.linkedin.com/in/rushikesh-tangirala) · [Portfolio](https://claude.ai/artifact/NXxNxrRZJV7LoAYz6ibR6o)
