/**
 * @file abi_encoder_arduino.cpp
 * @brief Implementation of ABI Encoder Arduino library
 * 
 * Converted from mbed to Arduino
 */

#include "abi_encoder_arduino.h"

// Static instance pointer for interrupt handlers
static abi_encoder_arduino* instance = nullptr;

abi_encoder_arduino::abi_encoder_arduino(uint8_t pin_A, uint8_t pin_B, uint16_t spr) 
    : pin_A(pin_A), pin_B(pin_B), spr(spr) {
    
    // Initialize state variables
    A_state = 0;
    B_state = 0;
    cnt = 0;
    before_state = 0;
    state = 0;
    related_distance = 0.0f;
    
    // Set instance pointer
    instance = this;
    
    // Configure pins as inputs with pull-up
    pinMode(pin_A, INPUT_PULLUP);
    pinMode(pin_B, INPUT_PULLUP);
    
    // Read initial state
    A_state = digitalRead(pin_A) ? 1 : 0;
    B_state = digitalRead(pin_B) ? 1 : 0;
    
    // Calculate initial state
    updateState();
    before_state = state;
    
    // Attach interrupts
    // Note: ESP32 supports attachInterruptArg, but we need separate handlers for RISING/FALLING
    // For simplicity, we'll use CHANGE mode and check state in handler
    attachInterruptArg(digitalPinToInterrupt(pin_A), A_rise_handler, this, CHANGE);
    attachInterruptArg(digitalPinToInterrupt(pin_B), B_rise_handler, this, CHANGE);
}

abi_encoder_arduino::~abi_encoder_arduino() {
    // Detach interrupts
    detachInterrupt(digitalPinToInterrupt(pin_A));
    detachInterrupt(digitalPinToInterrupt(pin_B));
    
    if (instance == this) {
        instance = nullptr;
    }
}

void abi_encoder_arduino::setSPR(uint16_t spr){
    this->spr = spr;
}

uint16_t abi_encoder_arduino::getSPR(){
    return spr;
}

// Static interrupt handler wrappers
void abi_encoder_arduino::A_rise_handler(void* obj) {
    if (obj) {
        ((abi_encoder_arduino*)obj)->A_rise();
    }
}

void abi_encoder_arduino::B_rise_handler(void* obj) {
    if (obj) {
        ((abi_encoder_arduino*)obj)->B_rise();
    }
}

void abi_encoder_arduino::A_fall_handler(void* obj) {
    if (obj) {
        ((abi_encoder_arduino*)obj)->A_fall();
    }
}

void abi_encoder_arduino::B_fall_handler(void* obj) {
    if (obj) {
        ((abi_encoder_arduino*)obj)->B_fall();
    }
}

void abi_encoder_arduino::A_rise(){
    // Read current state (handles both RISING and FALLING via CHANGE interrupt)
    A_state = digitalRead(pin_A) ? 0x01 : 0x00;
    updateState();
}

void abi_encoder_arduino::B_rise(){
    // Read current state (handles both RISING and FALLING via CHANGE interrupt)
    B_state = digitalRead(pin_B) ? 0x01 : 0x00;
    updateState();
}

void abi_encoder_arduino::A_fall(){
    // Not used when using CHANGE interrupt mode
    A_rise();  // Same handler
}

void abi_encoder_arduino::B_fall(){
    // Not used when using CHANGE interrupt mode
    B_rise();  // Same handler
}

void abi_encoder_arduino::updateState(){
    // AB    state_no
    // 00    0
    // 10    1
    // 11    2
    // 01    3

    before_state = state;

    switch(((uint16_t)A_state << 1) | (uint16_t)B_state){
        case (uint16_t)0x00:
            state = 0;
            break;

        case (uint16_t)0x02:  // A=1, B=0
            state = 1;
            break;

        case (uint16_t)0x03:  // A=1, B=1
            state = 2;
            break;

        case (uint16_t)0x01:  // A=0, B=1
            state = 3;
            break;

        default:
            break;
    }

    // Count logic:
    // 01 -> 00 -> 10 -> 11 -> 01 forward(+)    3 -> 0 -> 1 -> 2 -> 3
    // 01 <- 00 <- 10 <- 11 <- 01 reverse(-)    3 <- 0 <- 1 <- 2 <- 3
    // 00 -> 00, 10 -> 10, 11 -> 11, 01 -> 01 no movement
    // 00 -> 11 -> 00, 01 -> 10 -> 01 error     0 -> 2 -> 0, 3 -> 1 -> 3

    int delta = before_state - state;
    if(abs(delta) == 2){
        // Error - invalid transition
    }
    else if(delta == 0){
        // No movement
    }
    else if(delta == 3 || delta == -1){
        // Forward
        cnt++;
    }
    else if(delta == -3 || delta == 1){
        // Reverse
        cnt--;
    }
    else{
        // Error
    }
}

int64_t abi_encoder_arduino::getAmountSPR(){
    return cnt;
}

float abi_encoder_arduino::getRelatedTurns(){
    return (float)((double)cnt / spr);
}

void abi_encoder_arduino::reset(){
    cnt = 0;
    before_state = 0;
    state = 0;
    A_state = digitalRead(pin_A) ? 1 : 0;
    B_state = digitalRead(pin_B) ? 1 : 0;
    updateState();
    before_state = state;
}
