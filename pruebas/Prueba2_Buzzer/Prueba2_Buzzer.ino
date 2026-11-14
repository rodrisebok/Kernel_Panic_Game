/*
  Prueba 2 · Buzzer (ESP8266) — "Tema de Kernel Panic"
  Melodía original estilo videojuego retro (chiptune).
  - Si cada nota suena distinta: buzzer PASIVO (hace melodías).
  - Si siempre suena el mismo pitido: buzzer ACTIVO (solo marca el ritmo).
  Conexión: buzzer + → D6   ·   buzzer − → GND
*/

const uint8_t PIN_BUZZER = D6;

// Notas (frecuencia en Hz)
#define SIL 0
#define C4 262
#define D4 294
#define E4 330
#define F4 349
#define G4 392
#define A4 440
#define B4 494
#define C5 523
#define D5 587
#define E5 659
#define F5 698
#define G5 784
#define A5 880

const int TEMPO = 150;   // duración de una corchea en ms (más chico = más rápido)

// Cada par: nota, duración en corcheas
const int MELODIA[][2] = {
  // Parte A: arranque, sube con energía
  {E4,1},{G4,1},{C5,1},{G4,1},  {E5,2},{D5,1},{C5,1},
  {D5,1},{B4,1},{G4,1},{B4,1},  {D5,3},{SIL,1},
  {F4,1},{A4,1},{C5,1},{A4,1},  {F5,2},{E5,1},{D5,1},
  {E5,1},{C5,1},{G4,1},{C5,1},  {E5,3},{SIL,1},
  // Parte B: más tensa, como acercándose a NÚCLEO-0
  {A4,1},{A4,1},{C5,1},{A4,1},  {G4,1},{A4,1},{C5,2},
  {B4,1},{B4,1},{D5,1},{B4,1},  {A4,1},{B4,1},{D5,2},
  {C5,1},{E5,1},{G5,1},{E5,1},  {A5,2},{G5,1},{E5,1},
  {F5,1},{D5,1},{B4,1},{D5,1},  {G5,3},{SIL,1},
  // Final: resuelve y cierra
  {E5,1},{D5,1},{C5,1},{B4,1},  {C5,1},{G4,1},{E4,1},{G4,1},
  {C5,4},{SIL,4}
};
const int CANT = sizeof(MELODIA) / sizeof(MELODIA[0]);

void setup() {
  Serial.begin(115200);
  pinMode(PIN_BUZZER, OUTPUT);
  Serial.println("\nPrueba del buzzer: Tema de Kernel Panic");
}

void loop() {
  for (int i = 0; i < CANT; i++) {
    int nota = MELODIA[i][0];
    int dur = MELODIA[i][1] * TEMPO;
    if (nota == SIL) noTone(PIN_BUZZER);
    else tone(PIN_BUZZER, nota, dur * 0.9);   // 90 %: deja un respiro entre notas
    delay(dur);
  }
  noTone(PIN_BUZZER);
  delay(1000);   // pausa y vuelve a empezar
}
