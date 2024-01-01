#include "wavtape.h"


MyButton button(BTN_PIN_PREV, BTN_PIN_NEXT, BTN_PIN_ABORT, BTN_PIN_ENTER);
WAVhdr W;

// #ifdef USE_SDFAT
//   SdFat sd;
//   File32 dir;
//   File32 file;
// #endif

// char nBuf[NBUF_SIZE + 1];
// char pBuf[PBUF_SIZE + 1] = "/";
char sVer[DISP_COLS + 1];


tMenu mainMenu = {
  {text:"Playback", action:browse},
  {text:"Record", action:record},
  {text:"Settings", action:doSetMenu}
};

void(* resetFunc) (void) = 0;

int show = 0;
int sdready = 0;
char sBuf[SBUF_SIZE + 1];


void setup() {

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);
  
  Serial.begin(115200);

  dispInit();
  button.begin();

  pinMode(SD_CS, OUTPUT);
  pinMode(PIN_SPI_MOSI, OUTPUT);
  pinMode(PIN_SPI_SCK, OUTPUT);
  pinMode(PIN_SPI_MISO, INPUT);
  digitalWrite(SD_CS, HIGH);
  digitalWrite(PIN_SPI_MOSI, LOW);
  digitalWrite(PIN_SPI_SCK, LOW);

  sprintf(sVer, "%s v%d.%d-%s", PRODUCT, VER_MAJOR, VER_MINOR, VER_NAME);
  dispHeader(sVer);
  delay(1000);
  dispLine1("Initializing...");

  if (!fileio_init(REC_DIR)) {
    dispLine2("No SD card.");
    while (getButton() == 0);
    resetFunc();
  }
  dispLine2("SD ok.");

  PCM_init(DIG_OUT_PIN, ANA_IN_PIN);
 
}  // setup()


void loop() {

  menuDo(sVer, "Select mode:", mainMenu, size(mainMenu));

}  // loop()



