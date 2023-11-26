#include "wavtape.h"


MyButton button(BTN_PIN_PREV, BTN_PIN_NEXT, BTN_PIN_ABORT, BTN_PIN_ENTER);
LiquidCrystal_PCF8574 disp(DISP_ADDR);
SdFat sd;
WAVhdr W;

File32 dir;
File32 file;
char nBuf[NBUF_SIZE];
char pBuf[PBUF_SIZE] = "/";


tMenu mainMenu = {
  {text:"Play", action:browse},
  {text:"Record", action:record},
  {text:"Settings", action:doSetMenu}
};


int show = 0;
int sdready = 0;
char sBuf[80];


void setup() {
  int init_cnt = SD_INIT_TRY;

  Serial.begin(115200);

  dispInit();
  button.begin();

  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);

  sprintf(sBuf, "%s v%d.%d-%s", PRODUCT, VER_MAJOR, VER_MINOR, VER_NAME);
  dispLine(0, sBuf);
  delay(2000);
  disp.setCursor(0, 1);

  while (!(sdready = sd.begin(SD_CONFIG)) && init_cnt--) {
    display("#");
    digitalWrite(SD_CS, HIGH);
    delay(500);
  }

  if (!sdready) {
    display(" - No SD card.");
    while (true);
  }

  PCM_init(DIG_OUT_PIN, ANA_IN_PIN);
 
}  // setup()


void loop() {

  menuDo(mainMenu, size(mainMenu));

}  // loop()



