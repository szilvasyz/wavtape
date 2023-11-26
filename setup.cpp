#include "wavtape.h"


tMenu setMenu = {
  {text:"Input amp"},
  {text:"Output att"},
  {text:"Rec bitrate"},
  {text:"Play normalizing"}
};


void doSetMenu() {
  menuDo("Settings", "Select setting:", setMenu, size(setMenu));
}
