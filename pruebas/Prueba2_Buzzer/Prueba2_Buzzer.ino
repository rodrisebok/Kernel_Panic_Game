/*
  Prueba 2 · Buzzer (ESP8266)
  Toca una escala y la melodía de "game over".
  - Si suena distinto en cada nota: es un buzzer PASIVO (ideal, hace melodías).
  - Si siempre suena el mismo pitido: es un buzzer ACTIVO. Sirve igual, pero sin tonos.

  Conexión: buzzer + → D6, buzzer − → GND
*/

const uint8_t PIN_BUZZER = D6;
const int ESCALA[] = { 262, 294, 330, 349, 392, 440, 494, 523 };
const int GAME_OVER[][2] = { {880, 150}, {660, 150}, {440, 150}, {220, 420} };

void setup() {
  Serial.begin(115200);
  pinMode(PIN_BUZZER, OUTPUT);
  Serial.println("\nPrueba del buzzer");
}

void loop() {
  for (int f : ESCALA) { Serial.println(f); tone(PIN_BUZZER, f, 200); delay(250); }
  delay(500);
  for (auto& n : GAME_OVER) { tone(PIN_BUZZER, n[0], n[1]); delay(n[1] + 30); }
  delay(2000);
}
