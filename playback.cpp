#include "wavtape.h"


int nameMatch(char * buf, char * match) {
  if (strrchr(buf, '.'))
    if (strcasecmp(strrchr(buf, '.'), match) == 0)
      return true;
  return false;
}


int nextFile(int index) {
  int i, d;

  if (file.isOpen()) {
    file.close();
  }

  while (file.openNext(&dir, O_RDONLY)) {
    file.getName(nBuf, NBUF_SIZE);
    i = file.dirIndex();
    d = file.isDir();
    file.close();
    if (d || nameMatch(nBuf, FILEEXT))
      return i;
  }
  return index;
}


int prevFile(int index) {
  int i = index, j;

  dir.rewindDirectory();
  while ((j = nextFile(i)) != index) {
    // Serial.println(j);
    i = j;
  }
  return i;
}


int firstFile() {
  dir.rewindDirectory();
  return nextFile(0);
}


int lastFile() {
  int i = 0, j;
  while ((j = nextFile(i)) != i) { i = j; }
  return j;
}


int findFile(char * p) {
  int i;

  while (file.openNext(&dir, O_RDONLY)) {
    file.getName(nBuf, NBUF_SIZE);
    i = file.dirIndex();
    file.close();
    if (strcmp(nBuf, p) == 0)
      return i;
  }

  dir.rewindDirectory();
  return nextFile(0);
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
  int i, j;
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

    if ((i = nextFile(-1)) == -1) {
      dispError("No files");
      dir.close();
      if (!upDir())
        return;
    }
    else {

      i = findFile(sBuf);

      dirLoop = true;

      while (dirLoop) {

        if (file.isOpen()) {
          file.close();
          //delay(10);
        }
        // Serial.print(i);
        // Serial.print(":");
        // Serial.println(file.open(&dir, i, O_RDONLY));
        file.open(&dir, i, O_RDONLY);
        file.getName(nBuf, NBUF_SIZE);
        if (d = file.isDir()) strcat(nBuf, "/");

        file.close();

        dispLine1(pBuf);
        dispLine2(nBuf);
        dispButtons(" PRV NXT ESC SEL");

        while ((b = getButton()) == 0);

        switch (b) {

          case BTN_VAL_NEXT:
            j = nextFile(i);
            if (j == i) {
              j = firstFile();
            }
            i = j;
            break;

          case BTN_VAL_PREV:
            j = prevFile(i);
            if (j == i) {
              j = lastFile();
            }
            i = j;
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
              dir.close();
              dirLoop = false;
              break;
            }
            else {
              file.open(&dir, i, O_RDONLY);
              playWav(&file);
              file.close();
            }
        }
      }
    }
  }
}