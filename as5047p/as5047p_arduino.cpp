#include "as5047p_arduino.h"

// NOP macro for different platforms
#if defined(__AVR__)
    #define NOP_ASM() asm volatile("nop")
#elif defined(ESP32) || defined(ESP8266)
    #define NOP_ASM() asm volatile("nop")
#elif defined(__ARM__)
    #define NOP_ASM() __asm__ __volatile__("nop")
#else
    #define NOP_ASM() do { __asm__ __volatile__("nop"); } while(0)
#endif

as5047p_arduino::as5047p_arduino(uint8_t pin1, uint8_t pin2, uint8_t pin3) 
    : cs_pin(pin1), miso_pin(pin2), clk_pin(pin3) {
    // Configure pins
    pinMode(cs_pin, OUTPUT);
    pinMode(clk_pin, OUTPUT);
    
    #ifdef ESP32
        pinMode(miso_pin, INPUT_PULLUP);
    #else
        pinMode(miso_pin, INPUT);
        // For non-ESP32 boards, you may need external pull-up resistor
    #endif

    // Initialize CS high (inactive)
    digitalWrite(cs_pin, HIGH);
    digitalWrite(clk_pin, LOW);
}

void as5047p_arduino::begin(){
    delay();
    digitalWrite(cs_pin, LOW);
    delay();
}

void as5047p_arduino::end(){
    delay();
    digitalWrite(cs_pin, HIGH);
    delay();
}

void as5047p_arduino::delay_short(){
    for(int i = 0; i < 4; i++){
        NOP_ASM();
    }
}

void as5047p_arduino::delay(){
    for (int i = 0; i < 6; i++) {
        NOP_ASM();
    }
}

void as5047p_arduino::long_delay(){
    for (int i = 0; i < 40; i++) {
        NOP_ASM();
    }
}

void as5047p_arduino::receive16(uint16_t *buf){
    uint16_t receive = 0;

    for (int bit = 0; bit < 16; bit++) {
        digitalWrite(clk_pin, HIGH);
        delay_short();

        int samples = 0;
        samples += (int)digitalRead(miso_pin);
        NOP_ASM();
        samples += (int)digitalRead(miso_pin);
        NOP_ASM();
        samples += (int)digitalRead(miso_pin);
        NOP_ASM();
        samples += (int)digitalRead(miso_pin);
        NOP_ASM();
        samples += (int)digitalRead(miso_pin);

        receive <<= 1;
        if (samples > 2) {
            receive |= 1;
        }

        digitalWrite(clk_pin, LOW);
        delay_short();
    }

    buf[0] = receive;
}

float as5047p_arduino::readAngle(){
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
