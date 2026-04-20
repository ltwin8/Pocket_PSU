#ifndef HID_H
#define HID_H

#include <Arduino.h>

#define SW_PUSH     PIN_PA7
#define SW_CCW      PIN_PA3
#define SW_CW       PIN_PA6

#define ENABLE_SWITCH_INTERRUPTS 1 //for testing on this dev board which has these pins pulled low

#define HELD_DELAY 400

extern volatile bool switchHeldFlags[3];

enum SwitchID {
    SWITCH_PUSH,
    SWITCH_CW,
    SWITCH_CCW,
    SWITCH_CW_HELD,
    SWITCH_CCW_HELD,
    SWITCH_NONE
};

// Event type struct
struct SwitchEvent {
    SwitchID id; // Which switch was it
    bool isPressed; // Was it pressed
    bool isLongPressed; 
};

// Public functions
void HID_init();                                // Setup GPIO + interrupts
SwitchEvent HID_getEvent();                     // Get latest event (non-blocking)
void HID_testSimulatePress(SwitchID id);        // Simulate a press (for testing)
void HID_testSimulateLongPush(bool longPressNext);   // Simulate a long push (for testing)

#endif // HID_H
