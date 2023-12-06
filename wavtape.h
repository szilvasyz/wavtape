#ifndef WAVTAPE_H
#define WAVTAPE_H

#include <Arduino.h>
#include <mybutton.h>
#include <LiquidCrystal_PCF8574.h>
#include <U8g2lib.h>
#include <SdFat.h>
#include <WAVhdr.h>
#include <AVRPCM.h>


#include "config.h"
#include "display.h"
#include "menu.h"
#include "setup.h"
#include "playback.h"
#include "playwav.h"
#include "recwav.h"


#define SPI_CLOCK SD_SCK_MHZ(1)
#define SD_CONFIG SdSpiConfig(SD_CS, DEDICATED_SPI, SPI_CLOCK)

#define size(x) ((int)(sizeof(x) / sizeof(x[0])))


extern MyButton button;
extern SdFat sd;
extern WAVhdr W;

extern File32 dir;
extern File32 file;
extern char nBuf[];
extern char pBuf[];
extern char sBuf[];

#endif
