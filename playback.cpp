#include "wavtape.h"


int index = 0;


int nameMatch(char * buf, char * match) {
  // Serial.printf("---Namematch\n");
  if (strrchr(buf, '.'))
    if (strcasecmp(strrchr(buf, '.'), match) == 0)
      return true;
  return false;
}


void nextFile() {
  int d;

  // Serial.printf("---Nextfile\n");
  if (file.isOpen()) {
    file.close();
  }

  while (file.openNext(&dir, O_RDONLY)) {
    file.getName(nBuf, NBUF_SIZE);
    index = file.dirIndex();
    d = file.isDir();
    // Serial.printf("Next: %i:%d:%s\n", index, d, nBuf);
    file.close();
    if (d || nameMatch(nBuf, FILEEXT)) {
      return;
    }
  }
  index = -1;
}


void prevFile() {
  // Serial.printf("---Prevfile\n");
  int i, j;

  j = -1;
  i = index;
  firstFile();
  do {
    if (index == i) {
      index = j;
      return;
    }
    j = index;
    nextFile();
  } while (index != -1);
}


void firstFile() {
  // Serial.printf("---Firstfile\n");
  dir.rewindDirectory();
  nextFile();
}


void lastFile() {
  // Serial.printf("---Lastfile\n");
  int i;
  firstFile();
  i = index;
  while (nextFile(), index != -1) { i = index; }
  index = i;
}


void findFile(char * p) {
  int i = 0, j;

  // Serial.printf("---Findfile\n");
  firstFile();
  do {
    // Serial.printf("** %i **\n", index);
    if (strcmp(nBuf, p) == 0)
      return;
    nextFile();
    // Serial.printf("**# %i #**\n", index);
  } while (index != -1);
}




int upDir() {
  char *p;
  if (pBuf[1] == '\0') return false;
  *strrchr(pBuf, '/') = '\0';
  p = strrchr(pBuf, '/');
  strcpy(nBuf, p + 1);
  *(p + 1) = '\0';
  return true;
}


void browse() {
  uint16_t b;
  int j;
  int d;
  int dirLoop;

  dispHeader("Playback");
  while (true) {
    dir.close();

    if (!dir.open(pBuf)) {
      dispError("Error opening dir");
      return;
    }

    strcpy(sBuf, nBuf);

    if (firstFile(), index == -1) {
      dispError("No files");
      dir.close();
      if (!upDir())
        return;
    }
    else {

      if (findFile(sBuf), index == -1) firstFile();

      dirLoop = true;

      while (dirLoop) {

        if (file.isOpen()) {
          file.close();
          //delay(10);
        }
        // Serial.print(index);
        // Serial.print(":");
        // Serial.println(sBuf);
        // Serial.println(file.open(&dir, i, O_RDONLY));
        file.open(&dir, index, O_RDONLY);
        file.getName(nBuf, NBUF_SIZE);
        if (d = file.isDir()) strcat(nBuf, "/");

        file.close();

        dispLine1(pBuf);
        dispLine2(nBuf);
        dispButtons(" PRV NXT ESC SEL");

        while ((b = getButton()) == 0);

        switch (b) {

          case BTN_VAL_NEXT:
            nextFile();
            if (index == -1) {
              firstFile();
            }
            break;

          case BTN_VAL_PREV:
            prevFile();
            if (index == -1) {
              lastFile();
            }
            break;

          case BTN_VAL_ABORT:
            dir.close();
            if (!upDir())
              return;
            dirLoop = false;
            break;

          case BTN_VAL_ENTER:
            if (d) {
              strcat(pBuf, nBuf);
              nBuf[0] = '\0';
              dir.close();
              dirLoop = false;
              break;
            }
            else {
              file.open(&dir, index, O_RDONLY);
              playWav(&file);
              file.close();
            }
        }
      }
    }
  }
}