#include "HID.h"

static volatile bool switchFlags[3] = {false, false, false};
volatile bool switchHeldFlags[3] = {false, false, false};
static uint16_t lastHeldTime[3] = {0};
static uint8_t switchLongPressFlags[3] = {0};
static uint16_t lastPressTime[3] = {0, 0, 0};
const uint16_t DEBOUNCE_DELAY = 120;  // ms

volatile bool isPressed;
volatile bool pushHeld;        
volatile unsigned long pushPressTime = 0; 
volatile bool pushShortPress = false;   
volatile bool pushLongPress = false;   
const unsigned long LONG_PRESS_MS = 950; // define long press threshold

void HID_init() {
    pinMode(SW_PUSH, INPUT_PULLUP);
    pinMode(SW_CCW,  INPUT_PULLUP);
    pinMode(SW_CW,   INPUT_PULLUP);

#if defined(ENABLE_SWITCH_INTERRUPTS)
    // Enable pin-change interrupts on these pins (PORTA)
    PORTA.PIN3CTRL = PORT_ISC_BOTHEDGES_gc | PORT_PULLUPEN_bm; // CCW
    PORTA.PIN6CTRL = PORT_ISC_BOTHEDGES_gc | PORT_PULLUPEN_bm; // CW
    PORTA.PIN7CTRL = PORT_ISC_BOTHEDGES_gc | PORT_PULLUPEN_bm; // PUSH
#endif  
}

// === Interrupt Service Routine ===
ISR(PORTA_PORT_vect) {
    uint8_t flags = VPORTA.INTFLAGS;
    VPORTA.INTFLAGS = flags;
    uint16_t now = millis();

    // CCW (PA3)
    if (flags & PIN3_bm) {
        bool pressed = !(VPORTA.IN & PIN3_bm);
        if (pressed) {
            if (now - lastPressTime[2] > DEBOUNCE_DELAY) {
                switchFlags[2] = true;
                lastPressTime[2] = now;
            }
            switchHeldFlags[2] = true;
        } else {
            switchHeldFlags[2] = false;
        }
    }

    // CW (PA6)
    if (flags & PIN6_bm) {
        bool pressed = !(VPORTA.IN & PIN6_bm);
        if (pressed) {
            if (now - lastPressTime[1] > DEBOUNCE_DELAY) {
                switchFlags[1] = true;
                lastPressTime[1] = now;
            }
            switchHeldFlags[1] = true;
        } else {
            switchHeldFlags[1] = false;
        }
    }

    // PUSH (PA7) unchanged
    if (flags & PIN7_bm) {
        bool pressed = !(VPORTA.IN & PIN7_bm);
        if (pressed && !pushHeld) {
            pushHeld = true;
            pushPressTime = now;
        } else if (!pressed && pushHeld) {
            pushHeld = false;
            uint16_t pressDuration = now - pushPressTime;
            if (pressDuration < LONG_PRESS_MS) pushShortPress = true;
            else pushLongPress = true;
        }
    }
}


// === Poll events (non-blocking) ===
SwitchEvent HID_getEvent() {
    SwitchEvent event = { SWITCH_NONE, false, false };
    uint16_t now = millis();

    // --- PUSH button ---
    if (pushShortPress || pushLongPress) {
        event.id = SWITCH_PUSH;
        event.isPressed = true;
        event.isLongPressed = pushLongPress;
        pushShortPress = pushLongPress = false;
        return event;
    }

    // --- CW / CCW detection (edge + held) ---
    for (uint8_t i = 1; i <= 2; i++) {
        // edge-detected press
        if (switchFlags[i]) {
            noInterrupts();
            switchFlags[i] = false;
            interrupts();
            event.id = (i == 1) ? SWITCH_CW : SWITCH_CCW;
            event.isPressed = true;
            if (switchHeldFlags[i]) lastHeldTime[i] = now;
            return event;
        }

        // held/long press
        if (switchHeldFlags[i]) {
            if (!switchLongPressFlags[i] && now - lastHeldTime[i] >= HELD_DELAY)
                switchLongPressFlags[i] = true;

            if (switchLongPressFlags[i]) {
                event.id = (i == 1) ? SWITCH_CW_HELD : SWITCH_CCW_HELD;
                event.isPressed = true;
                return event;
            }
        } else {
            switchLongPressFlags[i] = false;
            lastHeldTime[i] = now;
        }
    }

    return event;
}




