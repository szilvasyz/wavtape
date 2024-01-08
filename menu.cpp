#include "wavtape.h"


void menuDummy() {}


void menuDo(char* header, char* title, tMenu menu, int size) {
  int mPos = 0;
  uint16_t b;

  while (true) {
    dispHeader(header);
    dispButtons(" PRV NXT ESC SEL");
    dispLine1(title);
    dispLine2(menu[mPos].text);
    while ((b = getButton()) == 0);
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
