#include "wavtape.h"


tMenu setMenu = {
  {text:"Play att"},
  {text:"Play normalizing"},
  {text:"Play phase"},
  {text:"Rec amp"},
  {text:"Rec phase"},
  {text:"Rec bitrate"},
};


void doSetMenu() {
  menuDo("Settings", "Select setting:", setMenu, size(setMenu));
}
