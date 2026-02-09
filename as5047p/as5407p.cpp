#include "as5407p.h"

as5407p::as5407p(PinName pin1, PinName pin2, PinName pin3) : cs(pin1), miso(pin2), clk(pin3){
    miso.mode(PullUp);

    cs = 1;
}

void as5407p::begin(){
    delay();
    cs = 0;
    delay();
}

void as5407p::end(){
    delay();
    cs = 1;
    delay();
}

void as5407p::delay_short(){
    for(int i = 0; i < 4; i++){
        __NOP();
    }
}

void as5407p::delay(){
    for (int i = 0; i < 6; i++) {
        __NOP();
    }
}

void as5407p::long_delay(){
    for (int i = 0; i < 40; i++) {
		__NOP();
	}
}

void as5407p::receive16(uint16_t *buf){
    uint16_t receive = 0;

    for (int bit = 0; bit < 16; bit++) {
        clk = 1;
        delay_short();

        int samples = 0;
        samples += (int)miso.read();
        __NOP();
        samples += (int)miso.read();
        __NOP();
        samples += (int)miso.read();
        __NOP();
        samples += (int)miso.read();
        __NOP();
        samples += (int)miso.read();

        receive <<= 1;
        if (samples > 2) {
            receive |= 1;
        }

        clk = 0;
        delay_short();
    }

    buf[0] = receive;
}

float as5407p::readAngle(){
    float deg = 0.0f;
    uint16_t pos = 0;

    begin();
    receive16(&pos);
    end();

    //filter
    pos &= 0x3FFF;

    //change the unit to deg
    deg = (float)(pos * 360.0f) / 16384.0f;

    return  deg;
}