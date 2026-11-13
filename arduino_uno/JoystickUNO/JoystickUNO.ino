/*
  JoystickUNO — El control del juego.
  Kernel Panic · Videojuego IoT con ESP8266

  El ESP8266 tiene una sola entrada analógica y el joystick necesita dos (ejes X e Y).
  El UNO lee el control (joystick + botón de disparo) y le manda los valores al ESP por UART,
  50 veces por segundo, a 19200 baudios:

      X,Y,SW,BOTON,CONTROL\n      ejemplo: 519,507,1,1,10

  CONTROL es un número de verificación (checksum): (X + 3·Y + 7·SW + 11·BOTON) mod 256.
  El ESP hace la misma cuenta; si no coincide, la línea llegó con errores y la descarta.

  Control:
    Joystick GND → GND · +5V → 5V · VRx → A0 · VRy → A1 · SW → pin 2
    Botón de disparo → pin 3 (la otra pata, en diagonal, a GND)
  Hacia el ESP8266:
    TX (pin 1) → divisor de tensión (1 kΩ arriba, 2 × 1 kΩ abajo) → D2 del ESP8266
    GND del UNO → GND del ESP8266
  Alimentación: cada placa con su propio USB.
*/

const int PIN_X = A0;
const int PIN_Y = A1;
const int PIN_SW = 2;
const int PIN_BOTON = 3;
const unsigned long PERIODO_MS = 20;   // 50 envíos por segundo

unsigned long ultimoEnvio = 0;

void setup() {
  Serial.begin(19200);                 // tiene que ser igual a UNO_BAUDIOS en config.h del ESP
  pinMode(PIN_SW, INPUT_PULLUP);       // suelto = 1, presionado = 0
  pinMode(PIN_BOTON, INPUT_PULLUP);
}

void loop() {
  if (millis() - ultimoEnvio >= PERIODO_MS) {   // sin delay()
    ultimoEnvio = millis();
    int x = analogRead(PIN_X);
    int y = analogRead(PIN_Y);
    int sw = digitalRead(PIN_SW);
    int boton = digitalRead(PIN_BOTON);
    int control = (x + 3L * y + 7 * sw + 11 * boton) % 256;
    Serial.print(x);     Serial.print(',');
    Serial.print(y);     Serial.print(',');
    Serial.print(sw);    Serial.print(',');
    Serial.print(boton); Serial.print(',');
    Serial.println(control);
  }
}
