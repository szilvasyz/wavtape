#include "wavtape.h"


int upDir() {
  char * p = fileio_path();
  if (p[1] == '\0') return false;
  *strrchr(p, '/') = '\0';
  *(strrchr(p, '/') + 1) = '\0';
  return true;
}


void browse() {
  uint16_t b;
  int i;
  int d;
  int dirLoop;

  dispHeader("Playback");
  while (true) {
    // dir.close();

    // if (!dir.open(pBuf)) {
    //   dispError("Error opening dir");
    //   return;
    // }

    Serial.println(fileio_path());
    
    if (!fileio_first(fileio_path(), ".wav")) {
      dispError("No files");
      if (!upDir())
        return;
    }
    else {

      dirLoop = true;

      while (dirLoop) {

        strcpy(sBuf, fileio_filename());
        if (d = fileio_isdir()) strcat(sBuf, "/");


        dispLine1(fileio_path());
        dispLine2(sBuf);
        dispButtons(" PRV NXT ESC SEL");

        while ((b = getButton()) == 0);

        switch (b) {

          case BTN_VAL_NEXT:
            fileio_next(".wav");
            break;

          case BTN_VAL_PREV:
            fileio_prev(".wav");
            break;

          case BTN_VAL_ABORT:
            if (!upDir())
              return;
            dirLoop = false;
            break;

          case BTN_VAL_ENTER:
            strcpy(sBuf, fileio_path());
            strcat(sBuf, fileio_filename());
            if (d) {
              strcpy(fileio_path(), sBuf);
              dirLoop = false;
              break;
            }
            else {
              fileio_fopenr(sBuf);
              playWav();
              fileio_fclose();
            }
        }
      }
    }
  }
}