#include <Arduino.h>
#include "DEL.h"

// Initialiser la DEL
void DEL::init(int pin) {
    broche = pin;
    pinMode(broche, OUTPUT);
    digitalWrite(broche, LOW);
}

// Allumer la DEL
void DEL::allumer() {
    digitalWrite(broche, HIGH);
}

// Clignoter la DEL
void DEL::clignoter(){
    // Clignoter la DEL 5 fois
    for(int i = 0; i < 5; i++){
      digitalWrite(broche, HIGH);
      delay(100);
      digitalWrite(broche, LOW);
    }
}

// Éteindre la DEL
void DEL::eteindre() {
    digitalWrite(broche, LOW);
}