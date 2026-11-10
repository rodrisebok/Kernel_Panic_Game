/*
  Kernel Panic · Consola (ESP32-S3) · simulación en Cirkit Designer
  Arquitectura de Computadoras · UNAHUR

  Igual que en el circuito real: el Arduino UNO lee el joystick y el botón y manda
  "X,Y,SW,BOTON" por UART a 38400 baudios. La señal pasa por un divisor de tensión
  (5 V → 3,3 V) y entra al ESP32-S3 por el GPIO 16.
  (Cirkit no simula el ESP8266; el ESP32-S3 cumple el mismo papel.)

  Esta consola:
    - recibe y valida las líneas del UNO (sin bloquear),
    - calcula la dirección con zona muerta,
    - maneja 8 LEDs con el 74HC595 (un LED sigue al eje X; con el disparo se prenden todos),
    - hace sonar el buzzer al disparar y al presionar SW,
    - muestra todo en el Monitor Serie.
*/

const uint8_t PIN_UNO_RX    = 16;  // recibe del TX del UNO (a través del divisor)
const uint8_t PIN_BUZZER    = 6;
const uint8_t PIN_595_DATOS = 11;  // 74HC595 DS (pata 14)
const uint8_t PIN_595_RELOJ = 12;  // 74HC595 SH_CP (pata 11)
const uint8_t PIN_595_LATCH = 10;  // 74HC595 ST_CP (pata 12)

const int ZONA_MUERTA = 150;
const char* const NOMBRE[] = { "CENTRO", "ARRIBA", "ABAJO", "IZQUIERDA", "DERECHA" };

char buf[24];
uint8_t len = 0;
int x = 512, y = 512, sw = 1, boton = 1, swAntes = 1, botonAntes = 1;
uint32_t ultimoDato = 0, ultimoInforme = 0, lineas = 0, errores = 0;

void escribir595(uint8_t b) {
  digitalWrite(PIN_595_LATCH, LOW);
  shiftOut(PIN_595_DATOS, PIN_595_RELOJ, MSBFIRST, b);   // bit 7 → Q7 ... bit 0 → Q0
  digitalWrite(PIN_595_LATCH, HIGH);
}

void procesarLinea() {
  buf[len] = 0;
  int nx, ny, nsw, nbt;
  if (sscanf(buf, "%d,%d,%d,%d", &nx, &ny, &nsw, &nbt) == 4 &&
      nx >= 0 && nx <= 1023 && ny >= 0 && ny <= 1023 && nsw >= 0 && nsw <= 1 && nbt >= 0 && nbt <= 1) {
    x = nx; y = ny; sw = nsw; boton = nbt;
    ultimoDato = millis(); lineas++;
  } else errores++;
}

void setup() {
  Serial.begin(115200);
  Serial1.begin(38400, SERIAL_8N1, PIN_UNO_RX, -1);   // solo recibe
  pinMode(PIN_595_DATOS, OUTPUT);
  pinMode(PIN_595_RELOJ, OUTPUT);
  pinMode(PIN_595_LATCH, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  escribir595(0);
  Serial.println("\nKernel Panic · consola ESP32-S3 esperando datos del Arduino UNO...");
}

void loop() {
  // 1) Leer lo que llegó del UNO, carácter por carácter, sin esperar
  while (Serial1.available()) {
    char c = Serial1.read();
    if (c == '\n') { procesarLinea(); len = 0; }
    else if (c != '\r') { if (len < sizeof(buf) - 1) buf[len++] = c; else len = 0; }
  }
  bool unoOk = millis() - ultimoDato < 300;
  if (!unoOk) { x = 512; y = 512; sw = 1; boton = 1; }   // sin datos: joystick al centro

  // 2) Dirección con zona muerta
  int dx = x - 512, dy = y - 512;
  uint8_t dir = 0;
  if (abs(dx) >= ZONA_MUERTA || abs(dy) >= ZONA_MUERTA)
    dir = abs(dx) > abs(dy) ? (dx < 0 ? 3 : 4) : (dy < 0 ? 1 : 2);

  // 3) Salidas: LEDs y buzzer
  uint8_t leds = 1 << map(x, 0, 1023, 0, 7);
  if (boton == 0) leds = 0xFF;
  escribir595(leds);
  if (botonAntes == 1 && boton == 0) tone(PIN_BUZZER, 1500, 60);
  if (swAntes == 1 && sw == 0)       tone(PIN_BUZZER, 880, 120);
  botonAntes = boton; swAntes = sw;

  // 4) Informe en el Monitor Serie
  if (millis() - ultimoInforme >= 250) {
    ultimoInforme = millis();
    if (!unoOk) { Serial.println("UNO sin señal: revisá TX → divisor → GPIO 16 y el GND común"); return; }
    Serial.printf("X=%4d  Y=%4d  dirección=%-9s  SW=%d  DISPARO=%d  líneas=%lu  errores=%lu\n",
                  x, y, NOMBRE[dir], sw, boton, (unsigned long)lineas, (unsigned long)errores);
  }
}
