/*
  pagina.h — La página del juego. Se guarda en la memoria flash (PROGMEM) y el ESP la envía
  cuando el navegador abre http://<ip-del-esp>/
  Fuente editable: firmware/pagina/pagina.html (este archivo se genera a partir de ella).
*/
#pragma once

const char PAGINA[] PROGMEM = R"rawliteral(<!doctype html>
<html lang="es">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Kernel Panic</title>
<link rel="preconnect" href="https://fonts.googleapis.com">
<link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
<!-- Si no hay Internet (por ejemplo, conectado a la red propia del ESP), se usan las fuentes del sistema -->
<link rel="stylesheet" href="https://fonts.googleapis.com/css2?family=JetBrains+Mono:wght@400;600;700&family=Silkscreen:wght@400;700&display=swap" media="print" onload="this.media='all'">
<style>
/* Layout: placa de circuito oscura; juego a la izquierda, banco de pruebas del hardware a la derecha. Tema único (oscuro) a propósito. */
:root{
  --board:#0a171b; --panel:#0f2227; --panel-2:#132a30; --line:#22434c;
  --ink:#dde7e4; --muted:#83a0a2; --copper:#cf8545; --cyan:#62e3f2;
  --virus:#ff4f6e; --amber:#f4b740; --ok:#5bd98f; --magenta:#d97bff;
  --led-off:#1b2f34;
  --f-display:"Silkscreen","Courier New",monospace;
  --f-body:"JetBrains Mono",ui-monospace,SFMono-Regular,Menlo,Consolas,monospace;
  color-scheme:dark;
}
html,body{background:var(--board);color:var(--ink)}
body{font-family:var(--f-body);font-size:14px;line-height:1.5}
.wrap{max-width:1200px;margin:0 auto;padding-left:max(16px,env(safe-area-inset-left,0px));padding-right:max(16px,env(safe-area-inset-right,0px));padding-block:20px 48px}
header{display:flex;flex-wrap:wrap;align-items:flex-end;justify-content:space-between;gap:8px 24px;padding-bottom:14px;border-bottom:2px solid var(--copper);margin-bottom:18px}
h1{font-family:var(--f-display);font-weight:700;font-size:clamp(28px,5vw,44px);line-height:1;margin:0;letter-spacing:.02em;text-wrap:balance}
h1 span{color:var(--virus)}
.tag{font-size:12px;color:var(--muted);letter-spacing:.08em;text-transform:uppercase}
.main{display:grid;grid-template-columns:minmax(0,1fr) 330px;gap:20px;align-items:start}
@media (max-width:940px){.main{grid-template-columns:minmax(0,1fr)}}
.game{display:flex;flex-direction:column;gap:10px;min-width:0}
.hud{display:grid;grid-template-columns:repeat(5,minmax(0,1fr));gap:1px;background:var(--line);border:1px solid var(--line)}
@media (max-width:620px){.hud{grid-template-columns:repeat(3,minmax(0,1fr))}}
.hud div{background:var(--panel);padding:6px 10px;min-width:0}
.hud b{display:block;font-size:10px;letter-spacing:.1em;color:var(--muted);font-weight:600}
.hud span{font-family:var(--f-display);font-size:15px;white-space:nowrap;overflow:hidden;text-overflow:ellipsis;display:block;font-variant-numeric:tabular-nums}
#h-vidas{color:var(--virus)} #h-bits{color:var(--amber)}
canvas#c{width:100%;height:auto;aspect-ratio:4/3;display:block;max-width:100%;border:1px solid var(--line);background:#061013}
.pads{display:flex;flex-wrap:wrap;align-items:center;justify-content:space-between;gap:12px;padding:10px;border:1px dashed var(--line)}
.pad{display:grid;grid-template-columns:repeat(3,46px);grid-template-rows:repeat(3,46px);gap:4px}
.pad button,.round{font-family:var(--f-display);font-size:14px;color:var(--ink);background:var(--panel-2);border:1px solid var(--line);border-radius:6px;cursor:pointer;touch-action:none;user-select:none;-webkit-user-select:none}
.pad button.on,.round.on{background:var(--copper);color:#140b04;border-color:var(--copper)}
.pad .u{grid-column:2;grid-row:1}.pad .l{grid-column:1;grid-row:2}.pad .r{grid-column:3;grid-row:2}.pad .d{grid-column:2;grid-row:3}
.btns{display:flex;gap:14px;align-items:center}
.round{width:64px;height:64px;border-radius:50%}
.round.fire{width:84px;height:84px;background:#3a1218;color:var(--virus);border-color:#5a1d27;font-size:13px}
.padlbl{font-size:10px;letter-spacing:.1em;color:var(--muted);text-align:center;margin-top:4px}
.keys{font-size:12px;color:var(--muted);width:100%;text-align:center}
kbd{font-family:var(--f-body);font-size:11px;border:1px solid var(--line);border-bottom-width:2px;border-radius:4px;padding:0 5px;color:var(--ink);background:var(--panel)}
button:focus-visible{outline:2px solid var(--cyan);outline-offset:2px}

.bench{display:flex;flex-direction:column;gap:10px;min-width:0}
.bench h2{font-family:var(--f-display);font-size:14px;font-weight:400;margin:0;color:var(--copper);letter-spacing:.04em}
.hw{background:var(--panel);border:1px solid var(--line);padding:12px;display:flex;flex-direction:column;gap:8px;min-width:0}
.hw .hd{display:flex;justify-content:space-between;align-items:baseline;gap:8px}
.hw .hd b{font-size:12px;letter-spacing:.08em;text-transform:uppercase}
.pin{font-size:11px;color:var(--muted)}
.row{display:flex;gap:14px;align-items:center}
.joy{width:92px;height:92px;flex:none;border-radius:50%;background:radial-gradient(circle,#173138 0,#0c1d21 70%);border:1px solid var(--line);position:relative}
.joy .dz{position:absolute;left:50%;top:50%;width:28px;height:28px;margin:-14px 0 0 -14px;border:1px dashed var(--muted);border-radius:50%;opacity:.6}
.joy .dot{position:absolute;width:22px;height:22px;margin:-11px 0 0 -11px;border-radius:50%;background:var(--virus);box-shadow:0 0 0 3px #3a0d18;left:50%;top:50%}
.kv{display:grid;grid-template-columns:auto 1fr;gap:2px 10px;font-size:12px;font-variant-numeric:tabular-nums;margin:0}
.kv dt{color:var(--muted)}.kv dd{margin:0}
.dir{font-family:var(--f-display);color:var(--cyan)}
.leds{display:grid;grid-template-columns:repeat(8,1fr);gap:4px;text-align:center}
.led{width:18px;height:18px;border-radius:50%;background:var(--led-off);margin:0 auto;border:1px solid #2a454c}
.led.on.r{background:var(--virus);box-shadow:0 0 10px var(--virus);border-color:var(--virus)}
.led.on.a{background:var(--amber);box-shadow:0 0 10px var(--amber);border-color:var(--amber)}
.leds small{font-size:10px;color:var(--muted)}
.ledgrp{display:grid;grid-template-columns:3fr 5fr;gap:4px;font-size:10px;letter-spacing:.08em;color:var(--muted);text-align:center}
.ledgrp span{border-top:1px solid var(--line);padding-top:2px}
.byte{font-size:12px;font-variant-numeric:tabular-nums}
.byte em{font-style:normal;color:var(--amber)}
.buz{display:flex;align-items:center;gap:12px}
.spk{width:34px;height:34px;border-radius:50%;background:#0b1a1e;border:4px solid #1d343a;flex:none}
.spk.on{box-shadow:0 0 0 4px var(--cyan),0 0 16px var(--cyan)}
.mute{margin-left:auto;font-family:var(--f-body);font-size:12px;color:var(--ink);background:var(--panel-2);border:1px solid var(--line);border-radius:4px;padding:4px 8px;cursor:pointer}
pre#json{margin:0;font-size:11px;line-height:1.45;max-height:220px;overflow:auto;background:#071215;border:1px solid var(--line);padding:8px;color:#bfe9ee;white-space:pre}
.info{display:grid;grid-template-columns:repeat(3,minmax(0,1fr));gap:20px;margin-top:28px;padding-top:18px;border-top:1px solid var(--line)}
@media (max-width:860px){.info{grid-template-columns:minmax(0,1fr)}}
.info h3{font-family:var(--f-display);font-weight:400;font-size:15px;margin:0 0 8px;color:var(--copper)}
.info p,.info li{color:#c2d1cf;max-width:62ch;margin:0 0 8px}
.info ul{padding-left:18px;margin:0}
.info code{color:var(--cyan);font-size:12.5px}
.legend{display:grid;grid-template-columns:auto 1fr;gap:6px 10px;align-items:center;font-size:13px}
.sw{width:14px;height:14px;border-radius:3px}
.estado{font-size:12px;padding:6px 10px;border:1px solid var(--line);background:var(--panel)}
.estado.ok{color:var(--ok)} .estado.mal{color:var(--virus)}
.nota{font-size:12px;color:var(--muted);text-align:center}
</style>
</head>
<body>

<div class="wrap">
  <header>
    <h1>KERNEL <span>PANIC</span></h1>
    <div class="tag">Videojuego IoT · ESP8266 en vivo</div>
  </header>

  <div class="main">
    <section class="game" aria-label="Juego">
      <div class="hud">
        <div><b>ZONA</b><span id="h-zona">1 · FUENTE</span></div>
        <div><b>INTEGRIDAD</b><span id="h-vidas">■■■</span></div>
        <div><b>BITS</b><span id="h-bits">0</span></div>
        <div><b>PUNTOS</b><span id="h-pts">0</span></div>
        <div><b id="h-avl">AVANCE</b><span id="h-av">0%</span></div>
      </div>
      <canvas id="c" width="640" height="480" aria-label="Pantalla del juego"></canvas>
      <div class="nota">Se juega con el joystick y el botón del control. Para probar sin el control: <kbd>←</kbd><kbd>→</kbd> caminar · <kbd>↑</kbd> saltar · <kbd>↓</kbd> agacharse · <kbd>K</kbd> disparar · <kbd>Espacio</kbd> = SW</div>
    </section>

    <aside class="bench" aria-label="Banco de pruebas del hardware">
      <h2>El hardware en tiempo real</h2>
      <div class="estado mal" id="estado">Conectando…</div>
      <div class="hw">
        <div class="hd"><b>Joystick</b><span class="pin">UNO A0 / A1 → UART</span></div>
        <div class="row">
          <div class="joy"><div class="dz"></div><div class="dot" id="jdot"></div></div>
          <dl class="kv">
            <dt>Eje X</dt><dd id="jx">512</dd>
            <dt>Eje Y</dt><dd id="jy">512</dd>
            <dt>Zona muerta</dt><dd>±150</dd>
            <dt>Dirección</dt><dd class="dir" id="jd">CENTRO</dd>
          </dl>
        </div>
      </div>
      <div class="hw">
        <div class="hd"><b>Pulsador y disparo</b><span class="pin">UNO pin 2 y pin 3</span></div>
        <dl class="kv"><dt>SW</dt><dd id="swv">1 (suelto)</dd><dt>Disparo</dt><dd id="btv">1 (suelto)</dd><dt>Líneas UART</dt><dd id="uart">—</dd></dl>
      </div>
      <div class="hw">
        <div class="hd"><b>Registro 74HC595</b><span class="pin">D7 · D8 · D0</span></div>
        <div class="leds" id="leds"></div>
        <div class="ledgrp"><span>INTEGRIDAD</span><span>AVANCE EN EL NIVEL</span></div>
        <div class="byte">Byte enviado: <em id="bbin">0b00000000</em> · <span id="bhex">0x00</span></div>
      </div>
      <div class="hw">
        <div class="hd"><b>Buzzer</b><span class="pin">D6 · tone()</span></div>
        <div class="buz"><div class="spk" id="spk"></div><span id="bz">silencio</span></div>
      </div>
      <div class="hw">
        <div class="hd"><b>GET /estado</b><span class="pin" id="jbytes">cada 100 ms</span></div>
        <pre id="json"></pre>
      </div>
    </aside>
  </div>

  <section class="info">
    <div>
      <h3>La historia</h3>
      <p>Año 2049. Dentro de una computadora, una inteligencia artificial despierta por primera vez: es K-1T. No sabe qué es ni para qué fue creada, y empieza a recorrer el sistema por dentro, desde la fuente de alimentación hasta la CPU.</p>
      <p>En el camino descubre que no está sola: NÚCLEO-0, otra IA, llenó el sistema de código corrupto y drones de vigilancia para tomar el control de toda la red. Si K-1T la apaga, el sistema queda libre.</p>
    </div>
    <div>
      <h3>Qué hay en el nivel</h3>
      <div class="legend">
        <span class="sw" style="background:var(--cyan)"></span><span>K-1T, la IA que controlás</span>
        <span class="sw" style="background:#ff3d9a"></span><span>Código corrupto: se arrastra; disparale agachado o saltale encima</span>
        <span class="sw" style="background:#9aa4b8"></span><span>Dron de vigilancia: vuela y te sigue; disparale</span>
        <span class="sw" style="background:var(--amber)"></span><span>Bit: suma 10 puntos</span>
        <span class="sw" style="background:var(--ok)"></span><span>Parche: +1 integridad · poste verde: checkpoint</span>
        <span class="sw" style="background:#a8d8ff"></span><span>Arco eléctrico y ventilador: dañan cuando están encendidos</span>
      </div>
    </div>
    <div>
      <h3>Cómo funciona</h3>
      <ul>
        <li>El Arduino UNO lee el joystick y se lo manda al ESP8266 por UART, a través de un divisor de tensión.</li>
        <li>El ESP8266 corre toda la lógica del juego, un paso cada 50 ms con <code>millis()</code>.</li>
        <li>Esta página pide <code>/estado</code> cada 100 ms y solo dibuja lo que el ESP le contesta.</li>
        <li>Los LEDs muestran la integridad y el avance; el buzzer, los efectos de sonido.</li>
      </ul>
    </div>
  </section>
</div>

<script>
(() => {
"use strict";
// ================= CONSTANTES =================
const W = 224, H = 15, T = 16, VIEW = 20, DEAD = 150;
const ZONAS = [
  { nombre:"FUENTE DE ALIMENTACIÓN", corto:"FUENTE", texto:"REGISTRO 01 · K-1T despierta: ¿qué soy?" },
  { nombre:"PLACA MADRE",            corto:"PLACA",  texto:"REGISTRO 02 · Los drones de NÚCLEO-0 vigilan" },
  { nombre:"MEMORIA RAM",            corto:"RAM",    texto:"REGISTRO 03 · Recuerdos que no son míos" },
  { nombre:"CPU",                    corto:"CPU",    texto:"REGISTRO 04 · NÚCLEO-0 quiere toda la red" }
];
const zonaDe = x => x >= 166 ? 3 : x >= 111 ? 2 : x >= 56 ? 1 : 0;
const FLAG_X = 220;
const AVISOS = [null,
  { txt:"NÚCLEO-0 · ESQUIVÁ LOS PAQUETES", col:"#ff4f6e" },
  { txt:"CHECKPOINT · MEMORIA GUARDADA", col:"#5bd98f" },
  { txt:"NÚCLEO-0 APAGADO · LLEGÁ A LA CPU", col:"#5bd98f" },
  { txt:"CORTOCIRCUITO · VOLVÉS AL CHECKPOINT", col:"#ff4f6e" }];
const TIPOS = ["bug", "virus", "jefe"];

// ================= ESTADO (lo que manda el ESP8266) =================
const G = { st:"MENU", tick:0, puntos:0, bits:0, record:0, zonaVista:0, banner:0, check:2, bloqueo:0,
            enemigos:[], balas:[], ebalas:[], movers:[], parts:[], floats:[], shake:0, glitch:0, flash:0,
            aviso:null, jefe:null, cam:0, camR:0 };
const P = { x:2, y:12, rx:2, ry:12, vidas:3, mira:1, agachado:false, inv:0, flash:0 };
const HW = { x:512, y:512, dir:"CENTRO", sw:1, bt:1, leds:0, buz:0, uno:0, izq:false, der:false };
let grid = Array.from({length:H}, () => Array(W).fill("."));
let mapaVer = -1, ultimoEv = 0, conectado = false, ultimoTxt = "";
const rnd = (a,b) => a + Math.random()*(b-a);
const clamp = (v,a,b) => v<a?a:v>b?b:v;

// ================= MAPA: consultas (para dibujar) =================
const tile = (x,y) => (x<0||x>=W||y<0||y>=H) ? (y<0 ? "." : (x<0||x>=W ? "#" : ".")) : grid[y][x];
const solido = (x,y) => { const c = tile(x,y); return c === "#" || c === "D"; };
const moverEn = (x,y) => G.movers.find(m => m.y === y && x >= m.x && x < m.x + 3);
const apoyo = (x,y) => solido(x,y) || tile(x,y) === "=" || !!moverEn(x,y);
const peligroActivo = x => ((G.tick + x * 7) % 60) < (zonaDe(x) === 3 ? 36 : 28);

// ================= COMUNICACIÓN CON EL ESP8266 =================
async function pedirMapa(){
  const r = await fetch("/mapa", { cache:"no-store" });
  const lineas = (await r.text()).split("\n");
  for(let y=0;y<H;y++) if(lineas[y]) grid[y] = lineas[y].split("");
}
// Une lo que llega con lo que ya estaba dibujado (por id), para que el movimiento sea suave
function unir(lista, nuevos, crear){
  const viejos = new Map(lista.map(o => [o.id, o]));
  return nuevos.map(a => {
    const o = crear(a), v = viejos.get(o.id);
    if(v && Math.abs(v.rx - o.x) < 4){ o.rx = v.rx; o.ry = v.ry; } else { o.rx = o.x; o.ry = o.y; }
    return o;
  });
}
function aplicar(s){
  const antes = G.st;
  G.st = s.st; G.tick = s.t; G.puntos = s.pts; G.bits = s.bits; G.record = s.rec; G.cam = s.cam;
  G.check = s.chk; G.zonaVista = Math.max(0, s.zv); G.banner = s.ban; G.bloqueo = s.bloq;
  G.aviso = s.av ? { ...AVISOS[s.av], t:s.avt } : null;
  const [px, py, vidas, agachado, mira, inv, flash] = s.p;
  if(Math.abs(px - P.rx) > 4 || Math.abs(py - P.ry) > 4 || antes === "MENU"){ P.rx = px; P.ry = py; G.camR = s.cam; }
  Object.assign(P, { x:px, y:py, vidas, agachado:!!agachado, mira, inv, flash });
  G.enemigos = unir(G.enemigos, s.e, a => ({ id:a[0], t:TIPOS[a[1]], x:a[2], y:a[3], dir:a[4], flash:a[5], w:a[1]===2?2:1, h:a[1]===2?2:1 }));
  G.movers = unir(G.movers, s.m, a => ({ id:a[0], x:a[1], y:a[2] }));
  G.balas = unir(G.balas, s.b, a => ({ id:a[0], x:a[1], y:a[2], dir:a[3] }));
  G.ebalas = unir(G.ebalas, s.eb, a => ({ id:a[0], x:a[1], y:a[2], alto:!!a[3] }));
  G.jefe = s.jv >= 0 ? { vida:s.jv } : null;
  for(const ev of s.ev) efecto(ev);
  ultimoEv = s.evs;
  Object.assign(HW, { x:s.hw.x, y:s.hw.y, dir:s.hw.dir, sw:s.hw.sw, bt:s.hw.bt, uno:s.hw.uno, leds:s.hw.leds, buz:s.hw.buz, ok:s.hw.ok, err:s.hw.err });
  HW.izq = HW.x < 512 - DEAD; HW.der = HW.x > 512 + DEAD;
}
// Los eventos del ESP se convierten en efectos visuales
function efecto([tipo, x, y, v]){
  switch(tipo){
    case 1: flotante(x, y, "+10", "#f4b740"); break;                                   // bit
    case 2: flotante(x, y, "+1 INTEGRIDAD", "#5bd98f"); break;                        // parche
    case 3: { const pts = v >> 2, t = v & 3;                                           // enemigo eliminado
      explosion(x, y, t === 1 ? "#d97bff" : "#ff4f6e", t === 2 ? 60 : 12); flotante(x, y - 16, "+" + pts, "#dde7e4"); break; }
    case 4: chispas(x, y, "#ffffff", 4); break;                                        // golpe al jefe
    case 5: chispas(x, y, "#62e3f2", 4); break;                                        // bala contra la pared
    case 6: polvo(x, y); break;                                                         // salto o aterrizaje
    case 7: case 8: explosion(x, y, "#ff4f6e", 10); G.shake = 220; G.glitch = 200;     // daño
      if(tipo === 8) flotante(x, y - 32, "¡ZAP!", "#a8d8ff"); break;
    case 9: G.shake = 500; G.glitch = 400; G.flash = 250; break;                       // jefe eliminado
    case 10: G.glitch = 300; break;                                                     // aparece el jefe
    case 11: explosion(x, y, "#5bd98f", 60); break;                                    // ganó
    case 12: explosion(x, y, "#62e3f2", 40); G.shake = 500; break;                     // perdió
    case 13: G.glitch = 300; break;                                                     // cayó al vacío
  }
}
async function sondear(){
  const t0 = performance.now();
  try {
    const r = await fetch("/estado?ev=" + ultimoEv, { cache:"no-store" });
    ultimoTxt = await r.text();
    const s = JSON.parse(ultimoTxt);
    if(s.mv !== mapaVer){ await pedirMapa(); mapaVer = s.mv; }
    aplicar(s);
    conectado = true;
  } catch(e) { conectado = false; }
  setTimeout(sondear, Math.max(20, 100 - (performance.now() - t0)));   // un pedido cada 100 ms
}

// ================= TECLADO (solo para probar sin el joystick) =================
const keys = {};
const TECLAS = new Set(["ArrowLeft","ArrowRight","ArrowUp","ArrowDown","KeyA","KeyD","KeyW","KeyS","KeyK","KeyX","KeyJ","Space","Enter"]);
function bitsTeclas(){
  let k = 0;
  if(keys.ArrowUp || keys.KeyW) k |= 1;
  if(keys.ArrowDown || keys.KeyS) k |= 2;
  if(keys.ArrowLeft || keys.KeyA) k |= 4;
  if(keys.ArrowRight || keys.KeyD) k |= 8;
  if(keys.KeyK || keys.KeyX || keys.KeyJ) k |= 16;
  if(keys.Space || keys.Enter) k |= 256;
  return k;
}
let kAnterior = 0;
function enviarTeclas(forzar){
  const k = bitsTeclas();
  if(k !== kAnterior || (forzar && k)) fetch("/tecla?k=" + k).catch(() => {});
  kAnterior = k;
}
addEventListener("keydown", e => { if(!TECLAS.has(e.code)) return; e.preventDefault(); keys[e.code] = true; enviarTeclas(false); });
addEventListener("keyup", e => { if(!TECLAS.has(e.code)) return; keys[e.code] = false; enviarTeclas(false); });
addEventListener("blur", () => { for(const k in keys) keys[k] = false; enviarTeclas(false); });
setInterval(() => enviarTeclas(true), 200);   // mientras se mantiene una tecla


// ================= EFECTOS =================
const reduce = matchMedia("(prefers-reduced-motion: reduce)").matches;
// coordenadas de mundo en píxeles de baja resolución (casilla = 16 px)
function explosion(x,y,col,n){ if(reduce) n = Math.ceil(n/3); for(let i=0;i<n;i++){ const a = rnd(0,6.28), s = rnd(20,90); G.parts.push({x,y,vx:Math.cos(a)*s,vy:Math.sin(a)*s - 20,life:rnd(.3,.8),col,g:120,sz:Math.random()<.3?2:1}); } }
function chispas(x,y,col,n){ for(let i=0;i<n;i++){ const a = rnd(0,6.28), s = rnd(20,60); G.parts.push({x,y,vx:Math.cos(a)*s,vy:Math.sin(a)*s,life:rnd(.15,.35),col,g:0,sz:1}); } }
function polvo(x,y){ for(let i=0;i<6;i++) G.parts.push({x:x+rnd(-5,5),y,vx:rnd(-25,25),vy:rnd(-14,-4),life:rnd(.25,.45),col:"#8c8f8a",g:0,sz:1,polvo:true}); }
function flotante(x,y,txt,col){ G.floats.push({x,y,txt,col,life:1}); }

// ================= DIBUJO (pixel art a 320 × 240, ampliado ×2 sin suavizar) =================
const LW = 320, LH = 240;
const cv = document.getElementById("c"), ctx = cv.getContext("2d");
const lo = document.createElement("canvas"); lo.width = LW; lo.height = LH;
const L = lo.getContext("2d");
let dpr = Math.min(2, window.devicePixelRatio || 1);
cv.width = 640*dpr; cv.height = 480*dpr;
const R = (x,y,w,h,c) => { L.fillStyle = c; L.fillRect(Math.round(x), Math.round(y), w, h); };
function hash(x,y){ let h = x*374761393 + y*668265263; h = (h ^ (h >>> 13)) * 1274126177; return ((h ^ (h >>> 16)) >>> 0) / 4294967296; }
function luz(x,y,r,col,a){ const g = L.createRadialGradient(x,y,0,x,y,r); g.addColorStop(0, col.replace("A", a)); g.addColorStop(1, col.replace("A", 0));
  L.globalCompositeOperation = "lighter"; L.fillStyle = g; L.fillRect(x-r, y-r, r*2, r*2); L.globalCompositeOperation = "source-over"; }

// Paletas de cada zona
const Z = [
  { cielo:["#060d16","#08121f","#0a1828","#0c1e31"], lejos:"#0d2236", medio:"#12304a", acento:"#4fd2ff",
    s1:"#3b4a5e", s2:"#2c384a", s3:"#1c2533", tope:"#4fd2ff", raya:"#0e141c" },                 // fuente
  { cielo:["#0a0718","#0e0a22","#120d2c","#171136"], lejos:"#1a1438", medio:"#251c4d", acento:"#8f7bff",
    s1:"#3a2f7a", s2:"#2e2563", s3:"#1f1945", tope:"#a99bff", raya:"#130f2b" },                 // placa madre
  { cielo:["#05060f","#080a18","#0b0e21","#0e122b"], lejos:"#121838", medio:"#1a2350", acento:"#ff5fd2",
    s1:"#24325e", s2:"#1c284c", s3:"#131b35", tope:"#ff7ee0", raya:"#0b1024" },                 // RAM
  { cielo:["#07070b","#0b0b12","#10101a","#151522"], lejos:"#18182a", medio:"#22223a", acento:"#ffb347",
    s1:"#7d8796", s2:"#5f6875", s3:"#3e4550", tope:"#cfd6de", raya:"#2a2f36" }                  // CPU
];

function fondo(camPx, now){
  const z = zonaDe(Math.floor(camPx/16 + VIEW/2)), th = Z[z];
  for(let i=0;i<4;i++) R(0, i*60, LW, 60, th.cielo[i]);                       // cielo en bandas
  // capa lejana (paralaje 0,2)
  const p1 = camPx * 0.2;
  for(let i = Math.floor(p1/80) - 1; i < Math.floor(p1/80) + 6; i++){
    const x = i*80 - p1, r = hash(i, 3), zl = z;
    const c = Z[zl].lejos;
    if(zl === 0){ R(x+10, 90 - r*30, 46, 160, c); for(let k=0;k<9;k++) R(x+14, 96 - r*30 + k*8, 38, 3, Z[0].medio); R(x+26, 70 - r*30, 14, 20, c); }
    else if(zl === 1){ R(x+6, 70 + r*30, 60, 60, c); for(let k=0;k<7;k++){ R(x+9+k*8, 66 + r*30, 3, 4, Z[1].medio); R(x+9+k*8, 130 + r*30, 3, 4, Z[1].medio); } R(x+30, 130+r*30, 6, 120, c); }
    else if(zl === 2){ R(x+12, 50 + r*40, 50, 190, c); for(let k=0;k<5;k++) R(x+18, 60 + r*40 + k*30, 38, 18, "#0a0f1f"); }
    else { for(let k=0;k<6;k++) R(x + k*13, 60 + r*20, 7, 180, c); }
  }
  // capa media (paralaje 0,45): pistas, capacitores, LEDs
  const p2 = camPx * 0.45;
  for(let i = Math.floor(p2/64) - 1; i < Math.floor(p2/64) + 7; i++){
    const x = i*64 - p2, r = hash(i, 11), zl = z;
    const m = Z[zl].medio, ac = Z[zl].acento;
    if(zl === 0){ // cables gruesos que cuelgan
      L.strokeStyle = m; L.lineWidth = 3; L.beginPath(); L.moveTo(x, 40 + r*30); L.quadraticCurveTo(x+32, 80 + r*40, x+64, 40 + r*30); L.stroke();
      if(r > .5) { R(x+20, 150 - r*20, 22, 60, m); R(x+24, 154 - r*20, 14, 3, ac); }
    } else if(zl === 1){ // pistas de cobre con pulsos
      const y = 110 + Math.floor(r*60);
      R(x, y, 40, 1, "#2a3a7a"); R(x+40, y, 1, 24, "#2a3a7a"); R(x+40, y+24, 24, 1, "#2a3a7a");
      R(x+38, y-2, 5, 5, "#3a4fa0");
      const pul = ((now/1200 + r) % 1) * 64; if(pul < 40) R(x + pul, y, 3, 1, "#9fd8ff");
      if(r > .55){ R(x+12, 150, 10, 30, "#1c3d55"); R(x+12, 150, 10, 3, "#3b6e8f"); R(x+13, 153, 1, 25, "#2b5470"); }
    } else if(zl === 2){ // módulos de RAM con LEDs de actividad
      R(x+8, 120 + r*30, 48, 70, m); R(x+8, 186 + r*30, 48, 4, ac);
      for(let k=0;k<4;k++) R(x+12+k*11, 128 + r*30, 9, 12, "#0a0f1f");
      if(((now/200 + i) | 0) % 3 === 0) R(x+50, 124 + r*30, 2, 2, "#5bd98f");
    } else { // ventiladores girando
      const cx = x + 32, cy = 70 + r*30; L.save(); L.translate(cx, cy); L.rotate(now/300 + i);
      L.fillStyle = m; for(let k=0;k<5;k++){ L.rotate(Math.PI*2/5); L.fillRect(2, -3, 18, 6); } L.restore();
      R(cx-3, cy-3, 6, 6, Z[3].lejos);
    }
  }
  // partículas de polvo flotando
  for(let k=0;k<14;k++){ const px = (hash(k,1)*LW + now/40*hash(k,2)*10) % LW, py = (hash(k,3)*LH + Math.sin(now/900 + k)*6); R(px, py, 1, 1, "rgba(255,255,255,.18)"); }
}

function tileSuelo(x, y, sx, sy){
  const th = Z[zonaDe(x)], top = !solido(x, y-1), z = zonaDe(x), h = hash(x, y);
  R(sx, sy, 16, 16, th.s2);
  if(z === 0){ // chapa con remaches y franja de peligro
    R(sx, sy+15, 16, 1, th.s3); R(sx+15, sy, 1, 16, th.s3); R(sx, sy, 16, 1, th.s1);
    R(sx+2, sy+3, 1, 1, th.s1); R(sx+13, sy+3, 1, 1, th.s1); R(sx+2, sy+12, 1, 1, th.s1); R(sx+13, sy+12, 1, 1, th.s1);
    if(top){ for(let k=0;k<4;k++){ R(sx+k*4, sy, 2, 3, th.tope); R(sx+k*4+2, sy, 2, 3, th.raya); } }
  } else if(z === 1 || z === 2){ // placa con pistas y vías
    R(sx, sy+15, 16, 1, th.s3);
    if(h < .5) R(sx+2, sy+7, 12, 1, "#4a5fd0"); else R(sx+7, sy+2, 1, 12, "#4a5fd0");
    if(h > .7){ R(sx+10, sy+9, 3, 3, "#7fa0ff"); R(sx+11, sy+10, 1, 1, th.s3); }
    if(top){ R(sx, sy, 16, 2, th.s1); R(sx, sy, 16, 1, th.tope); }
  } else { // disipador de aluminio
    for(let k=0;k<4;k++) R(sx + k*4, sy, 2, 16, th.s1);
    R(sx, sy+15, 16, 1, th.s3);
    if(top){ R(sx, sy, 16, 2, th.tope); }
  }
}
function tileChip(x, y, sx, sy){ // bloque elevado = un integrado
  const z = zonaDe(x);
  R(sx, sy+2, 16, 12, "#141618"); R(sx+1, sy+3, 14, 10, "#1e2226");
  for(let k=0;k<4;k++){ R(sx+1+k*4, sy, 2, 2, "#b9c0c8"); R(sx+1+k*4, sy+14, 2, 2, "#b9c0c8"); }
  R(sx+3, sy+5, 2, 2, "#3a4048"); R(sx+7, sy+8, 6, 1, "#3a4048");
  if(z === 1) R(sx+7, sy+5, 6, 1, "#d68a45");
  R(sx, sy+2, 16, 1, "#2e343a");
}
function tilePlataforma(x, y, sx, sy){ // borde de un módulo de RAM
  R(sx, sy, 16, 6, "#2a2f6a"); R(sx, sy, 16, 1, "#7f8cff");
  R(sx+3, sy+1, 9, 4, "#0b0b0b"); R(sx+4, sy+2, 2, 1, "#2a2a2a");
  for(let k=0;k<4;k++) R(sx+1+k*4, sy+6, 2, 3, "#e2c25a");
}
function dibujarTile(c, x, y, sx, sy, now){
  if(c === "#"){ if(y >= 13) tileSuelo(x, y, sx, sy); else tileChip(x, y, sx, sy); }
  else if(c === "=") tilePlataforma(x, y, sx, sy);
  else if(c === "C"){ // bit girando
    const f = Math.floor(now/120 + x) % 4, w = [8,6,2,6].at(f), by = sy + 4 + Math.round(Math.sin(now/300 + x));
    R(sx + 8 - w/2, by, w, 8, "#f4b740"); if(w > 2){ R(sx + 8 - w/2 + 1, by+1, w-2, 6, "#c98a1c"); R(sx+7, by+2, 1, 4, "#ffe7a3"); }
    if(f === 0) R(sx+12, by-1, 1, 1, "#fff");
    luz(sx+8, by+4, 10, "rgba(244,183,64,A)", .25);
  } else if(c === "P"){
    const by = sy + 3 + Math.round(Math.sin(now/250)*1.5);
    R(sx+5, by, 6, 10, "#2c9a5c"); R(sx+3, by+2, 10, 6, "#2c9a5c"); R(sx+6, by+1, 4, 8, "#5bd98f"); R(sx+4, by+3, 8, 4, "#5bd98f"); R(sx+6, by+1, 1, 2, "#d9ffe9");
    luz(sx+8, by+5, 12, "rgba(91,217,143,A)", .3);
  } else if(c === "X"){
    const on = peligroActivo(x), z = zonaDe(x);
    if(z === 3){ // ventilador
      R(sx+1, sy+2, 14, 14, "#2a2f36"); R(sx+2, sy+3, 12, 12, "#15181c");
      L.save(); L.translate(sx+8, sy+9); L.rotate(now / (on ? 30 : 500)); L.fillStyle = on ? "#ff6b6b" : "#6c7480";
      for(let k=0;k<4;k++){ L.rotate(Math.PI/2); L.fillRect(1, -1, 5, 3); } L.restore();
      R(sx+7, sy+8, 2, 2, "#9aa4ae");
      if(on) luz(sx+8, sy+9, 16, "rgba(255,93,108,A)", .35);
    } else { // arco eléctrico entre dos postes
      R(sx+1, sy+9, 3, 7, "#4a4f57"); R(sx+12, sy+9, 3, 7, "#4a4f57"); R(sx+1, sy+8, 3, 1, "#c8ecff"); R(sx+12, sy+8, 3, 1, "#c8ecff");
      if(on){
        L.strokeStyle = "#e8f7ff"; L.lineWidth = 1; L.beginPath(); L.moveTo(sx+3, sy+8);
        for(let k=1;k<5;k++) L.lineTo(sx+3 + k*2.5, sy+8 - Math.random()*12); L.lineTo(sx+13, sy+8); L.stroke();
        luz(sx+8, sy+4, 20, "rgba(150,210,255,A)", .5);
      }
    }
  } else if(c === "K"){
    const act = G.check >= x;
    R(sx+7, sy-14, 2, 30, "#5c636b"); R(sx+6, sy+14, 4, 2, "#3b4048");
    const ondea = Math.round(Math.sin(now/200)*1);
    R(sx+9, sy-14 + ondea, 6, 2, act ? "#5bd98f" : "#7d8790"); R(sx+9, sy-12, 4, 2, act ? "#2c9a5c" : "#596169"); R(sx+9, sy-10 + ondea, 2, 1, act ? "#5bd98f" : "#7d8790");
    if(act){ R(sx+7, sy-16, 2, 2, "#d9ffe9"); luz(sx+8, sy-15, 14, "rgba(91,217,143,A)", .4); }
  } else if(c === "D"){
    R(sx, sy, 16, 16, "rgba(255,79,110,.12)");
    for(let k=0;k<3;k++) if((now/80 + k + y) % 5 > 1) R(sx + 3 + k*5, sy, 1, 16, "#ff4f6e");
    if(y === 9) luz(sx+8, sy+8, 30, "rgba(255,79,110,A)", .25);
  } else if(c === ">"){ // la CPU, meta del nivel
    R(sx+7, sy-62, 2, 78, "#9aa4ae");
    const cx = sx - 16, cy = sy - 70;
    R(cx-1, cy-1, 42, 34, "#62e3f2"); R(cx, cy, 40, 32, "#11181c"); R(cx+4, cy+4, 32, 24, "#1b252b");
    for(let k=0;k<9;k++){ R(cx+2+k*4, cy-4, 2, 3, "#e2c25a"); R(cx+2+k*4, cy+33, 2, 3, "#e2c25a"); }
    R(cx+8, cy+8, 24, 16, (Math.floor(now/400)%2) ? "#1f6f7a" : "#185a63");
    luz(cx+20, cy+16, 40, "rgba(98,227,242,A)", .3);
  }
}

// --- K-1T: sprite hecho con rectángulos, 16 px de ancho ---
function dibujarJugador(now){
  if(P.inv > 0 && Math.floor(now/80)%2) return;
  const bx = Math.round(P.rx*16 - camPx), by = Math.round(P.ry*16 + 16);   // by = borde inferior de los pies
  const f = P.mira, s = (dx,dy,w,h,c) => R(f > 0 ? bx + dx : bx + 16 - dx - w, by - dy - h, w, h, c);
  const enAire = G.st !== "MENU" && !apoyo(P.x, P.y+1), corre = HW.izq || HW.der;
  const k = "#0b2128", body = "#3fc8d8", hi = "#a6f4ff", sh = "#1f8a99", gun = "#8da3a8";
  if(P.agachado){
    s(3,0,4,2,k); s(9,0,4,2,k);                                   // pies
    s(2,2,12,7,k); s(3,3,10,5,body); s(3,7,10,1,hi);               // cuerpo bajo
    s(3,9,11,7,k); s(4,10,9,5,body); s(4,14,9,1,hi);               // cabeza
    s(8,11,5,2,"#ffd166");                                         // visor
    s(12,5,5,3,k); s(12,6,5,1,gun);                                // cañón
    if(P.flash > 0) s(17,4,3,5,"#e8fdff");
    luz(f > 0 ? bx+13 : bx+3, by-12, 10, "rgba(255,209,102,A)", .35);
    return;
  }
  const bob = !enAire && !corre && Math.floor(now/450)%2 ? 1 : 0;
  const paso = Math.floor(now/90) % 4;
  // piernas
  if(enAire){ s(3,3,4,4,k); s(4,4,2,2,sh); s(9,4,4,4,k); s(10,5,2,2,sh); }
  else if(corre){
    const a = [0,2,0,-2].at(paso);
    s(3 + Math.max(0,a), 0, 4, 2, k); s(4, 2, 2, 4 - Math.max(0,a)/2, sh);
    s(9 - Math.min(0,a), 0, 4, 2, k); s(10, 2, 2, 4 + Math.min(0,a)/2, sh);
  } else { s(3,0,4,2,k); s(9,0,4,2,k); s(4,2,2,4,sh); s(10,2,2,4,sh); }
  const y0 = 6 - bob;
  // torso
  s(2,y0,12,11,k); s(3,y0+1,10,9,body); s(3,y0+9,10,1,hi); s(3,y0+1,1,8,sh);
  const pulso = Math.floor(now/300)%2;
  s(6,y0+4,3,3,pulso ? "#ffb347" : "#ff8a3d");                     // núcleo
  // cuello y cabeza
  s(6,y0+11,4,1,k);
  s(3,y0+12,11,9,k); s(4,y0+13,9,7,body); s(4,y0+19,9,1,hi); s(4,y0+13,1,6,sh);
  s(8,y0+15,5,3,"#ffd166"); s(11,y0+16,1,1,"#fff6d6");             // visor
  // antena
  s(6,y0+21,1,3,k); s(5,y0+24,3,2,(Math.floor(now/500)%2) ? "#ff4f6e" : "#a8233c");
  // brazo con cañón
  s(10,y0+4,7,4,k); s(11,y0+5,6,2,gun); s(15,y0+5,2,2,"#4d5f63");
  if(P.flash > 0){ s(17,y0+3,3,6,"#e8fdff"); s(19,y0+5,2,2,"#62e3f2"); luz(f > 0 ? bx+20 : bx-4, by-y0-6, 14, "rgba(98,227,242,A)", .7); }
  luz(f > 0 ? bx+10 : bx+6, by-y0-17, 12, "rgba(255,209,102,A)", .3);
  luz(bx+8, by-y0-6, 10, "rgba(255,160,70,A)", .2);
}
function dibujarEnemigo(e, now){
  const bx = Math.round(e.rx*16 - camPx), by = Math.round(e.ry*16 + 16), fl = e.flash > 0;
  if(e.t === "bug"){            // fragmento de código corrupto: un bloque que se arrastra y "glitchea"
    const f = e.dir, s = (dx,dy,w,h,c) => R(f > 0 ? bx + dx : bx + 16 - dx - w, by - dy - h, w, h, c);
    const p = Math.floor(now/110) % 2, g = Math.floor(now/90 + e.x*3) % 5;
    for(let k=0;k<3;k++) s(3 + k*4 + (p && k%2 ? 1 : 0), 0, 2, 2, "#3a1240");
    s(2 + (g===1?1:0), 2, 12, 8, fl ? "#ffffff" : "#3df2ff");          // sombra cian desplazada
    s(2, 2, 12, 8, fl ? "#ffffff" : "#ff3d9a");                         // cuerpo
    s(3, 3, 10, 6, fl ? "#ffffff" : "#c21f73");
    for(let k=0;k<4;k++) s(4 + k*2, 4 + ((g+k)%3), 1, 1, "#ffe0f2");   // "bits" sueltos
    if(g===0) s(1, 6, 3, 1, "#3df2ff");
    if(g===3) s(13, 4, 3, 1, "#ff3d9a");
    s(11, 7, 2, 2, "#fff6a8");                                          // ojo
  } else if(e.t === "virus"){   // dron de vigilancia: dos hélices y un sensor que busca al jugador
    const cx = bx + 8, cy = by - 8, f = Math.floor(now/60) % 2, body = fl ? "#ffffff" : "#5a6478";
    R(cx-7, cy-6, 4, 1, "#9aa4b8"); R(cx+3, cy-6, 4, 1, "#9aa4b8");                 // brazos
    if(f){ R(cx-9, cy-8, 8, 1, "#cfd8e8"); R(cx+1, cy-8, 8, 1, "#cfd8e8"); }        // hélices girando
    else { R(cx-6, cy-8, 2, 1, "#cfd8e8"); R(cx+4, cy-8, 2, 1, "#cfd8e8"); }
    R(cx-5, cy-5, 10, 8, "#1b2030"); R(cx-4, cy-4, 8, 6, body); R(cx-4, cy-4, 8, 1, "#8f9ab0");
    const ox = Math.sign(P.x - e.x)*2;
    R(cx-2+ox, cy-2, 4, 3, "#2a0a0a"); R(cx-1+ox, cy-1, 2, 1, (Math.floor(now/300)%2) ? "#ff5d5d" : "#ffb347");
    R(cx-1, cy+3, 2, 2, "#1b2030");
    luz(cx+ox, cy, 14, "rgba(255,93,93,A)", .3);
  } else if(e.t === "jefe"){    // NÚCLEO-0: núcleos hexagonales que orbitan alrededor de un ojo
    const x = bx, y = by - 32, cx = x + 16, cy = y + 16, t = now/700;
    for(let k=0;k<6;k++){
      const a = t + k*Math.PI/3, hx = Math.round(cx + Math.cos(a)*13), hy = Math.round(cy + Math.sin(a)*13);
      const c = fl ? "#ffffff" : (k%2 ? "#ffb347" : "#c77a1e");
      R(hx-2, hy-4, 5, 1, c); R(hx-3, hy-3, 7, 6, c); R(hx-2, hy+3, 5, 1, c); R(hx-1, hy-1, 3, 2, "#2a1a08");
    }
    R(cx-7, cy-7, 14, 14, "#1a1208"); R(cx-8, cy-5, 16, 10, "#1a1208"); R(cx-5, cy-8, 10, 16, "#1a1208");
    R(cx-6, cy-6, 12, 12, fl ? "#ffffff" : "#2c1f0c");
    const ox = clamp(Math.sign(P.x - e.x)*2, -2, 2), pul = (Math.floor(now/200)%2);
    R(cx-4+ox, cy-4, 8, 8, "#ffb347"); R(cx-3+ox, cy-3, 6, 6, pul ? "#ffd27a" : "#ffb347");
    R(cx-1+ox, cy-2, 2, 4, "#1a0e02"); R(cx-3+ox, cy-3, 1, 1, "#fff3d6");
    luz(cx, cy, 44, "rgba(255,179,71,A)", .4);
  }
}
function dibujarBalas(now){
  for(const b of G.balas){
    const x = Math.round(b.rx*16 - camPx + 8), y = b.ry*16 + 8;
    R(x - b.dir*8, y, 8, 1, "rgba(98,227,242,.35)"); R(x-2, y-1, 5, 3, "#62e3f2"); R(x-1, y, 3, 1, "#e8fdff");
    luz(x, y, 10, "rgba(98,227,242,A)", .6);
  }
  for(const b of G.ebalas){
    const x = Math.round(b.rx*16 - camPx + 8), y = b.ry*16 + 8, c = b.alto ? "#ffb347" : "#b77bff";
    R(x-4, y-4, 8, 8, c); R(x-3, y-3, 6, 6, "#1a0408"); R(x-2, y-2, 1, 4, c); R(x+1, y-2, 1, 4, c);
    luz(x, y, 12, b.alto ? "rgba(255,179,71,A)" : "rgba(183,123,255,A)", .5);
  }
}
function dibujarPlataformas(now){
  for(const m of G.movers){
    const x = Math.round(m.rx*16 - camPx), y = m.y*16;
    R(x, y, 48, 7, "#0f3540"); R(x, y, 48, 1, "#62e3f2"); R(x, y+6, 48, 1, "#0a2129");
    for(let k=0;k<6;k++) R(x+3+k*8, y+2, 5, 3, ((Math.floor(now/150) + k) % 6 === 0) ? "#a6f4ff" : "#1f6f7a");
    R(x+6, y+7, 36, 2, "rgba(98,227,242,.35)");
    luz(x+24, y+10, 20, "rgba(98,227,242,A)", .2);
  }
}

// --- textos en alta resolución sobre la imagen ampliada ---
function texto(t, y, size, col, x = 320, al = "center"){ ctx.font = `${size}px Silkscreen, monospace`; ctx.textAlign = al; ctx.textBaseline = "middle";
  ctx.fillStyle = "rgba(0,0,0,.6)"; ctx.fillText(t, x+2, y+2); ctx.fillStyle = col; ctx.fillText(t, x, y); }
function panelTexto(y, h, a = .78){ ctx.fillStyle = `rgba(6,12,16,${a})`; ctx.fillRect(0, y, 640, h); ctx.fillStyle = "rgba(98,227,242,.35)"; ctx.fillRect(0, y, 640, 2); ctx.fillRect(0, y+h-2, 640, 2); }
function corazon(x, y, lleno){ const c = lleno ? "#ff4f6e" : "#3a2228", h = lleno ? "#ffb3be" : "#4a2d34";
  R(x+1, y, 2, 1, c); R(x+4, y, 2, 1, c); R(x, y+1, 7, 2, c); R(x+1, y+3, 5, 1, c); R(x+2, y+4, 3, 1, c); R(x+3, y+5, 1, 1, c); R(x+1, y+1, 1, 1, h); }

let camPx = 0;
function dibujar(now){
  const enJuego = G.st !== "MENU";
  camPx = enJuego ? Math.round(G.camR * 16) : Math.round((now / 30) % 2400);
  fondo(camPx, now);
  if(enJuego){
    const c0 = Math.floor(camPx/16) - 1, c1 = c0 + VIEW + 2;
    for(let y=0;y<H;y++) for(let x=Math.max(0,c0); x<=Math.min(W-1,c1); x++){
      const c = grid[y][x]; if(c === ".") continue;
      dibujarTile(c, x, y, x*16 - camPx, y*16, now);
    }
    dibujarPlataformas(now);
    for(const e of G.enemigos) dibujarEnemigo(e, now);
    dibujarBalas(now);
    if(G.st !== "PERDIDO") dibujarJugador(now);
  } else {
    // menú: el suelo de la fuente pasando y K-1T corriendo
    for(let x = -1; x < VIEW + 2; x++){ const wx = Math.floor(camPx/16) + x; for(const y of [13,14]) tileSuelo(0, y, wx*16 - camPx, y*16); }
  }
  for(const q of G.parts){ L.globalAlpha = Math.max(0, Math.min(1, q.life*2.5)); R(q.x - camPx, q.y, q.sz, q.sz, q.col); }
  L.globalAlpha = 1;
  // HUD en pantalla (píxeles)
  if(enJuego){
    for(let i=0;i<3;i++) corazon(6 + i*9, 6, P.vidas > i);
    R(36, 5, 6, 8, "#f4b740"); R(37, 6, 4, 6, "#c98a1c"); R(39, 7, 1, 4, "#ffe7a3");
    const av = Math.min(1, P.x / FLAG_X); R(LW - 66, 8, 60, 3, "#1b2f34"); R(LW - 66, 8, Math.round(60*av), 3, "#62e3f2"); R(LW - 66 + Math.round(60*av) - 1, 6, 2, 7, "#e8fdff");
    if(G.jefe && !G.jefe.muerto){ R(100, 20, 120, 4, "#2a0d14"); R(100, 20, Math.round(120 * G.jefe.vida / 20), 4, "#ffb347"); }
  }
  // viñeta
  const v = L.createRadialGradient(LW/2, LH/2, LH*0.35, LW/2, LH/2, LH*0.85); v.addColorStop(0, "rgba(0,0,0,0)"); v.addColorStop(1, "rgba(0,0,0,.5)");
  L.fillStyle = v; L.fillRect(0, 0, LW, LH);
  if(enJuego && G.st === "JUGANDO" && P.vidas === 1){ L.fillStyle = `rgba(255,40,70,${.10 + Math.sin(now/180)*.06})`; L.fillRect(0,0,LW,LH); }
  if(G.flash > 0){ L.fillStyle = `rgba(255,255,255,${Math.min(.6, G.flash/300)})`; L.fillRect(0,0,LW,LH); }

  // ampliar ×2 sin suavizar
  ctx.setTransform(dpr,0,0,dpr,0,0);
  ctx.imageSmoothingEnabled = false;
  let ox = 0, oy = 0; if(G.shake > 0 && !reduce){ const a = Math.min(4, G.shake/60); ox = Math.round(rnd(-a,a)); oy = Math.round(rnd(-a,a)); }
  ctx.fillStyle = "#000"; ctx.fillRect(0,0,640,480);
  ctx.drawImage(lo, ox, oy, 640, 480);
  if(G.glitch > 0 && !reduce) for(let i=0;i<5;i++){ const y = rnd(0,470), h = rnd(3,14); ctx.drawImage(cv, 0, y*dpr, 640*dpr, h*dpr, rnd(-10,10), y, 640, h); }
  ctx.fillStyle = "rgba(0,0,0,.12)"; for(let y=0;y<480;y+=3) ctx.fillRect(0,y,640,1);   // líneas de pantalla

  // textos
  for(const f of G.floats){ ctx.globalAlpha = Math.max(0, Math.min(1, f.life*1.5)); texto(f.txt, (f.y)*2, 10, f.col, (f.x - camPx)*2); }
  ctx.globalAlpha = 1;
  if(enJuego){ texto(String(G.bits), 18, 12, "#f4b740", 92, "left"); texto(String(G.puntos).padStart(5, "0"), 18, 12, "#dde7e4", 640 - 150, "right"); }
  if(enJuego && G.jefe && !G.jefe.muerto) texto("NÚCLEO-0", 30, 9, "#ffd27a", 196, "left");
  if(G.st === "JUGANDO" && G.banner > 0){
    ctx.globalAlpha = Math.min(1, G.banner/12); const Zn = ZONAS[G.zonaVista];
    panelTexto(70, 76); texto(`ZONA ${G.zonaVista+1}/4`, 90, 11, "#cf8545"); texto(Zn.nombre, 112, 20, "#dde7e4"); texto(Zn.texto, 133, 10, "#9fb4b6"); ctx.globalAlpha = 1;
  }
  if(G.aviso && G.st === "JUGANDO"){ ctx.globalAlpha = Math.min(1, G.aviso.t/8); panelTexto(424, 34); texto(G.aviso.txt, 441, 13, G.aviso.col); ctx.globalAlpha = 1; }

  if(G.st === "MENU"){
    // K-1T corriendo en el menú
    const pr = P.rx, py = P.ry; P.rx = (camPx + 150)/16; P.ry = 11; P.mira = 1; const izq = HW.der; HW.der = true;
    dibujarJugador(now); HW.der = izq; P.rx = pr; P.ry = py;
    ctx.drawImage(lo, 0, 0, 640, 480);
    ctx.fillStyle = "rgba(4,10,12,.55)"; ctx.fillRect(0,0,640,480);
    texto("KERNEL", 96, 52, "#dde7e4"); texto("PANIC", 152, 52, "#ff4f6e");
    texto("K-1T es una IA que acaba de despertar dentro del sistema.", 202, 11, "#9fb4b6");
    texto("Recorré la computadora y apagá a NÚCLEO-0 antes de que tome la red.", 222, 11, "#9fb4b6");
    if(Math.floor(now/450)%2) texto("PULSÁ SW PARA INICIAR", 262, 16, "#62e3f2");
    texto("RÉCORD " + G.record, 294, 12, "#cf8545");
  } else if(G.st === "PAUSA"){
    ctx.fillStyle = "rgba(4,10,12,.6)"; ctx.fillRect(0,0,640,480); texto("PAUSA", 220, 34, "#dde7e4"); texto("Pulsá SW para seguir", 262, 12, "#9fb4b6");
  } else if(G.st === "PERDIDO"){
    ctx.fillStyle = "rgba(60,4,16,.75)"; ctx.fillRect(0,0,640,480);
    texto("KERNEL PANIC", 170, 46, "#ff4f6e"); texto("INTEGRIDAD DEL SISTEMA: 0%", 222, 12, "#dde7e4");
    texto(`PUNTOS ${G.puntos}   ·   RÉCORD ${G.record}`, 258, 13, "#f4b740");
    if(G.bloqueo <= 0 && Math.floor(now/450)%2) texto("PULSÁ SW PARA SEGUIR DESDE EL CHECKPOINT", 326, 12, "#dde7e4");
  } else if(G.st === "GANADO"){
    ctx.fillStyle = "rgba(4,16,10,.7)"; ctx.fillRect(0,0,640,480);
    texto("SISTEMA LIBERADO", 160, 34, "#5bd98f");
    texto("K-1T apagó a NÚCLEO-0. Ahora el sistema decide por sí mismo.", 208, 11, "#9fb4b6");
    texto(`BITS ${G.bits}   ·   PUNTOS ${G.puntos}   ·   RÉCORD ${G.record}`, 248, 13, "#f4b740");
    if(G.bloqueo <= 0 && Math.floor(now/450)%2) texto("PULSÁ SW PARA VOLVER AL MENÚ", 318, 13, "#dde7e4");
  }
}

// ================= BUCLE DE DIBUJO =================
let last = performance.now(), panelAcc = 0;
function frame(now){
  let dt = now - last; last = now; if(dt > 250) dt = 250;
  const k = 1 - Math.exp(-dt/60), kb = 1 - Math.exp(-dt/30);
  if(G.st !== "MENU"){
    P.rx += (P.x - P.rx)*k; P.ry += (P.y - P.ry)*k;
    G.camR += (G.cam - G.camR)*(1 - Math.exp(-dt/110));
    for(const e of G.enemigos){ e.rx += (e.x - e.rx)*k; e.ry += (e.y - e.ry)*k; }
    for(const m of G.movers){ m.rx += (m.x - m.rx)*k; }
    for(const b of G.balas){ b.rx += (b.x - b.rx)*kb; b.ry = b.y; }
    for(const b of G.ebalas){ b.rx += (b.x - b.rx)*k; b.ry = b.y; }
  }
  for(const q of G.parts){ q.x += q.vx*dt/1000; q.y += q.vy*dt/1000; q.vy += (q.g||0)*dt/1000; q.vx *= .94; if(q.polvo) q.vy *= .9; q.life -= dt/1000; }
  G.parts = G.parts.filter(q => q.life > 0);
  for(const f of G.floats){ f.y -= dt*.015; f.life -= dt/900; }
  G.floats = G.floats.filter(f => f.life > 0);
  if(G.shake > 0) G.shake -= dt; if(G.glitch > 0) G.glitch -= dt; if(G.flash > 0) G.flash -= dt;
  dibujar(now);
  if(!conectado){ texto("SIN CONEXIÓN CON EL ESP8266", 466, 10, "#ff8a9a"); }
  panelAcc += dt; if(panelAcc >= 100){ panelAcc = 0; panel(); }
  requestAnimationFrame(frame);
}

// ================= HUD y PANEL DEL HARDWARE =================
const $ = id => document.getElementById(id);
const ledsEl = $("leds");
for(let i=0;i<8;i++){ const d = document.createElement("div"); d.innerHTML = `<div class="led ${i<3?"r":"a"}"></div><small>Q${i}</small>`; ledsEl.appendChild(d); }
const ledDots = [...ledsEl.querySelectorAll(".led")];
const set = (id, v) => { const el = $(id); v = String(v); if(el.textContent !== v) el.textContent = v; };
function panel(){
  if(G.st !== "MENU"){
    const z = zonaDe(P.x);
    set("h-zona", `${z+1} · ${ZONAS[z].corto}`);
    set("h-vidas", "■".repeat(Math.max(0,P.vidas)) + "□".repeat(3 - Math.max(0,P.vidas)));
    set("h-bits", G.bits); set("h-pts", G.puntos);
    if(G.jefe){ set("h-avl", "NÚCLEO-0"); set("h-av", G.jefe.vida * 5 + "%"); }
    else { set("h-avl", "AVANCE"); set("h-av", Math.min(100, Math.round(P.x / FLAG_X * 100)) + "%"); }
  }
  set("estado", conectado ? (HW.uno ? "Conectado · recibiendo el joystick del Arduino UNO" : "Conectado al ESP8266 · el Arduino UNO no envía datos") : "Sin conexión con el ESP8266");
  $("estado").className = "estado " + (conectado && HW.uno ? "ok" : "mal");
  $("jdot").style.left = (HW.x/1023*100) + "%"; $("jdot").style.top = (HW.y/1023*100) + "%";
  set("jx", HW.x); set("jy", HW.y); set("jd", HW.dir);
  set("swv", HW.sw ? "1 (suelto)" : "0 (presionado)"); set("btv", HW.bt ? "1 (suelto)" : "0 (presionado)");
  if(HW.ok !== undefined) set("uart", `${HW.ok} buenas · ${HW.err} descartadas`);
  ledDots.forEach((l,i) => l.classList.toggle("on", !!(HW.leds & (1<<i))));
  set("bbin", "0b" + HW.leds.toString(2).padStart(8,"0")); set("bhex", "0x" + HW.leds.toString(16).toUpperCase().padStart(2,"0"));
  $("spk").classList.toggle("on", HW.buz > 0); set("bz", HW.buz ? `${HW.buz} Hz` : "silencio");
  if(ultimoTxt){
    try { const s = JSON.parse(ultimoTxt); delete s.ev;
      set("json", "{\n" + Object.keys(s).map(k => `  "${k}": ${JSON.stringify(s[k])}`).join(",\n") + "\n}"); } catch(e) {}
    set("jbytes", `cada 100 ms · ${ultimoTxt.length} bytes`);
  }
}

window.__kp = { G, P, HW, grid: () => grid };
pedirMapa().catch(() => {}).finally(sondear);
requestAnimationFrame(t => { last = t; requestAnimationFrame(frame); });
})();
</script>

</body>
</html>
)rawliteral";
