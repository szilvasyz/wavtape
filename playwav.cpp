#include "wavtape.h"


File32 *ff;
int atten = 0;


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
  sprintf(sBuf, " ___  %3i STOP PAUSE", atten ? -12 : 0);
  dispStatus(sBuf);
}


void playWav(File32 * f) {

  uint16_t sr = 48000;
  uint32_t ds = 0, ss = 0;
  int pp = 1;
  int pct;
  int pct0 = 0;

  setAtten();
  playStatus();

  f->rewind();
  if (f->available()) {
    if (wavInfo(f)) {
      sr = W.getData().sampleRate;
      ds = W.getData().dataSize;

      Serial.print("Setup: ");
      Serial.println(PCM_setupPWM(sr, 0));
      Serial.print("Start play: ");
      Serial.println(PCM_startPlay(true));

      while ((ss < ds) && pp) {
        f->read(PCM_getPlayBuf(), PCM_BUFSIZ);
        PCM_pushPlayBuf();
        ss += PCM_BUFSIZ;
        pct = 100 * ss / ds;
        if (pct != pct0) {
          pct0 = pct;
          if (((pct % 10) % 3) == 1) Serial.print(".");
          if ((pct % 10) == 0) {
            Serial.print(pct);
            Serial.print("%");
          }
        }
        switch (button.get()) {
          case 1:
            Serial.print(" -break-");
            pp = 0;
            break;
          case 2:
            atten = 1 - atten;
            setAtten();
            playStatus();
          break;
        }
      }
      Serial.println();
    }
    else {
      Serial.println("Invalid file.");
    }
  }
  else
    Serial.println("File not available.");

  Serial.println("Done.");

  Serial.print("Stop play: ");
  Serial.println(PCM_stop());


}
