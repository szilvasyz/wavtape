#include "Arduino.h"
#include "config.h"
#ifdef USE_SDFAT


#include <SdFat.h>


#define SPI_CLOCK SD_SCK_MHZ(1)
#define SD_CONFIG SdSpiConfig(SD_CS, DEDICATED_SPI, SPI_CLOCK)
SdFat sd;
File32 dir;
File32 file;

char nBuf[NBUF_SIZE + 1];
char pBuf[PBUF_SIZE + 1] = "/";
extern char * sBuf;
int isdir;
int dpos;


int nameMatch(char * buf, char * match) {
  if (strrchr(buf, '.'))
    if (strcasecmp(strrchr(buf, '.'), match) == 0)
      return true;
  return false;
}


int fileio_init(char * recdir) {
  int r;
  int init_cnt = SD_INIT_TRY;

  while (!(r = sd.begin(SD_CONFIG)) && init_cnt--) {
    digitalWrite(SD_CS, HIGH);
    delay(500);
  }

  if (!r) {
    Serial.println("Init failed.");
    return false;
  }

  dir.close();
  if (!dir.open(recdir)) {
    Serial.println("Creating rec dir");
    if (!sd.mkdir(recdir, true)) {
      Serial.println("Can't create dir");
      return false;
    }
  }
  dir.close();
  return dir.open("/");
}


int fileio_getlabel(char * l) {
  strcpy(l, "SDFAT");
  return true;
};

int fileio_first(char * d, char * m) {
  dir.close();
  dpos = 0;
  int r;

  if (!dir.open(d)) 
    return false;

  strcpy(pBuf, d);
  r = file.openNext(&dir, O_RDONLY);
  while (r) {
    file.getName(nBuf, NBUF_SIZE);
    isdir = file.isDir();
    file.close();
    if (nameMatch(nBuf, m) || isdir) {
      return true;
    }
    r = file.openNext(&dir, O_RDONLY);
  }

  return false;
}


int fileio_next(char * m) {
  int r;

  r = file.openNext(&dir, O_RDONLY);
  while (r) {
    file.getName(nBuf, NBUF_SIZE);
    isdir = file.isDir();
    file.close();
    if (nameMatch(nBuf, m) || isdir) {
      dpos++;
      return true;
    }
    r = file.openNext(&dir, O_RDONLY);
  }

  return false;
}


int fileio_prev(char * m) {
  int r, c;

  if (dpos == 0)
    return false;

//  dir.close();
  // if (!dir.open(pBuf)) 
  //   return false;
  dir.rewindDirectory();

  //if (file.isOpen()) file.close();
  r = file.openNext(&dir, O_RDONLY);
  c = dpos;
  while (r) {
    nBuf[0] = '\0';
    file.getName(nBuf, NBUF_SIZE);
    isdir = file.isDir();
    //if (file.isOpen()) file.close();
    file.close();
    if (nameMatch(nBuf, m) || isdir) {
      delay(1);
      if (--c == 0) {
        dpos--;
        return true;
      }
    }
    //if (file.isOpen()) file.close();
    r = file.openNext(&dir, O_RDONLY);
  }

  return false;
}


char * fileio_filename() {
  return nBuf;
}


char * fileio_path() {
  return pBuf;
}


int fileio_isdir() {
  return isdir;
}


int fileio_dirpos() {
  return dpos;
}


int fileio_fopenr(char * name) {
  if (file.isOpen()) file.close();
  return file.open(name, O_RDONLY);
}


int fileio_fopenw(char * name) {
  if (file.isOpen()) file.close();
  return file.createContiguous(name, 512UL * 2048 * 20 );
}


int fileio_fclose() {
  return file.close();
}


int fileio_fread(uint8_t * b, int l) {
  return file.readBytes(b, l);
}


int fileio_fwrite(uint8_t * b, int l) {
  return file.write(b, l);
}


void fileio_frewind() {
  file.rewind();
}


int fileio_ftrunc(long l) {
  return file.truncate(l);
}


int fileio_fseek(long l) {
  return file.seekSet(l);
}


int fileio_feof() {
  return !file.available();
}


int fileio_ferror() {
  return file.getError();
}


int fileio_dopen(char * name) {
  dir.close();
  strcpy(pBuf, name);
  if (int n = strlen(pBuf)) {
    if (pBuf[n - 1] != '/') {
      pBuf[n] = '/'; pBuf[n + 1] = '\0';
    }
  }
  else {
    pBuf[0] = '/'; pBuf[1] = '\0';
  }
  return dir.open(pBuf);
}


int fileio_fexists(char * name) {
  return dir.exists(name);
}


#endif
