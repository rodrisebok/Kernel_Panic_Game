/*
  Kernel Panic · Prueba del circuito en Wokwi (ESP32)
  Arquitectura de Computadoras · UNAHUR

  Prueba todos los componentes del protoboard:
    - Joystick: lee los ejes X e Y, calcula la dirección (con zona muerta) y el pulsador SW.
    - Botón de disparo: suena el buzzer y se encienden todos los LEDs.
    - 74HC595 + 8 LEDs: un LED "sigue" al joystick de izquierda a derecha.
    - Buzzer: suena al disparar y al presionar SW.
  Todo se muestra en el Monitor Serie. Sin delay(): todo funciona con millis().

  Wokwi no tiene ESP8266, por eso la simulación usa un ESP32. En el ESP32 el joystick se lee
  directo (tiene varias entradas analógicas); en el hardware real lo lee el Arduino UNO.
*/

const uint8_t PIN_JOY_X     = 34;  // joystick HORZ
const uint8_t PIN_JOY_Y     = 35;  // joystick VERT
const uint8_t PIN_JOY_SW    = 27;  // pulsador del joystick
const uint8_t PIN_BOTON     = 32;  // botón de disparo
const uint8_t PIN_BUZZER    = 26;
const uint8_t PIN_595_DATOS = 23;  // 74HC595 DS
const uint8_t PIN_595_RELOJ = 18;  // 74HC595 SHCP
const uint8_t PIN_595_LATCH = 19;  // 74HC595 STCP

const int ZONA_MUERTA = 150;
const char* const NOMBRE[] = { "CENTRO", "ARRIBA", "ABAJO", "IZQUIERDA", "DERECHA" };

uint32_t ultimoPaso = 0, ultimoInforme = 0;
uint8_t swAntes = 1, botonAntes = 1;

void escribir595(uint8_t b) {
  digitalWrite(PIN_595_LATCH, LOW);
  shiftOut(PIN_595_DATOS, PIN_595_RELOJ, MSBFIRST, b);   // bit 7 → Q7 ... bit 0 → Q0
  digitalWrite(PIN_595_LATCH, HIGH);
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_595_DATOS, OUTPUT);
  pinMode(PIN_595_RELOJ, OUTPUT);
  pinMode(PIN_595_LATCH, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_JOY_SW, INPUT_PULLUP);
  pinMode(PIN_BOTON, INPUT_PULLUP);
  analogReadResolution(10);          // 0..1023, como el Arduino UNO
  escribir595(0);
  Serial.println("\nKernel Panic · prueba del circuito");
  Serial.println("Mové el joystick, presioná SW (clic en el joystick) y el botón DISPARO (o la tecla K).");
}

void loop() {
  if (millis() - ultimoPaso < 50) return;   // un paso cada 50 ms, sin delay()
  ultimoPaso = millis();

  // En el joystick de Wokwi, izquierda y arriba dan 1023: se invierten para que coincidan con el real
  int x = 1023 - analogRead(PIN_JOY_X);
  int y = 1023 - analogRead(PIN_JOY_Y);
  uint8_t sw = digitalRead(PIN_JOY_SW);      // 1 = suelto, 0 = presionado
  uint8_t boton = digitalRead(PIN_BOTON);

  int dx = x - 512, dy = y - 512;
  uint8_t dir = 0;
  if (abs(dx) >= ZONA_MUERTA || abs(dy) >= ZONA_MUERTA)
    dir = abs(dx) > abs(dy) ? (dx < 0 ? 3 : 4) : (dy < 0 ? 1 : 2);

  // LEDs: uno que sigue al eje X; con el botón de disparo se prenden todos
  uint8_t leds = 1 << map(x, 0, 1023, 0, 7);
  if (boton == 0) leds = 0xFF;
  escribir595(leds);

  // Buzzer: un pitido al presionar cada botón
  if (botonAntes == 1 && boton == 0) tone(PIN_BUZZER, 1500, 60);
  if (swAntes == 1 && sw == 0)       tone(PIN_BUZZER, 880, 120);
  botonAntes = boton; swAntes = sw;

  if (millis() - ultimoInforme >= 250) {
    ultimoInforme = millis();
    Serial.printf("X=%4d  Y=%4d  dirección=%-9s  SW=%d  DISPARO=%d  LEDs=0b", x, y, NOMBRE[dir], sw, boton);
    for (int i = 7; i >= 0; i--) Serial.print((leds >> i) & 1);
    Serial.println();
  }
}
