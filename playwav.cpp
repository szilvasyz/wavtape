#include "wavtape.h"


File32 *ff;
int atten = 0;
int paused = 0;
int pinvert = 0;


size_t readHandler(uint8_t *b, size_t s) {
  return ff->readBytes(b, s);
}


int wavInfo(File32 *f) {
  ff = f;
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
  sprintf(sBuf, " %s %s  STP %s", pinvert ? "INV" : "NRM", atten ? "LO" : "HI", paused ? "RES" : "PAU");
  dispButtons(sBuf);
}


void playWav(File32 * f) {

  uint16_t sr = 48000;
  uint32_t ds = 0, ss = 0;
  int pp = 1;
  int pct;
  int pct0 = 0;
  uint8_t *buf; 
  int wsup;
  int bcnt, bmax;
  int bphase;

  paused = 0;
  atten = defCfg.pAtt;
  pinvert = defCfg.pPhase;
  PCM_setPlayInv(pinvert);
  setAtten();
  playStatus();

  f->rewind();
  f->getName(sBuf, SBUF_SIZE);
  dispLine1(sBuf);
  dispLine2("");

  if (f->available()) {
    wsup = wavInfo(f);
    sprintf(sBuf, "Playing   %02uk%1u%c", 
      (uint16_t) (W.getData().sampleRate / 1000),
      W.getData().bitsPerSample,
      W.getData().numChannels < 2 ? 'M' : 'S');
    dispHeader(sBuf);

    if (wsup) {
      sr = W.getData().sampleRate;
      ds = W.getData().dataSize;

      Serial.print("Setup: ");
      Serial.println(PCM_setupPWM(sr, pinvert));
      Serial.print("Start play: ");
      delay(200);
      Serial.println(PCM_startPlay(defCfg.pNorm));

      PCM_clearOverrun();
      digitalWrite(RED_LED, LEDLOW);
      bmax = W.getData().sampleRate / PCM_BUFSIZ;
      bcnt = bmax;
      bphase = 0;

      while ((ss < ds) && pp) {
        if (!paused) {
          buf = PCM_getPlayBuf();
          //digitalWrite(GREEN_LED, LEDHIGH);
          f->read(buf, PCM_BUFSIZ);
          //digitalWrite(GREEN_LED, LEDLOW);
          PCM_pushPlayBuf();
          ss += PCM_BUFSIZ;
          pct = 100 * ss / ds;

          if (!--bcnt) {
            bcnt = bmax;
            dispSetPos(0, DISP_LINE2);
            sprintf(sBuf, "%c%d%%", "\x01|/-"[bphase++], pct);
            display(sBuf);
            bphase %= 4;
          }
          // if (pct != pct0) {
          //   pct0 = pct;
          //   if (((pct % 10) % 3) == 1) {
          //     dispSetPos(0, DISP_LINE2);
          //     display("\x01/-"[(pct % 10) / 3]);
          //     Serial.print(".");
          //   }
          //   if ((pct % 10) == 0) {
          //     sprintf(sBuf, "+%d%%", pct);
          //     dispLine2(sBuf);
          //     Serial.print(pct);
          //     Serial.print("%");
          //   }
          // }
        }

        if (PCM_getOverrun() != 0) digitalWrite(RED_LED, LEDHIGH);

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

  dispHeader("Playback");

}
