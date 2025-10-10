#include <Arduino.h>
#include "MPU9250.h"

// MPU9250 object on I2C address 0x68
MPU9250 IMU(Wire, 0x68);
int status;

// Madgwick filter state
float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f; // quaternion
float beta = 0.08f; // filter gain


// gyro bias
float gbx = 0, gby = 0, gbz = 0;

// stationary detection / adaptive bias
int stationaryCounter = 0;
const int STATIONARY_REQUIRED = 5; // samples
const float GYRO_STATIONARY_THRESHOLD = 0.02f; // rad/s (~1.15 deg/s)
const float ACCEL_NORM_TOL = 0.05f; // g
bool isStationary = false;
const float GYRO_BIAS_ADAPT_ALPHA = 0.01f; // slow adaptation while stationary

// initial heading reference
float initialHeading = 0.0f;

// saved quaternion when stationary to freeze reported heading
float saved_q0 = 1.0f, saved_q1 = 0.0f, saved_q2 = 0.0f, saved_q3 = 0.0f;

// magnetometer min/max for simple hard-iron calibration
float mx_min = 1e6, my_min = 1e6, mz_min = 1e6;
float mx_max = -1e6, my_max = -1e6, mz_max = -1e6;

// helper: Madgwick update using rad/s for gyro, accel in g, mag arbitrary but consistent
void madgwickUpdate(float gx, float gy, float gz,
                    float ax, float ay, float az,
                    float mx, float my, float mz, float dt)
{
  // normalize accel
  float norm = sqrt(ax*ax + ay*ay + az*az);
  if (norm == 0.0f) return;
  ax /= norm; ay /= norm; az /= norm;

  // normalize mag
  norm = sqrt(mx*mx + my*my + mz*mz);
  if (norm == 0.0f) return;
  mx /= norm; my /= norm; mz /= norm;

  // Auxiliary variables
  float _2q0 = 2.0f * q0;
  float _2q1 = 2.0f * q1;
  float _2q2 = 2.0f * q2;
  float _2q3 = 2.0f * q3;
  float q0q0 = q0 * q0;
  float q1q1 = q1 * q1;
  float q2q2 = q2 * q2;
  float q3q3 = q3 * q3;

  // Reference direction of Earth's magnetic field
  float hx = 2.0f * (mx * (0.5f - q2q2 - q3q3) + my * (q1 * q2 - q0 * q3) + mz * (q1 * q3 + q0 * q2));
  float hy = 2.0f * (mx * (q1 * q2 + q0 * q3) + my * (0.5f - q1q1 - q3q3) + mz * (q2 * q3 - q0 * q1));
  float _2bx = sqrt(hx * hx + hy * hy);
  float _2bz = 2.0f * (mx * (q1 * q3 - q0 * q2) + my * (q2 * q3 + q0 * q1) + mz * (0.5f - q1q1 - q2q2));

  // Gradient descent algorithm corrective step (approximation)
  float s0 = 4.0f * q0 * (q1q1 + q2q2 - 0.5f) + 2.0f * q2 * (2.0f * q1 * q3 - 2.0f * q0 * q2 - ax) + 2.0f * q1 * (2.0f * q0 * q1 + 2.0f * q2 * q3 - ay) - _2bz * q2 * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1 * q3 - q0 * q2) - mx) + (-_2bx * q3 + _2bz * q1) * (_2bx * (q1 * q2 - q0 * q3) + _2bz * (q0 * q1 + q2 * q3) - my) + _2bx * q2 * (_2bx * (q0 * q2 + q1 * q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
  float s1 = 4.0f * q1 * (q0q0 + q1q1 - 0.5f) + 2.0f * q3 * (2.0f * q1 * q3 - 2.0f * q0 * q2 - ax) + 2.0f * q0 * (2.0f * q0 * q1 + 2.0f * q2 * q3 - ay) + (-_2bx * q2 + _2bz * q0) * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1 * q3 - q0 * q2) - mx) + (_2bx * q1 + _2bz * q3) * (_2bx * (q1 * q2 - q0 * q3) + _2bz * (q0 * q1 + q2 * q3) - my) + (_2bx * q0 - _2bz * q2) * (_2bx * (q0 * q2 + q1 * q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
  float s2 = 4.0f * q2 * (q0q0 + q2q2 - 0.5f) + 2.0f * q0 * (2.0f * q1 * q3 - 2.0f * q0 * q2 - ax) + 2.0f * q3 * (2.0f * q0 * q1 + 2.0f * q2 * q3 - ay) + (-_2bx * q3 + _2bz * q1) * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1 * q3 - q0 * q2) - mx) + (_2bx * q0 + _2bz * q2) * (_2bx * (q1 * q2 - q0 * q3) + _2bz * (q0 * q1 + q2 * q3) - my) + (_2bx * q1 - _2bz * q0) * (_2bx * (q0 * q2 + q1 * q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
  float s3 = 4.0f * q3 * (q0q0 + q3q3 - 0.5f) + 2.0f * q1 * (2.0f * q1 * q3 - 2.0f * q0 * q2 - ax) + 2.0f * q2 * (2.0f * q0 * q1 + 2.0f * q2 * q3 - ay) + (_2bx * q2) * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1 * q3 - q0 * q2) - mx) + (-_2bx * q1 + _2bz * q3) * (_2bx * (q1 * q2 - q0 * q3) + _2bz * (q0 * q1 + q2 * q3) - my) + (_2bx * q0) * (_2bx * (q0 * q2 + q1 * q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);

  // normalize step
  norm = sqrt(s0*s0 + s1*s1 + s2*s2 + s3*s3);
  if (norm == 0.0f) return;
  s0 /= norm; s1 /= norm; s2 /= norm; s3 /= norm;

  // rate of change of quaternion from gyroscope
  float qDot0 = 0.5f * (-q1 * gx - q2 * gy - q3 * gz) - beta * s0;
  float qDot1 = 0.5f * ( q0 * gx + q2 * gz - q3 * gy) - beta * s1;
  float qDot2 = 0.5f * ( q0 * gy - q1 * gz + q3 * gx) - beta * s2;
  float qDot3 = 0.5f * ( q0 * gz + q1 * gy - q2 * gx) - beta * s3;

  // integrate
  q0 += qDot0 * dt;
  q1 += qDot1 * dt;
  q2 += qDot2 * dt;
  q3 += qDot3 * dt;

  // normalize quaternion
  norm = sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
  q0 /= norm; q1 /= norm; q2 /= norm; q3 /= norm;
}

