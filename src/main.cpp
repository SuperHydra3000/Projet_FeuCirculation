#include <Arduino.h>
#include "led/Led.h"
#include <math.h>
#include <esp_task_wdt.h>

/* ##Bouton##  
  //Sur la rue\\
-- Ouest tourné gauche OT PB-0
-- Nord NT PB-1
-- Est ET PB-2
-- Sud ST PB-3
  //Piéton\\
-- Ouest TO PB-4
-- Nord TN PB-5
-- Est TE PB-6
-- Sud TS PB-7

  ##Feu Voiture##
-- Ouest Fleche  OF - Ouest Vert OV - Ouest Jaune OJ - Ouest Rouge OR
-- Est Fleche  EF - Est Vert EV - Est Jaune EJ - Est Rouge ER
-- Nord Rouge NR - Nord Vert NV - Nord Jaune NJ
-- Sud Rouge SR - Sud Vert SV - Sud Jaune EJ

  ##Feu Piéton##
-- Ouest Traverse TOF
-- Est Traverse TEF
-- Sud Traverse TSF
-- Nord Traverse TNF

// ##TN|  |  |TN##
// TO##|NT|  |##TE
// ----       ----
//         EF ET  
// ----       ----
//   OT OF      
// ----       ----
// TO##|  |ST|##TE
// ##TS|  |  |TS##
*/

// Définition des Broche Bouton
//--Detection rue--//
const uint8_t NT = 20; // PB-1
const uint8_t ET = 21; // PB-2
const uint8_t ST = 35; // PB-3
const uint8_t OT = 36; // PB-0
//--Bouton piéton--//
const uint8_t TO = 37; // PB-4
const uint8_t TN = 38; // PB-5
const uint8_t TE = 39; // PB-6
const uint8_t TS = 40; // PB-7

const char* const nomsLed[] = {
  "OF", "OV", "OJ", "OR", "EF", "EV", "EJ", "ER",
  "NR", "NV", "NJ", "SR", "SV", "SJ", "TOF", "TEF",
  "TSF", "TNF"
};

Led leds[18] = {
  {0, nomsLed[0]}, {1, nomsLed[1]}, {2, nomsLed[2]}, {3, nomsLed[3]},
  {4, nomsLed[4]}, {5, nomsLed[5]}, {6, nomsLed[6]}, {7, nomsLed[7]},
  {8, nomsLed[8]}, {9, nomsLed[9]}, {10, nomsLed[10]}, {11, nomsLed[11]},
  {12, nomsLed[12]}, {13, nomsLed[13]}, {14, nomsLed[14]}, {15, nomsLed[15]},
  {16, nomsLed[16]}, {17, nomsLed[17]}
};

enum IndiceLed {
  OF, OV, OJ, OR, EF, EV, EJ, ER,
  NR, NV, NJ, SR, SV, SJ, TOF, TEF, TSF, TNF
};

// Bit Evenement
#define OuestPieton_ON (1 << 0)
#define EstPieton_ON (1 << 0)
#define NordPieton_ON (1 << 0)
#define SudPieton_ON (1 << 0)


TaskHandle_t capteurHandle = nullptr;
TaskHandle_t ledVerteHandle = nullptr;
TaskHandle_t ledHandle = nullptr;


TimerHandle_t timerHandle = nullptr;

SemaphoreHandle_t capteurSemaphore = nullptr;
SemaphoreHandle_t distanceMutex = nullptr;
SemaphoreHandle_t serialMutex = nullptr;

EventGroupHandle_t groupeEvenements;




void vTaskInitialisation(void*pvParameters)
{
  for(int i = 0; i < 18; i++){
    leds[i].init();
    Serial.println(leds[i].getName());
  }
}

void vTaskOuestPieton(void *ptr)
{
  esp_task_wdt_add(NULL); 
  while(1)
  {
    bool etat = digitalRead(TO);
    esp_task_wdt_reset();
    if (etat == true)
    {
      xEventGroupSetBits(groupeEvenements, OuestPieton_ON);
      Serial.println("TO HIGH");
    }
    else
    {
      xEventGroupClearBits(groupeEvenements, OuestPieton_ON);
      Serial.println("TO LOW");
    }
  }
}


void vTaskLumiereOuestPieton( void * pvParameters )
{
    esp_task_wdt_add(NULL); 
    while(1)
    {
      esp_task_wdt_reset();
      EventBits_t etat = xEventGroupGetBits(groupeEvenements);

      if (etat & OuestPieton_ON)
      {
        digitalWrite(TOF, HIGH);
      }
      else
      {
        digitalWrite(TOF, LOW);
      }

      vTaskDelay(pdMS_TO_TICKS(30000));
    }
}

void vTaskEstPieton(void *ptr)
{
  esp_task_wdt_add(NULL); 
  while(1)
  {
    bool etat = digitalRead(TE);
    esp_task_wdt_reset();
    if (etat == true)
    {
      xEventGroupSetBits(groupeEvenements, EstPieton_ON);
      Serial.println("TE HIGH");
    }
    else
    {
      xEventGroupClearBits(groupeEvenements, EstPieton_ON);
      Serial.println("TEF LOW");
    }
  }
}


void vTaskLumiereEstPieton( void * pvParameters )
{
    esp_task_wdt_add(NULL); 
    while(1)
    {
      esp_task_wdt_reset();
      EventBits_t etat = xEventGroupGetBits(groupeEvenements);

      if (etat & EstPieton_ON)
      {
        digitalWrite(TEF, HIGH);
        Serial.println("TEF HIGH");
      }
      else
      {
        digitalWrite(TEF, LOW);
        Serial.println("TEF LOW");
      }
      
      vTaskDelay(pdMS_TO_TICKS(30000));
    }
}





void setup() {
  Serial.begin(115200);
  esp_task_wdt_init(5, true);

  groupeEvenements = xEventGroupCreate();

  xTaskCreate(vTaskOuestPieton, "BT_Ouest", 2048, NULL, 2, NULL);
  xTaskCreate(vTaskLumiereOuestPieton, "LED_Ouest", 2048, NULL, 1, NULL);
  xTaskCreate(vTaskEstPieton, "BT_Est", 2048, NULL, 2, NULL);
  xTaskCreate(vTaskLumiereEstPieton, "LED_Est", 2048, NULL, 1, NULL);
 

}

void loop() {
  // put your main code here, to run repeatedly:

}

