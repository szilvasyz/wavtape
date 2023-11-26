#include "wavtape.h"


void dispInit() {
  //disp = new LiquidCrystal_PCF8574(DISP_ADDR); 
  disp.begin(DISP_COLS, DISP_ROWS);
  disp.setBacklight(255);
}

void dispClear() {
  disp.clear();
}

void display(char * buf) {
  disp.print(buf);
  Serial.print(buf);
}

void dispLine(int line, char * buf) {
  int l = min(strlen(buf), DISP_COLS);
  disp.setCursor(0, line);
  for (int i = 0; i < DISP_COLS; i++)
    disp.write(i < l ? buf[i] : ' ');
  Serial.println(buf);
}

void dispLine(int line, int i) {
  char b[10];
  sprintf(b, "%d", i);
  dispLine(line, b);
}

void dispStatus(char * buf) {
  dispLine(3, buf);
}
