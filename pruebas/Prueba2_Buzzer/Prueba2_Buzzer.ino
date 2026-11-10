/*
  Prueba 2 · Buzzer (ESP8266)
  Toca una escala y un par de efectos de sonido del juego.
  - Si suena distinto en cada nota: es un buzzer PASIVO (ideal, hace melodías).
  - Si siempre suena el mismo pitido: es un buzzer ACTIVO. Sirve igual, pero sin tonos.

  Conexión: buzzer + → D6   ·   buzzer − → GND
*/

const uint8_t PIN_BUZZER = D6;
const int NOTAS[] = { 262, 294, 330, 349, 392, 440, 494, 523 };

void setup() {
  Serial.begin(115200);
  pinMode(PIN_BUZZER, OUTPUT);
  Serial.println("\nPrueba del buzzer");
}

void loop() {
  for (int i = 0; i < 8; i++) {          // escala ascendente
    tone(PIN_BUZZER, NOTAS[i], 180);
    Serial.println(NOTAS[i]);
    delay(220);
  }
  delay(400);
  tone(PIN_BUZZER, 1500, 60); delay(150);   // "disparo"
  tone(PIN_BUZZER, 1800, 60); delay(150);
  tone(PIN_BUZZER, 180, 300); delay(600);   // "daño"
  noTone(PIN_BUZZER);
  delay(1500);
}
