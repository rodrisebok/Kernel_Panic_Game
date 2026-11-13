/*
  hardware.h — Entradas y salidas físicas.
    - Joystick y botón de disparo: en el ESP8266 llegan por UART desde el Arduino UNO
      ("X,Y,SW,BOTON\n"); en el ESP32 (Wokwi) se leen directo
    - Pulsador SW: antirrebote y detección del flanco
    - 74HC595: barra de 8 LEDs
    - Buzzer: cola de notas que no bloquea
*/
#pragma once

// ===================== Direcciones =====================
enum Direccion : uint8_t { CENTRO, ARRIBA, ABAJO, IZQUIERDA, DERECHA };
const char* const DIR_NOMBRE[] = { "CENTRO", "ARRIBA", "ABAJO", "IZQUIERDA", "DERECHA" };

// ===================== Estado del hardware =====================
struct Hardware {
  // Joystick (valores crudos 0..1023)
  int joyX = 512, joyY = 512;          // última lectura recibida
  int vistaX = 512, vistaY = 512;      // lo que usó el último tick (para mostrar en la web)
  int centroX = 512, centroY = 512;    // centro calibrado al arrancar
  bool calibrado = false;
  uint8_t calN = 0;
  long calSumaX = 0, calSumaY = 0;
  Direccion dir = CENTRO;              // dirección principal (la que pide la consigna)

  // En un juego de plataformas se usan los dos ejes a la vez (caminar mientras se salta)
  bool izq = false, der = false, arriba = false, abajo = false;
  bool arribaAntes = false;
  bool saltoFlanco = false;            // "arriba" se acaba de activar en este tick

  // Pulsador SW (INPUT_PULLUP: 1 = suelto, 0 = presionado)
  uint8_t swCrudo = 1, swEstable = 1, swCuenta = 0;
  bool swFlanco = false;               // se acaba de presionar (este tick)

  // Botón de disparo (1 = suelto, 0 = presionado)
  uint8_t botonCrudo = 1, boton = 1;

  // Enlace con el UNO
  bool unoOk = false;
  uint32_t unoUltimo = 0;
  uint32_t unoPaquetes = 0, unoErrores = 0;

  // Salidas
  uint8_t leds = 0, ledsEnviados = 0xFF;
  uint16_t buzzer = 0;

  // Teclado de la página (solo para pruebas)
  uint16_t teclasWeb = 0;
  uint32_t teclasWebT = 0;
} HW;

// ===================== Joystick =====================
// Guarda una lectura nueva (venga del UNO o leída directo) y calibra el centro.
void nuevaLecturaJoystick(long x, long y, long sw, long boton) {
#if INTERCAMBIAR_EJES
  long t = x; x = y; y = t;
#endif
#if INVERTIR_X
  x = 1023 - x;
#endif
#if INVERTIR_Y
  y = 1023 - y;
#endif
  HW.joyX = x; HW.joyY = y; HW.swCrudo = sw; HW.botonCrudo = boton;
  HW.unoOk = true; HW.unoUltimo = millis(); HW.unoPaquetes++;

  // Calibración: promedio de las primeras 16 lecturas con la palanca suelta
  if (!HW.calibrado && abs(x - 512) < 120 && abs(y - 512) < 120) {
    HW.calSumaX += x; HW.calSumaY += y;
    if (++HW.calN >= 16) {
      HW.centroX = HW.calSumaX / 16;
      HW.centroY = HW.calSumaY / 16;
      HW.calibrado = true;
    }
  }
}

#if JOYSTICK_DIRECTO
// ESP32 (Wokwi): tiene varias entradas analógicas, lee los dos ejes directamente.
void leerUno() {}
void leerJoystickDirecto() {
  nuevaLecturaJoystick(analogRead(PIN_JOY_X), analogRead(PIN_JOY_Y), digitalRead(PIN_JOY_SW), digitalRead(PIN_BOTON));
}
#else
// ESP8266: el joystick llega por UART desde el Arduino UNO.
SoftwareSerial unoSerial(PIN_UNO_RX, -1);   // solo recibe
char unoBuf[24];
uint8_t unoLen = 0;

// Procesa una línea completa "X,Y,SW,BOTON,CONTROL". Si algo no cierra, se descarta.
// CONTROL es un número de verificación (checksum) que calcula el UNO con los otros cuatro:
// si un carácter llegó mal, la cuenta no coincide y la línea se ignora.
long checksumUno(long x, long y, long sw, long boton) { return (x + 3 * y + 7 * sw + 11 * boton) % 256; }

bool leerNumero(char*& p, long& v, bool ultimo) {
  char* fin;
  v = strtol(p, &fin, 10);
  if (fin == p) return false;
  if (ultimo) { if (*fin != 0) return false; }
  else if (*fin != ',') return false;
  p = fin + 1;
  return true;
}

