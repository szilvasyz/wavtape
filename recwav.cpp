#include "wavtape.h"


int preamp = 0;
int recNo = 0;
int rpaused = 0;


int openRecDir() {
  dir.close();
  if (dir.open(REC_DIR))
    return true;
    
  dispError("Creating rec dir");
  if (!sd.mkdir(REC_DIR, true))
    dispError("Can't create dir");
     
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


void recButtons1() {
  sprintf(sBuf, " ___ %s ESC REC", preamp ? "+12" : "  0");
  dispButtons(sBuf);
}


void recButtons2() {
  sprintf(sBuf, " ___ %s STP %s", preamp ? "+12" : "  0", rpaused ? "RES" : "PAU");
  dispButtons(sBuf);
}

int findRecName(int rno) {
  while (true) {
    sprintf(nBuf, "rec%05d.wav", rno);
    if (!dir.exists(nBuf))
      break;
    rno++;
  }
  return rno;
}


void record() {
  int b;

  dispHeader("Record");
  if (!openRecDir()) {
    return;
  }

  strcpy(pBuf, REC_DIR);
  if (pBuf[strlen(pBuf) - 1] != '/') {
    strcat(pBuf, "/");
  }
  recNo = findRecName(0);
  
  while (true) {

    dispLine1(pBuf);
    dispLine2(nBuf);
    recButtons1();

    while (button.peek() == 0);

    switch (button.get()) {
      
      case BTN_VAL_NEXT:
        preamp = 1 - preamp;
        Serial.print("Preamp :");
        Serial.println(preamp ? "12dB" : "0dB");
        break;

      case BTN_VAL_ABORT:
        return;

      case BTN_VAL_ENTER:
        Serial.print("File open: ");
        //Serial.println(dataFile.open(nBuf, O_RDWR | O_CREAT));
        Serial.println(file.createContiguous(&dir, nBuf, 512UL * 2048 * 20 ));
        Serial.print("Recording file ");
        Serial.println(nBuf);

        recFile(&file);
        file.close();
        recNo = findRecName(recNo);
        break;

    }
  }
}


void recFile(File32 *f) {
  uint16_t sr = REC_SAMPLERATE;
  uint32_t ds = 0, ss = 0;
  int rr = true;
  int recphase = 0;

  rpaused = 0;

  dispLine1(nBuf);
  dispLine2("");
  recButtons2();

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
    recButtons2();

    while (rr) {
      if (!rpaused) {
        f->write(PCM_getRecBuf(), PCM_BUFSIZ);
        PCM_releaseRecBuf();
        ss += PCM_BUFSIZ;
        if ((++ds % 100) == 0) {
          Serial.print(".");
          recphase = ++recphase % 4;
          disp.setCursor(0, DISP_LINE2);
          disp.write("\x01|/-"[recphase]);
          if (ds >= 4000) {
            Serial.println("!");
            ds = 0;
          }
        }
      }

      switch (button.get()) {

        case BTN_VAL_NEXT:
          preamp = 1 - preamp;
          setPreamp();
          recButtons2();
          break;

        case BTN_VAL_ABORT:
          sprintf(sBuf, "%ld bytes", ss);
          W.finalizeBuffer(ss);
          f->seekSet(0);
          f->write(W.getBuffer(), WAVHDR_LEN);
          f->truncate(ss + WAVHDR_LEN); 
          dispError(sBuf);
          rr = 0;
          break;

        case BTN_VAL_ENTER:
          rpaused = 1 - rpaused;
          recButtons2();
          break;

      }
    }
  }
  else
    dispError("Can't write file");
  
  Serial.println("Done.");
  Serial.println(PCM_stop());

}
