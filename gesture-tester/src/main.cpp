/*
 * Get tilt angles on X and Y, and rotation angle on Z using
 * ESP32 FireBeetle 2 board, MPU6050 I2C sensor, and OLED SH1106.
 * 
 * FireBeetle 2 pinout:
 *   https://image.dfrobot.com/image/data/DFR0654-F/Pinout.jpg
 * 
 * Library Required: Adafruit GFX Library (Version 1.11.9)
 *                   Adafruit_ESP32_SH1106 (Version 1.0.2)
 *                   MPU6050_light (Version 1.1.0)
 * 
 * License: MIT
 * 
 * Description (using SSD1306 OLED driver):
 *   https://techexplorations.com/blog/drones/empowering-education-exploring-open-source-hardware-drone-control-with-esp32-and-the-tello-api/
 * 
 * Code:
 *   https://github.com/jsolderitsch/ESP32Controller
 */

#include <Wire.h>
#include <MPU6050_light.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>


// Config LED pins
// https://image.dfrobot.com/image/data/DFR0654-F/Pinout.jpg
#define LED_FORWARD  17
#define LED_BACK     16
#define LED_RIGHT    4
#define LED_LEFT     12

// Declaration for an SH1106 display connected to I2C (SDA, SCL pins)
#define OLED_RESET 4  // Reset pin
Adafruit_SH1106 display(OLED_RESET);

MPU6050 mpu(Wire);

// Motions: https://i.ytimg.com/vi/FXabvMSQNxA/maxresdefault.jpg
int8_t mpuRoll;   // Left/Right
int8_t mpuPitch;  // Forward/backward
int8_t mpuYaw;    // Rotate right/left


void setup(void)
{
    // Init hardware serial
    Serial.begin(115200);
    while (!Serial);

    // Initialize OLED display with I2C address 0x3C
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    display.begin(SH1106_SWITCHCAPVCC, 0x3C);
    delay(500);
    display.display();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setRotation(0);
    display.clearDisplay();
    display.setCursor(0, 0);

    // Initialize MPU6050 sensor
    uint8_t status = mpu.begin();
    Serial.print(F("MPU6050 status: "));
    Serial.println(status);
    display.print("MPU6050 status: ");
    display.println(status);
    display.display();
    while (status != 0) {
        // Loop here if could not connect to MPU6050
    }
    // Get the idle controller position
    Serial.print(F("Calculating offsets, do not move MPU6050... "));
    delay(1000);
    mpu.calcOffsets();
    Serial.println("Done");
    delay(100);

    // Configure LEDs
    pinMode(LED_FORWARD, OUTPUT);
    pinMode(LED_BACK, OUTPUT);
    pinMode(LED_RIGHT, OUTPUT);
    pinMode(LED_LEFT, OUTPUT);
    digitalWrite(LED_FORWARD, LOW);
    digitalWrite(LED_BACK, LOW);
    digitalWrite(LED_RIGHT, LOW);
    digitalWrite(LED_LEFT, LOW);
}


void loop()
{
    uint16_t timer = 0;

    mpu.update();
    mpuRoll = mpu.getAngleX();
    mpuPitch = mpu.getAngleY();
    mpuYaw = mpu.getAngleZ();

    display.clearDisplay();
    display.setCursor(0, 0);

    // Turn LEDs OFF
    if (abs(mpuRoll) <= 10) {
        digitalWrite(LED_LEFT,LOW);
        digitalWrite(LED_RIGHT,LOW);
    }
    if (abs(mpuPitch) <= 15) {
        digitalWrite(LED_FORWARD,LOW);
        digitalWrite(LED_BACK,LOW);
    }

    // Turn LEDs ON
    // Move forward
    if (mpuPitch < -16) {
      digitalWrite(LED_FORWARD,HIGH);
    }

    // Move backward
    if (mpuPitch > 16) {
        digitalWrite(LED_BACK,HIGH);
    }

    // Move right
    if (mpuRoll < -11) {
        digitalWrite(LED_RIGHT,HIGH);
    }

    // Move left
    if (mpuRoll > 11) {
        digitalWrite(LED_LEFT,HIGH);
    }

    // Update display data every 100 ms
    if ((millis()-timer) > 100) {
        display.println("Roll: " + String(mpuRoll));
        display.println("Pitch: " + String(mpuPitch));
        display.println("Yaw: " + String(mpuYaw));
        display.display();

        timer = millis();  
    }
}
