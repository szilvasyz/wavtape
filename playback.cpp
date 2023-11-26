#include "wavtape.h"


int nameMatch(char * buf, char * match) {
  if (strrchr(buf, '.'))
    if (strcasecmp(strrchr(buf, '.'), match) == 0)
      return true;
  return false;
}


int nextFile(int index) {
  int i, d;

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
  while ((j = nextFile(i)) != index) i = j;
  return i;
}


int upDir() {
  if (pBuf[1] == '\0') return false;
  *strrchr(pBuf, '/') = '\0';
  *(strrchr(pBuf, '/') + 1) = '\0';
  return true;
}


void browse() {
  uint16_t b;
  int i;
  int d;
  int dirLoop;

  while (true) {
    dir.close();
    if (!dir.open(pBuf)) {
      dispLine(1, "Error opening dir");
      return;
    }
    
    if ((i = nextFile(0)) == 0) {
      dispLine(1, "no files");
      dir.close();
      if (!upDir())
        return;
    }
    else {
    
      dispLine(3, pBuf);

      dirLoop = true;

      while (dirLoop) {
        file.open(&dir, i, O_RDONLY);
        file.getName(nBuf, 60);
        if (d = file.isDir()) strcat(nBuf, "/");

        file.close();

        dispLine(2, i);
        dispLine(1, nBuf);

        while ((b = button.get()) == 0);

        switch (b) {

          case BTN_VAL_NEXT:
            i = nextFile(i);
            break;

          case BTN_VAL_PREV:
            i = prevFile(i);
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
              dispLine(3, "play");
              file.open(&dir, i, O_RDONLY);
              playWav(&file);
              file.close();
            }
        }
      }
    }
  }
}