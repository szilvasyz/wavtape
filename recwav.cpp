#include "wavtape.h"


int preamp = 0;
int recNo = 0;


int openRecDir() {
  dir.close();
  if (dir.open(REC_DIR))
    return true;
    
  Serial.println("No rec dir");
  if (!sd.mkdir(REC_DIR, true))
    Serial.println("Can't create recdir");
     
  return dir.open(REC_DIR);
}



void setPreamp() {
  if (preamp == 0) {
    digitalWrite(ANA_PREAMP_PIN, LOW);
    pinMode(ANA_PREAMP_PIN, INPUT);
  }
  else {
    digitalWrite(ANA_PREAMP_PIN, LOW);
    pinMode(ANA_PREAMP_PIN, OUTPUT);
  }
}


void recStatus() {
  sprintf(sBuf, " ___  %3i STOP PAUSE", preamp ? 12 : 0);
  dispStatus(sBuf);
}


void record() {
  int b;

  recNo = 0;

  if (!openRecDir()) {
    return;
  }

  Serial.println("Press to record");
  while (button.peek() == 0);

  switch (button.get()) {
    case 1:
      dir.getName(nBuf, NBUF_SIZE);
      Serial.println(nBuf);

      while (true) {
        sprintf(nBuf, "rec%05d.wav", recNo);
        if (!dir.exists(nBuf))
          break;
        recNo++;
      }
      Serial.print("File open: ");
      //Serial.println(dataFile.open(nBuf, O_RDWR | O_CREAT));
      Serial.println(file.createContiguous(&dir, nBuf, 512UL * 2048 * 20 ));
      Serial.print("Recording file ");
      Serial.println(nBuf);

      recFile(&file);
      file.close();
      recNo++;
      break;
    case 2:
      preamp = 1 - preamp;
      Serial.print("Preamp :");
      Serial.println(preamp ? "12dB" : "0dB");
      if (preamp) {
        digitalWrite(ANA_PREAMP_PIN, LOW);
        pinMode(ANA_PREAMP_PIN, OUTPUT);
      }
      else {
        digitalWrite(ANA_PREAMP_PIN, LOW);
        pinMode(ANA_PREAMP_PIN, INPUT);
      }
      break;

  }

}


void recFile(File32 *f) {
  uint16_t sr = REC_SAMPLERATE;
  uint32_t ds = 0, ss = 0;
  int rr = true;

  if (f->isWritable()) {
    Serial.print("!");
    //Serial.print(f->seekSet(1024L * 1024L));
    W.createBuffer(sr, 1, 8);
    f->seekSet(0);
    f->write(W.getBuffer(), WAVHDR_LEN);

    Serial.print("Setup: ");
    Serial.println(PCM_setupPWM(sr, 0));
    Serial.print("Start recording: ");
    Serial.println(PCM_startRec(true));

    setPreamp();
    recStatus();

    while (rr) {
      f->write(PCM_getRecBuf(), PCM_BUFSIZ);
      PCM_releaseRecBuf();
      ss += PCM_BUFSIZ;
      if ((++ds % 100) == 0) {
        Serial.print(".");
        if (ds >= 4000) {
          Serial.println("!");
          ds = 0;
        }
      }
      switch (button.get()) {
        case BTN_VAL_PREV:
          preamp = 1 - preamp;
          setPreamp();
          recStatus();
          break;
        case BTN_VAL_ABORT:
          Serial.print(" -break-");
          rr = 0;
          break;
        case BTN_VAL_ENTER:
          break;

      }
    }
  }
  else
    Serial.println("File not available.");
  Serial.println("Done.");

  Serial.print("Stop recording: ");
  Serial.println(PCM_stop());
  sprintf(sBuf, "%0ld bytes written.", ss);
  Serial.println(sBuf);

  W.finalizeBuffer(ss);
  f->seekSet(0);
  f->write(W.getBuffer(), WAVHDR_LEN);
  f->truncate(ss + WAVHDR_LEN);
}
