/*
 * LS7366R Encoder Counter Main Program (Single Chip Version)
 * 
 * Hardware Connections:
 * - LS7366R connected to ESP32 via SPI
 * - Encoder A/B signals connected to LS7366R input pins
 * 
 * SPI Connections (ESP32):
 * - MOSI: GPIO 23 (default)
 * - MISO: GPIO 19 (default)
 * - SCK:  GPIO 18 (default)
 * - CS:   GPIO 5  (customizable)
 */

#include <Arduino.h>
#include "LS7366R_Single.h"

// ========== Pin Definitions ==========
#define LS7366R_CS_PIN   5   // CS pin

// ========== Configuration Parameters ==========
// MDR0 Configuration: 4x quadrature mode, free-running, no index
#define MDR0_CONFIG  B00000011

// MDR1 Configuration: 32-bit counter, enabled
#define MDR1_CONFIG  B00000000

// ========== Object Creation ==========
LS7366R_Single encoder(LS7366R_CS_PIN, MDR0_CONFIG, MDR1_CONFIG);

// ========== Variable Definitions ==========
unsigned long lastReadTime = 0;
const unsigned long READ_INTERVAL = 100;  // Read interval (milliseconds)

// Variables for speed calculation
long lastCount = 0;
unsigned long lastSpeedCalcTime = 0;
const unsigned long SPEED_CALC_INTERVAL = 200;  // Speed calculation interval (milliseconds)

// Encoder parameters (adjust according to your encoder)
const float PULSES_PER_REVOLUTION = 4000.0;  // Pulses per revolution (actual pulses in 4x mode)

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("========================================");
    Serial.println("LS7366R Encoder Counter (Single Chip)");
    Serial.println("========================================");
    Serial.println();
    
    Serial.println("LS7366R initialized!");
    Serial.print("CS Pin: ");
    Serial.println(LS7366R_CS_PIN);
    Serial.print("Pulses per revolution: ");
    Serial.println(PULSES_PER_REVOLUTION);
    Serial.println();
    
    // Reset counter
    encoder.reset();
    Serial.println("Counter reset to zero");
    Serial.println();
    
    Serial.println("Starting to read encoder values...");
    Serial.println("Format: Count | Turns | Speed (rps)");
    Serial.println("----------------------------------------");
    
    lastReadTime = millis();
    lastSpeedCalcTime = millis();
}

void loop() {
    unsigned long currentTime = millis();
    
    // Periodically sync and read encoder values
    if (currentTime - lastReadTime >= READ_INTERVAL) {
        // Sync and read counter value from LS7366R chip
        encoder.sync();
        
        // Get counter value
        long count = encoder.getCount();
        
        // Calculate turns
        float turns = (float)count / PULSES_PER_REVOLUTION;
        
        // Calculate speed (revolutions per second)
        float speed = 0.0;
        
        if (currentTime - lastSpeedCalcTime >= SPEED_CALC_INTERVAL) {
            long countDiff = count - lastCount;
            float timeDiff = (currentTime - lastSpeedCalcTime) / 1000.0;  // Convert to seconds
            
            speed = ((float)countDiff / PULSES_PER_REVOLUTION) / timeDiff;
            
            lastCount = count;
            lastSpeedCalcTime = currentTime;
        }
        
        // Display results
        Serial.print("Count: ");
        Serial.print(count);
        Serial.print(" | Turns: ");
        Serial.print(turns, 3);
        Serial.print(" | Speed: ");
        Serial.print(speed, 2);
        Serial.println(" rps");
        
        lastReadTime = currentTime;
    }
    
    // Check if reset is needed (reset via serial command 'r' or 'R')
    if (Serial.available()) {
        char cmd = Serial.read();
        if (cmd == 'r' || cmd == 'R') {
            encoder.reset();
            lastCount = 0;
            Serial.println("\nCounter reset!");
        }
    }
}
