/*
 * LS7366R Test on ESP32 (using LS7366R_Single library)
 * Two encoders support
 *
 * - Configures two LS7366R for 4x quadrature, 32-bit counter, free-run
 * - Reads count periodically and prints via Serial
 *
 * Connections (default VSPI on ESP32):
 *   SCK  -> GPIO 18
 *   MISO -> GPIO 19
 *   MOSI -> GPIO 23
 *   CS1  -> GPIO 5  (encoder 1)
 *   CS2  -> GPIO 15 (encoder 2)
 *   VCC  -> 3.3V
 *   GND  -> GND
 *
 * Encoder 1: A/B/Z to LS7366R #1
 * Encoder 2: A/B/Z to LS7366R #2
 */

#include <Arduino.h>
#include <SPI.h>
#include "LS7366R_Single.h"

// --- Pin configuration ---
#define LS7366_CS_PIN_1  5   // Encoder 1
#define LS7366_CS_PIN_2  15  // Encoder 2

// --- Library objects ---
LS7366R_Single encoder1(LS7366_CS_PIN_1);
LS7366R_Single encoder2(LS7366_CS_PIN_2);

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("\nLS7366R ESP32 Test - Two Encoders");

  if (encoder1.begin()) {
    Serial.println("LS7366R #1 initialized (CS=5)");
  } else {
    Serial.println("LS7366R #1 init failed!");
  }

  if (encoder2.begin()) {
    Serial.println("LS7366R #2 initialized (CS=15)");
  } else {
    Serial.println("LS7366R #2 init failed!");
  }

  // Clear status for both
  encoder1.clearStatus();
  encoder2.clearStatus();

  // Initial status and count
  encoder1.sync();
  encoder2.sync();
  Serial.print("Enc1 STR=0x");
  Serial.print(encoder1.readStatus(), HEX);
  Serial.print(" Count=");
  Serial.print(encoder1.getCount());
  Serial.print(" | Enc2 STR=0x");
  Serial.print(encoder2.readStatus(), HEX);
  Serial.print(" Count=");
  Serial.println(encoder2.getCount());
}

unsigned long lastPrint = 0;

void loop() {
  // Periodically print counts
  if (millis() - lastPrint >= 250) {
    lastPrint = millis();

    encoder1.sync();
    encoder2.sync();

    Serial.print("Enc1: ");
    Serial.print(encoder1.getCount());
    Serial.print(" (STR=0x");
    Serial.print(encoder1.readStatus(), HEX);
    Serial.print(") | Enc2: ");
    Serial.print(encoder2.getCount());
    Serial.print(" (STR=0x");
    Serial.print(encoder2.readStatus(), HEX);
    Serial.println(")");
  }

  // Serial commands
  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'z' || c == 'Z') {
      encoder1.reset();
      encoder2.reset();
      encoder1.clearStatus();
      encoder2.clearStatus();
      Serial.println("Both counters and status cleared.");
    } else if (c == '1') {
      encoder1.reset();
      encoder1.clearStatus();
      Serial.println("Encoder 1 cleared.");
    } else if (c == '2') {
      encoder2.reset();
      encoder2.clearStatus();
      Serial.println("Encoder 2 cleared.");
    } else if (c == 'r' || c == 'R') {
      encoder1.sync();
      encoder2.sync();
      Serial.print("Enc1=");
      Serial.print(encoder1.getCount());
      Serial.print(" Enc2=");
      Serial.println(encoder2.getCount());
    }
  }
}
