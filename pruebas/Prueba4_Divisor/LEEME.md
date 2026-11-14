# Prueba 4 · Divisor de tensión

Esta prueba no lleva programa: se hace con el **tester**.

1. Cargá `arduino_uno/JoystickUNO` en el Arduino UNO, para que el TX esté transmitiendo.
2. Poné el tester en **tensión continua (V⎓), escala 20 V**. Punta negra a GND y punta roja en cada punto:
   - **Entrada del divisor** (TX del UNO): tiene que dar entre 4,5 y 5 V.
   - **Salida del divisor** (el punto que va a D2 del ESP8266): tiene que dar 2/3 de la entrada, alrededor de 3 V y nunca más de 3,6 V.

Resultado obtenido: 4,40 V a la entrada y 2,93 V a la salida (ver `docs/pruebas.md`). En una medición posterior la salida dio 2,99 V; la foto está en `docs/medicion_divisor.jpg`.
