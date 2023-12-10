#include "Arduino.h"
#include "config.h"
#ifdef USE_CHANFF


extern "C" {
#include "ff.h"
#include "diskio.h"
#include "spiavr.h"
#include "mmc.h"
}


#define FAT_BUF_SIZE        256


#define _delay_ms(m) delay(m)


char nBuf[NBUF_SIZE + 1];
char pBuf[PBUF_SIZE + 1] = "/";
extern char * sBuf;

FATFS g_fs;
DIR g_dir;
FIL g_fil;
uint8_t g_fat_buffer[FAT_BUF_SIZE];

DWORD sn = 0;
FILINFO fno;
FRESULT res;
int dpos = 0;


#if defined(__AVR_ATmega32__)
ISR(TIMER2_COMP_vect) {
#else
ISR(TIMER2_COMPA_vect) {
#endif
  disk_timerproc(); // Drive timer procedure for FatFs low level disk I/O module
}


DWORD get_fattime (void) {
  /* Returns current time packed into a DWORD variable */
  return ((DWORD)(2014 - 1980) << 25) /* Year 2014 */
            | ((DWORD)6 << 21)        /* Month 6 */
            | ((DWORD)1 << 16)        /* Mday 1 */
            | ((DWORD)0 << 11)        /* Hour 0 */
            | ((DWORD)0 << 5)         /* Min 0 */
            | ((DWORD)0 >> 1);        /* Sec 0 */
}


void disk_timer_setup() {
#if defined(__AVR_ATmega32__)
  TCCR2 = 0;

  OCR2 = F_CPU / 1024 / 100 - 1; // 100Hz timer
  TCCR2 = _BV(WGM21);            // CTC Mode
  TCCR2 |=  (1 << CS22) | (1 << CS21) | (1 << CS20);  //pre-scaler 1024 
  TIMSK |= _BV(OCIE2);
#else
  TCCR2A = 0;
  TCCR2B = 0;

  OCR2A = F_CPU / 1024 / 100 - 1; // 100Hz timer
  TCCR2A = _BV(WGM21);            // CTC Mode
  TCCR2B |=  (1 << CS22) | (1 << CS21) | (1 << CS20);  //pre-scaler 1024 
  TIMSK2 |= _BV(OCIE2A);
#endif
}


int nameMatch(char * buf, char * match) {
  if (strrchr(buf, '.'))
    if (strcasecmp(strrchr(buf, '.'), match) == 0)
      return true;
  return false;
}


int fileio_init(char * recdir) {
  int tmp, res;

  disk_timer_setup();
  disk_initialize(0);

  // something (possibly) dodgy in the bootloader causes a fail on cold boot.
  // retrying here seems to fix it (could just be the bootloader on my cheap Chinese clone?)
  for (tmp = 0; tmp < 10; tmp++) {
    res = f_mount(&g_fs, "", 1);
    _delay_ms(200);
    if (res == FR_OK) break;
  }

  SPI_Speed_Fast();

  if (res == FR_OK) {
    // attempt to open the recording dir
    strcpy((char*)g_fat_buffer, recdir);
    Serial.println((char*)g_fat_buffer);
    res = f_opendir(&g_dir, (char*)g_fat_buffer);
    Serial.println(res);
    if (res != FR_OK) { // try to make it if its not there
      res = f_mkdir((char*)g_fat_buffer);
      Serial.println(res);
      if (res != FR_OK || f_opendir(&g_dir, (char*)g_fat_buffer) != FR_OK) {
        Serial.println(res);
        Serial.println("Init failed.");
        Serial.println("Mkdir failed.");
        return 0;
      }
    }
    res = f_opendir(&g_dir, "/");
    Serial.println(res);
  } else {
    Serial.println("Init failed.");
  }

  //Serial.println("End init");
  return(res == FR_OK);

}


int fileio_getlabel(char * l) {
  return f_getlabel("", l, &sn);
}


int fileio_first(char * d, char * m) {
  dpos = 0;
  res = f_findfirst(&g_dir, &fno, d, "*");
  if (res != FR_OK)
    return false;

  strcpy(pBuf, d);
  while ((res == FR_OK) && fno.fname[0]) {
    if (nameMatch(fno.fname, m) || (fno.fattrib & AM_DIR)) {
      return true;
    }
    res = f_findnext(&g_dir, &fno);
  }
  return false;
}


int fileio_next(char * m) {
  res = f_findnext(&g_dir, &fno);
  if (res != FR_OK)
    return false;

  while ((res == FR_OK) && fno.fname[0]) {
    if (nameMatch(fno.fname, m) || (fno.fattrib & AM_DIR)) {
      dpos++;
      return true;
    }
    res = f_findnext(&g_dir, &fno);
  }
  return false;
}


int fileio_prev(char * m) {
  int c;

  if (dpos == 0)
    return false;

  res = f_rewinddir(&g_dir);
  if (res != FR_OK) {
    //Serial.print("rewind"); Serial.println(res);
    return false;
  }
  res = f_findnext(&g_dir, &fno);
  if (res != FR_OK) {
    //Serial.print("findnext"); Serial.println(res);
    return false;
  }

  c = dpos;
  while ((res == FR_OK) && fno.fname[0]) {
    if (nameMatch(fno.fname, m) || (fno.fattrib & AM_DIR)) {
      delay(1);
      if (--c == 0) {
        dpos--;
        return true;
      }
    }
    res = f_findnext(&g_dir, &fno);
  }

  //Serial.print("loop"); Serial.println(res);
  return false;

}


char * fileio_filename() {
  return fno.fname;
}


char * fileio_path() {
  return pBuf;
}


int fileio_isdir() {
  return (fno.fattrib & AM_DIR) != 0; 
}


int fileio_dirpos() {
  return dpos; 
}


int fileio_fopenr(char * name) {
  f_close(&g_fil);
  f_open(&g_fil, name, FA_CREATE_ALWAYS | FA_WRITE);
  return f_expand(&g_fil, 512UL * 2048 * 20, 1);
}


int fileio_fopenw(char * name) {
  f_close(&g_fil);
  return f_open(&g_fil, name, FA_READ);
}


int fileio_fclose() {
  return f_close(&g_fil);
}


int fileio_fread(uint8_t * b, int l) {
  unsigned int r;
  f_read(&g_fil, b, l, &r);
  return r;
}


int fileio_fwrite(uint8_t * b, int l) {
  unsigned int w;
  f_write(&g_fil, b, l, &w);
  return w;
}


void fileio_frewind() {
  f_lseek(&g_fil, 0);
}


int fileio_ftrunc(long l) {
  f_lseek(&g_fil, l);
  return f_truncate(&g_fil);
}


int fileio_fseek(long l) {
    return f_lseek(&g_fil, l);
}


int fileio_feof() {
  return f_eof(&g_fil);
}


int fileio_ferror() {
  return f_error(&g_fil);
}


int fileio_dopen(char * name) {
  f_closedir(&g_dir);
  strcpy(pBuf, name);
  if (int n = strlen(pBuf)) {
    if (pBuf[n - 1] != '/') {
      pBuf[n] = '/'; pBuf[n + 1] = '\0';
    }
  }
  else {
    pBuf[0] = '/'; pBuf[1] = '\0';
  }
  return f_opendir(&g_dir, pBuf);
}


int fileio_fexists(char * name) {
  strcpy(sBuf, pBuf);
  strcat(sBuf, name);
  return (f_stat(sBuf, nullptr) == FR_OK);
}


#endif

