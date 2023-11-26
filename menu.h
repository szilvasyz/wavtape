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


void menuDo(char* header, char* title, tMenu menu, int size);


#endif

