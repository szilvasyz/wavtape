#ifndef DISPLAY_H
#define DISPLAY_H

#include "wavtape.h"


void dispInit();
void dispClear();
void display(char * buf);
void dispLine(int line, char * buf);
void dispLine(int line, int i);
void dispStatus(char * buf);

#endif
