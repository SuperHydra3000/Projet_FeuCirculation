#include <Arduino.h>
#include "BP.h"

// Initialiser le bouton-poussoir
void BP::init(int pin){
    broche = pin;
    pinMode(broche, INPUT_PULLUP);
}

// Lire l'état du bouton-poussoir
bool BP::lire(){
    return digitalRead(broche) == HIGH;
}