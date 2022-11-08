#include "CarrefourControleur.h"

using namespace std;
using namespace CarfCont;

Syscarrefour carrefour;

void setup() {
  carrefour.Setup();

}

void loop() {
  // put your main code here, to run repeatedly:
  carrefour.Loop();
}
