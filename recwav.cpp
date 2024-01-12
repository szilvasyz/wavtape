#include "wavtape.h"


int preamp = 0;
int recNo = 0;
int rpaused = 0;
int rinvert = 0;
uint16_t rsamplerate = REC_SRATE;


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
  sprintf(sBuf, " %02uk %s ESC REC", rsrates[rsamplerate] / 1000U, preamp ? "LO" : "HI");
  dispButtons(sBuf);
}


void recButtons2() {
  sprintf(sBuf, "  %s  %s STP %s", rinvert ? "INV" : "NRM", preamp ? "LO" : "HI", rpaused ? "RES" : "PAU");
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
  //int rsr = 0;

  rsamplerate = defCfg.rSRate;
  preamp = defCfg.rAmp;

  dispHeader("Record");
  if (!openRecDir()) {
    return;
  }

  strcpy(pBuf, REC_DIR);
  if (pBuf[strlen(pBuf) - 1] != '/') {
    strcat(pBuf, "/");
  }
  recNo = findRecName(0);
  
  dispLine1(pBuf);
  dispLine2(nBuf);
  recButtons1();

  while (true) {

    //while (button.peek() == 0);

    switch (getButton()) {

      case BTN_VAL_PREV:
        if (++rsamplerate >= numrsrates)
          rsamplerate = 0;
        // for (rsr = 0; rsr < numrsrates; rsr++)
        //   if (rsamplerate == rsrates[rsr])
        //     break;
        // if (++rsr >= numrsrates) rsr = 0;
        // rsamplerate = rsrates[rsr];
        recButtons1();
        break;

      case BTN_VAL_NEXT:
        preamp = 1 - preamp;
        Serial.print("Preamp :");
        Serial.println(preamp ? "12dB" : "0dB");
        recButtons1();
        break;

      case BTN_VAL_ABORT:
        return;

      case BTN_VAL_ENTER:
        Serial.print("File open: ");
        //Serial.println(dataFile.open(nBuf, O_RDWR | O_CREAT));
        Serial.println(file.createContiguous(&dir, nBuf, 512UL * 2048 * REC_PREALLOC_MB ));
        Serial.print("Recording file ");
        Serial.println(nBuf);

        recFile(&file, rsrates[rsamplerate]);
        file.close();
        recNo = findRecName(recNo);
        break;

    }
  }
}


void recFile(File32 *f, uint16_t sr) {
  uint32_t ds = 0, ss = 0;
  int rr = true;
  int recphase = 0;
  uint32_t overrun;

  rpaused = 0;
  rinvert = defCfg.rPhase;

  setPreamp();
  PCM_setRecInv(rinvert);

  dispLine1(nBuf);
  dispLine2("");
  recButtons2();

  if (f->isWritable()) {
    Serial.print("!");
    //Serial.print(f->seekSet(1024L * 1024L));
    W.createBuffer(sr, 1, 8);
    f->seekSet(0);
    f->write(W.getBuffer(), WAVHDR_LEN);

    recButtons2();

    Serial.print("Setup: ");
    Serial.println(PCM_setupPWM(sr, defCfg.pPhase));
    Serial.print("Start recording: ");
    delay(200);
    Serial.println(PCM_startRec(true));

    PCM_clearOverrun();
    digitalWrite(RED_LED, LEDLOW);

    while (rr) {
      if (!rpaused) {
        f->write(PCM_getRecBuf(), PCM_BUFSIZ);
        PCM_releaseRecBuf();
        ss += PCM_BUFSIZ;
        if ((++ds % 100) == 0) {
          Serial.print(".");
          recphase = ++recphase % 4;
          dispSetPos(0, DISP_LINE2);
          display("\x01|/-"[recphase]);
          if (ds >= 4000) {
            Serial.println("!");
            ds = 0;
          }
        }
      }

      if (PCM_getOverrun() != 0) digitalWrite(RED_LED, LEDHIGH);
      switch (getButton()) {

        case BTN_VAL_PREV:
          rinvert = !rinvert;
          PCM_setRecInv(rinvert);
          recButtons2();
          break;

        case BTN_VAL_NEXT:
          preamp = 1 - preamp;
          setPreamp();
          recButtons2();
          break;

        case BTN_VAL_ABORT:
          overrun = PCM_getOverrun();
          PCM_stop();

          sprintf(sBuf, "%ld bytes", ss);
          W.finalizeBuffer(ss);
          f->seekSet(0);
          f->write(W.getBuffer(), WAVHDR_LEN);
          f->truncate(ss + WAVHDR_LEN); 
          dispError(sBuf);
          Serial.print(overrun);
          Serial.println("overruns");
          rr = 0;
          break;

        case BTN_VAL_ENTER:
          rpaused = 1 - rpaused;
          PCM_setPause(rpaused);
          recButtons2();
          break;

      }
    }
  }
  else
    dispError("Can't write file");
  
  Serial.println("Done.");

}