// quaternion -> yaw degrees (-180..180)
float getYawDeg() {
  float uq0 = q0, uq1 = q1, uq2 = q2, uq3 = q3;
  if (isStationary) {
    uq0 = saved_q0; uq1 = saved_q1; uq2 = saved_q2; uq3 = saved_q3;
  }
  float yaw = atan2f(2.0f*(uq0*uq3 + uq1*uq2), 1.0f - 2.0f*(uq2*uq2 + uq3*uq3));
  return yaw * RAD_TO_DEG;
}

// simple gyro bias calibration (robot must be stationary)
void calibrateGyroBias(int samples = 200) {
  float sx=0, sy=0, sz=0;
  for (int i=0;i<samples;i++) {
    IMU.readSensor();
    sx += IMU.getGyroX_rads();
    sy += IMU.getGyroY_rads();
    sz += IMU.getGyroZ_rads();
    delay(5);
  }
  gbx = sx / samples;
  gby = sy / samples;
  gbz = sz / samples;
  Serial.print("Gyro bias (rad/s): "); Serial.print(gbx); Serial.print(" "); Serial.print(gby); Serial.print(" "); Serial.println(gbz);
}

// simple magnetometer min/max collection (rotate robot during this period)
void collectMagMinMax(unsigned long ms_collect = 4000) {
  unsigned long t0 = millis();
  while (millis() - t0 < ms_collect) {
    IMU.readSensor();
    float mx = IMU.getMagX_uT();
    float my = IMU.getMagY_uT();
    float mz = IMU.getMagZ_uT();
    if (mx < mx_min) mx_min = mx;
    if (my < my_min) my_min = my;
    if (mz < mz_min) mz_min = mz;
    if (mx > mx_max) mx_max = mx;
    if (my > my_max) my_max = my;
    if (mz > mz_max) mz_max = mz;
    delay(20);
  }
  Serial.println("Mag min/max collected");
  Serial.print("mx_min mx_max: "); Serial.print(mx_min); Serial.print(" "); Serial.println(mx_max);
}

