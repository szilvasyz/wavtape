#ifndef DISPLAY_H
#define DISPLAY_H

#include "wavtape.h"


void dispInit();
void dispClear();
void display(char * buf);
void dispLine(int line, char * buf);
void dispError(char * buf);
void dispHeader(char * buf);
void dispLine1(char * buf);
void dispLine2(char * buf);
void dispButtons(char * buf);

#endif
