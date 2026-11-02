/*
  JoystickUNO — El control del juego.
  Kernel Panic · Videojuego IoT con ESP8266

  El ESP8266 tiene una sola entrada analógica y el joystick necesita dos (ejes X e Y).
  El UNO lee el control (joystick + botón de ataque) y le manda los valores crudos
  al ESP por UART, 50 veces por segundo:

      X,Y,SW,BOTON\n      ejemplo: 519,507,1,1

  El UNO NO procesa nada: la calibración, la zona muerta, la dirección y el antirrebote
  los hace el ESP8266.

  Control (en la mano, unido al UNO con un cable de 6 hilos):
    Joystick GND  → GND del UNO
    Joystick +5V  → 5V del UNO
    Joystick VRx  → A0
    Joystick VRy  → A1
    Joystick SW   → pin 2
    Botón ataque  → pin 3   (la otra pata del botón, al mismo GND del joystick)

  Hacia el ESP8266 (solo estos dos cables cruzan entre las placas):
    TX (pin 1)    → divisor de tensión (1 kΩ arriba, 2 x 1 kΩ abajo) → D2 del ESP8266
    GND del UNO   → GND del ESP8266

  Alimentación: cada placa con su propio cable USB (notebook, cargador doble o power bank).
  No se une el 5V del UNO con el NodeMCU.

  Para cargar este programa podés dejar conectado el cable al ESP: el pin TX no molesta.
  Abriendo el Monitor Serie (38400 baudios) ves exactamente lo que recibe el ESP.
*/

const int PIN_X = A0;
const int PIN_Y = A1;
const int PIN_SW = 2;
const int PIN_BOTON = 3;
const unsigned long PERIODO_MS = 20;   // 50 envíos por segundo

unsigned long ultimoEnvio = 0;

void setup() {
  Serial.begin(38400);
  pinMode(PIN_SW, INPUT_PULLUP);       // suelto = 1, presionado = 0
  pinMode(PIN_BOTON, INPUT_PULLUP);    // suelto = 1, presionado = 0
}

void loop() {
  if (millis() - ultimoEnvio >= PERIODO_MS) {   // sin delay()
    ultimoEnvio = millis();
    Serial.print(analogRead(PIN_X));
    Serial.print(',');
    Serial.print(analogRead(PIN_Y));
    Serial.print(',');
    Serial.print(digitalRead(PIN_SW));
    Serial.print(',');
    Serial.println(digitalRead(PIN_BOTON));   // println agrega "\r\n": el ESP ignora el \r
  }
}
