#include "wavtape.h"


// #ifdef USE_SDFAT
//   File32 *ff;
// #endif

int atten = 0;
int paused = 0;
int pinvert = 0;


size_t readHandler(uint8_t *b, size_t s) {
  return fileio_fread(b, s);
}


int wavInfo() {
  return W.processBuffer(readHandler);
}


void setAtten() {
  if (atten == 0) {
    digitalWrite(ANA_ATTEN_PIN, LOW);
    pinMode(ANA_ATTEN_PIN, INPUT);
  }
  else {
    digitalWrite(ANA_ATTEN_PIN, LOW);
    pinMode(ANA_ATTEN_PIN, OUTPUT);
  }
}


void playStatus() {
  sprintf(sBuf, "  %c  %s STP %s", pinvert ? '-' : '+', atten ? "-12" : "  0", paused ? "RES" : "PAU");
  dispButtons(sBuf);
}


void playWav() {

  uint16_t sr = 48000;
  uint32_t ds = 0, ss = 0;
  int pp = 1;
  int pct;
  int pct0 = 0;

  paused = 0;
  pinvert = PCM_getPlayInv();
  setAtten();
  playStatus();

  fileio_frewind();
  strcpy(sBuf, fileio_filename());
  dispLine1(sBuf);
  dispLine2("");

  if (!fileio_feof()) {
    if (wavInfo()) {
      sr = W.getData().sampleRate;
      ds = W.getData().dataSize;

      Serial.print("Setup: ");
      Serial.println(PCM_setupPWM(sr, pinvert));
      Serial.print("Start play: ");
      delay(200);
      Serial.println(PCM_startPlay(true));

      PCM_clearOverrun();
      digitalWrite(RED_LED, LOW);

      while ((ss < ds) && pp) {
        if (!paused) {
          fileio_fread(PCM_getPlayBuf(), PCM_BUFSIZ);
          PCM_pushPlayBuf();
          ss += PCM_BUFSIZ;
          pct = 100 * ss / ds;
          if (pct != pct0) {
            pct0 = pct;
            if (((pct % 10) % 3) == 1) {
              dispSetPos(0, DISP_LINE2);
              display("\x01/-"[(pct % 10) / 3]);
              Serial.print(".");
            }
            if ((pct % 10) == 0) {
              sprintf(sBuf, "+%d%%", pct);
              dispLine2(sBuf);
              Serial.print(pct);
              Serial.print("%");
            }
          }
        }

        if (PCM_getOverrun() != 0) digitalWrite(RED_LED, HIGH);

        switch (getButton()) {

          case BTN_VAL_PREV:
            pinvert = !pinvert;
            PCM_setPlayInv(pinvert);
            playStatus();
            break;

          case BTN_VAL_NEXT:
            atten = 1 - atten;
            setAtten();
            playStatus();
            break;

          case BTN_VAL_ABORT:
            Serial.print(" -break-");
            pp = 0;
            break;

          case BTN_VAL_ENTER:
            paused = 1 - paused;
            PCM_setPause(paused);
            playStatus();
            break;
        }
      }
      Serial.println();
    }
    else {
      dispError("Unsupported file");
      Serial.println("Unsupported file");
    }
  }
  else {
    dispError("Can't open file");
    Serial.println("Can't open file");
  }

  Serial.print(PCM_getOverrun());
  Serial.println("overruns");

  dispError("Done");
  Serial.println("Done.");

  Serial.print("Stop play: ");
  Serial.println(PCM_stop());


}
