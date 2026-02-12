/**
 * @file as5047p_arduino.h
 * @brief AS5047P Magnetic Encoder Library for Arduino
 * 
 * Based on AS5047P datasheet and complete implementation
 * 
 * This library provides an interface to the AS5047P magnetic encoder chip.
 */

#ifndef _AS5047P_ARDUINO_H
#define _AS5047P_ARDUINO_H

#include <Arduino.h>

// AS5047P Register Addresses
#define AS5047P_REG_ANGLECOM      0x3FFF
#define AS5047P_REG_ABI_CTRL     0x0018
#define AS5047P_REG_ABI_SETTINGS 0x0019

// ABI Resolution Settings
#define AS5047P_ABI_RES_100       0x00
#define AS5047P_ABI_RES_200       0x01
#define AS5047P_ABI_RES_400       0x02
#define AS5047P_ABI_RES_800       0x03
#define AS5047P_ABI_RES_1600      0x04  // Recommended for 4000 PPR (4x = 1600)
#define AS5047P_ABI_RES_3200      0x05
#define AS5047P_ABI_RES_6400      0x06
#define AS5047P_ABI_RES_12800     0x07

// ABI Direction
#define AS5047P_ABI_DIR_CW        0x00
#define AS5047P_ABI_DIR_CCW       0x08

// ABI Enable
#define AS5047P_ABI_ENABLE        0x10
#define AS5047P_ABI_DISABLE       0x00

// ABI Index Enable
#define AS5047P_ABI_INDEX_ENABLE  0x20
#define AS5047P_ABI_INDEX_DISABLE 0x00

class as5047p_arduino{
    private:
        uint8_t cs_pin;
        uint8_t miso_pin;
        uint8_t clk_pin;
        uint8_t mosi_pin;  // Added for write operations

        void begin();
        void end();

        void delay_short();
        void delay();
        void long_delay();

        void receive16(uint16_t *buf);
        void send16(uint16_t data);
        uint16_t transfer16(uint16_t data);

    public:
        /** Creates as5047p object with specific content.
         *
         *  @param pin1     CS Pin
         *  @param pin2     MISO Pin
         *  @param pin3     CLK Pin
         *  @param pin4     MOSI Pin (optional, for write operations)
         */
        as5047p_arduino(uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4 = 23);  //cs, miso, clk, mosi

        /** Get the angle (deg)
         *
         *  @return     get the angle (deg)
         */
        float readAngle();
        
        /** Read a register
         *
         *  @param address  Register address
         *  @return         Register value
         */
        uint16_t readRegister(uint16_t address);
        
        /** Write a register
         *
         *  @param address  Register address
         *  @param value    Value to write
         *  @return         true if successful
         */
        bool writeRegister(uint16_t address, uint16_t value);
        
        /** Configure ABI mode
         *
         *  @param resolution  ABI resolution (AS5047P_ABI_RES_xxx)
         *  @param direction   ABI direction (AS5047P_ABI_DIR_CW or AS5047P_ABI_DIR_CCW)
         *  @param enableIndex Enable index pulse
         *  @return            true if successful
         */
        bool configureABI(uint8_t resolution, uint8_t direction, bool enableIndex = false);
        
        /** Enable ABI output */
        bool enableABI();
        
        /** Disable ABI output */
        bool disableABI();
        
        /** Read ABI settings register */
        uint16_t readABISettings();
        
        /** Read ABI control register */
        uint16_t readABICtrl();
};

#endif
