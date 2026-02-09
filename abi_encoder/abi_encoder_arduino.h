#ifndef _ABI_ENCODER_ARDUINO_H
#define _ABI_ENCODER_ARDUINO_H

#include <Arduino.h>

class abi_encoder_arduino{
    private:
        uint8_t pin_A;
        uint8_t pin_B;

        uint8_t A_state, B_state = 0;

        volatile int64_t cnt = 0;

        volatile int before_state = 0;
        volatile int state = 0;

        uint16_t spr = 4000;
        float related_distance = 0.0f;

        void init_pins();
        void updateState();

        // Static interrupt handlers
        static void A_rise_handler();
        static void B_rise_handler();
        static void A_fall_handler();
        static void B_fall_handler();

        // Instance pointer for interrupt callbacks
        static abi_encoder_arduino* instance;

        // Instance methods called by static handlers
        void A_change();
        void B_change();

    public:
        /** Creates abi_encoder object with specific content.
         *
         *  @param pin_A    Pin of incremental signal A
         *  @param pin_B    Pin of incremental signal B
         *  @param spr      Steps per revolution
         */
        abi_encoder_arduino(uint8_t pin_A, uint8_t pin_B, uint16_t spr = 4000);

        /** Destructor */
        ~abi_encoder_arduino();

        /** Set the Steps per revolution (SPR).
         *
         *  @param spr      Steps per revolution
         */
        void setSPR(uint16_t spr);

        /* Get the value of Steps per revolution (SPR)*/
        uint16_t getSPR();

        /** Get the amount of steps per revolution.
         *
         *  @return     Get the amount of steps per revolution
         */
        int64_t getAmountSPR();

        /** Get the number of related rotation turns
         *
         *  @return     Get the number of related rotation turns
         */
        float getRelatedTurns();

        /** Reset the counter to zero */
        void reset();
};

#endif
