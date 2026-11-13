/*
  juego.h — La lógica completa del juego de plataformas.
  Kernel Panic · Videojuego IoT con ESP8266

  Todo corre en el ESP8266, un paso (tick) cada 50 ms. La página web solo dibuja lo que
  el ESP le cuenta por /estado.

  El nivel es una grilla de 224 × 15 casillas guardada como texto:
    '#' bloque sólido      '=' plataforma (se atraviesa desde abajo)
    'C' bit (+10)          'P' parche (+1 integridad)
    'X' peligro (arco eléctrico o ventilador, se prende y se apaga)
    'K' checkpoint         'D' compuerta del jefe     '>' la CPU (meta)
*/
#pragma once

// ===================== Constantes =====================
const int W = 224, H = 15, VIEW = 20;
const int BOSS_X0 = 192, GATE_X = 216, FLAG_X = 220;

enum Estado : uint8_t { MENU, JUGANDO, PAUSA, PERDIDO, GANADO };
const char* const ESTADO_NOMBRE[] = { "MENU", "JUGANDO", "PAUSA", "PERDIDO", "GANADO" };
enum TipoEnemigo : uint8_t { BUG, VIRUS, JEFE };

// Avisos que la página muestra como cartel
enum Aviso : uint8_t { AV_NADA, AV_JEFE, AV_CHECK, AV_JEFE_MUERTO, AV_VACIO, AV_FASE2 };

// Eventos para los efectos visuales de la página (explosiones, textos que suben, etc.)
enum TipoEvento : uint8_t {
  EV_BIT = 1, EV_PARCHE, EV_MUERTE, EV_GOLPE, EV_PARED, EV_POLVO,
  EV_HERIDO, EV_ZAP, EV_JEFE_MUERTO, EV_JEFE, EV_GANADO, EV_PERDIDO, EV_VACIO, EV_FASE2
};

int zonaDe(int x) { return x >= 166 ? 3 : x >= 111 ? 2 : x >= 56 ? 1 : 0; }

// ===================== Estructuras =====================
struct Enemigo {
  bool activo;
  uint8_t id, tipo;
  int16_t x, y;
  int8_t w, h, dir;
  int8_t cd, hcd, base, fase, vida, flash;
  int8_t tiro;
  bool alto;
};
struct Spawn { uint8_t tipo; int16_t x, y; };
struct Plataforma { int16_t x, y, a, b; int8_t dir, cd; };
struct Bala { bool activa; uint16_t id; int16_t x, y; int8_t dir, vida, cd; bool alto; };
struct Evento { uint16_t seq; uint8_t tipo; int16_t x, y, v; };

struct Jugador {
  int16_t x, y;
  int8_t vidas, mira;
  bool agachado;
  int8_t subir, flotar, caidas, vcd, hcd, disparoCd, inv, flash;
} P;

struct Juego {
  Estado st = MENU;
  uint32_t tick = 0;
  uint32_t puntos = 0, record = 0;
  uint16_t bits = 0;
  int16_t check = 2;
  uint32_t ckPuntos = 0;
  uint16_t ckBits = 0;
  int8_t zonaVista = -1;
  uint8_t banner = 0;
  uint8_t aviso = AV_NADA, avisoT = 0;
  uint8_t bloqueo = 0;
  int16_t cam = 0;
  int8_t jefe = -1;          // índice del jefe en el arreglo de enemigos, -1 si no está
  bool jefeMuerto = false;
  uint16_t mapaVersion = 1;  // la página vuelve a pedir el mapa cuando cambia
} G;

// ===================== Nivel =====================
char grid[H][W + 1];
uint8_t cosechado[H][(W + 7) / 8];   // bits y parches ya recogidos (1 bit por casilla)

const int MAX_SPAWNS = 16, MAX_EN = 16, MAX_PLAT = 3, MAX_BALAS = 3, MAX_EBALAS = 6, MAX_EV = 24;
Spawn spawns[MAX_SPAWNS]; uint8_t nSpawns = 0;
int16_t checkpoints[4]; uint8_t nChecks = 0;
Plataforma plats[MAX_PLAT]; uint8_t nPlats = 0;
Plataforma platsIni[MAX_PLAT];
Enemigo en[MAX_EN];
Bala balas[MAX_BALAS], ebalas[MAX_EBALAS];
uint16_t idBala = 0;

