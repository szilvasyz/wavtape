#ifndef MENU_H
#define MENU_H


#include "wavtape.h"


typedef void (*tMenuAction)();


void menuDummy();


typedef struct {
  char* text;
  tMenuAction action = menuDummy;
} tMenuItem;

typedef tMenuItem tMenu[];


void menuDo(tMenu menu, int size);


#endif

