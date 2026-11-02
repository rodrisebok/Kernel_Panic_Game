/*
  Prueba 4 · Recepción del joystick desde el Arduino UNO (ESP8266)
  Antes: cargá JoystickUNO.ino en el UNO.
  Esta prueba muestra lo que llega por D2 (X,Y,SW,BOTON) y la dirección calculada.

  Conexiones:
    TX del UNO (pin 1) → 1 kΩ → D2 del ESP8266
                                 └→ 1 kΩ → 1 kΩ → GND   (divisor: 5 V → 3,3 V)
    GND del UNO → GND del ESP8266   (¡obligatorio!)

  Si no llega nada: revisá el GND común y que los dos usen 38400 baudios.
*/
#include <SoftwareSerial.h>

SoftwareSerial uno(D2, -1);
char buf[24];
uint8_t len = 0;
const int ZONA_MUERTA = 150;

void setup() {
  Serial.begin(115200);
  uno.begin(38400);
  Serial.println("\nEsperando datos del UNO...");
}

void loop() {
  while (uno.available()) {
    char c = uno.read();
    if (c == '\n') {
      buf[len] = 0; len = 0;
      int x, y, sw, bt = 1;
      if (sscanf(buf, "%d,%d,%d,%d", &x, &y, &sw, &bt) >= 3) {
        int dx = x - 512, dy = y - 512;
        const char* dir = "CENTRO";
        if (abs(dx) >= ZONA_MUERTA || abs(dy) >= ZONA_MUERTA)
          dir = abs(dx) > abs(dy) ? (dx < 0 ? "IZQUIERDA" : "DERECHA") : (dy < 0 ? "ARRIBA" : "ABAJO");
        Serial.printf("X=%4d  Y=%4d  SW=%d  BOTON=%d  → %s\n", x, y, sw, bt, dir);
      } else {
        Serial.print("Línea con error: "); Serial.println(buf);
      }
    } else if (c != '\r' && len < sizeof(buf) - 1) buf[len++] = c;
  }
}
