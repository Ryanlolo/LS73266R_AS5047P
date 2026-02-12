/*
 * LS7366R Test on ESP32 (using LS7366R_Single library)
 *
 * - Configures LS7366R for 4x quadrature, 32-bit counter, free-run
 * - Reads count periodically and prints via Serial
 *
 * Connections (default VSPI on ESP32):
 *   SCK  -> GPIO 18
 *   MISO -> GPIO 19
 *   MOSI -> GPIO 23
 *   CS   -> GPIO 5 (configurable below)
 *   VCC  -> 3.3V
 *   GND  -> GND
 *
 * Encoder:
 *   A/B/Z to LS7366R A/B/IDX pins as per your board
 */

#include <Arduino.h>
#include <SPI.h>
#include "LS7366R_Single.h"

// --- Pin configuration ---
#define LS7366_CS_PIN  5

// --- Library object ---
LS7366R_Single encoder(LS7366_CS_PIN);

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("\nLS7366R ESP32 Test (using library)");

  if (encoder.begin()) {
    Serial.println("LS7366R initialized.");
  } else {
    Serial.println("LS7366R init failed!");
  }

  // Clear status
  encoder.clearStatus();

  // Initial status
  uint8_t str = encoder.readStatus();
  Serial.print("Initial STR=0x");
  Serial.println(str, HEX);

  // Initial count
  encoder.sync();
  int32_t cnt = encoder.getCount();
  Serial.print("Initial Count: ");
  Serial.println(cnt);
}

unsigned long lastPrint = 0;

void loop() {
  // Periodically print count
  if (millis() - lastPrint >= 250) {
    lastPrint = millis();

    encoder.sync();
    int32_t count = encoder.getCount();
    Serial.print("Count: ");
    Serial.print(count);

    // Optional: read status
    uint8_t str = encoder.readStatus();
    Serial.print("  STR=0x");
    Serial.print(str, HEX);

    Serial.println();
  }

  // Serial commands
  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'z' || c == 'Z') {
      encoder.reset();
      encoder.clearStatus();
      Serial.println("Counter and status cleared.");
    } else if (c == 'r' || c == 'R') {
      encoder.sync();
      Serial.print("Count=");
      Serial.print(encoder.getCount());
      Serial.print(" STR=0x");
      Serial.println(encoder.readStatus(), HEX);
    }
  }
}
