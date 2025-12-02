# Stepper-Motor-Control
Controllo avanzato di un motore stepper tramite pulsanti, macchina a stati e libreria AccelStepper.

Questo progetto Arduino implementa un sistema di controllo per motore stepper basato su una macchina a stati finiti (FSM), con tre modalità operative principali:

- **Movimento continuo in avanti**
- **Vibrazione a inversione rapida**
- **Stop di emergenza con pausa temporanea**

Il sistema include debounce software, gestione della priorità dei pulsanti e logging via seriale.


## Funzionalità principali

- Controllo tramite tre pulsanti:
  - **Avanti** → movimento continuo
  - **Vibrazione** → oscillazione avanti/indietro
  - **Stop** → arresto immediato (priorità massima)
- Macchina a stati: Movimento, Vibrazione, Pausa
- Timer per durata fasi e intervallo inversione
- Debounce software (50 ms)
- Serial log per debugging
- Driver stepper con limite di velocità configurabile


##  Hardware richiesto

- Arduino Mega, UNO/Nano o compatibile
- Driver stepper (A4988, DRV8825, TMC)
- Motore stepper NEMA 17
- 3 pulsanti (NO)
- Cablaggio base


## Cablaggio

| Funzione              | Arduino Pin |
|----------------------|-------------|
| STEP                 | D8          |
| DIR                  | D2          |
| ENABLE driver        | D7          |
| Pulsante Avanti      | D3          |
| Pulsante Vibrazione  | D4          |
| Pulsante Stop        | D5          |

> I pulsanti vanno collegati a **GND** perché si utilizza `INPUT_PULLUP`.


## Parametri configurabili

```cpp
const float VELOCITA_NORMALE = 50;
const float VELOCITA_VIBRAZIONE = 5000;
const long INTERVALLO_VIBRAZIONE = 100;
const long DURATA_FASE1 = 50000;
const long DURATA_VIBRAZIONE = 0;
const long DURATA_FASE3 = 0;
const long DEBOUNCE_DELAY = 50;
```

## Macchina a stati

Gli stati sono definiti così:

```cpp
enum Stato { 
  FASE_MOVIMENTO,
  FASE_VIBRAZIONE,
  FASE_PAUSA
};
```

### Stato 1 — Movimento

Motore in avanti a VELOCITA_NORMALE.
Dopo DURATA_FASE1 passa automaticamente alla vibrazione.

### Stato 2 — Vibrazione

Inversione ogni INTERVALLO_VIBRAZIONE ms a VELOCITA_VIBRAZIONE.
Dopo DURATA_VIBRAZIONE torna al movimento.

### Stato 3 — Pausa

Motore fermo.
Dopo DURATA_FASE3 riprende il movimento.

## Priorità dei pulsanti

STOP

VIBRAZIONE

AVANTI

Ogni pressione provoca cambio stato immediato.

## Serial Monitor

Esempi output:

Sistema inizializzato con controllo pulsanti
- Cambio stato: Movimento
- Cambio stato: Vibrazione
- Cambio stato: Pausa

## Struttura del progetto

```css
├── Stepper-Motor-Control
│   ├── src/
│   │   └── main.cpp
│   ├── README.md
│   ├── LICENSE
```

## Come utilizzare

Clona il repository:

git clone https://github.com/Marcocamp/Stepper-Motor-Control

Apri il progetto su Arduino IDE o PlatformIO.

Carica il codice sulla board.

Collega driver, motore e pulsanti.

Avvia il Serial Monitor (9600 baud).

## Estensioni possibili

Aggiunta encoder (closed-loop)

Integrazione con ROS2

Controllo da joystick

Modalità test automatica

Aggiunta finecorsa (homing)

Profilo di accelerazione personalizzato

## Licenza

Rilasciato sotto licenza MIT.
