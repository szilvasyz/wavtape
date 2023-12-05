#include "wavtape.h"

#ifdef DISP_SSD1306

U8X8_SSD1306_128X64_NONAME_HW_I2C disp;

char dBuf[DISP_COLS];

void dispInit() {
  disp.setI2CAddress(DISP_ADDR << 1);
  disp.begin();
  disp.setFont(u8x8_font_7x14_1x2_f);
//  disp.createChar(1, (const uint8_t *)F("\x00\x10\x08\x04\x02\x01\x00\x00"));
// disp.setBacklight(255);
}

void dispClear() {
  disp.clear();
}

void display(char * buf) {
  disp.print(buf);
  Serial.print(buf);
}

void display(char c) {
  disp.print(c);
  Serial.print(c);
}

void dispSetPos(int x, int y) {
  disp.setCursor(x, y * 2);
}

void dispLine(int line, char * buf) {
  int i, l;

  l = strlen(buf);
  for (i = 0; i < DISP_COLS; i++) dBuf[i] = ' ';

  if (l <= DISP_COLS) {
    for (i = 0; i < l; i++) dBuf[i] = buf[i];
  }
  else {
    for (i = 0; i < 3; i++) dBuf[i] = buf[i];
    for (i = 3; i < 6; i++) dBuf[i] = '.';
    for (i = 6; i < DISP_COLS; i++) dBuf[i] = buf[l - DISP_COLS + i];
  }
  disp.setCursor(0, 2 * line);
  for (i = 0; i < DISP_COLS; i++) disp.write(dBuf[i]);
  Serial.println(buf);
}

void dispError(char * buf) {
  dispLine2(buf);
  delay(2000);
}

void dispHeader(char * buf) {
#ifdef DISP_HEADER
  dispLine(DISP_HEADER, buf);
#endif
}

void dispLine1(char * buf) {
  dispLine(DISP_LINE1, buf);
}

void dispLine2(char * buf) {
  dispLine(DISP_LINE2, buf);
}

void dispButtons(char * buf) {
#ifdef DISP_BUTTONS
  dispLine(DISP_BUTTONS, buf);
#endif
}

#endif