Evento eventos[MAX_EV];
uint16_t eventoSeq = 0;
void evento(uint8_t tipo, int x, int y, int v = 0) {
  eventoSeq++;
  eventos[eventoSeq % MAX_EV] = { eventoSeq, tipo, (int16_t)x, (int16_t)y, (int16_t)v };
}

void ponerCasilla(int x, int y, char c) { if (x >= 0 && x < W && y >= 0 && y < H) grid[y][x] = c; }
void suelo(int a, int b)                 { for (int x = a; x <= b; x++) for (int y = 13; y < H; y++) ponerCasilla(x, y, '#'); }
void bloque(int x, int y, int w, int h)  { for (int i = 0; i < w; i++) for (int j = 0; j < h; j++) ponerCasilla(x + i, y + j, '#'); }
void plat(int a, int b, int y)           { for (int x = a; x <= b; x++) ponerCasilla(x, y, '='); }
void bitsFila(int a, int b, int y)       { for (int x = a; x <= b; x++) ponerCasilla(x, y, 'C'); }
void bug(int x)                          { if (nSpawns < MAX_SPAWNS) spawns[nSpawns++] = { BUG, (int16_t)x, 12 }; }
void virus(int x, int y)                 { if (nSpawns < MAX_SPAWNS) spawns[nSpawns++] = { VIRUS, (int16_t)x, (int16_t)y }; }
void mover(int a, int b, int y)          { if (nPlats < MAX_PLAT) platsIni[nPlats++] = { (int16_t)a, (int16_t)y, (int16_t)a, (int16_t)b, 1, 8 }; }
void checkpoint(int x)                   { ponerCasilla(x, 12, 'K'); if (nChecks < 4) checkpoints[nChecks++] = x; }

void construirNivel() {
  for (int y = 0; y < H; y++) { memset(grid[y], '.', W); grid[y][W] = 0; }
  nSpawns = 0; nChecks = 0; nPlats = 0;

  // Zona 1 · fuente de alimentación
  suelo(0, 14); bitsFila(6, 9, 10); bug(12);
  suelo(17, 26); ponerCasilla(21, 12, 'X'); bloque(23, 10, 3, 1); bitsFila(23, 25, 9);
  suelo(29, 40); bitsFila(30, 32, 11); bloque(34, 11, 1, 2); bug(37); ponerCasilla(39, 12, 'X');
  suelo(43, 55); bitsFila(44, 46, 11); bug(48); bloque(50, 10, 2, 1); ponerCasilla(50, 9, 'P'); ponerCasilla(53, 12, 'X');
  // Zona 2 · placa madre
  checkpoint(57); suelo(56, 66); bloque(61, 11, 1, 2); bitsFila(60, 62, 9);
  suelo(70, 80); virus(73, 9); bloque(74, 10, 2, 1); bitsFila(74, 75, 9); bug(78);
  suelo(84, 97); bloque(86, 11, 1, 2); bloque(89, 10, 1, 3); bitsFila(87, 89, 8); virus(94, 10);
  suelo(101, 110); bitsFila(101, 104, 11); bug(103); bug(108);
  // Zona 3 · memoria RAM
  checkpoint(112); suelo(111, 118); plat(115, 117, 10); bitsFila(115, 117, 9);
  mover(119, 124, 13);
  suelo(127, 133); plat(129, 132, 9); bitsFila(129, 132, 8); virus(131, 11);
  mover(134, 139, 12);
  suelo(142, 150); plat(144, 147, 10); bitsFila(144, 147, 9); virus(149, 9);
  mover(151, 154, 11);
  suelo(157, 165); bug(160); ponerCasilla(163, 11, 'P');
  // Zona 4 · CPU
  checkpoint(167); suelo(166, 175); bitsFila(169, 170, 10); ponerCasilla(171, 12, 'X'); ponerCasilla(173, 12, 'X');
  suelo(179, 188); ponerCasilla(182, 12, 'X'); bloque(183, 10, 2, 1); bitsFila(183, 184, 9); virus(186, 9);
  suelo(189, W - 1); ponerCasilla(194, 9, 'P');
  for (int y = 6; y <= 12; y++) ponerCasilla(GATE_X, y, 'D');
  ponerCasilla(FLAG_X, 12, '>');
}

