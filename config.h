#ifndef CONFIG_H
#define CONFIG_H

#define PRODUCT "WavTape"
#define VER_MAJOR 0
#define VER_MINOR 1
#define VER_NAME "dev"

// hardware built on TAPUINO PCB
#define TAPUINO_PCB

#define BTN_PIN_PREV  18
#define BTN_PIN_NEXT  19
#define BTN_PIN_ABORT 20
#define BTN_PIN_ENTER 21

#define GREEN_LED 0
#define RED_LED 10
#define INVERT_LEDS 1


#define BTN_VAL_PREV  1
#define BTN_VAL_NEXT  2
#define BTN_VAL_ABORT 4
#define BTN_VAL_ENTER 8

#define DISP_HD44780
#define DISP_ADDR 0x27
#define DISP_ROWS 4
#define DISP_COLS 20

// #define DISP_SSD1306
// #define DISP_SH1106
// #define DISP_SSD1309
// #define DISP_ADDR 0x3C
// #define DISP_ROWS 4
// #define DISP_COLS 16

#if (DISP_ROWS == 4)
  #define DISP_HEADER 0
  #define DISP_LINE1 1
  #define DISP_LINE2 2
  #define DISP_BUTTONS 3
#else
  #define DISP_LINE1 0
  #define DISP_LINE2 1
#endif

#define SD_CS 4
#define SD_FAT_TYPE 1
#define SD_INIT_TRY 5


#define SBUF_SIZE 80
#define NBUF_SIZE 60
#define PBUF_SIZE 200
#define FILEEXT ".wav"

#ifdef TAPUINO_PCB
  #define DIG_OUT_PIN  11
#else
  #define DIG_OUT_PIN  14
#endif
#define ANA_IN_PIN A0
#define ANA_PREAMP_PIN A1
#define ANA_ATTEN_PIN A2

#define REC_SRATES {8000, 11025, 16000, 22100, 24000, 32000, 44100, 48000}
#define REC_DIR "/recs"
#define REC_PREALLOC_MB 20

#define CFG_FILE "/wavtape.cfg"

// defaults
#define PLAY_LEVEL 0
#define PLAY_NORM 1
#define PLAY_PHASE 0
#define REC_LEVEL 1
#define REC_PHASE 0
#define REC_SRATE 2


#ifdef INVERT_LEDS
  #define LEDLOW HIGH
  #define LEDHIGH LOW
#else
  #define LEDLOW LOW
  #define LEDHIGH HIGH
#endif

#endif
