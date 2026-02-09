#ifndef _AS5407P_H
#define _AS5407P_H

#include "mbed.h"

class as5407p{
    private:
        DigitalOut cs;
        DigitalIn miso;
        DigitalOut clk;

        void begin();
        void end();

        void delay_short();
        void delay();
        void long_delay();

        void receive16(uint16_t *buf);

    public:
        /** Creates as5407p object with specific content.
         *
         *  @param pin1     CS Pin
         *  @param pin2     MISO Pin
         *  @param pin3     CLK Pin
         */
        as5407p(PinName pin1, PinName pin2, PinName pin3);  //cs, miso, clk

        /** Get the angle (deg)
         *
         *  @return     get the angle (deg)
         */
        float readAngle();
};

#endif