bool fueCosechado(int x, int y) { return cosechado[y][x >> 3] & (1 << (x & 7)); }
void marcarCosechado(int x, int y) { cosechado[y][x >> 3] |= (1 << (x & 7)); }

// ===================== Consultas al mapa =====================
char casilla(int x, int y) {
  if (y < 0) return '.';
  if (x < 0 || x >= W) return '#';    // los bordes del nivel son paredes
  if (y >= H) return '.';             // abajo del nivel: el vacío
  return grid[y][x];
}
bool solido(int x, int y) { char c = casilla(x, y); return c == '#' || c == 'D'; }
int plataformaEn(int x, int y) {
  for (int i = 0; i < nPlats; i++) if (plats[i].y == y && x >= plats[i].x && x < plats[i].x + 3) return i;
  return -1;
}
bool apoyo(int x, int y) { return solido(x, y) || casilla(x, y) == '=' || plataformaEn(x, y) >= 0; }
bool ocupaJugador(int x, int y) { return x == P.x && (y == P.y || (!P.agachado && y == P.y - 1)); }
int enemigoEn(int x, int y) {
  for (int i = 0; i < MAX_EN; i++) {
    Enemigo& e = en[i];
    if (e.activo && x >= e.x && x < e.x + e.w && y > e.y - e.h && y <= e.y) return i;
  }
  return -1;
}
bool peligroActivo(int x) { return ((G.tick + x * 7) % 60) < (uint32_t)(zonaDe(x) == 3 ? 36 : 28); }
int16_t limitar(int v, int a, int b) { return v < a ? a : v > b ? b : v; }

// ===================== Récord en la memoria flash =====================
const uint32_t RECORD_MAGIC = 0x4B504C41;   // "KPLA"
void cargarRecord() {
  EEPROM.begin(16);
  uint32_t magic, rec;
  EEPROM.get(0, magic); EEPROM.get(4, rec);
  G.record = (magic == RECORD_MAGIC && rec < 1000000) ? rec : 0;
}
void guardarRecord() {
  if (G.puntos <= G.record) return;
  G.record = G.puntos;
  EEPROM.put(0, RECORD_MAGIC); EEPROM.put(4, G.record);
  EEPROM.commit();
}

// ===================== Partida =====================
void avisar(uint8_t a) { G.aviso = a; G.avisoT = 50; }

void reaparecer(bool desdeCero) {
  construirNivel();
  for (int y = 0; y < H; y++) for (int x = 0; x < W; x++) if (fueCosechado(x, y)) grid[y][x] = '.';
  G.mapaVersion++;

  memset(en, 0, sizeof(en));
  uint8_t n = 0;
  for (int i = 0; i < nSpawns; i++) {
    Spawn& s = spawns[i];
    if (s.x <= G.check + 2) continue;   // los enemigos de antes del checkpoint no vuelven
    Enemigo& e = en[n++];
    e.activo = true; e.id = i; e.tipo = s.tipo; e.x = s.x; e.y = s.y; e.w = 1; e.h = 1; e.vida = 1;
    if (s.tipo == BUG) { e.dir = -1; e.cd = 6; }
    else { e.base = s.y; e.fase = 0; e.cd = 3; e.hcd = 6; }
  }
  for (int i = 0; i < nPlats; i++) plats[i] = platsIni[i];
  memset(balas, 0, sizeof(balas)); memset(ebalas, 0, sizeof(ebalas));
  G.jefe = -1; G.jefeMuerto = false;

  P.x = G.check; P.y = 12; P.vidas = 3; P.mira = 1; P.agachado = false;
  P.subir = P.flotar = P.caidas = P.vcd = P.hcd = P.disparoCd = P.flash = 0;
  P.inv = desdeCero ? 0 : 30;
  G.cam = limitar(P.x - 7, 0, W - VIEW);
  G.st = JUGANDO;
}

void nuevaPartida() {
  memset(cosechado, 0, sizeof(cosechado));
  G.puntos = 0; G.bits = 0; G.check = 2; G.ckPuntos = 0; G.ckBits = 0; G.zonaVista = -1;
  reaparecer(true);
  sfx(S_INICIO, true);
}

