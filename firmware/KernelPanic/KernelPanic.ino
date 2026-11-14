/*
  KernelPanic.ino — Videojuego IoT de plataformas con ESP8266
  Arquitectura de Computadoras · Universidad Nacional de Hurlingham
  Integrantes: Sebok Rodrigo, Trautweiler Franco

  K-1T, una inteligencia artificial que acaba de despertar, recorre una computadora (fuente de alimentación,
  placa madre, memoria RAM y CPU) hasta apagar a NÚCLEO-0, una IA rival que quiere tomar el control de la red.

  Hardware:
    - Arduino UNO: lee el joystick y el botón de disparo y los manda por UART (JoystickUNO.ino)
    - NodeMCU ESP8266: lógica del juego, servidor web, LEDs y buzzer (este programa)
    - TX del UNO → divisor de tensión (1 kΩ / 2 × 1 kΩ) → D2 del ESP · GND común
    - 74HC595 en D7 (datos), D8 (reloj), D0 (latch) → 8 LEDs: Q0–Q2 integridad, Q3–Q7 avance
    - Buzzer en D6

  Cómo usarlo:
    1. Poné el nombre y la clave de tu Wi-Fi en config.h (WIFI_RED y WIFI_CLAVE).
    2. Cargá el programa (placa: NodeMCU 1.0 (ESP-12E Module)).
    3. Abrí el Monitor Serie a 115200: muestra la dirección para abrir en el navegador.
       Si no se conecta a tu red, el ESP crea la suya: "KernelPanic-ESP" (clave: kernelpanic)
       y la página se abre en http://192.168.4.1
    4. Pulsá SW (apretá la palanca) para empezar.

  No hace falta instalar bibliotecas: todo viene con el soporte de placas ESP8266.
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>

#include "config.h"
#include "hardware.h"
#include "juego.h"
#include "pagina.h"
#include "web.h"

uint32_t ultimoTick = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("\n\n=== KERNEL PANIC ===");
  iniciarHardware();
  iniciarJuego();
  iniciarWifi();
  iniciarServidor();
  Serial.println("Listo. Pulsá SW en el joystick para empezar.");
}

void loop() {
  server.handleClient();   // atiende los pedidos de la página
  leerUno();               // junta los caracteres que llegan del Arduino UNO

  // El juego avanza un paso cada TICK_MS, sin delay()
  if (millis() - ultimoTick >= TICK_MS) {
    ultimoTick += TICK_MS;
    if (millis() - ultimoTick > 500) ultimoTick = millis();   // si se atrasó mucho, no intenta recuperar
    tickJuego();
  }
}
