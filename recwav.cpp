#include "wavtape.h"


int preamp = 0;
int recNo = 0;
int rpaused = 0;
int rinvert = 0;
uint16_t rsamplerate = REC_SAMPLERATE;
uint16_t rsrates[] = {8000, 11025, 16000, 22100, 24000, 32000, 44100, 48000};


int openRecDir() {
  return fileio_dopen(REC_DIR);
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
  sprintf(sBuf, " %02uk %s ESC REC", rsamplerate / 1000U, preamp ? "+12" : "  0");
  dispButtons(sBuf);
}


void recButtons2() {
  sprintf(sBuf, "  %c  %s STP %s", rinvert ? '-' : '+', preamp ? "+12" : "  0", rpaused ? "RES" : "PAU");
  dispButtons(sBuf);
}


int findRecName(int rno) {
  while (true) {
    sprintf(sBuf, "rec%05d.wav", rno);
    if (!fileio_fexists(sBuf))
      break;
    rno++;
  }
  return rno;
}


void record() {
  int b;
  int rsr = 0;

  dispHeader("Record");
  if (!openRecDir()) {
    return;
  }

  recNo = findRecName(0);
  
  while (true) {

    sprintf(sBuf, "rec%05d.wav", recNo);

    dispLine1(fileio_path());
    dispLine2(fileio_filename());
    recButtons1();

    while (button.peek() == 0);

    switch (button.get()) {

      case BTN_VAL_PREV:
        for (rsr = 0; rsrates[rsr] != 0; rsr++)
          if (rsamplerate == rsrates[rsr])
            break;
        if (rsrates[rsr] == 0) rsr =0;
        if (rsrates[++rsr] == 0) rsr = 0;
        rsamplerate = rsrates[rsr];
        recButtons2();
        break;

      case BTN_VAL_NEXT:
        preamp = 1 - preamp;
        Serial.print("Preamp :");
        Serial.println(preamp ? "12dB" : "0dB");
        break;

      case BTN_VAL_ABORT:
        return;

      case BTN_VAL_ENTER:
        Serial.print("Recording file ");
        sprintf(sBuf, "rec%05d.wav", recNo);
        Serial.println(sBuf);
        sprintf(sBuf, "%srec%05d.wav", fileio_path(), recNo);
        //Serial.println(dataFile.open(sBuf, O_RDWR | O_CREAT));
        Serial.print("File open: ");
        Serial.println(fileio_fopenw(sBuf));

        recFile(rsamplerate);
        fileio_fclose();
        recNo = findRecName(recNo);
        break;

    }
  }
}


void recFile(uint16_t sr) {
  uint32_t ds = 0, ss = 0;
  int rr = true;
  int recphase = 0;
  uint32_t overrun;

  rpaused = 0;
  rinvert = PCM_getRecInv();

  dispLine1(fileio_filename());
  dispLine2("");
  recButtons2();

  if (!fileio_ferror()) {
    Serial.print("!");
    //Serial.print(f->seekSet(1024L * 1024L));
    W.createBuffer(sr, 1, 8);
    fileio_fseek(0);
    fileio_fwrite(W.getBuffer(), WAVHDR_LEN);

    setPreamp();
    recButtons2();

    Serial.print("Setup: ");
    Serial.println(PCM_setupPWM(sr, 0));
    Serial.print("Start recording: ");
    delay(200);
    Serial.println(PCM_startRec(true));

    PCM_clearOverrun();
    digitalWrite(RED_LED, LOW);

    while (rr) {
      if (!rpaused) {
        fileio_fwrite(PCM_getRecBuf(), PCM_BUFSIZ);
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

      if (PCM_getOverrun() != 0) digitalWrite(RED_LED, HIGH);
      switch (button.get()) {

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
          fileio_fseek(0);
          fileio_fwrite(W.getBuffer(), WAVHDR_LEN);
          fileio_ftrunc(ss + WAVHDR_LEN); 
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
