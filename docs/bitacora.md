# Bitácora de pruebas · Kernel Panic

Registro del montaje y de las pruebas del circuito real, en el orden en que se hicieron.
Cada prueba usa un sketch de la carpeta `pruebas/` (o del Arduino UNO) y prueba una sola parte del sistema.

| # | Prueba | Sketch | Estado |
|---|---|---|---|
| 1 | NodeMCU + 74HC595 + 8 LEDs | `pruebas/Prueba1_LEDs_595` | ✅ Superada |
| 2 | Buzzer | `pruebas/Prueba2_Buzzer` | ✅ Superada |
| 3 | Control: joystick y botón en el Arduino UNO | `pruebas/Prueba3_Control` y `arduino_uno/JoystickUNO` | ✅ Superada |
| 4 | Divisor de tensión (medición con tester) | `pruebas/Prueba4_Divisor` (sin programa) | ✅ Superada |
| 5 | Comunicación UART UNO → ESP8266 | `pruebas/Prueba5_UART_UNO` | ✅ Superada |
| 6 | Juego completo con página web | `firmware/KernelPanic` (ver `pruebas/Prueba6_Juego_completo`) | ✅ Superada |

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

---

## Prueba 3 · Control: joystick y botón en el Arduino UNO — 10/11/2026 ✅

**Objetivo:** comprobar que el Arduino UNO lee los dos ejes del joystick (A0 y A1), el pulsador SW (pin 2) y el botón de disparo (pin 3).

**Resultado esperado:** con la palanca suelta, cada eje cerca de 512; en los extremos, 0 y 1023. Cada botón vale 1 suelto y 0 presionado (`INPUT_PULLUP`).

**Resultado obtenido:** los dos ejes recorren de 0 a 1023 y los dos botones responden correctamente.

**Problemas encontrados y cómo se resolvieron:**

1. **"Acceso denegado" al cargar en COM4.** El Monitor Serie tenía el puerto abierto. Se cerró y la carga funcionó.
2. **Los dos ejes quedaban fijos en 1023.** Sin GND, los potenciómetros del joystick quedan en 5 V. El cable de GND estaba flojo: funcionaba solo al inclinar el módulo (falso contacto). Se ajustaron las puntas y se pasó el GND a la tira POWER del UNO.
3. **El botón de disparo daba siempre 0.** Las dos patas usadas estaban unidas por dentro del pulsador. Se giró el botón un cuarto de vuelta.
4. **El botón de disparo no cambiaba nunca.** Su cable estaba en el pin 2 (el del SW del joystick) en lugar del pin 3. Se movió al pin 3.
5. **El botón no se podía sostener en la mano** porque sus patas son muy cortas. Se ubicó en el protoboard, sobre la ranura (columnas 58 y 60), con una pata al pin 3 y la diagonal a GND.

**Programa de diagnóstico:** `pruebas/Prueba3_Control` muestra en el Monitor Serie (9600 baudios) los valores de los ejes, la dirección y el estado de cada botón con texto.

---

## Prueba 4 · Divisor de tensión — 10/11/2026 ✅

**Objetivo:** antes de unir las placas, comprobar que la señal del TX del UNO (5 V) llega al ESP8266 reducida a un nivel seguro (3,3 V como máximo).

**Montaje:** TX del UNO → 1 kΩ → punto de unión → 1 kΩ → 1 kΩ → GND. El cable de D2 del NodeMCU sale del punto de unión. Solo el UNO estaba alimentado (por USB, corriendo `JoystickUNO`); el NodeMCU todavía desconectado.

**Mediciones** (tester en V⎓, escala 20 V, punta negra en GND):

| Punto | Esperado | Medido |
|---|---|---|
| Entrada del divisor (TX del UNO) | 4,5 – 5 V | **4,40 V** |
| Punto de unión (hacia D2) | Entrada × 2/3 | **2,93 V** |

