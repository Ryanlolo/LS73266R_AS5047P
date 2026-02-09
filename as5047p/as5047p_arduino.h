#ifndef _AS5047P_ARDUINO_H
#define _AS5047P_ARDUINO_H

#include <Arduino.h>

class as5047p_arduino{
    private:
        uint8_t cs_pin;
        uint8_t miso_pin;
        uint8_t clk_pin;

        void begin();
        void end();

        void delay_short();
        void delay();
        void long_delay();

        void receive16(uint16_t *buf);

    public:
        /** Creates as5047p object with specific content.
         *
         *  @param pin1     CS Pin
         *  @param pin2     MISO Pin
         *  @param pin3     CLK Pin
         */
        as5047p_arduino(uint8_t pin1, uint8_t pin2, uint8_t pin3);  //cs, miso, clk

        /** Get the angle (deg)
         *
         *  @return     get the angle (deg)
         */
        float readAngle();
};

#endif
