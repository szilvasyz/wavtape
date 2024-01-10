#ifndef MENU_H
#define MENU_H


#include "wavtape.h"


typedef void (*tMenuAction)();
typedef char* (*tMenuValue)();


void menuActionDummy();
char* menuValueDummy();


typedef struct {
  char* text;
  tMenuAction action = menuActionDummy;
  tMenuValue value = menuValueDummy;
} tMenuItem;

typedef tMenuItem tMenu[];


void menuDo(char* header, char* title, tMenu menu, int size);


#endif

