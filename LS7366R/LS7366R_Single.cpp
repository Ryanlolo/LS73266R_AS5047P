/**
 * @file LS7366R_Single.cpp
 * @brief Implementation of LS7366R_Single class
 */

#include "LS7366R_Single.h"
#include <SPI.h>

// SPI Settings for LS7366R
// According to datasheet: max 10MHz, but 500kHz is more reliable
#define LS7366R_SPI_SPEED    500000  // 500 kHz
#define LS7366R_SPI_MODE     SPI_MODE0
#define LS7366R_SPI_BITORDER MSBFIRST

// Timing delays (microseconds)
#define LS7366R_CS_SETUP     5   // CS setup time
#define LS7366R_CS_HOLD      5   // CS hold time
#define LS7366R_CMD_DELAY    2   // Delay between command and data
#define LS7366R_OTR_LOAD     10  // OTR load delay (use delayMicroseconds)

// ============================================================================
// Constructor
// ============================================================================

LS7366R_Single::LS7366R_Single(uint8_t csPin, uint8_t mdr0_config, uint8_t mdr1_config)
    : csPin(csPin), countValue(0), mdr0Config(mdr0_config), mdr1Config(mdr1_config)
{
    // Pin setup will be done in begin()
}

// ============================================================================
// Public Methods
// ============================================================================

bool LS7366R_Single::begin()
{
    // Initialize CS pin (HIGH = inactive)
    pinMode(csPin, OUTPUT);
    digitalWrite(csPin, HIGH);
    delayMicroseconds(10);  // Allow pin to stabilize
    
    // Initialize SPI
    SPI.begin();
    
    // Configure registers
    reconfigure(mdr0Config, mdr1Config);
    
    // Reset counter
    reset();
    
    return true;
}

void LS7366R_Single::reset()
{
    spiBegin();
    
    // Clear counter register
    digitalWrite(csPin, LOW);
    delayMicroseconds(LS7366R_CS_SETUP);
    SPI.transfer(LS7366R_CMD_CLEAR | LS7366R_REG_CNTR);
    delayMicroseconds(LS7366R_CS_HOLD);
    digitalWrite(csPin, HIGH);
    
    spiEnd();
    
    // Update cached value
    countValue = 0;
    
    delayMicroseconds(5);  // Small delay after reset
}

void LS7366R_Single::sync()
{
    spiBegin();
    
    // Step 1: Load counter value into OTR (Output Transfer Register)
    digitalWrite(csPin, LOW);
    delayMicroseconds(LS7366R_CS_SETUP);
    SPI.transfer(LS7366R_CMD_LOAD | LS7366R_REG_OTR);
    delayMicroseconds(LS7366R_CS_HOLD);
    digitalWrite(csPin, HIGH);
    
    // Wait for OTR to load (datasheet requirement)
    delayMicroseconds(LS7366R_OTR_LOAD);
    
    // Step 2: Read 32-bit value from OTR register
    digitalWrite(csPin, LOW);
    delayMicroseconds(LS7366R_CS_SETUP);
    SPI.transfer(LS7366R_CMD_READ | LS7366R_REG_OTR);
    
    // Read 4 bytes (MSB first)
    uint32_t count = 0;
    count = ((uint32_t)SPI.transfer(0x00)) << 24;
    count |= ((uint32_t)SPI.transfer(0x00)) << 16;
    count |= ((uint32_t)SPI.transfer(0x00)) << 8;
    count |= (uint32_t)SPI.transfer(0x00);
    
    delayMicroseconds(LS7366R_CS_HOLD);
    digitalWrite(csPin, HIGH);
    
    spiEnd();
    
    // Convert to signed 32-bit integer
    countValue = (int32_t)count;
}

void LS7366R_Single::reconfigure(uint8_t mdr0_config, uint8_t mdr1_config)
{
    spiBegin();
    
    // Save configurations
    mdr0Config = mdr0_config;
    mdr1Config = mdr1_config;
    
    // Configure MDR0 register
    writeRegister(LS7366R_REG_MDR0, mdr0_config);
    delay(5);  // Allow register to settle
    
    // Configure MDR1 register
    writeRegister(LS7366R_REG_MDR1, mdr1_config);
    delay(5);  // Allow register to settle
    
    spiEnd();
}

uint8_t LS7366R_Single::readStatus()
{
    spiBegin();
    
    uint8_t status = readRegister(LS7366R_REG_STR);
    
    spiEnd();
    
    return status;
}

void LS7366R_Single::enable()
{
    spiBegin();
    
    // Set bit 2 to 0 (enable counting)
    uint8_t newMdr1 = mdr1Config & ~LS7366R_MDR1_COUNT_DISABLE;
    writeRegister(LS7366R_REG_MDR1, newMdr1);
    mdr1Config = newMdr1;
    
    spiEnd();
}

void LS7366R_Single::disable()
{
    spiBegin();
    
    // Set bit 2 to 1 (disable counting)
    uint8_t newMdr1 = mdr1Config | LS7366R_MDR1_COUNT_DISABLE;
    writeRegister(LS7366R_REG_MDR1, newMdr1);
    mdr1Config = newMdr1;
    
    spiEnd();
}

bool LS7366R_Single::isEnabled() const
{
    return (mdr1Config & LS7366R_MDR1_COUNT_DISABLE) == 0;
}

void LS7366R_Single::clearStatus()
{
    spiBegin();
    
    // Clear status register (this clears phase errors and flags)
    digitalWrite(csPin, LOW);
    delayMicroseconds(LS7366R_CS_SETUP);
    SPI.transfer(LS7366R_CMD_CLEAR | LS7366R_REG_STR);
    delayMicroseconds(LS7366R_CS_HOLD);
    digitalWrite(csPin, HIGH);
    
    spiEnd();
    
    delayMicroseconds(10);  // Small delay after clear
}

// ============================================================================
// Private Methods
// ============================================================================

void LS7366R_Single::writeRegister(uint8_t reg, uint8_t value)
{
    digitalWrite(csPin, LOW);
    delayMicroseconds(LS7366R_CS_SETUP);
    
    // Send write command + register address
    SPI.transfer(LS7366R_CMD_WRITE | reg);
    delayMicroseconds(LS7366R_CMD_DELAY);
    
    // Send data
    SPI.transfer(value);
    delayMicroseconds(LS7366R_CS_HOLD);
    
    digitalWrite(csPin, HIGH);
}

uint8_t LS7366R_Single::readRegister(uint8_t reg)
{
    digitalWrite(csPin, LOW);
    delayMicroseconds(LS7366R_CS_SETUP);
    
    // Send read command + register address
    SPI.transfer(LS7366R_CMD_READ | reg);
    delayMicroseconds(LS7366R_CMD_DELAY);
    
    // Read data
    uint8_t value = SPI.transfer(0x00);
    delayMicroseconds(LS7366R_CS_HOLD);
    
    digitalWrite(csPin, HIGH);
    
    return value;
}

void LS7366R_Single::spiBegin()
{
    SPI.beginTransaction(SPISettings(LS7366R_SPI_SPEED, LS7366R_SPI_BITORDER, LS7366R_SPI_MODE));
}

void LS7366R_Single::spiEnd()
{
    SPI.endTransaction();
}
