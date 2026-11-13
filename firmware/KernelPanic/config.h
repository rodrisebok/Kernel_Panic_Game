/*
  config.h — Todo lo que se puede ajustar sin tocar la lógica del juego.
  Kernel Panic · Videojuego IoT con ESP8266

  El mismo código compila para dos placas:
    - NodeMCU ESP8266: el hardware real del TP (joystick leído por el Arduino UNO).
    - ESP32 DevKit: la simulación en Wokwi (que no tiene ESP8266). El ESP32 tiene
      varias entradas analógicas, así que ahí el joystick y el botón se leen directo.
  El compilador elige solo según la placa seleccionada.
*/
#pragma once

#if defined(ESP32)
// =====================================================================
//  ESP32 · simulación en Wokwi
// =====================================================================
const char* WIFI_RED   = "Wokwi-GUEST";   // red simulada de Wokwi, sin clave
const char* WIFI_CLAVE = "";
const int   WIFI_CANAL = 6;               // Wokwi conecta más rápido en el canal 6

const uint8_t PIN_JOY_X     = 34;  // joystick HORZ (entrada analógica)
const uint8_t PIN_JOY_Y     = 35;  // joystick VERT (entrada analógica)
const uint8_t PIN_JOY_SW    = 27;  // joystick SEL (pulsador)
const uint8_t PIN_BOTON     = 32;  // botón de disparo
const uint8_t PIN_BUZZER    = 26;
const uint8_t PIN_595_DATOS = 23;  // 74HC595 DS
const uint8_t PIN_595_RELOJ = 18;  // 74HC595 SHCP
const uint8_t PIN_595_LATCH = 19;  // 74HC595 STCP

#define JOYSTICK_DIRECTO 1   // el ESP32 lee el joystick sin el Arduino UNO
#define INTERCAMBIAR_EJES 0
#define INVERTIR_X 1         // en el joystick de Wokwi, izquierda da 1023
#define INVERTIR_Y 1         // y arriba da 1023

#else
// =====================================================================
//  NodeMCU ESP8266 · hardware real
// =====================================================================
// Modo estación (STA): el ESP se conecta a una red existente.
// En clase, usá la red y la clave que les dio la profesora.
const char* WIFI_RED   = "TU_RED";
const char* WIFI_CLAVE = "TU_CLAVE";

const uint8_t PIN_UNO_RX    = D2;  // recibe "X,Y,SW,BOTON" del Arduino UNO (a través del divisor)
const uint8_t PIN_BUZZER    = D6;  // buzzer
const uint8_t PIN_595_DATOS = D7;  // 74HC595 pata 14 (DS)
const uint8_t PIN_595_RELOJ = D8;  // 74HC595 pata 11 (SH_CP)
const uint8_t PIN_595_LATCH = D0;  // 74HC595 pata 12 (ST_CP)

#define JOYSTICK_DIRECTO 0   // el joystick llega por UART desde el Arduino UNO

// Orientación del joystick. En nuestro control el módulo quedó girado 90°:
// la palanca hacia adelante movía el eje X y hacia la derecha movía el eje Y.
// Por eso se intercambian los ejes y se invierte el vertical.
// Si en tu armado las direcciones quedan mal, probá cambiando estos tres valores (0 o 1).
#define INTERCAMBIAR_EJES 1  // 1 = el eje X del UNO es el vertical y el Y es el horizontal
#define INVERTIR_X 0         // 1 = izquierda y derecha al revés
#define INVERTIR_Y 1         // 1 = arriba y abajo al revés
#endif

// ===================== Comunes a las dos placas =====================
// Si no logra conectarse en WIFI_ESPERA_MS, crea su propia red (modo Access Point).
// Te conectás con el celular a esta red y entrás a http://192.168.4.1
const char* AP_RED   = "KernelPanic-ESP";
const char* AP_CLAVE = "kernelpanic";      // mínimo 8 caracteres
const uint32_t WIFI_ESPERA_MS = 10000;

// Comunicación con el UNO
const uint32_t UNO_BAUDIOS     = 19200;   // tiene que ser igual a la del JoystickUNO.ino
const uint16_t UNO_TIMEOUT_MS  = 300;   // sin datos durante este tiempo = joystick desconectado

// Joystick
const int ZONA_MUERTA = 150;   // alrededor del centro, se ignora el movimiento

// Varios
const uint16_t TICK_MS = 50;   // el juego avanza un paso cada 50 ms (20 veces por segundo)
#define SONIDO 1               // 0 = buzzer apagado

// Disparo con el pulsador del joystick.
//   0 = el botón aparte (pin 3 del UNO) dispara y SW pausa la partida.
//   1 = durante la partida, apretar la palanca (SW) también dispara (no hay pausa).
//       Sirve si el botón aparte falla. El botón aparte sigue funcionando igual.
#define DISPARO_CON_SW 1

// Permite jugar desde la página con el teclado, para probar sin el joystick conectado.
// Para la entrega, poné 0: así el juego solo se controla con el hardware.
#define MODO_TECLADO_WEB 1
