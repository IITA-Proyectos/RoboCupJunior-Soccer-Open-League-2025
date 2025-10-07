#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

// Definicion de Motores
#define INA1 2
#define INB1 5
#define PWM1 3

#define INA2 8
#define INB2 7
#define PWM2 6

#define INA3 11
#define INB3 12
#define PWM3 4

// Definicion de Sensores de Luz
#define linepin1 A11
#define linepin2 A13
#define linepin3 A12


Adafruit_BNO055 bno = Adafruit_BNO055(55);
sensors_event_t event;
float correccion;
float error = 0;
float initialYaw = 0;
float currentYaw;
float kp = 0.45;
float kd = 0.9; // derivative gain to damp oscillations
float lastError = 0;
unsigned long lastTime = 0;
// Which motor is the physical 'front' wheel? Set to 1, 2 or 3 depending on your robot wiring.
// If you're not sure, try values until forward/back bias matches the wheel you want.
#define FRONT_MOTOR 1
// Small PWM bias added to the front wheel during strafes to counteract mechanical drift.
int frontWheelBias = 10; // tune this (positive adds power to front wheel)
// Motor base PWM limits
const int PWM_MAX = 100;
const int PWM_MIN = 0;

// Timing for strafes (ms)
const unsigned long STRAFE_TIME = 2000;

void setup(void) {
Serial.begin(115200);

  // Configure motor pins
  pinMode(INA1, OUTPUT);
  pinMode(INB1, OUTPUT);
  pinMode(PWM1, OUTPUT);

  pinMode(INA2, OUTPUT);
  pinMode(INB2, OUTPUT);
  pinMode(PWM2, OUTPUT);

  pinMode(INA3, OUTPUT);
  pinMode(INB3, OUTPUT);
  pinMode(PWM3, OUTPUT);

	if (!bno.begin()) {
		Serial.println("Failed to initialize BNO055! Check your wiring.");
		while (1);
	}
	bno.setExtCrystalUse(true);
	Serial.println("BNO055 initialized.");

  // Give sensor a moment and read initial heading
  delay(500);
  bno.getEvent(&event);
  // We'll use event.orientation.x as yaw (may need to adjust depending on board mounting)
  initialYaw = 0;
  Serial.print("Initial Yaw: "); Serial.println(initialYaw);
}
//funciones de movimiento
void avance() {
  digitalWrite(INA3, 0);
  digitalWrite(INB3, 1);
  analogWrite(PWM3, 100);

  digitalWrite(INA2, 1);
  digitalWrite(INB2, 0);
  analogWrite(PWM2, 100);

  digitalWrite(INA1, 0);
  digitalWrite(INB1, 1);
  analogWrite(PWM1, 0);
}

void retroceso() {
  digitalWrite(INA3, 1);
  digitalWrite(INB3, 0);
  analogWrite(PWM3, 100);

  digitalWrite(INA2, 0);
  digitalWrite(INB2, 1);
  analogWrite(PWM2, 100);

  digitalWrite(INA1, 0);
  digitalWrite(INB1, 1);
  analogWrite(PWM1, 0);
}
void izquierda() {
  digitalWrite(INA3, 0);
  digitalWrite(INB3, 1);
  analogWrite(PWM3, 50);

  digitalWrite(INA2, 0);
  digitalWrite(INB2, 1);
  analogWrite(PWM2, 80);

  digitalWrite(INA1, 1);
  digitalWrite(INB1, 0);
  analogWrite(PWM1, 100);
}

void derecha() {
  digitalWrite(INA3, 1);
  digitalWrite(INB3, 0);
  analogWrite(PWM3, 80);

  digitalWrite(INA2, 1);
  digitalWrite(INB2, 0);
  analogWrite(PWM2, 50);

  digitalWrite(INA1, 0);
  digitalWrite(INB1, 1);
  analogWrite(PWM1, 100);
}

void stopMotors() {
  analogWrite(PWM1, 0);
  analogWrite(PWM2, 0);
  analogWrite(PWM3, 0);
}

// Apply yaw correction by slightly changing wheel PWMs.
// correction is in degrees; positive means rotate right, so we need to counter-rotate.
int computeCorrectionPWM(float correction)
{
  // PD controller: uses proportional + derivative on the yaw error
  unsigned long now = millis();
  float dt = 0.0;
  if (lastTime != 0) dt = (now - lastTime) / 1000.0; // seconds
  float derivative = 0.0;
  if (dt > 0.0) {
    derivative = (correction - lastError) / dt;
  }

  float out = kp * correction + kd * derivative;
  int idelta = (int)round(out * 2.0); // scale a bit
  if (idelta > 80) idelta = 80;
  if (idelta < -80) idelta = -80;

  lastError = correction;
  lastTime = now;
  return idelta;
}