void procesarLineaUno() {
  unoBuf[unoLen] = 0;
  int comas = 0;
  for (uint8_t i = 0; i < unoLen; i++) if (unoBuf[i] == ',') comas++;
  char* p = unoBuf;
  long x, y, sw, boton = 1, control;
  bool ok;
  if (comas == 4)        // formato nuevo, con verificación
    ok = leerNumero(p, x, false) && leerNumero(p, y, false) && leerNumero(p, sw, false) &&
         leerNumero(p, boton, false) && leerNumero(p, control, true) && control == checksumUno(x, y, sw, boton);
  else if (comas == 3)   // formato viejo, sin verificación
    ok = leerNumero(p, x, false) && leerNumero(p, y, false) && leerNumero(p, sw, false) && leerNumero(p, boton, true);
  else ok = false;
  if (ok) ok = x >= 0 && x <= 1023 && y >= 0 && y <= 1023 && (sw == 0 || sw == 1) && (boton == 0 || boton == 1);
  if (!ok) { HW.unoErrores++; return; }
  nuevaLecturaJoystick(x, y, sw, boton);
}

// Se llama en cada vuelta del loop(): lee los caracteres que llegaron, sin esperar.
void leerUno() {
  while (unoSerial.available()) {
    char c = unoSerial.read();
    if (c == '\n') { procesarLineaUno(); unoLen = 0; }
    else if (c != '\r') {
      if (unoLen < sizeof(unoBuf) - 1) unoBuf[unoLen++] = c;
      else unoLen = 0;   // línea demasiado larga: basura, se descarta
    }
  }
  if (HW.unoOk && millis() - HW.unoUltimo > UNO_TIMEOUT_MS) HW.unoOk = false;
}
#endif

// ===================== Lectura de entradas (una vez por tick) =====================
void leerEntradas() {
#if JOYSTICK_DIRECTO
  leerJoystickDirecto();
#endif
  // Si el UNO no responde, el joystick queda en el centro y los botones sueltos.
  int x = HW.unoOk ? HW.joyX : HW.centroX;
  int y = HW.unoOk ? HW.joyY : HW.centroY;
  uint8_t sw = HW.unoOk ? HW.swCrudo : 1;
  uint8_t boton = HW.unoOk ? HW.botonCrudo : 1;

#if MODO_TECLADO_WEB
  // Teclas enviadas por la página (bits: 1 arriba, 2 abajo, 4 izquierda, 8 derecha, 16 disparo, 256 SW)
  if (millis() - HW.teclasWebT < 400 && HW.teclasWeb) {
    uint16_t k = HW.teclasWeb;
    if (k & 1) y = 0;
    if (k & 2) y = 1023;
    if (k & 4) x = 0;
    if (k & 8) x = 1023;
    if (k & 16) boton = 0;
    if (k & 256) sw = 0;
  }
#endif
  HW.vistaX = x; HW.vistaY = y;
  HW.boton = boton;

  // Dirección: zona muerta alrededor del centro; si sale de ahí, manda el eje con mayor desvío
  int dx = x - HW.centroX, dy = y - HW.centroY;
  if (abs(dx) < ZONA_MUERTA && abs(dy) < ZONA_MUERTA) HW.dir = CENTRO;
  else if (abs(dx) > abs(dy)) HW.dir = dx < 0 ? IZQUIERDA : DERECHA;
  else                         HW.dir = dy < 0 ? ARRIBA : ABAJO;

  // Para el juego, cada eje por separado (se puede caminar y saltar a la vez)
  HW.izq = dx < -ZONA_MUERTA;  HW.der = dx > ZONA_MUERTA;
  HW.arriba = dy < -ZONA_MUERTA; HW.abajo = dy > ZONA_MUERTA;
  HW.saltoFlanco = HW.arriba && !HW.arribaAntes;
  HW.arribaAntes = HW.arriba;

  // Pulsador: el cambio cuenta recién cuando se repite en 2 lecturas seguidas (antirrebote)
  HW.swFlanco = false;
  if (sw != HW.swEstable) {
    if (++HW.swCuenta >= 2) {
      HW.swEstable = sw; HW.swCuenta = 0;
      if (sw == 0) HW.swFlanco = true;
    }
  } else HW.swCuenta = 0;
}

// ===================== 74HC595 =====================
// Envía 8 bits en serie y el latch los pasa juntos a las salidas Q0..Q7.
void escribir595(uint8_t b) {
  digitalWrite(PIN_595_LATCH, LOW);
  shiftOut(PIN_595_DATOS, PIN_595_RELOJ, MSBFIRST, b);   // bit 7 → Q7 ... bit 0 → Q0
  digitalWrite(PIN_595_LATCH, HIGH);
  HW.ledsEnviados = b;
}
void actualizar595() {
  if (HW.leds != HW.ledsEnviados) escribir595(HW.leds);   // solo si cambió
}

