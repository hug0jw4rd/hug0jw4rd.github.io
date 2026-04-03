#include <Arduino.h>
#include <Wire.h>
#include <vector>
#include <algorithm>
#include <Adafruit_NeoPixel.h>
#include "I2Cdev.h"
#include "MPU6050.h"

// --- CONFIGURATION ---
#define RGB_BUILTIN 16   // ESP32 Status LED
#define LED_PIN     27   // Your Goggle LED Strip
#define LED_COUNT   5    // Your Goggle LED Count
#define BUTTON_PIN  36
#define SDA_PIN     21
#define SCL_PIN     22
#define POLLING_RATE 100

// Motor Pins
#define MOTOR0 18
#define MOTOR1 19
#define MOTOR2 26
#define MOTOR3 13
#define MOTOR4 14
#define MOTOR5 25
#define MOTOR6 17

// --- OBJECTS & GLOBALS ---
Adafruit_NeoPixel status_rgb(1, RGB_BUILTIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800); 
MPU6050 mpu;

struct Crumb { 
    int steps; 
    uint8_t segment; 
    Crumb(int s, uint8_t seg) : steps(s), segment(seg) {} 
};

std::vector<Crumb> return_trail;
std::vector<int> motors = {MOTOR3, MOTOR4, MOTOR5, MOTOR6, 0, 0, 0, 0, 0, MOTOR0, MOTOR1, MOTOR2};

unsigned long last_time;
bool button_pressed, last_button_pressed, returning = false;
uint8_t segment, last_segment = 255;
float current_gyro_x = 0; 

// --- HUD FUNCTIONS (YOUR CODE) ---
uint32_t blendColor(uint32_t c1, uint32_t c2, float t) {
  t = constrain(t, 0.0, 1.0);
  uint8_t r = (uint8_t)((1.0 - t) * ((c1 >> 16) & 0xFF) + t * ((c2 >> 16) & 0xFF));
  uint8_t g = (uint8_t)((1.0 - t) * ((c1 >> 8)  & 0xFF) + t * ((c2 >> 8)  & 0xFF));
  uint8_t b = (uint8_t)((1.0 - t) * ( c1        & 0xFF) + t * ( c2        & 0xFF));
  return strip.Color(r, g, b);
}

void updateHUD(int error) {
  const float SCALE = 30.0; 
  const uint32_t GREEN  = strip.Color(0, 200, 0);
  const uint32_t YELLOW = strip.Color(210, 150, 0);
  const uint32_t RED    = strip.Color(210, 0, 0);
  const float ledPos[LED_COUNT] = {-2.0, -1.0, 0.0, 1.0, 2.0};

  float norm = (float)error / SCALE;
  float direction = (error >= 0) ? 1.0 : -1.0;

  for (int i = 0; i < LED_COUNT; i++) {
    float danger = ledPos[i] * direction * fabs(norm);
    uint32_t col;
    if (danger <= 0.0) col = GREEN;
    else if (danger <= 1.0) col = blendColor(GREEN, YELLOW, danger);
    else col = blendColor(YELLOW, RED, constrain(danger - 1.0, 0.0, 1.0));
    strip.setPixelColor(i, col);
  }
  strip.show();
}

// --- NAVIGATION HELPERS ---
int seg_diff(int target, int current) {
  int d = target - current;
  if (d > 6)  d -= 12;
  if (d < -6) d += 12;
  return d;
}

// --- SETUP ---
void setup() {
  Serial.begin(115200);
  strip.begin();
  strip.setBrightness(80);
  
  status_rgb.begin();
  status_rgb.setPixelColor(0, status_rgb.Color(255, 0, 0)); 
  status_rgb.show();

  Wire.begin(SDA_PIN, SCL_PIN);
  pinMode(BUTTON_PIN, INPUT); // Note: Hardware may need 10k resistor to 3.3V
  for (int p : motors) { if(p > 0) pinMode(p, OUTPUT); }

  mpu.initialize();
  mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_500);
  
  status_rgb.setPixelColor(0, status_rgb.Color(0, 255, 0)); 
  status_rgb.show();
  last_time = micros();
}

// --- MAIN LOOP ---
void loop() {
  unsigned long now = micros();
  float dt = (now - last_time) / 1000000.0f;

  if (dt > (1.0f / POLLING_RATE)) {
    last_time = now;
    
    int16_t gx, gy, gz;
    mpu.getRotation(&gx, &gy, &gz);
    float gyro_dps = (gx) * (1.0f / 65.5f); 
    current_gyro_x += gyro_dps * dt;
    
    if(current_gyro_x >= 360) current_gyro_x -= 360;
    if(current_gyro_x < 0) current_gyro_x += 360;
    
    segment = (uint8_t)floorf(current_gyro_x / 30.0f);

    if (returning) {
      if (!return_trail.empty()) {
        uint8_t target_seg = return_trail.back().segment;
        int diff = seg_diff((int)target_seg, (int)segment);

        // 1. Haptic Feedback
        int d = diff;
        if (d > 3) d = 3; if (d < -3) d = -3;
        static const uint8_t err_to_seg[7] = {9, 10, 11, 0, 1, 2, 3};
        uint8_t motor_idx = err_to_seg[d + 3];
        
        for (int p : motors) { if(p > 0) digitalWrite(p, LOW); }
        if (motors[motor_idx] > 0) digitalWrite(motors[motor_idx], HIGH);

        // 2. Visual HUD (Your Code Integrated)
        updateHUD(diff * 30); 
      }
    } else {
        // Simple recording logic
        if (segment != last_segment) {
            uint8_t opp = (segment + 6) % 12;
            return_trail.push_back(Crumb(0, opp));
            last_segment = segment;
        }
        strip.clear();
        strip.show();
    }
  }

  button_pressed = digitalRead(BUTTON_PIN);
  if (!last_button_pressed && button_pressed && !returning) {
    returning = true;
    std::reverse(return_trail.begin(), return_trail.end());
  }
  last_button_pressed = button_pressed;
}
