/**
 * @file abi_encoder_arduino.h
 * @brief Software ABI Encoder Decoder Library for Arduino
 * 
 * Converted from mbed to Arduino
 * 
 * This library provides software-based decoding of A/B quadrature encoder signals
 * using interrupt handlers. Useful when hardware counter is not available.
 * 
 * Note: If you have LS7366R hardware counter, you don't need this library.
 * This is for software-based decoding only.
 */

#ifndef _ABI_ENCODER_ARDUINO_H
#define _ABI_ENCODER_ARDUINO_H

#include <Arduino.h>

class abi_encoder_arduino{
    private:
        uint8_t pin_A;
        uint8_t pin_B;

        volatile uint8_t A_state;
        volatile uint8_t B_state;

        volatile int64_t cnt;

        volatile int before_state;
        volatile int state;

        uint16_t spr;  // Steps per revolution
        float related_distance;

        void updateState();
        
        // Interrupt handlers (static wrappers)
        static void A_rise_handler(void* obj);
        static void B_rise_handler(void* obj);
        static void A_fall_handler(void* obj);
        static void B_fall_handler(void* obj);
        
        // Instance interrupt handlers
        void A_rise();
        void B_rise();
        void A_fall();
        void B_fall();

    public:
        /** Creates abi_encoder object with specific content.
         *
         *  @param pin_A    Pin of incremental signal A
         *  @param pin_B    Pin of incremental signal B
         *  @param spr      Steps per revolution (default: 4000)
         */
        abi_encoder_arduino(uint8_t pin_A, uint8_t pin_B, uint16_t spr = 4000);

        /** Destructor - detach interrupts */
        ~abi_encoder_arduino();

        /** Set the Steps per revolution (SPR).
         *
         *  @param spr      Steps per revolution
         */
        void setSPR(uint16_t spr);

        /** Get the value of Steps per revolution (SPR)
         *
         *  @return     Steps per revolution
         */
        uint16_t getSPR();

        /** Get the amount of steps per revolution.
         *
         *  @return     Current count value
         */
        int64_t getAmountSPR();

        /** Get the number of related rotation turns
         *
         *  @return     Number of rotation turns
         */
        float getRelatedTurns();
        
        /** Reset the counter to zero */
        void reset();
};

#endif
