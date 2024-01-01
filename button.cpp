#include "button.h"

int getButton() {
  int b, c;
  b = button.get();
  if (b) return b;

  b = 0;
  if (Serial.available()) {
    switch (c = Serial.read()) {
      case '1': b = 1; break;
      case '2': b = 2; break;
      case '3': b = 4; break;
      case '4': b = 8; break;
    }
    Serial.print(c); Serial.print(",");
  }
  return b;
}
