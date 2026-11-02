/*
  Prueba 1 · 74HC595 con 8 LEDs (ESP8266)
  Enciende un LED por vez de Q0 a Q7 y vuelve, y cada tanto los prende todos.
  Si un LED no enciende: revisá su resistencia y la pata del 595 que le corresponde.

  Conexiones del 74HC595:
    16 VCC → 3.3V      8 GND → GND
    14 DS  → D7        11 SH_CP → D8      12 ST_CP → D0
    13 OE  → GND       10 MR → 3.3V
    Q0 (15) y Q1..Q7 (1..7) → pata larga del LED; pata corta → 220 Ω → GND
*/

const uint8_t PIN_DATOS = D7;
const uint8_t PIN_RELOJ = D8;
const uint8_t PIN_LATCH = D0;

void escribir595(uint8_t b) {
  digitalWrite(PIN_LATCH, LOW);
  shiftOut(PIN_DATOS, PIN_RELOJ, MSBFIRST, b);
  digitalWrite(PIN_LATCH, HIGH);
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_DATOS, OUTPUT);
  pinMode(PIN_RELOJ, OUTPUT);
  pinMode(PIN_LATCH, OUTPUT);
  Serial.println("\nPrueba del 74HC595");
}

void loop() {
  for (int i = 0; i < 8; i++) {
    escribir595(1 << i);
    Serial.print("Q"); Serial.println(i);
    delay(200);              // en una prueba simple delay() está bien
  }
  for (int i = 6; i > 0; i--) { escribir595(1 << i); delay(200); }
  escribir595(0xFF); Serial.println("Todos"); delay(600);
  escribir595(0x00); delay(300);
}
