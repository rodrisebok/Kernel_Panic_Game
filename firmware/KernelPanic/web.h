/*
  web.h — Wi-Fi y servidor web.
    GET /            la página del juego (guardada en la memoria flash)
    GET /mapa        el nivel como texto: 15 líneas de 224 caracteres
    GET /estado      el estado del juego en JSON (la página lo pide cada 100 ms)
    GET /tecla?k=N   teclas de la página, solo para probar (MODO_TECLADO_WEB)
*/
#pragma once

ESP8266WebServer server(80);

String ipTexto;

// ===================== Wi-Fi =====================
void iniciarWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_RED, WIFI_CLAVE);
  Serial.printf("\nConectando a la red \"%s\"", WIFI_RED);
  uint32_t t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < WIFI_ESPERA_MS) {
    delay(250);   // solo al arrancar, antes de que empiece el juego
    Serial.print('.');
    // Mientras espera, los LEDs muestran que está conectando
    escribir595(1 << ((millis() / 120) % 8));
  }
  if (WiFi.status() == WL_CONNECTED) {
    ipTexto = WiFi.localIP().toString();
    Serial.printf("\nConectado. Abrí en el navegador:  http://%s\n", ipTexto.c_str());
  } else {
    // No encontró la red: crea la suya propia (Access Point)
    WiFi.disconnect();
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_RED, AP_CLAVE);
    ipTexto = WiFi.softAPIP().toString();
    Serial.printf("\nNo se pudo conectar a \"%s\".\n", WIFI_RED);
    Serial.printf("Se creó la red \"%s\" (clave: %s).\n", AP_RED, AP_CLAVE);
    Serial.printf("Conectá el celular a esa red y abrí:  http://%s\n", ipTexto.c_str());
  }
  escribir595(0);
}

// ===================== JSON del estado =====================
// Se arma "a mano" con un String: así no hace falta instalar ninguna biblioteca.
String json;

void agregar(const char* clave, long valor) {
  json += '"'; json += clave; json += "\":"; json += valor; json += ',';
}

void manejarEstado() {
  uint16_t desde = server.hasArg("ev") ? server.arg("ev").toInt() : 0;
  json = "";
  json.reserve(1800);
  json += '{';
  json += "\"st\":\""; json += ESTADO_NOMBRE[G.st]; json += "\",";
  agregar("t", G.tick);
  agregar("pts", G.puntos);
  agregar("bits", G.bits);
  agregar("rec", G.record);
  agregar("cam", G.cam);
  agregar("chk", G.check);
  agregar("zv", G.zonaVista);
  agregar("ban", G.banner);
  agregar("av", G.aviso);
  agregar("avt", G.avisoT);
  agregar("bloq", G.bloqueo);
  agregar("mv", G.mapaVersion);
  agregar("jv", G.jefe >= 0 ? en[G.jefe].vida : -1);

  // Jugador: x, y, vidas, agachado, mira, invulnerable, fogonazo
  json += "\"p\":[";
  json += P.x; json += ','; json += P.y; json += ','; json += P.vidas; json += ',';
  json += P.agachado ? 1 : 0; json += ','; json += P.mira; json += ',';
  json += P.inv; json += ','; json += P.flash; json += "],";

  // Enemigos cerca de la cámara: id, tipo, x, y, dir, golpeado
  json += "\"e\":[";
  bool primero = true;
  for (int i = 0; i < MAX_EN; i++) {
    Enemigo& e = en[i];
    if (!e.activo || e.x < G.cam - 3 || e.x > G.cam + VIEW + 3) continue;
    if (!primero) json += ',';
    primero = false;
    json += '['; json += e.id; json += ','; json += e.tipo; json += ','; json += e.x; json += ',';
    json += e.y; json += ','; json += e.dir; json += ','; json += e.flash; json += ']';
  }
  json += "],";

  // Plataformas móviles: x, y
  json += "\"m\":[";
  for (int i = 0; i < nPlats; i++) {
    if (i) json += ',';
    json += '['; json += i; json += ','; json += plats[i].x; json += ','; json += plats[i].y; json += ']';
  }
  json += "],";

  // Balas del jugador y paquetes del jefe: id, x, y, dir (o alto)
  json += "\"b\":[";
  primero = true;
  for (int i = 0; i < MAX_BALAS; i++) if (balas[i].activa) {
    if (!primero) json += ',';
    primero = false;
    json += '['; json += balas[i].id; json += ','; json += balas[i].x; json += ','; json += balas[i].y; json += ','; json += balas[i].dir; json += ']';
  }
  json += "],\"eb\":[";
  primero = true;
  for (int i = 0; i < MAX_EBALAS; i++) if (ebalas[i].activa) {
    if (!primero) json += ',';
    primero = false;
    json += '['; json += ebalas[i].id; json += ','; json += ebalas[i].x; json += ','; json += ebalas[i].y; json += ','; json += ebalas[i].alto ? 1 : 0; json += ']';
  }
  json += "],";

  // Eventos nuevos desde el último pedido (para explosiones y textos en la página)
  json += "\"ev\":[";
  primero = true;
  if ((uint16_t)(eventoSeq - desde) > MAX_EV) desde = eventoSeq;   // página nueva o muy atrasada: sin eventos viejos
  for (uint16_t s = desde + 1; s != (uint16_t)(eventoSeq + 1); s++) {
    Evento& v = eventos[s % MAX_EV];
    if (v.seq != s) continue;
    if (!primero) json += ',';
    primero = false;
    json += '['; json += v.tipo; json += ','; json += v.x; json += ','; json += v.y; json += ','; json += v.v; json += ']';
  }
  json += "],";
  agregar("evs", eventoSeq);

  // Hardware: lo que pide la consigna mostrar (ejes, dirección, pulsadores, LEDs, buzzer)
  json += "\"hw\":{";
  agregar("x", HW.vistaX);
  agregar("y", HW.vistaY);
  json += "\"dir\":\""; json += DIR_NOMBRE[HW.dir]; json += "\",";
  agregar("sw", HW.swEstable);
  agregar("bt", HW.boton);
  agregar("uno", HW.unoOk ? 1 : 0);
  agregar("ok", HW.unoPaquetes);
  agregar("err", HW.unoErrores);
  agregar("leds", HW.leds);
  json += "\"buz\":"; json += HW.buzzer;
  json += "}}";

  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "application/json", json);
}

void manejarMapa() {
  String m;
  m.reserve(H * (W + 1));
  for (int y = 0; y < H; y++) { m += grid[y]; m += '\n'; }
  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "text/plain", m);
}

void manejarTecla() {
#if MODO_TECLADO_WEB
  HW.teclasWeb = server.arg("k").toInt();
  HW.teclasWebT = millis();
#endif
  server.send(204);
}

void iniciarServidor() {
  server.on("/", []() { server.send_P(200, "text/html; charset=utf-8", PAGINA); });
  server.on("/mapa", manejarMapa);
  server.on("/estado", manejarEstado);
  server.on("/tecla", manejarTecla);
  server.onNotFound([]() { server.send(404, "text/plain", "No existe"); });
  server.begin();
}
