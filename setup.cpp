#include "wavtape.h"


tCfg defCfg = {
  pAtt:   PLAY_LEVEL,
  pNorm:  PLAY_NORM,
  pPhase: PLAY_PHASE,
  rAmp:   REC_LEVEL,
  rPhase: REC_PHASE,
  rSRate: REC_SRATE
};


char cBuf[20];


void noAction() {}
void pAttAction()   { defCfg.pAtt   = 1 - defCfg.pAtt;   }
void pNormAction()  { defCfg.pNorm  = 1 - defCfg.pNorm;  }
void pPhaseAction() { defCfg.pPhase = 1 - defCfg.pPhase; }
void rAmpAction()   { defCfg.rAmp   = 1 - defCfg.rAmp;   }
void rPhaseAction() { defCfg.rPhase = 1 - defCfg.rPhase; }
void rSRateAction() { if (++defCfg.rSRate >= numrsrates) defCfg.rSRate = 0; }

char* pAttVal()   { return (char *)(defCfg.pAtt   ? "HI"   : "LO" ); }
char* pNormVal()  { return (char *)(defCfg.pNorm  ? "ON"   : "OFF"); }
char* pPhaseVal() { return (char *)(defCfg.pPhase ? "NORM" : "INV"); }
char* rAmpVal()   { return (char *)(defCfg.rAmp   ? "LO"   : "HI" ); }
char* rPhaseVal() { return (char *)(defCfg.rPhase ? "NORM" : "INV"); }
char* rSRateVal() { return strcat(itoa(rsrates[defCfg.rSRate] / 1000U, cBuf, DEC), "k"); }


void loadConfigEE() {
}


void loadConfigSD() {
}


void saveConfigEE() {
  dispError("Config saved EE");
}


void saveConfigSD() {
  dispError("Config saved SD");
}


tMenu setMenu = {
  {text:"Play level: "  , action:pAttAction,   value:pAttVal},
  {text:"Play ALC:   "  , action:pNormAction,  value:pNormVal},
  {text:"Play phase: "  , action:pPhaseAction, value:pPhaseVal},
  {text:"Rec level:  "  , action:rAmpAction,   value:rAmpVal},
  {text:"Rec phase:  "  , action:rPhaseAction, value:rPhaseVal},
  {text:"Rec srate:  "  , action:rSRateAction, value:rSRateVal},
  {text:"Save cfg to EE", action:saveConfigEE},
  {text:"Save cfg to SD", action:saveConfigSD},
};


void doSetMenu() {
  loadConfigEE();
  loadConfigSD();
  menuDo("Settings", "Select setting:", setMenu, size(setMenu));
}