void crearJefe() {
  for (int i = 0; i < MAX_EN; i++) if (!en[i].activo) {
    Enemigo& e = en[i];
    memset(&e, 0, sizeof(e));
    e.activo = true; e.id = 99; e.tipo = JEFE; e.x = 206; e.y = 12; e.w = 2; e.h = 2;
    e.vida = 20; e.dir = -1; e.cd = 8; e.tiro = 30; e.alto = true;
    G.jefe = i;
    break;
  }
  avisar(AV_JEFE); sfx(S_JEFE, true); evento(EV_JEFE, 0, 0);
}

void perder() {
  G.st = PERDIDO; G.bloqueo = 25;
  guardarRecord();
  sfx(S_PERDIDO, true);
  evento(EV_PERDIDO, P.x * 16 + 8, P.y * 16);
}

void herir(bool zap) {
  if (P.inv > 0 || G.st != JUGANDO) return;
  P.vidas--; P.inv = 30;
  evento(zap ? EV_ZAP : EV_HERIDO, P.x * 16 + 8, P.y * 16);
  sfx(S_DANO, true);
  int kb = -P.mira;   // retroceso de una casilla
  if (!solido(P.x + kb, P.y) && !solido(P.x + kb, P.y - 1)) P.x += kb;
  if (P.vidas <= 0) perder();
}

void caerAlVacio() {
  P.vidas--;
  sfx(S_DANO, true);
  evento(EV_VACIO, 0, 0);
  if (P.vidas <= 0) { perder(); return; }
  avisar(AV_VACIO);
  int8_t vidas = P.vidas; uint32_t pts = G.puntos; uint16_t b = G.bits;
  reaparecer(false);
  P.vidas = vidas; G.puntos = pts; G.bits = b;
}

void matar(int i, bool pisado) {
  Enemigo& e = en[i];
  int cx = e.x * 16 + 8 * e.w, cy = (e.y - e.h + 1) * 16 + 8 * e.h;
  int pts = e.tipo == JEFE ? 500 : e.tipo == VIRUS ? 30 : 20;
  G.puntos += pts;
  evento(EV_MUERTE, cx, cy, pts * 4 + e.tipo);   // la página separa puntos y tipo
  sfx(pisado ? S_PISAR : S_MUERTE, true);
  if (e.tipo == JEFE) {
    for (int y = 0; y < H; y++) if (grid[y][GATE_X] == 'D') grid[y][GATE_X] = '.';
    G.mapaVersion++;
    memset(ebalas, 0, sizeof(ebalas));
    G.jefe = -1; G.jefeMuerto = true;
    avisar(AV_JEFE_MUERTO);
    evento(EV_JEFE_MUERTO, cx, cy);
  }
  e.activo = false;
}

// El jefe tiene dos fases. e.fase: 0 = fase 1, 1 = cambiando (con escudo), 2 = fase 2
const int JEFE_VIDA_FASE2 = 10;   // con esta vida pasa a la fase 2
const int JEFE_CAMBIO = 30;       // ticks que dura el cambio de fase (1,5 s)
const Nota S_FASE2[] = { {1200, 80}, {600, 80}, {1200, 80}, {600, 80}, {90, 320}, {0, 0} };   // alarma

void danar(int i) {
  Enemigo& e = en[i];
  int cx = e.x * 16 + 8 * e.w, cy = (e.y - e.h + 1) * 16 + 8 * e.h;
  if (e.tipo == JEFE && e.fase == 1) {   // escudo: el disparo rebota
    evento(EV_PARED, cx, cy);
    return;
  }
  e.vida--; e.flash = 3;
  if (e.tipo == JEFE && e.fase == 0 && e.vida <= JEFE_VIDA_FASE2) {
    e.fase = 1; e.hcd = JEFE_CAMBIO;
    memset(ebalas, 0, sizeof(ebalas));   // borra sus paquetes
    avisar(AV_FASE2); sfx(S_FASE2, true);
    evento(EV_FASE2, cx, cy);
    return;
  }
  if (e.vida > 0) {
    evento(EV_GOLPE, e.x * 16 + 8 * e.w, (e.y - e.h + 1) * 16 + 8 * e.h);
    sfx(S_GOLPE, false);
    return;
  }
  matar(i, false);
}

