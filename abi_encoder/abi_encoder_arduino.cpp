#include "abi_encoder_arduino.h"

// Static instance pointer for interrupt callbacks
abi_encoder_arduino* abi_encoder_arduino::instance = nullptr;

abi_encoder_arduino::abi_encoder_arduino(uint8_t pin_A, uint8_t pin_B, uint16_t spr) 
    : pin_A(pin_A), pin_B(pin_B) {
    setSPR(spr);
    instance = this;  // Set instance pointer
    init_pins();
}

abi_encoder_arduino::~abi_encoder_arduino() {
    if (instance == this) {
        #ifdef ESP32
            detachInterrupt(pin_A);
            detachInterrupt(pin_B);
        #else
            detachInterrupt(digitalPinToInterrupt(pin_A));
            detachInterrupt(digitalPinToInterrupt(pin_B));
        #endif
        instance = nullptr;
    }
}

void abi_encoder_arduino::init_pins(){
    // Configure pins as inputs with pull-down (ESP32 supports INPUT_PULLDOWN)
    #ifdef ESP32
        pinMode(pin_A, INPUT_PULLDOWN);
        pinMode(pin_B, INPUT_PULLDOWN);
    #else
        // For other Arduino boards, use INPUT (external pull-down required)
        pinMode(pin_A, INPUT);
        pinMode(pin_B, INPUT);
    #endif

    // Attach interrupts (CHANGE mode handles both rising and falling edges)
    #ifdef ESP32
        attachInterrupt(pin_A, A_change_handler, CHANGE);
        attachInterrupt(pin_B, B_change_handler, CHANGE);
    #else
        attachInterrupt(digitalPinToInterrupt(pin_A), A_change_handler, CHANGE);
        attachInterrupt(digitalPinToInterrupt(pin_B), B_change_handler, CHANGE);
    #endif
}

void abi_encoder_arduino::setSPR(uint16_t spr){
    this->spr = spr;
}

uint16_t abi_encoder_arduino::getSPR(){
    return spr;
}

// Static interrupt handlers
void abi_encoder_arduino::A_change_handler(){
    if (instance != nullptr) {
        instance->A_change();
    }
}

void abi_encoder_arduino::B_change_handler(){
    if (instance != nullptr) {
        instance->B_change();
    }
}

// Instance methods - detect rise/fall by reading pin state
void abi_encoder_arduino::A_change(){
    A_state = digitalRead(pin_A) ? 0x01 : 0x00;
    updateState();
}

void abi_encoder_arduino::B_change(){
    B_state = digitalRead(pin_B) ? 0x01 : 0x00;
    updateState();
}

void abi_encoder_arduino::updateState(){
    //AB    state_no
    //00    0
    //10    1
    //11    2
    //01    3

    before_state = state;

    switch(((uint16_t)A_state << 4) | (uint16_t)B_state){
        case (uint16_t)0x00:
            state = 0;
            break;

        case (uint16_t)0x10:
            state = 1;
            break;

        case (uint16_t)0x11:
            state = 2;
            break;

        case (uint16_t)0x01:
            state = 3;
            break;

        default:
            break;
    }

    //count plus
    // 01 -> 00 -> 10 -> 11 -> 01 forward(+)    3 -> 0 -> 1 -> 2 -> 3
    // 01 <- 00 <- 10 <- 11 <- 01 reverse(-)    3 <- 0 <- 1 <- 2 <- 3
    // 00 -> 00, 10 -> 10, 11 -> 11, 01 -> 01 no movement
    // 00 -> 11 -> 00, 01 -> 10 -> 01 error     0 -> 2 -> 0, 3 -> 1 -> 3

    int delta = before_state - state;
    if(abs(delta) == 2){
        //error
    }
    else if(delta == 0){
        //no movement
    }
    else if(delta == 3 || delta == -1){
        //forward
        cnt++;
    }
    else if(delta == -3 || delta == 1){
        //reverse
        cnt--;
    }
    else{
        //error
    }
}

int64_t abi_encoder_arduino::getAmountSPR(){
    return cnt;
}

float abi_encoder_arduino::getRelatedTurns(){
    return (float)((double)cnt/spr);
}

void abi_encoder_arduino::reset(){
    cnt = 0;
}