**Cálculo:** V<sub>salida</sub> = V<sub>entrada</sub> × 2 kΩ / (1 kΩ + 2 kΩ) = 4,40 V × 2/3 ≈ 2,93 V. El valor medido coincide con el calculado.

**Conclusión:** 2,93 V no supera los 3,3 V del ESP8266, y alcanza para que lo reconozca como un "1" lógico (el umbral está alrededor de 2,5 V). El TX del UNO queda en 4,4 V y no en 5 V por la alimentación del USB y la pequeña corriente del divisor (≈ 1,5 mA); es normal.

---

## Prueba 5 · Comunicación UART del UNO al ESP8266 — 12/11/2026 ✅

**Objetivo:** comprobar que el ESP8266 recibe los datos del joystick que envía el Arduino UNO.

**Montaje:** cada placa con su propio USB. El UNO corre `JoystickUNO` y envía `X,Y,SW,BOTON` 50 veces por segundo a 38400 baudios por su TX. La señal pasa por el divisor (2,93 V) y entra al ESP8266 por D2, que la lee con `SoftwareSerial`. GND común entre las dos placas.

**Resultado obtenido:** el Monitor Serie del NodeMCU (115200 baudios) muestra los valores del joystick y de los botones en tiempo real, y la dirección calculada con la zona muerta.

**Problemas encontrados y cómo se resolvieron:**

1. **El Arduino IDE mostraba las dos placas como "NodeMCU".** El IDE no reconoce automáticamente el UNO clon; hay que elegir la placa y el puerto a mano antes de cada carga. Se identificaron los puertos en el Administrador de dispositivos: COM3 = NodeMCU (CP2102), COM4 = Arduino UNO.
2. **"Failed to connect to ESP8266: Invalid head of packet".** Se estaba cargando el programa del ESP en el COM4, que es el UNO. Con el COM3 la carga funcionó.

**Conclusión:** toda la cadena de entrada funciona: joystick → conversor analógico-digital del UNO → UART → divisor de tensión → ESP8266.

---

## Prueba 6 · Juego completo con página web — 13/11/2026 ✅

**Objetivo:** jugar una partida completa con todo el sistema funcionando junto: control en el UNO, juego en el ESP8266, página web, LEDs y buzzer.

**Montaje:** el circuito completo del protoboard. El UNO corre `JoystickUNO` y el NodeMCU el firmware `KernelPanic`. La página se abre desde el navegador con la dirección que muestra el Monitor Serie.

**Resultado obtenido:**
- La página muestra la intro con la historia; con SW empieza la partida.
- K-1T camina, salta, se agacha y dispara con el joystick y el botón, y recorre las 4 zonas.
- NÚCLEO-0 pasa a la fase 2 a mitad de vida, y al eliminarlo se abre la CPU.
- Los LEDs muestran las vidas y el avance, y el buzzer suena con cada evento.
- El panel de la página muestra en tiempo real los ejes, la dirección, los pulsadores, los LEDs, el buzzer y las líneas UART recibidas.

**Problemas encontrados y cómo se resolvieron:**

1. **Los controles respondían al revés** (derecha movía hacia abajo, izquierda hacia arriba). El joystick está girado en el protoboard. Se agregaron en `config.h` las opciones `INTERCAMBIAR_EJES`, `INVERTIR_X` e `INVERTIR_Y` para corregir la orientación sin tocar el cableado.
2. **Los botones respondían a veces sí y a veces no.** Había cables flojos. Además, para que una línea dañada no se tome como una pulsación, se bajó la velocidad de la UART de 38400 a 19200 baudios y se agregó un valor de control a cada línea: `X,Y,SW,BOTON,CONTROL`. El ESP descarta las líneas que no cierran.
3. **El botón de disparo dejó de funcionar.** Su cable de GND estaba en la línea positiva del protoboard. Al pasarlo a la línea negativa volvió a funcionar.

**Conclusión:** el sistema completo funciona: joystick → UNO → UART → divisor → ESP8266 → juego → página web, LEDs y buzzer.
