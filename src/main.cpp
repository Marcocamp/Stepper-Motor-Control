#include <Arduino.h>
#include <AccelStepper.h>

// =============================================
//         DEFINIZIONI HARDWARE E COSTANTI
// =============================================
#define STEP_PIN          8       // Pin per il segnale STEP
#define DIR_PIN           2       // Pin per il segnale DIR
#define ENABLE_PIN        7       // Pin per abilitare il driver
#define PIN_PULSANTE_AVANTI   3    // Pulsante movimento normale
#define PIN_PULSANTE_VIBRA    4    // Pulsante attivazione vibrazione
#define PIN_PULSANTE_STOP     5    // Pulsante stop di emergenza

// Parametri di configurazione motore
const float VELOCITA_NORMALE = 50;    // Velocità fase 1 (steps/secondo)
const float VELOCITA_VIBRAZIONE = 5000; // Velocità vibrazione 
const long INTERVALLO_VIBRAZIONE = 100; // Cambio direzione ogni 100ms
const long DURATA_FASE1 = 50000;        // 10 secondi fase iniziale
const long DURATA_VIBRAZIONE = 0;    // 1 secondo vibrazione

const long DURATA_FASE3 = 0;         // 0.5 secondo pausa
const long DEBOUNCE_DELAY = 50;         // Tempo antirimbalzo pulsanti

// =============================================
//            VARIABILI GLOBALI
// =============================================
enum Stato { 
  FASE_MOVIMENTO,    // Movimento in avanti continuo
  FASE_VIBRAZIONE,   // Vibrazione alternata
  FASE_PAUSA         // Fermo completo
};

Stato statoCorrente = FASE_MOVIMENTO;
unsigned long tempoInizioFase = 0;
unsigned long ultimoCambioDirezione = 0;
unsigned long ultimoPressione = 0;
bool direzioneVibrazione = true;  // true = avanti, false = indietro

AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

// =============================================
//               INIZIALIZZAZIONE
// =============================================
void setup() {
  Serial.begin(9600);
  while (!Serial); // Attendiamo solo su board con USB nativa

  // Configurazione pin
  pinMode(ENABLE_PIN, OUTPUT);
  digitalWrite(ENABLE_PIN, LOW); // Abilita il driver
  
  // Configurazione pulsanti
  pinMode(PIN_PULSANTE_AVANTI, INPUT_PULLUP);
  pinMode(PIN_PULSANTE_VIBRA, INPUT_PULLUP);
  pinMode(PIN_PULSANTE_STOP, INPUT_PULLUP);

  // Configurazione motore
  stepper.setMaxSpeed(100000); // Velocità massima di sicurezza
  
  Serial.println("Sistema inizializzato con controllo pulsanti");
}

// =============================================
//           FUNZIONE DI CAMBIO STATO
// =============================================
void cambiaStato(Stato nuovoStato) {
  statoCorrente = nuovoStato;
  tempoInizioFase = millis(); // Resetta il timer della fase
  ultimoCambioDirezione = millis(); // Resetta il timer della vibrazione
  
  // Log su seriale per debug
  Serial.print("Cambio stato: ");
  switch(statoCorrente) {
    case FASE_MOVIMENTO: 
      Serial.println("Movimento");
      
      break;
      
    case FASE_VIBRAZIONE: 
      Serial.println("Vibrazione");
      break;
      
    case FASE_PAUSA: 
      Serial.println("Pausa");
      stepper.setSpeed(0);
      break;
  }
}

// =============================================
//           FUNZIONE DI CONTROLLO PULSANTI
// =============================================
void gestisciPulsanti() {
  if((millis() - ultimoPressione) < DEBOUNCE_DELAY) return;

  // Controllo priorità 1: STOP di emergenza
  if(digitalRead(PIN_PULSANTE_STOP) == LOW) {
    cambiaStato(FASE_PAUSA);
    ultimoPressione = millis();
    return;
  }

  // Controllo priorità 2: VIBRAZIONE
  if(digitalRead(PIN_PULSANTE_VIBRA) == LOW) {
    cambiaStato(FASE_VIBRAZIONE);
    ultimoPressione = millis();
    return;
  }

  // Controllo priorità 3: MOVIMENTO AVANTI
  if(digitalRead(PIN_PULSANTE_AVANTI) == LOW) {
    cambiaStato(FASE_MOVIMENTO);
    ultimoPressione = millis();
    return;
  }
}

// =============================================
//                LOOP PRINCIPALE
// =============================================
void loop() {
  // Controllo pulsanti con priorità assoluta
  gestisciPulsanti();

  // Macchina a stati principale
  switch(statoCorrente) {
    // -----------------------------------------
    // FASE 1: Movimento in avanti continuo
    // -----------------------------------------
    case FASE_MOVIMENTO:
      stepper.setSpeed(VELOCITA_NORMALE);
      // Transizione automatica dopo 10 secondi
      if(millis() - tempoInizioFase >= DURATA_FASE1) {
        cambiaStato(FASE_VIBRAZIONE);
      }
      break;

    // -----------------------------------------
    // FASE 2: Vibrazione controllata
    // -----------------------------------------
    case FASE_VIBRAZIONE:

      if(millis() - ultimoCambioDirezione >= INTERVALLO_VIBRAZIONE) {
        direzioneVibrazione = !direzioneVibrazione;
        ultimoCambioDirezione = millis();
        stepper.setSpeed(direzioneVibrazione ? VELOCITA_VIBRAZIONE: -VELOCITA_VIBRAZIONE);
      }
      
      if(millis() - tempoInizioFase>= DURATA_VIBRAZIONE) {
        cambiaStato(FASE_MOVIMENTO);
        tempoInizioFase = millis();
      }
      break;

    // -----------------------------------------
    // FASE 3: Pausa con motore fermo
    // -----------------------------------------
    case FASE_PAUSA:
      // Transizione automatica dopo 1 secondo
      if(millis() - tempoInizioFase >= DURATA_FASE3) {
        cambiaStato(FASE_MOVIMENTO);
      }
      break;
  }

  // Esegue il movimento
  stepper.runSpeed();
}