#include "wavtape.h"


int pAtt = 0;
int pNorm = 1;
int pPhase = 0;
int rAmp = 1;
int rPhase = 0;
int rSRate = 0;

char cBuf[20];


void noAction() {}
void pAttAction() { pAtt = 1 - pAtt; }
void pNormAction() { pNorm = 1 - pNorm; }
void pPhaseAction() { pPhase = 1 - pPhase; }
void rAmpAction() { rAmp = 1 - rAmp; }
void rPhaseAction() { rPhase = 1 - rPhase; }
void rSRateAction() { if (++rSRate >= numrsrates) rSRate = 0; }

char* pAttVal() { return (char *)(pAtt ? "LO" : "HI"); }
char* pNormVal() { return (char *)(pNorm ? "ON" : "OFF"); }
char* pPhaseVal() { return (char *)(pPhase ? "NORM" : "INV"); }
char* rAmpVal() { return (char *)(rAmp ? "LO" : "HI"); }
char* rPhaseVal() { return (char *)(rPhase ? "NORM" : "INV"); }
char* rSRateVal() { return strcat(itoa(rsrates[rSRate] / 1000U, cBuf, DEC), "k"); }

void loadConfig() {

}

void saveConfig() {
  dispError("Config saved");
}


tMenu setMenu = {
  {text:"Play level: ", action:pAttAction, value:pAttVal},
  {text:"Play ALC:   ", action:pNormAction, value:pNormVal},
  {text:"Play phase: ", action:pPhaseAction, value:pPhaseVal},
  {text:"Rec level:  ", action:rAmpAction, value:rAmpVal},
  {text:"Rec phase:  ", action:rPhaseAction, value:rPhaseVal},
  {text:"Rec srate:  ", action:rSRateAction, value:rSRateVal},
  {text:"Save config ", action:saveConfig},
};


void doSetMenu() {
  loadConfig();
  menuDo("Settings", "Select setting:", setMenu, size(setMenu));
}