// ===================== Partes de un tick de juego =====================
void recoger() {
  int filas[2] = { P.y, P.agachado ? P.y : P.y - 1 };
  for (int k = 0; k < 2; k++) {
    int y = filas[k];
    char c = casilla(P.x, y);
    if (c == 'C') {
      grid[y][P.x] = '.'; marcarCosechado(P.x, y); G.mapaVersion++;
      G.bits++; G.puntos += 10;
      evento(EV_BIT, P.x * 16 + 8, y * 16);
      sfx(S_BIT, false);
    } else if (c == 'P') {
      grid[y][P.x] = '.'; marcarCosechado(P.x, y); G.mapaVersion++;
      if (P.vidas < 3) P.vidas++;
      evento(EV_PARCHE, P.x * 16 + 8, y * 16);
      sfx(S_PARCHE, true);
    }
  }
}

void disparar() {
  if (HW.boton != 0 || P.disparoCd > 0) return;
  for (int i = 0; i < MAX_BALAS; i++) if (!balas[i].activa) {
    balas[i] = { true, ++idBala, P.x, (int16_t)(P.agachado ? P.y : P.y - 1), P.mira, 12, 0, false };
    P.disparoCd = 5; P.flash = 2;
    sfx(S_DISPARO, false);
    return;
  }
}

void moverBalas() {
  for (int i = 0; i < MAX_BALAS; i++) {
    Bala& b = balas[i];
    if (!b.activa) continue;
    b.x += b.dir; b.vida--;
    if (solido(b.x, b.y) || b.vida <= 0) {
      b.activa = false;
      evento(EV_PARED, b.x * 16 + 8 - b.dir * 6, b.y * 16 + 8);
      continue;
    }
    int e = enemigoEn(b.x, b.y);
    if (e >= 0) { b.activa = false; danar(e); }
  }
}

void moverPlataformas() {
  for (int i = 0; i < nPlats; i++) {
    Plataforma& m = plats[i];
    if (--m.cd > 0) continue;
    m.cd = 8;
    if (m.x + m.dir < m.a || m.x + m.dir > m.b) m.dir = -m.dir;
    bool encima = P.y + 1 == m.y && P.x >= m.x && P.x < m.x + 3;
    m.x += m.dir;
    if (encima && !solido(P.x + m.dir, P.y) && !solido(P.x + m.dir, P.y - 1)) P.x += m.dir;   // lleva al jugador
  }
}

void moverEnemigos() {
  static const int8_t ONDA[4] = { 0, -1, 0, 1 };
  for (int i = 0; i < MAX_EN; i++) {
    Enemigo& e = en[i];
    if (!e.activo) continue;
    if (e.flash > 0) e.flash--;
    if (e.tipo != JEFE && (e.x < G.cam - 2 || e.x > G.cam + VIEW + 2)) continue;   // solo cerca de la cámara
    if (e.tipo == BUG) {
      if (--e.cd > 0) continue;
      e.cd = 6;
      int nx = e.x + e.dir;
      if (solido(nx, e.y) || !apoyo(nx, e.y + 1)) e.dir = -e.dir; else e.x = nx;
    } else if (e.tipo == VIRUS) {
      if (--e.cd <= 0) {
        e.cd = 4; e.fase = (e.fase + 1) % 4;
        int ny = e.base + ONDA[e.fase];
        if (!solido(e.x, ny)) e.y = ny;
      }
      if (--e.hcd <= 0) {
        e.hcd = 7;
        int dx = (P.x > e.x) - (P.x < e.x);
        if (dx && abs(P.x - e.x) <= 9 && !solido(e.x + dx, e.y)) e.x += dx;
      }
    } else {   // JEFE: va y viene, y tira paquetes altos y bajos alternados
      if (e.fase == 1) {                 // cambiando de fase: quieto y con escudo
        if (--e.hcd <= 0) { e.fase = 2; e.cd = 4; e.tiro = 12; }
        continue;
      }
      bool f2 = e.fase == 2;             // fase 2: más rápido y tira de a dos paquetes
      if (--e.cd <= 0) {
        e.cd = f2 ? 6 : 9;
        int nx = e.x + e.dir;
        if (nx < 199 || nx > 208) e.dir = -e.dir; else e.x = nx;
      }
      if (--e.tiro <= 0) {
        e.tiro = f2 ? 24 : 34;
        int dir = (P.x > e.x) - (P.x < e.x); if (!dir) dir = -1;
        int y = e.alto ? 11 : 12;
        int cuantas = f2 ? 2 : 1;
        for (int n = 0; n < cuantas; n++)
          for (int k = 0; k < MAX_EBALAS; k++) if (!ebalas[k].activa) {
            // el segundo paquete sale 3 casillas detrás del primero
            ebalas[k] = { true, ++idBala, (int16_t)(dir < 0 ? e.x - 1 : e.x + 2), (int16_t)y, (int8_t)dir, 0, (int8_t)(2 + n * 6), y == 11 };
            break;
          }
        e.alto = !e.alto;
        sfx(S_TIRO, false);
      }
    }
  }
}

