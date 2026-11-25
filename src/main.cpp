#include <Arduino.h>
#include "BP/BP.h"
#include "DEL/DEL.h"

// Kruger, Hydro-Québec (6 personnes), Lotto Québec (3-4), CISSS de l'Outaouais, steamvolt???, (possibilité travailler avec entreprise anglophone mais, il faut qu'il puissent signé/remplir document français)

// https://i0.wp.com/randomnerdtutorials.com/wp-content/uploads/2024/09/ESP32-S3-pinout.jpg?quality=100&strip=all&ssl=1 pinout
// https://github.com/badiarra?tab=repositories
// 

// =============================================================
// Relier les broches à leur variable
// ===================git==========================================

// Initialiser les variables pour les 
// Feux de Circulation des Piétons (FCP)
const int NB_FCP = 4;
const int brochesFCP[NB_FCP] = {
  47, // PA4 : Traverse piétonnière Ouest <-- devrais remplacer commentaire par verbe! (si j'ai le temps)
  21, // PA5 : Traverse piétonnière Nord
  20, // PA6 : Traverse piétonnière Est
  19  // PA7 : Traverse piétonnière Sud
};

// Initialiser les variables pour les retour des 
// Boutons-Poussoirs (BP) via bascules 74LS279
const int NB_BP = 8;
const int brochesBP[NB_BP] = {
  37, // PB0 : Passage des véhicules Ouest pour tourner à gauche
  38, // PB1 : Passage des véhicules Nord
  39, // PB2 : Passage des véhicules Est pour tourner à gauche
  40, // PB3 : Passage des véhicules Sud
  41, // PB4 : Passage des piétons Ouest
  42, // PB5 : Passage des piétons Nord
  3,  // PB6 : Passage des piétons Est
  1   // PB7 : Passage des piétons Sud
};

// Initialiser les variables pour les 
// Feux de Circulation des Véhicules (FCV)
const int NB_FCV = 11;
const int brochesFCV[NB_FCV] = {
  35, // PA0 : Flèche Ouest
  0,  // PA1 : Vert Ouest
  45, // PA2 : Orange Ouest
  48, // PA3 : Rouge Ouest
  4,  // PC0 : Rouge Est
  5,  // PC1 : Orange Est
  6,  // PC2 : Vert Est
  7,  // PC3 : Flèche Est
  15, // PC4 : Rouge Nord et Sud
  16, // PC5 : Orange Nord et Sud
  17  // PC6 : Vert Nord et Sud
};

// Regrouper les broches pour que les véhicules 
// tourne à gauche : {Rouge, Orange, Vert, Flèche}
const int GR_Ouest[4] = {3, 2, 1, 0}; // à l'Ouest
const int GR_Est[4] = {4, 5, 6, 7};   // à l'Est

// Regrouper les broches pour avancer dans la direction 
// Nord ou Sud : {Rouge, Orange, Vert} 
const int GR_NordSud[3] = {8, 9, 10};

// PC7 : Broche reset pour les bascules 74LS279
const int broche_RESET_74LS279 = 18;

// =============================================================
// Créer les objets
// =============================================================

DEL fcp[NB_FCP];
BP bp[NB_BP];
DEL fcv[NB_FCV];

// =============================================================
// Fonction pour effacer la mémoire des bascules 74LS279
// =============================================================

void resetBascules() {
  digitalWrite(broche_RESET_74LS279, LOW);
  delay(1);
  digitalWrite(broche_RESET_74LS279, HIGH);
}

// =============================================================
// Fonction pour ...
// =============================================================

void tournerVehicule(const int* feux){
  fcv[feux[0]].eteindre();
  fcv[feux[3]].allumer();
  delay(2000);
  fcv[feux[3]].eteindre();
  fcv[feux[2]].allumer();
  delay(2000);
  fcv[feux[2]].eteindre();
  fcv[feux[1]].allumer();
  delay(2000);
  fcv[feux[1]].eteindre();
  fcv[feux[0]].allumer();
}

// =============================================================
// Fonction...
// =============================================================

void avancerVehicule(const int* feux){
  fcv[feux[0]].eteindre();
  fcv[feux[2]].allumer();
  delay(2000);
  fcv[feux[2]].eteindre();
  fcv[feux[1]].allumer();
  delay(2000);
  fcv[feux[1]].eteindre();
  fcv[feux[0]].allumer();
}

// =============================================================
// Fonction...
// =============================================================

void avancerPieton(int feu){
  // rajouter .blink ou ailleur si on veut
  fcp[feu].allumer();
  delay(2000);
  fcp[feu].eteindre();
}

// =============================================================
// Boucle d'initialisation
// =============================================================

void setup() {
  Serial.begin(9600);  // Débuter la communication sérielle
  Serial.println("test");
  delay(3000); // Changer plus tard
  
  pinMode(broche_RESET_74LS279, OUTPUT);
  digitalWrite(broche_RESET_74LS279, HIGH);
  resetBascules();

  // Initialiser
  for(int i = 0; i < NB_FCP; i++){
    fcp[i].init(brochesFCP[i]);
    fcp[i].eteindre();
  }

  // Initialiser
  for(int j = 0; j < NB_BP; j++){
    bp[j].init(brochesBP[j]);
  }

  // Initialiser
  for(int l = 0; l < NB_FCV; l++){
    fcv[l].init(brochesFCV[l]);
    fcv[l].eteindre();
  }

  // Allumer les DELs rouge ... 
  // normalement DEL rouge pour les piétons déjà allumer
  fcv[3].allumer();
  fcv[4].allumer(); 
  fcv[8].allumer();

  delay(1500);
}

// =============================================================
// Boucle principale
// =============================================================

void loop() {
  // Vérifier si un bouton est appuyé
  for(int a = 0; a < NB_BP; a++){
    if(bp[a].lire()){
      Serial.print("bp appuyé : ");
      Serial.println(a);
      delay(100);
      switch(a){
        // Ouest pour tourner à gauche
        case 0:
          tournerVehicule(GR_Ouest);
          break;
        
        // Nord
        case 1:
          avancerVehicule(GR_NordSud);
          break;
        
        // Est pour tourner à gauche
        case 2:
          tournerVehicule(GR_Est);
          break;
        
        // Sud
        case 3:
          avancerVehicule(GR_NordSud);
          break;
        
        // Passage des piétons Ouest
        case 4:
          avancerPieton(0);
          break;
        
        // Passage des piétons Nord
        case 5:
          avancerPieton(1);
          break;

        // Passage des piétons Est
        case 6:
          avancerPieton(2);
          break;

        // Passage des piétons Sud
        case 7:
          avancerPieton(3);
          break;
        
        default:
        break;
      }
      resetBascules();
    }
  }
  delay(20);
  Serial.println("===== FIN =====");
}

// le bouton 2 arrête pas d'être activé pour une raison inconnue il faut que vérifie
// avec autre microcontroleur/board. La pin est potentiellement endommager ):
