#include <Arduino.h>
#include "MPU9250.h"

MPU9250 mpu;

// Cached IMU values updated when new sensor data arrives
volatile float latestYaw = 90.0f;

// Print at a fixed, slower interval without blocking sensor updates
static uint32_t prev_ms = 0;
const uint32_t PRINT_INTERVAL_MS = 100; // print every 1000 ms

void print_roll_pitch_yaw() {
    noInterrupts();
    float y = latestYaw;
    interrupts();

    Serial.print("Yaw");
    Serial.println(y, 2);
}
void setup() {
    Serial.begin(115200);
    Wire.begin();
    delay(2000);

    if (!mpu.setup(0x68)) {  // change to your own address
        while (1) {
            Serial.println("MPU connection failed. Please check your connection with `connection_check` example.");
            delay(5000);
        }
    }

    // Wait until yaw stabilizes before continuing (tolerance 5 deg for 2000 ms)
    Serial.println("Waiting for yaw to stabilize...");
    bool ok = waitForYawStable(5.0f, 2000, 50, 10000);
    if (ok) Serial.println("Yaw stabilized. Starting loop.");
    else Serial.println("Yaw did not stabilize within timeout; starting loop anyway.");
}

// Wait until yaw variation stays within tolDeg for stableMs milliseconds.
// sampleIntervalMs: how often to read new yaw (ms)
// timeoutMs: total maximum waiting time (ms)
bool waitForYawStable(float tolDeg = 5.0f, unsigned long stableMs = 2000, unsigned long sampleIntervalMs = 50, unsigned long timeoutMs = 10000) {
    unsigned long startAll = millis();
    unsigned long windowStart = millis();
    float yawMin = 1e6f, yawMax = -1e6f;

    while (millis() - startAll < timeoutMs) {
        // Poll sensor until update returns true
        if (mpu.update()) {
            float y = mpu.getYaw();
            // normalize yaw to -180..180 to avoid wrap issues
            while (y > 180.0f) y -= 360.0f;
            while (y <= -180.0f) y += 360.0f;

            if (y < yawMin) yawMin = y;
            if (y > yawMax) yawMax = y;

            unsigned long now = millis();
            if (now - windowStart >= stableMs) {
                float span = yawMax - yawMin;
                // If span crosses the -180/180 wrap, adjust
                if (span < 0) span += 360.0f;
                if (span <= tolDeg) return true;
                // reset window
                windowStart = now;
                yawMin = yawMax = y;
            }
        }
        delay(sampleIntervalMs);
    }
    return false;
}

void loop() {
    // Continuously poll the MPU; when new data is available cache it immediately
    if (mpu.update()) {
        latestYaw = mpu.getYaw();
    }

    uint32_t now = millis();
    if (now - prev_ms >= PRINT_INTERVAL_MS) {
        prev_ms = now;
        print_roll_pitch_yaw();
    }

}