// ===================== Buzzer =====================
struct Nota { uint16_t frec; uint16_t ms; };
// Cada sonido es una lista de notas terminada en {0,0}
const Nota S_SALTO[]   = { {520, 30}, {780, 40}, {0, 0} };
const Nota S_DISPARO[] = { {1500, 25}, {0, 0} };
const Nota S_BIT[]     = { {1700, 40}, {2200, 50}, {0, 0} };
const Nota S_PISAR[]   = { {300, 40}, {600, 50}, {0, 0} };
const Nota S_MUERTE[]  = { {700, 40}, {1100, 40}, {0, 0} };
const Nota S_GOLPE[]   = { {400, 30}, {0, 0} };
const Nota S_DANO[]    = { {180, 110}, {120, 120}, {0, 0} };
const Nota S_PARCHE[]  = { {1300, 50}, {1800, 60}, {0, 0} };
const Nota S_CHECK[]   = { {660, 80}, {990, 120}, {0, 0} };
const Nota S_PERDIDO[] = { {880, 150}, {660, 150}, {440, 150}, {220, 420}, {0, 0} };
const Nota S_GANADO[]  = { {523, 120}, {659, 120}, {784, 120}, {1047, 360}, {0, 0} };
const Nota S_INICIO[]  = { {440, 60}, {880, 90}, {0, 0} };
const Nota S_JEFE[]    = { {150, 150}, {110, 150}, {80, 260}, {0, 0} };
const Nota S_TIRO[]    = { {300, 40}, {0, 0} };
const Nota S_PAUSA[]   = { {600, 60}, {0, 0} };

// Música del menú: "Tema de Kernel Panic" (melodía original). Corchea = 150 ms; frecuencia 1 = silencio.
#define NC(f, n) { f, (uint16_t)((n) * 150) }
const Nota S_TEMA[] = {
  NC(330,1),NC(392,1),NC(523,1),NC(392,1), NC(659,2),NC(587,1),NC(523,1),
  NC(587,1),NC(494,1),NC(392,1),NC(494,1), NC(587,3),NC(1,1),
  NC(349,1),NC(440,1),NC(523,1),NC(440,1), NC(698,2),NC(659,1),NC(587,1),
  NC(659,1),NC(523,1),NC(392,1),NC(523,1), NC(659,3),NC(1,1),
  NC(440,1),NC(440,1),NC(523,1),NC(440,1), NC(392,1),NC(440,1),NC(523,2),
  NC(494,1),NC(494,1),NC(587,1),NC(494,1), NC(440,1),NC(494,1),NC(587,2),
  NC(523,1),NC(659,1),NC(784,1),NC(659,1), NC(880,2),NC(784,1),NC(659,1),
  NC(698,1),NC(587,1),NC(494,1),NC(587,1), NC(784,3),NC(1,1),
  NC(659,1),NC(587,1),NC(523,1),NC(494,1), NC(523,1),NC(392,1),NC(330,1),NC(392,1),
  NC(523,4),NC(1,8),
  {0, 0}
};

const Nota* sonidoActual = nullptr;
uint8_t notaIdx = 0;
int16_t notaRestante = 0;

// importante = interrumpe lo que esté sonando. Si no, solo suena si el buzzer está libre.
void sfx(const Nota* s, bool importante) {
#if SONIDO
  if (importante) { sonidoActual = s; notaIdx = 0; notaRestante = 0; noTone(PIN_BUZZER); HW.buzzer = 0; }
  else if (!sonidoActual && notaRestante <= 0) { sonidoActual = s; notaIdx = 0; }
#endif
}

// Una vez por tick: avanza la cola de notas. tone() con duración no bloquea.
void actualizarBuzzer() {
  if (notaRestante > 0) {
    notaRestante -= TICK_MS;
    if (notaRestante <= 0) HW.buzzer = 0;
  }
  if (notaRestante <= 0 && sonidoActual) {
    Nota n = sonidoActual[notaIdx];
    if (n.ms == 0) { sonidoActual = nullptr; HW.buzzer = 0; }
    else {
      if (n.frec > 1) { tone(PIN_BUZZER, n.frec, n.ms * 9 / 10); HW.buzzer = n.frec; }   // 90 %: un respiro entre notas
      else { noTone(PIN_BUZZER); HW.buzzer = 0; }                                    // frecuencia 1 = silencio
      notaRestante = n.ms;
      notaIdx++;
    }
  }
}

// ===================== Inicio =====================
void iniciarHardware() {
  pinMode(PIN_595_DATOS, OUTPUT);
  pinMode(PIN_595_RELOJ, OUTPUT);
  pinMode(PIN_595_LATCH, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  escribir595(0);
#if JOYSTICK_DIRECTO
  pinMode(PIN_JOY_SW, INPUT_PULLUP);
  pinMode(PIN_BOTON, INPUT_PULLUP);
  analogReadResolution(10);   // el ESP32 lee 0..4095 por defecto; así queda 0..1023 como el UNO
#else
  unoSerial.begin(UNO_BAUDIOS, SWSERIAL_8N1, PIN_UNO_RX, -1, false, 256);   // búfer de 256 bytes
#endif
}
