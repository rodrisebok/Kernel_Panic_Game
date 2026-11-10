/*
  Prueba 3 · Control en el Arduino UNO (diagnóstico)
  Muestra los ejes del joystick, la dirección y el estado de los dos botones.
  Monitor Serie a 9600 baudios.
  Joystick: GND→GND · +5V→5V · VRx→A0 · VRy→A1 · SW→pin 2
  Botón de disparo: pin 3 y GND (patas en diagonal)
*/
const int PIN_X = A0;
const int PIN_Y = A1;
const int PIN_SW = 2;
const int PIN_BOTON = 3;
const int ZONA_MUERTA = 150;

int swAntes = 1, botonAntes = 1;
unsigned long ultimo = 0;

void setup() {
  Serial.begin(9600);
  pinMode(PIN_SW, INPUT_PULLUP);      // suelto = 1, presionado = 0
  pinMode(PIN_BOTON, INPUT_PULLUP);
  Serial.println("\n=== Prueba del control ===");
}

void loop() {
  int x = analogRead(PIN_X);
  int y = analogRead(PIN_Y);
  int sw = digitalRead(PIN_SW);
  int boton = digitalRead(PIN_BOTON);

  // Avisa en el momento exacto en que se aprieta o se suelta cada botón
  if (sw != swAntes)       { Serial.println(sw == 0 ? ">>> SW del joystick APRETADO" : ">>> SW del joystick suelto"); swAntes = sw; }
  if (boton != botonAntes) { Serial.println(boton == 0 ? ">>> BOTÓN de disparo APRETADO" : ">>> BOTÓN de disparo suelto"); botonAntes = boton; }

  if (millis() - ultimo >= 300) {      // un resumen cada 0,3 s
    ultimo = millis();
    int dx = x - 512, dy = y - 512;
    const char* dir = "CENTRO";
    if (abs(dx) >= ZONA_MUERTA || abs(dy) >= ZONA_MUERTA)
      dir = abs(dx) > abs(dy) ? (dx < 0 ? "X baja (hacia 0)" : "X sube (hacia 1023)")
                              : (dy < 0 ? "Y baja (hacia 0)" : "Y sube (hacia 1023)");
    Serial.print("X="); Serial.print(x);
    Serial.print("  Y="); Serial.print(y);
    Serial.print("  -> "); Serial.print(dir);
    Serial.print("   SW="); Serial.print(sw == 0 ? "APRETADO" : "suelto");
    Serial.print("   BOTON="); Serial.println(boton == 0 ? "APRETADO" : "suelto");
  }
}