// apply hard-iron correction
void applyMagCorrection(float &mx, float &my, float &mz) {
  float offx = (mx_min + mx_max) / 2.0f;
  float offy = (my_min + my_max) / 2.0f;
  float offz = (mz_min + mz_max) / 2.0f;
  mx -= offx; my -= offy; mz -= offz;
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {}
  status = IMU.begin();
  if (status < 0) {
    Serial.println("IMU init unsuccessful");
    while (1) {}
  }
  IMU.setAccelRange(MPU9250::ACCEL_RANGE_8G);
  IMU.setGyroRange(MPU9250::GYRO_RANGE_500DPS);
  IMU.setDlpfBandwidth(MPU9250::DLPF_BANDWIDTH_20HZ);
  IMU.setSrd(19); // 50 Hz

  Serial.println("Calibrating gyro bias (stay still)...");
  calibrateGyroBias();

  Serial.println("Collecting magnetometer min/max: please rotate robot slowly for 6s...");
  collectMagMinMax(6000);

  // run a few Madgwick updates to settle
  unsigned long last = micros();
  for (int i=0;i<100;i++) {
    IMU.readSensor();
    float ax = IMU.getAccelX_mss() / 9.80665f; // convert m/s2 to g
    float ay = IMU.getAccelY_mss() / 9.80665f;
    float az = IMU.getAccelZ_mss() / 9.80665f;
    float gx = IMU.getGyroX_rads() - gbx;
    float gy = IMU.getGyroY_rads() - gby;
    float gz = IMU.getGyroZ_rads() - gbz;
    float mx = IMU.getMagX_uT();
    float my = IMU.getMagY_uT();
    float mz = IMU.getMagZ_uT();
    applyMagCorrection(mx,my,mz);
    unsigned long now = micros();
    float dt = (now - last) / 1e6f; if (dt <= 0) dt = 0.001f; last = now;
    madgwickUpdate(gx,gy,gz, ax,ay,az, mx,my,mz, dt);
    delay(10);
  }
  initialHeading = getYawDeg();
  Serial.print("Initial heading (deg): "); Serial.println(initialHeading);
}

unsigned long lastLoop = micros();
void loop() {
  unsigned long now = micros();
  float dt = (now - lastLoop) / 1e6f; if (dt <= 0) dt = 0.001f; lastLoop = now;

  IMU.readSensor();
  float ax = IMU.getAccelX_mss() / 9.80665f;
  float ay = IMU.getAccelY_mss() / 9.80665f;
  float az = IMU.getAccelZ_mss() / 9.80665f;
  float gx = IMU.getGyroX_rads() - gbx;
  float gy = IMU.getGyroY_rads() - gby;
  float gz = IMU.getGyroZ_rads() - gbz;
  float mx = IMU.getMagX_uT();
  float my = IMU.getMagY_uT();
  float mz = IMU.getMagZ_uT();
  applyMagCorrection(mx,my,mz);

  // Stationary detection: low angular rate and accel magnitude near 1g
  float angRate = sqrt(gx*gx + gy*gy + gz*gz);
  float accelNorm = sqrt(ax*ax + ay*ay + az*az);
  if (angRate < GYRO_STATIONARY_THRESHOLD && fabs(accelNorm - 1.0f) < ACCEL_NORM_TOL) {
    stationaryCounter++;
  } else {
    stationaryCounter = 0;
  }

  if (stationaryCounter >= STATIONARY_REQUIRED) {
    if (!isStationary) {
      isStationary = true;
      // save quaternion to freeze reported heading
      saved_q0 = q0; saved_q1 = q1; saved_q2 = q2; saved_q3 = q3;
      Serial.println("Stationary detected - freezing heading and adapting gyro bias");
    }
    // while stationary, slowly adapt gyro bias to remove drift
    float raw_gx = IMU.getGyroX_rads();
    float raw_gy = IMU.getGyroY_rads();
    float raw_gz = IMU.getGyroZ_rads();
    gbx = (1.0f - GYRO_BIAS_ADAPT_ALPHA) * gbx + GYRO_BIAS_ADAPT_ALPHA * raw_gx;
    gby = (1.0f - GYRO_BIAS_ADAPT_ALPHA) * gby + GYRO_BIAS_ADAPT_ALPHA * raw_gy;
    gbz = (1.0f - GYRO_BIAS_ADAPT_ALPHA) * gbz + GYRO_BIAS_ADAPT_ALPHA * raw_gz;
    // do NOT update Madgwick filter - keep quaternion (heading) constant
  } else {
    if (isStationary) {
      isStationary = false;
      Serial.println("Motion detected - resuming filter updates");
    }
    madgwickUpdate(gx,gy,gz, ax,ay,az, mx,my,mz, dt);
  }

  float absHeading = getYawDeg();
  float relHeading = absHeading - initialHeading;
  while (relHeading > 180.0f) relHeading -= 360.0f;
  while (relHeading <= -180.0f) relHeading += 360.0f;

  Serial.print("abs: "); Serial.print(absHeading);
  Serial.print(" rel: "); Serial.println(relHeading);

  delay(10);
}