void moverBalasEnemigas() {
  for (int i = 0; i < MAX_EBALAS; i++) {
    Bala& b = ebalas[i];
    if (!b.activa || --b.cd > 0) continue;
    b.cd = 2; b.x += b.dir;
    if (solido(b.x, b.y) || b.x < G.cam - 4 || b.x > G.cam + VIEW + 4) b.activa = false;
  }
}

void peligros() {
  int filas[2] = { P.y, P.agachado ? P.y : P.y - 1 };
  for (int k = 0; k < 2; k++)
    if (casilla(P.x, filas[k]) == 'X' && peligroActivo(P.x)) herir(true);
}

void contactos() {
  for (int i = 0; i < MAX_EBALAS; i++) {
    Bala& b = ebalas[i];
    if (b.activa && ocupaJugador(b.x, b.y)) { b.activa = false; herir(false); }
  }
  for (int i = 0; i < MAX_EN; i++) {
    Enemigo& e = en[i];
    if (!e.activo) continue;
    for (int a = 0; a < e.w; a++) for (int b = 0; b < e.h; b++)
      if (ocupaJugador(e.x + a, e.y - b)) { herir(false); return; }
  }
}

void jugar() {
  if (P.inv > 0) P.inv--;
  if (P.disparoCd > 0) P.disparoCd--;
  if (P.flash > 0) P.flash--;
  bool piso = apoyo(P.x, P.y + 1);

  // Agacharse: solo en el piso, y para pararse hace falta lugar arriba
  bool quiereAgachar = HW.abajo && piso;
  if (quiereAgachar != P.agachado) {
    if (quiereAgachar) P.agachado = true;
    else if (!solido(P.x, P.y - 1)) P.agachado = false;
  }

  // Caminar: una casilla cada 2 ticks (agachado no camina)
  if (P.hcd > 0) P.hcd--;
  int dx = HW.izq ? -1 : HW.der ? 1 : 0;
  if (dx) P.mira = dx;
  if (dx && !P.agachado && P.hcd == 0) {
    if (!solido(P.x + dx, P.y) && !solido(P.x + dx, P.y - 1)) P.x += dx;
    P.hcd = 2;
  }

  // Saltar: con el flanco de "arriba", estando en el piso
  if (HW.saltoFlanco && piso && !P.agachado) {
    P.subir = 3; P.vcd = 0; P.caidas = 0;
    evento(EV_POLVO, P.x * 16 + 8, (P.y + 1) * 16);
    sfx(S_SALTO, false);
  }

  // Movimiento vertical: sube 3, flota 2 ticks y cae cada vez más rápido
  if (P.vcd > 0) P.vcd--;
  if (P.vcd == 0) {
    if (P.subir > 0) {
      if (!solido(P.x, P.y - 2)) { P.y--; P.subir--; P.vcd = 2; if (P.subir == 0) P.flotar = 2; }
      else { P.subir = 0; P.flotar = 0; }
    } else if (P.flotar > 0) { P.flotar--; P.vcd = 1; }
    else if (!apoyo(P.x, P.y + 1)) {
      P.y++; P.caidas++; P.vcd = (HW.abajo || P.caidas > 2) ? 1 : 2;
      int e = enemigoEn(P.x, P.y);   // ¿cayó encima de un enemigo?
      if (e >= 0 && en[e].tipo != JEFE) { matar(e, true); P.subir = 2; P.caidas = 0; P.vcd = 1; }
      if (P.y >= H) { caerAlVacio(); return; }
    } else {
      if (P.caidas > 1) evento(EV_POLVO, P.x * 16 + 8, (P.y + 1) * 16);
      P.caidas = 0;
    }
  }

  recoger();
  disparar();
  moverPlataformas();
  moverBalas();
  moverEnemigos();
  moverBalasEnemigas();
  if (G.st != JUGANDO) return;
  peligros();
  contactos();
  if (G.st != JUGANDO) return;

  // Checkpoints, zonas, jefe y meta
  for (int i = 0; i < nChecks; i++) if (P.x >= checkpoints[i] && G.check < checkpoints[i]) {
    G.check = checkpoints[i]; G.ckPuntos = G.puntos; G.ckBits = G.bits;
    avisar(AV_CHECK); sfx(S_CHECK, true);
  }
  int z = zonaDe(P.x);
  if (z != G.zonaVista) { G.zonaVista = z; G.banner = 60; }
  if (G.banner > 0) G.banner--;
  if (G.jefe < 0 && !G.jefeMuerto && P.x >= BOSS_X0 && grid[6][GATE_X] == 'D') crearJefe();
  if (P.x >= FLAG_X) {
    G.puntos += 50 * P.vidas;
    guardarRecord();
    G.st = GANADO; G.bloqueo = 30;
    sfx(S_GANADO, true);
    evento(EV_GANADO, P.x * 16 + 8, P.y * 16);
  }

  // Cámara: el jugador queda en el tercio izquierdo (en la pelea con el jefe, más a la izquierda)
  if (G.jefe >= 0) G.cam = limitar(P.x - 4, 0, W - VIEW);
  else G.cam = limitar(max((int)G.cam, P.x - 11), 0, W - VIEW);
  if (P.x - G.cam < 3) G.cam = limitar(P.x - 3, 0, W - VIEW);
}

