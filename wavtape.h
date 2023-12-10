#ifndef WAVTAPE_H
#define WAVTAPE_H

#include "config.h"

#include <Arduino.h>
#include <mybutton.h>
#include <LiquidCrystal_PCF8574.h>
#include <U8g2lib.h>
#include <WAVhdr.h>
#include <AVRPCM.h>

#ifdef USE_SDFAT
  // #include <SdFat.h>
  // #define SPI_CLOCK SD_SCK_MHZ(1)
  // #define SD_CONFIG SdSpiConfig(SD_CS, DEDICATED_SPI, SPI_CLOCK)
  // extern SdFat sd;
  // extern File32 dir;
  // extern File32 file;
#endif

#ifdef USE_CHANFF
  #include "chanff.h"
#endif

#include "fileio.h"
#include "display.h"
#include "menu.h"
#include "setup.h"
#include "playback.h"
#include "playwav.h"
#include "recwav.h"


#define size(x) ((int)(sizeof(x) / sizeof(x[0])))


extern MyButton button;
extern WAVhdr W;

// extern char nBuf[];
// extern char pBuf[];
extern char sBuf[];

#endif
