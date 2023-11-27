#ifndef CONFIG_H
#define CONFIG_H

#define PRODUCT "WavTape"
#define VER_MAJOR 0
#define VER_MINOR 1
#define VER_NAME "dev"

#define BTN_PIN_PREV  18
#define BTN_PIN_NEXT  19
#define BTN_PIN_ABORT 20
#define BTN_PIN_ENTER 21

#define GREEN_LED 0
#define RED_LED 10


#define BTN_VAL_PREV  1
#define BTN_VAL_NEXT  2
#define BTN_VAL_ABORT 4
#define BTN_VAL_ENTER 8

#define DISP_ADDR 0x27
#define DISP_ROWS 4
#define DISP_COLS 20

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

#define DIG_OUT_PIN  14
#define ANA_IN_PIN A0
#define ANA_PREAMP_PIN A1
#define ANA_ATTEN_PIN A2

#define REC_SAMPLERATE 16000
#define REC_DIR "/recs"

#endif