// ===================== LEDs: integridad y avance =====================
void actualizarLeds() {
  uint8_t b = 0;
  uint32_t t = G.tick;
  if (G.st == MENU) { int k = (t / 3) % 14; b = 1 << (k < 8 ? k : 14 - k); }   // barrido de un lado a otro
  else if (G.st == PERDIDO) b = ((t / 5) % 2) ? 0xAA : 0x55;
  else if (G.st == GANADO)  b = ((t / 4) % 2) ? 0xFF : 0x00;
  else {
    for (int i = 0; i < 3; i++) if (P.vidas > i) b |= 1 << i;                 // Q0..Q2: integridad
    if (P.inv > 0 && ((t >> 1) & 1)) b &= ~7;                                 // titilan después de un golpe
    int av = min(5, P.x * 5 / W + 1);
    for (int i = 0; i < av; i++) b |= 1 << (3 + i);                           // Q3..Q7: avance en el nivel
    if (G.st == PAUSA && ((t >> 3) & 1)) b = 0;
  }
  HW.leds = b;
}

// ===================== Un tick (cada 50 ms) =====================
void tickJuego() {
  G.tick++;
  leerEntradas();
  if (G.avisoT > 0 && --G.avisoT == 0) G.aviso = AV_NADA;
  switch (G.st) {
    case MENU:
      if (HW.swFlanco) nuevaPartida();
      else if (!sonidoActual && notaRestante <= 0) sfx(S_TEMA, false);   // música del menú
      break;
    case JUGANDO:
#if DISPARO_CON_SW
      // Modo de un solo botón: durante la partida, apretar la palanca (SW) dispara. No hay pausa.
      if (HW.swEstable == 0) HW.boton = 0;
#else
      if (HW.swFlanco) { G.st = PAUSA; sfx(S_PAUSA, true); break; }
#endif
      jugar();
      break;
    case PAUSA:
      if (HW.swFlanco) G.st = JUGANDO;
      break;
    case PERDIDO:
      if (G.bloqueo > 0) G.bloqueo--;
      else if (HW.swFlanco) { G.puntos = G.ckPuntos; G.bits = G.ckBits; reaparecer(false); sfx(S_INICIO, true); }
      break;
    case GANADO:
      if (G.bloqueo > 0) G.bloqueo--;
      else if (HW.swFlanco) G.st = MENU;
      break;
  }
  actualizarLeds();
  actualizar595();
  actualizarBuzzer();
}

void iniciarJuego() {
  cargarRecord();
  construirNivel();
  memset(cosechado, 0, sizeof(cosechado));
  for (int i = 0; i < nPlats; i++) plats[i] = platsIni[i];
  G.st = MENU;
}
