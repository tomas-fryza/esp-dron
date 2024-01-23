/*
 * Serial echo with ESP32 and I2C OLED SH1106
 *
 * Description (for SSD1306):
 *   https://techexplorations.com/blog/drones/empowering-education-exploring-open-source-hardware-drone-control-with-esp32-and-the-tello-api/
 * 
 * Code:
 *   https://github.com/jsolderitsch/ESP32Controller
 * 
 * Library Required: Adafruit GFX Library (Version 1.11.9)
 *                   Adafruit_ESP32_SH1106 (Version 1.0.2)
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>


const unsigned int MAX_MESSAGE_LENGTH = 24;

// Declaration for an SH1106 display connected to I2C (SDA, SCL pins)
#define OLED_RESET 4  // Reset pin
Adafruit_SH1106 display(OLED_RESET);


void setup()
{
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
}


void loop()
{
    display.clearDisplay();    // Clear the buffer
    display.setCursor(0,0);    // Set cursor to the top-left corner

    // Check to see if anything is available in the serial receive buffer
    while (Serial.available() > 0) {
        // Create a place to hold the incoming message
        static char message[MAX_MESSAGE_LENGTH];
        static unsigned int message_pos = 0;

        // Read the next available byte in the serial receive buffer
        char inByte = Serial.read();

        // Message coming in (check not terminating character) and guard for over message size
        if (inByte != '\n' && (message_pos < MAX_MESSAGE_LENGTH - 1)) {
            //Add the incoming byte to our message
            message[message_pos] = inByte;
            message_pos++;
        }
        // Full message received...
        else {
            // Add null character to string
            message[message_pos] = '\0';

            // Print the message (or do other things)
            Serial.println(message);

            display.println("Message: ");
            display.println(message);
            display.display();  // Update the OLED display
            message_pos = 0;
        }
    }
}
