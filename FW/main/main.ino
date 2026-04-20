#include "Display.h"
#include "UI.h"
#include "HID.h"
#include "BQ25798.h"
#include "HW.h"

uint8_t rightCol = SSD1306_X_origin + SSD1306_X_pixels - 1;

uint16_t I_BAT_SET = 60;
uint16_t V_OTG_SET = 2800;
uint16_t I_OTG_SET = 160;  // Lower limit of BQ

// For software PWM control of low VOTG
uint8_t votgPwmCounter = 0;
uint8_t votgDuty = 255;
uint32_t lastPwmUpdate = 0;

// create a Display instance
Display display;
UI ui(display);
BQ25798 bq;
UIPage currentPage;

void setup() {
  delay(1000);
  HID_init();
  display.begin();
  bq.begin();
  VPORTA.INTFLAGS = 0xFF;
  display.clearScreen();
}


void VOTG_SW_PWM(uint16_t V_OTG_SET) {
  static bool state = false;

  if (V_OTG_SET < 2800) {
    state = !state;
    if (state) bq.VOTG_ON();
    else bq.VOTG_OFF();
  } else {
    bq.VOTG_ON();
  }
}



void loop() {
  static unsigned long lastInputTime = 0;
  unsigned long now = millis();

  ui.updatePage(!bq.isCharging());  //change what page is displayed

  ui.renderStaticIfChanged();  //180uS
  //delay(1);

  SwitchEvent localEvent = HID_getEvent();

  if (localEvent.isPressed) {
    lastInputTime = now;  // reset idle timer

    switch (localEvent.id) {
      case SWITCH_PUSH:
        if (localEvent.isLongPressed) {
          // Long press toggles VOTG border
          if (ui.getCurrentPage() == PAGE_V_OTG) {
            bq.toggleVOTG();
            display.toggleBorder();
          }
        } else {
          // Short press cycles active page
          if (display.getActivePage() != PAGE_NONE) {
            // Clear the previous active page first
            display.clearColumn(display.getActivePage(), rightCol);

            // Cycle to next page
            if (ui.getCurrentPage() == PAGE_V_OTG) {
              if (display.getActivePage() == PAGE_2) {
                display.setActivePage(PAGE_3);
              } else {
                display.setActivePage(PAGE_2);
              }
            }
          } else {
            // No active page, start with PAGE_2
            display.setActivePage(PAGE_2);
          }

          display.indicateActivePage();  // highlight new active page
        }
        break;

      case SWITCH_CCW:
        if (ui.getCurrentPage() == PAGE_V_OTG) {
          if (display.getActivePage() == PAGE_2) {
            V_OTG_SET = bq.incOTGVoltage(V_OTG_SET, 10);
            bq.setOTG_Voltage(V_OTG_SET);
          } else if (display.getActivePage() == PAGE_3) {
            I_OTG_SET = bq.incOTGCurrent(I_OTG_SET);
            bq.setOTG_Current(I_OTG_SET);
          }
        } else {
          if (display.getActivePage() == PAGE_2) {
            I_BAT_SET = bq.incChargeCurrent(I_BAT_SET);
            bq.setChargeCurrent(I_BAT_SET);
          }
        }
        break;

      case SWITCH_CCW_HELD:
        if (ui.getCurrentPage() == PAGE_V_OTG) {
          if (display.getActivePage() == PAGE_2) {
            V_OTG_SET = bq.incOTGVoltage(V_OTG_SET, 10);
            bq.setOTG_Voltage(V_OTG_SET);
          }
        } else {
          if (display.getActivePage() == PAGE_2) {
            I_BAT_SET = bq.incChargeCurrent(I_BAT_SET);
            bq.setChargeCurrent(I_BAT_SET);
          }
        }
        break;

      case SWITCH_CW:
        if (ui.getCurrentPage() == PAGE_V_OTG) {
          if (display.getActivePage() == PAGE_2) {
            V_OTG_SET = bq.decOTGVoltage(V_OTG_SET, 10);
            bq.setOTG_Voltage(V_OTG_SET);
          } else if (display.getActivePage() == PAGE_3) {
            I_OTG_SET = bq.decOTGCurrent(I_OTG_SET);
            bq.setOTG_Current(I_OTG_SET);
          }
        } else {
          if (display.getActivePage() == PAGE_2) {
            I_BAT_SET = bq.decChargeCurrent(I_BAT_SET);
            bq.setChargeCurrent(I_BAT_SET);
          }
        }
        break;

      case SWITCH_CW_HELD:
        if (ui.getCurrentPage() == PAGE_V_OTG) {
          if (display.getActivePage() == PAGE_2) {
            V_OTG_SET = bq.decOTGVoltage(V_OTG_SET, 10);
            bq.setOTG_Voltage(V_OTG_SET);
          }
        } else {
          if (display.getActivePage() == PAGE_2) {
            I_BAT_SET = bq.decChargeCurrent(I_BAT_SET);
            bq.setChargeCurrent(I_BAT_SET);
          }
        }
        break;

      default:

        break;
    }
  }

  //Clear active page timout
  if (now - lastInputTime > 1500) {
    display.clearColumn(display.getActivePage(), rightCol);
    display.setActivePage(PAGE_NONE);
  }

  //PAGE_CHARGE
  uint16_t V_BAT = bq.readV_Bat();
  int16_t I_BAT = abs(bq.readI_Bat());
  uint16_t V_BUS = bq.readV_Bus();

  //PAGE_V_OTG
  uint16_t V_OTG = V_BUS;
  int16_t I_OTG = abs(bq.readI_Bus());

  // Protection check
  //if (V_BAT < 10600) {


  if (V_BAT < 10500 || I_BAT > 3000) {
    bq.VOTG_OFF();
    display.clearBorder();
  }

  /*
  // SW PWM
  if (V_OTG_SET < 2800) {
    VOTG_SW_PWM(V_OTG_SET);
  } else {*/
  ui.renderDynamic(V_BAT, I_BAT, I_BAT_SET, V_BUS, V_OTG, I_OTG, V_OTG_SET, I_OTG_SET);  //3ms
  //}
}
