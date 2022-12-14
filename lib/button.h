
#ifndef BUTTON
#define BUTTON

#include <Arduino.h>

struct Button {
  const uint8_t PIN;
  bool pressed;
};

const int buttonPin = D6;

Button button = {buttonPin, false};

//variables to keep track of the timing of recent interrupts
unsigned long button_time = 0;  
unsigned long last_button_time = 0; 


void ICACHE_RAM_ATTR isr();

#endif