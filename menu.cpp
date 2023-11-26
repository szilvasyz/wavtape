#include "wavtape.h"


void menuDummy() {}


void menuDo(tMenu menu, int size) {
  int mPos = 0;
  uint16_t b;

  while (true) {
    disp.setCursor(0, 0);
    disp.printf("%16s", menu[mPos].text);
    while ((b = button.get()) == 0);
    switch (b) {
      case BTN_VAL_NEXT:
        if (++mPos == size) mPos = 0;
        break;
      case BTN_VAL_PREV:
        if (mPos-- == 0) mPos = size - 1;
        break;
      case BTN_VAL_ABORT:
        return;
        break;
      case BTN_VAL_ENTER:
        menu[mPos].action();
        break;
    }
  }
}
