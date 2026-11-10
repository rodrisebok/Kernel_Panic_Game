# Bitácora de pruebas · Kernel Panic

Registro del montaje y de las pruebas del circuito real, en el orden en que se hicieron.
Cada prueba usa un sketch de la carpeta `pruebas/` (o del Arduino UNO) y prueba una sola parte del sistema.

| # | Prueba | Sketch | Estado |
|---|---|---|---|
| 1 | NodeMCU + 74HC595 + 8 LEDs | `pruebas/Prueba1_LEDs_595` | ✅ Superada |
| 2 | Buzzer | `pruebas/Prueba2_Buzzer` | ✅ Superada |
| 3 | Control: joystick y botón en el Arduino UNO | `arduino_uno/JoystickUNO` | ⏳ Pendiente |
| 4 | Divisor de tensión (medición con tester) | — | ⏳ Pendiente |
| 5 | Comunicación UART UNO → ESP8266 | `pruebas/Prueba4_UART_UNO` | ⏳ Pendiente |
| 6 | Juego completo con página web | firmware del juego | ⏳ Pendiente |

---

## Prueba 1 · LEDs con el 74HC595 — 10/11/2026 ✅

**Objetivo:** comprobar que el ESP8266 maneja los 8 LEDs a través del registro de desplazamiento usando solo 3 pines (D7 datos, D8 reloj, D0 latch).

**Resultado esperado:** los LEDs se encienden de a uno de Q0 a Q7, vuelven, y después se encienden todos juntos.

**Resultado obtenido:** funciona. Los 8 LEDs encienden en orden.

**Problemas encontrados y cómo se resolvieron:**

1. **El Arduino IDE no mostraba ningún puerto** ("No ports discovered").
   - Primero se probó otro cable USB: el original era solo de carga y no transmitía datos.
   - Con el cable nuevo, el Administrador de dispositivos mostraba *"CP2102 USB to UART Bridge Controller"* con un triángulo amarillo: faltaba el driver.
   - Se instaló el *CP210x Universal Windows Driver* de Silicon Labs y apareció el puerto COM.
2. **Al enchufar, se encendía un LED rojo solo.** Es normal: el 74HC595 arranca con un valor al azar hasta que el ESP le envía datos.
3. **Los 3 LEDs rojos encendían pero los 5 amarillos no.**
   - Como fallaban los 5 a la vez, se buscó una causa común.
   - Los LEDs amarillos estaban colocados al revés (pata larga del lado de GND). Un LED invertido no se daña, simplemente no conduce.
   - Se dieron vuelta y encendieron los 8.

---

## Prueba 2 · Buzzer — 10/11/2026 ✅

**Objetivo:** comprobar que el ESP8266 genera sonidos en el buzzer con `tone()` desde el pin D6.

**Primera versión:** una escala de 8 notas y dos efectos (disparo y daño). Funcionó.

**Versión final:** se reemplazó la escala por una melodía original estilo chiptune, el *"Tema de Kernel Panic"*, que después se usa como música del menú del juego. Cada nota se guarda como un par (frecuencia, duración) y se reproduce con `tone()`.

**Resultado obtenido:** la melodía suena correctamente; cada nota tiene una altura distinta, así que el buzzer es **pasivo** y puede reproducir melodías.

**Observación:** durante esta prueba quedó encendido un LED amarillo. Es normal: el sketch del buzzer no usa el 74HC595, y el chip conserva el último valor que recibió hasta que le llega uno nuevo.
