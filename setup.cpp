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

char* pAttVal()   { return (char *)(defCfg.pAtt   ? "LO"   : "HI" ); }
char* pNormVal()  { return (char *)(defCfg.pNorm  ? "ON"   : "OFF"); }
char* pPhaseVal() { return (char *)(defCfg.pPhase ? "NORM" : "INV"); }
char* rAmpVal()   { return (char *)(defCfg.rAmp   ? "LO"   : "HI" ); }
char* rPhaseVal() { return (char *)(defCfg.rPhase ? "NORM" : "INV"); }
char* rSRateVal() { return strcat(itoa(rsrates[defCfg.rSRate] / 1000U, cBuf, DEC), "k"); }


void validateConfig() {
  if ((defCfg.pAtt   < 0) || (defCfg.pAtt   > 1)) defCfg.pAtt   = PLAY_LEVEL;
  if ((defCfg.pNorm  < 0) || (defCfg.pNorm  > 1)) defCfg.pNorm  = PLAY_NORM;
  if ((defCfg.pPhase < 0) || (defCfg.pPhase > 1)) defCfg.pPhase = PLAY_PHASE;
  if ((defCfg.rAmp   < 0) || (defCfg.rAmp   > 1)) defCfg.rAmp   = REC_LEVEL;
  if ((defCfg.rPhase < 0) || (defCfg.rPhase > 1)) defCfg.rPhase = REC_PHASE;
  if ((defCfg.rSRate < 0) || (defCfg.rSRate >= numrsrates)) defCfg.rSRate = REC_SRATE;

}


void loadConfigEE() {
  uint32_t sig;

  EEPROM.get(0, sig);
  if (sig != EESIG) return;
  EEPROM.get(sizeof(sig), defCfg);
  validateConfig();
}


void loadConfigSD() {
  uint32_t sig;

  if (file.open(CFG_FILE, O_RDONLY)) {
    file.read(&sig, sizeof(sig));
    if (sig == EESIG) {
      file.read(&defCfg, sizeof(defCfg));
      validateConfig();
    }
    file.close();
  }
}


void saveConfigEE() {
  uint32_t sig;

  sig = EESIG;
  EEPROM.put(0, sig);
  EEPROM.put(sizeof(sig), defCfg);
  dispError("Config saved EE");
}


void saveConfigSD() {
  uint32_t sig;

  sig = EESIG;
  if (file.open(CFG_FILE, O_CREAT | O_WRITE)) {
    file.write(&sig, sizeof(sig));
    file.write(&defCfg, sizeof(defCfg));
    file.close();
    dispError("Config saved SD");
  }
  else
    dispError("Cfgsave error SD");
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
  menuDo("Settings", "Select setting:", setMenu, size(setMenu));
}