// Strafe left while holding heading using P controller
void strafeLeftHold(unsigned long durationMs)
{
  unsigned long start = millis();
  while (millis() - start < durationMs) {
    bno.getEvent(&event);
  currentYaw = event.orientation.x;
    // shortest angle difference
    float diff = initialYaw -;
    if (diff > 180) diff -= 360;
    if (diff < -180) diff += 360;
    int corr = computeCorrectionPWM(diff);

    // Base PWMs for left strafe
    int p1 = 100; // motor1
    int p2 = 80;  // motor2
    int p3 = 50;  // motor3

    // Apply correction: to rotate counter to yaw error
    // If robot yaw is positive (turned right), we need to rotate left -> reduce right-driving motors and increase left-driving motors
    p1 = constrain(p1 - corr, PWM_MIN, PWM_MAX);
    p2 = constrain(p2 + corr, PWM_MIN, PWM_MAX);
    p3 = constrain(p3 - corr, PWM_MIN, PWM_MAX);

  // Apply a small front wheel bias to counter mechanical drift
  if (FRONT_MOTOR == 1) p1 = constrain(p1 + frontWheelBias, PWM_MIN, PWM_MAX);
  else if (FRONT_MOTOR == 2) p2 = constrain(p2 + frontWheelBias, PWM_MIN, PWM_MAX);
  else if (FRONT_MOTOR == 3) p3 = constrain(p3 + frontWheelBias, PWM_MIN, PWM_MAX);

    // Set directions for strafing left (use existing izquierda() pattern but with PWMs overridden)
    digitalWrite(INA3, 0);
    digitalWrite(INB3, 1);
    analogWrite(PWM3, p3);

    digitalWrite(INA2, 0);
    digitalWrite(INB2, 1);
    analogWrite(PWM2, p2);

    digitalWrite(INA1, 1);
    digitalWrite(INB1, 0);
    analogWrite(PWM1, p1);

    // telemetry
    Serial.print("Yaw: "); Serial.print(currentYaw);
    Serial.print(" err: "); Serial.print(diff);
    Serial.print(" corrPWM: "); Serial.println(corr);

    delay(50);
  }
  stopMotors();
}

// Strafe right while holding heading using P controller
void strafeRightHold(unsigned long durationMs)
{
  unsigned long start = millis();
  while (millis() - start < durationMs) {
    bno.getEvent(&event);
  currentYaw = event.orientation.x;
    float diff = currentYaw - initialYaw;
    if (diff > 180) diff -= 360;
    if (diff < -180) diff += 360;
    int corr = computeCorrectionPWM(diff);

    // Base PWMs for right strafe
    int p1 = 100; // motor1
    int p2 = 50;  // motor2
    int p3 = 80;  // motor3

    // Apply correction
    p1 = constrain(p1 + corr, PWM_MIN, PWM_MAX);
    p2 = constrain(p2 - corr, PWM_MIN, PWM_MAX);
    p3 = constrain(p3 + corr, PWM_MIN, PWM_MAX);

  // Apply a small front wheel bias to counter mechanical drift
  if (FRONT_MOTOR == 1) p1 = constrain(p1 + frontWheelBias, PWM_MIN, PWM_MAX);
  else if (FRONT_MOTOR == 2) p2 = constrain(p2 + frontWheelBias, PWM_MIN, PWM_MAX);
  else if (FRONT_MOTOR == 3) p3 = constrain(p3 + frontWheelBias, PWM_MIN, PWM_MAX);

    // Set directions for strafing right (use existing derecha() pattern but with PWMs overridden)
    digitalWrite(INA3, 1);
    digitalWrite(INB3, 0);
    analogWrite(PWM3, p3);

    digitalWrite(INA2, 1);
    digitalWrite(INB2, 0);
    analogWrite(PWM2, p2);

    digitalWrite(INA1, 0);
    digitalWrite(INB1, 1);
    analogWrite(PWM1, p1);

    // telemetry
    Serial.print("Yaw: "); Serial.print(currentYaw);
    Serial.print(" err: "); Serial.print(diff);
    Serial.print(" corrPWM: "); Serial.println(corr);

    delay(50);
  }
  stopMotors();
}



void loop(void) 
{  
  // Main routine: strafe left for STRAFE_TIME while holding heading,
  // then stop 1s, strafe right for STRAFE_TIME, then stop 1s, repeat.
  strafeLeftHold(STRAFE_TIME);
  delay(1000);
  strafeRightHold(STRAFE_TIME);
  delay(1000);
}
