#ifndef SETUP_H
#define SETUP_H


#include "wavtape.h"


typedef struct {
  int pAtt;
  int pNorm;
  int pPhase;
  int rAmp;
  int rPhase;
  int rSRate;
} tCfg;


void doSetMenu();


#endif
