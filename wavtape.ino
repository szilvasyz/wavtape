#include "wavtape.h"


MyButton button(BTN_PIN_PREV, BTN_PIN_NEXT, BTN_PIN_ABORT, BTN_PIN_ENTER);
SdFat sd;
WAVhdr W;

File32 dir;
File32 file;
char nBuf[NBUF_SIZE + 1];
char pBuf[PBUF_SIZE + 1] = "/";
char sVer[DISP_COLS + 1];

uint16_t rsrates[] = REC_SRATES;
uint16_t numrsrates = size(rsrates);

tMenu mainMenu = {
  {text:"Playback", action:browse},
  {text:"Record", action:record},
  {text:"Settings", action:doSetMenu}
};

void(* resetFunc) (void) = 0;

int show = 0;
int sdready = 0;
char sBuf[SBUF_SIZE];


void setup() {
  int init_cnt = SD_INIT_TRY;

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  digitalWrite(GREEN_LED, LEDLOW);
  digitalWrite(RED_LED, LEDLOW);
  
  Serial.begin(115200);

  dispInit();
  button.begin();

  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);

  sprintf(sVer, "%s v%d.%d-%s", PRODUCT, VER_MAJOR, VER_MINOR, VER_NAME);
  dispHeader(sVer);
  delay(2000);
  dispLine1("Initializing...");
  sBuf[0] = '\0';

  while (!(sdready = sd.begin(SD_CONFIG)) && init_cnt--) {
    strcat(sBuf, "#");
    dispLine2(sBuf);
    digitalWrite(SD_CS, HIGH);
    delay(500);
  }

  if (!sdready) {
    dispLine2("No SD card.");
    while (getButton() == 0);
    resetFunc();
  }
  dispLine2("SD ok.");

  PCM_init(DIG_OUT_PIN, ANA_IN_PIN);
 
}  // setup()


void loop() {

  loadConfigEE();
  loadConfigSD();

  menuDo(sVer, "Select mode:", mainMenu, size(mainMenu));

}  // loop()



