// The front door of a bring-your-own-files host, drawn as a Diablo II title screen: the widescreen
// game-select backdrop, the burning logo, the eleven archives the game needs and retail's wide buttons.
// The retail art comes staged with the site (door-art/, tools/door-art.mjs); a host without it cuts the same
// art from the player's own archives once the three core ones are given, and until then draws the layout in
// plain canvas text. Nothing is uploaded. ENTER SANCTUARY resolves with the mounts and turns the door into
// retail's join-game loading screen, whose frame follows the real boot and lifts on the game's first frame (window.__d2Shown).
import { createCollection, fromDrop, pickFiles, pickFolder, restore, forget } from './byo-source.mjs';
import { ROLES } from './byo-detect.mjs';
import { readMpqFile } from './mpq-read.mjs';
import { buildDoorArt, DOOR_IMAGES } from './door-art-build.mjs';

const CSS = `
body.door #viewport,body.door #session-bar,body.door .controls,body.door #log,body.door #hud{display:none}
#door{position:fixed;inset:0;z-index:1000;background:#000}
#door canvas{position:absolute;inset:0;width:100%;height:100%;display:block;touch-action:manipulation}
#door .legal{position:absolute;left:16px;right:16px;bottom:6px;margin:0;color:#6d6552;font:11px/1.4 system-ui,sans-serif;text-align:center;pointer-events:none}
#door.loading .legal{display:none}
#door :focus-visible{outline:2px solid #c7b377;outline-offset:2px}
`;
const DISCORD = 'https://discord.gg/PzFfDkbSue';
const el = (tag, cls, text) => { const e = document.createElement(tag); if (cls) e.className = cls; if (text != null) e.textContent = text; return e; };

// Every file is required; the list reads in three columns.
const FILES = [
  ['d2data', 'The world: maps, monsters, items'], ['d2exp', 'Lord of Destruction'], ['d2char', 'The heroes'], ['d2sfx', 'Sound effects'],
  ['patch_d2', 'The 1.13c patch'], ['d2speech', 'Speech'], ['d2music', 'Music'], ['d2xmusic', 'Expansion music'],
  ['d2xtalk', 'Expansion speech'], ['d2video', 'Cinematics'], ['d2xvideo', 'Expansion cinematics'],
].filter(([r]) => ROLES[r]).map(([role, what]) => ({ role, f: ROLES[role].file.toLowerCase(), what }));
const COLS = [FILES.slice(0, 4), FILES.slice(4, 8), FILES.slice(8)];
const LIST = { y: 314, cx: [264, 400, 536] };
const BTN = { dir: { x: 264, y: 418, label: 'CHOOSE FOLDER' }, files: { x: 264, y: 460, label: 'CHOOSE FILES' },
  enter: { x: 264, y: 502, label: 'ENTER SANCTUARY' }, discord: { x: 264, y: 544, label: 'JOIN OUR DISCORD' } };
const AGAIN = { y: 401, label: 'Use the files from last time' };

// ---- retail art: the site's staged copy (tools/door-art.mjs), else cut from the player's archives ----
/** The staged door-art/ beside the site: meta.json and one PNG per image. */
async function loadStagedArt() {
  const base = new URL('../door-art/', import.meta.url);
  const meta = await (await fetch(new URL('meta.json', base))).json();
  const img = {};
  await Promise.all(DOOR_IMAGES.map(async (n) => {
    const r = await fetch(new URL(n + '.png', base)); if (!r.ok) throw new Error('door art ' + n);
    img[n] = await createImageBitmap(await r.blob());
  }));
  return { meta, img };
}
/** The same art, decoded in the browser from patch_d2, d2exp and d2data (first hit wins). */
async function loadArchiveArt(col) {
  const order = ['patch_d2', 'd2exp', 'd2data'].map((r) => col.state.slots[r]).filter((s) => s?.have)
    .map((s) => col.entries.find((e) => e.info === s.file)).filter(Boolean);
  const rd = async (name) => {
    for (const e of order) {
      const at = e.info.names?.(name); if (!at) continue;
      const b = await readMpqFile(e.file, e.info.base, e.info.shift, at, name).catch(() => null);
      if (b) return b.arrayBuffer();
    }
    throw new Error('missing ' + name);
  };
  const { meta, images } = await buildDoorArt(rd), img = {};
  for (const [n, s] of Object.entries(images)) {
    const c = document.createElement('canvas'); c.width = s.w; c.height = s.h;
    c.getContext('2d').putImageData(new ImageData(s.px, s.w, s.h), 0, 0); img[n] = c;
  }
  return { meta, img };
}

// Before the art: the same screen in canvas text.
const PLAIN = { font30: '26px Georgia,serif', font16: '13px Georgia,serif', fontexocet10: '600 13px Georgia,serif' };
const INK = { white: '#d8d8d8', red: '#e05050', gold: '#c7b377', grey: '#8b8b8b', tan: '#a8997a' };

export function frontDoor({ onReady } = {}) {
  const style = el('style'); style.textContent = CSS; document.head.appendChild(style);
  const root = el('div'); root.id = 'door';
  const cv = el('canvas'); cv.setAttribute('role', 'img');
  cv.setAttribute('aria-label', 'Diablo II title screen: choose the folder where Diablo II is installed, then enter.');
  root.append(cv, el('p', 'legal', 'Unofficial fan project, not affiliated with or endorsed by Blizzard Entertainment. You need to own Diablo II: Lord of Destruction.'));
  document.body.append(root);
  document.body.classList.add('door');
  const g = cv.getContext('2d');
  const col = createCollection();

  let art = null, artTried = false, back = null, offerAgain = false;
  let msg = null, msgUntil = 0, down = null, hover = null, loading = null, done = false;
  let view = { s: 1, x: 0, y: 0 };

  // The staged retail art; failing that it is cut from the player's archives once the core three are given.
  loadStagedArt().then((a) => { art ??= a; }).catch(() => {});

  // The game-select backdrop, registered to the 800x600 frame (served with the site; not retail-extracted at runtime).
  (async () => {
    try {
      const base = new URL('../widebg/', import.meta.url);
      const e = (await (await fetch(new URL('manifest.json', base))).json()).gameselectscreenexp;
      back = { ...e, sy: e.sy ?? e.s, img: await createImageBitmap(await (await fetch(new URL(e.file, base))).blob()) };
    } catch { /* black */ }
  })();

  // ---- text ----
  function measure(name, s) {
    const f = art?.meta.fonts[name];
    if (!f) { g.save(); g.font = PLAIN[name]; const w = g.measureText(s).width; g.restore(); return w; }
    let w = 0; for (const ch of s) w += f.glyphs[ch.charCodeAt(0)]?.[1] || 0; return w;
  }
  function text(name, colour, s, x, y, op) {
    const f = art?.meta.fonts[name], img = art?.img[`f_${name}_${colour}`] || art?.img[`f_${name}_white`] || art?.img[`f_${name}_gold`];
    g.save(); if (op) g.globalCompositeOperation = op;
    if (!f || !img) {
      g.font = PLAIN[name]; g.textBaseline = 'top'; g.fillStyle = name === 'fontexocet10' ? '#d8cdb0' : INK[colour] || INK.white;
      g.globalCompositeOperation = 'source-over'; g.fillText(s, x, y + 1);
    } else {
      let cx = Math.round(x); const cy = Math.round(y);
      for (const ch of s) {
        const gl = f.glyphs[ch.charCodeAt(0)]; if (!gl) continue;
        const [i, adv] = gl, fr = f.frames[i];
        if (fr && fr[0] && fr[1]) g.drawImage(img, (i % 16) * f.cw, ((i / 16) | 0) * f.ch, fr[0], fr[1], cx + fr[2], cy + fr[3], fr[0], fr[1]);
        cx += adv;
      }
    }
    g.restore();
  }
  const centre = (name, colour, s, cx, y, op) => text(name, colour, s, cx - measure(name, s) / 2, y, op);

  // ---- the title screen, in retail's 800x600 coordinates ----
  const ready = () => !!col.state.ready && !col.state.busy;
  const have = (x) => !!col.state.slots[x.role]?.have;
  function logo(t) {
    if (!art) return;
    const f = Math.floor(t * 25 / 1000);
    for (const k of ['blackL', 'fireL', 'blackR', 'fireR']) {
      const frs = art.meta['logo_' + k], [sx, w, h, ox, oy] = frs[f % Math.max(1, frs.length - 1)];
      g.save(); if (k.startsWith('fire')) g.globalCompositeOperation = 'lighter';
      g.drawImage(art.img['logo_' + k], sx, 0, w, h, 400 + ox, 102 + oy - h + 1, w, h);
      g.restore();
    }
  }
  function wideButton(b, pressed, off) {
    if (art) g.drawImage(art.img.wide, 0, pressed ? 35 : 0, 272, 35, b.x, b.y, 272, 35);
    else {
      g.fillStyle = pressed ? '#100d08' : '#1d1810'; g.fillRect(b.x, b.y, 272, 35);
      g.strokeStyle = '#5a4a2a'; g.lineWidth = 2; g.strokeRect(b.x + 1, b.y + 1, 270, 33);
    }
    const d = pressed ? 1 : 0;
    text('fontexocet10', 'white', b.label, b.x + (272 - measure('fontexocet10', b.label)) / 2 - d, b.y + 9 + d, 'multiply');
    if (off) {   // locked: the same button, sunk into shadow and drained of colour
      g.save(); g.globalCompositeOperation = 'saturation'; g.fillStyle = '#000'; g.fillRect(b.x, b.y, 272, 35);
      g.globalCompositeOperation = 'source-over'; g.fillStyle = 'rgba(0,0,0,0.6)'; g.fillRect(b.x, b.y, 272, 35); g.restore();
    }
  }
  const entryBox = (x) => { const c = COLS.findIndex((cl) => cl.includes(x)), r = COLS[c].indexOf(x), w = measure('font16', x.f);
    return { x: LIST.cx[c] - w / 2, y: LIST.y + r * 16 + 6, w, h: 14 }; };
  const againBox = () => { const w = measure('font16', AGAIN.label); return { x: 400 - w / 2, y: AGAIN.y, w, h: 14 }; };
  function tooltip(lines, ax, ay, side) {
    const w = Math.max(...lines.map(([s]) => measure('font16', s))) + 10, h = lines.length * 16 + 4;
    let x = Math.round(side < 0 ? ax - w - 6 : ax + 6); const y = Math.round(ay - h / 2); x = Math.max(2, Math.min(798 - w, x));
    g.fillStyle = 'rgba(0,0,0,0.85)'; g.fillRect(x, y, w, h);
    lines.forEach(([s, c], i) => centre('font16', c, s, x + w / 2, y + 2 + i * 16));
  }
  function status() {
    const s = col.state;
    if (s.busy) return ['Reading files…', 'tan'];
    if (msg && performance.now() < msgUntil) return msg;
    const bad = s.issues.find((i) => i.code === 'casc' || i.code === 'truncated'), other = s.issues[s.issues.length - 1];
    if (!ready() && (bad || other)) return [(bad || other).text, bad ? 'red' : 'tan'];
    const n = FILES.filter(have).length;
    return ready() ? ['All files found.', 'gold'] : [`${n} of ${FILES.length} found. Drop them here, or choose:`, 'tan'];
  }
  function wrap(s, max) {   // at most two lines
    if (measure('font16', s) <= max) return [s];
    const words = s.split(' '); let a = '';
    while (words.length && measure('font16', a + words[0] + ' ') <= max) a += words.shift() + ' ';
    return [a.trim(), words.join(' ')];
  }
  function scene(t) {
    logo(t);
    const sh = g.createRadialGradient(400, 350, 40, 400, 350, 300); sh.addColorStop(0, 'rgba(0,0,0,0.78)'); sh.addColorStop(1, 'rgba(0,0,0,0)');
    g.save(); g.translate(400, 350); g.scale(1, 0.6); g.translate(-400, -350); g.fillStyle = sh; g.fillRect(0, 0, 800, 700); g.restore();
    centre('font30', 'gold', 'Your Diablo II Files', 400, 236);
    centre('font16', 'grey', 'Choose the folder where Diablo II is installed.', 400, 276);
    centre('font16', 'grey', 'Your files are read here, never uploaded.', 400, 292);
    COLS.forEach((c, i) => c.forEach((x, r) => centre('font16', have(x) ? 'white' : 'red', x.f, LIST.cx[i], LIST.y + r * 16)));
    const [s, c] = status(), lines = wrap(s, 600);
    lines.forEach((l, i) => centre('font16', c, l, 400, (lines.length > 1 ? 380 : 386) + i * 15));
    if (offerAgain && !ready() && lines.length === 1) centre('font16', hover === 'again' ? 'white' : 'gold', AGAIN.label, 400, AGAIN.y);
    wideButton(BTN.dir, down === 'dir');
    wideButton(BTN.files, down === 'files');
    wideButton(BTN.enter, down === 'enter' && ready(), !ready());
    wideButton(BTN.discord, down === 'discord');
    if (hover && hover.f) { const b = entryBox(hover), left = COLS[0].includes(hover);
      tooltip([[hover.f, 'white'], [hover.what, 'grey'], have(hover) ? ['Found', 'gold'] : ['Required', 'red']], left ? b.x : b.x + b.w, b.y + b.h / 2, left ? -1 : 1); }
  }

  // ---- loading: retail's join-game door (ui\Loading\loadingscreen.dc6), its frame the boot's progress ----
  // The stages are the boot's own: the engine script arriving, the runtime up, the first frame drawn.
  function progress(t) {
    const L = loading, d2 = window.__d2;
    const cap = window.__d2Shown ? 1 : d2?.ready ? 0.88 : window.Module ? 0.58 : 0.28;
    const dt = Math.min(100, t - L.last); L.last = t;
    L.p = cap === 1 ? 1 : L.p + (cap - L.p) * (1 - Math.exp(-dt / 1800));
    if (L.p >= 1 && !L.full) L.full = t;
    if (d2?.ready && !L.readyAt) L.readyAt = t;
    // Close: the first frame plus a beat; a boot failure at once, so its message shows; never forever.
    if ((L.full && t - L.full > 700) || d2?.error || (L.readyAt && t - L.readyAt > 60000) || t - L.t0 > 240000) close();
    return L.p;
  }
  function loadingScreen(t) {
    const i = Math.min(9, Math.floor(progress(t) * 10));
    if (art) g.drawImage(art.img.loading, Math.min(i, art.meta.loading - 1) * 256, 0, 256, 256, 272, 172, 256, 256);
    else { g.fillStyle = '#3a301c'; g.fillRect(300, 296, 200, 8); g.fillStyle = '#c7b377'; g.fillRect(300, 296, 20 * (i + 1), 8); }
  }

  // ---- the window: the backdrop fills it; the 800x600 frame sits centred, scaled to fit ----
  function frame(t) {
    if (done) return;
    const dpr = devicePixelRatio || 1, W = Math.round(innerWidth * dpr), H = Math.round(innerHeight * dpr);
    if (cv.width !== W || cv.height !== H) { cv.width = W; cv.height = H; }
    const s = Math.min(W / 460, H / 600);   // narrow screens crop the sides; everything drawn sits in x 170..630
    view = { s, x: (W - 800 * s) / 2, y: (H - 600 * s) / 2 };
    g.setTransform(1, 0, 0, 1, 0, 0); g.globalCompositeOperation = 'source-over';
    g.fillStyle = '#000'; g.fillRect(0, 0, W, H);
    if (!loading && back) { const kx = s / back.s, ky = s / back.sy; g.imageSmoothingEnabled = true; g.imageSmoothingQuality = 'high';
      g.drawImage(back.img, view.x - back.ox * kx, view.y - back.oy * ky, back.w * kx, back.h * ky); }
    g.setTransform(s, 0, 0, s, view.x, view.y);
    g.imageSmoothingEnabled = Math.abs(s - Math.round(s)) > 0.01;   // crisp at whole multiples, smooth between
    if (loading) loadingScreen(t); else scene(t);
    if (!done) requestAnimationFrame(frame);
  }

  // ---- input ----
  const inside = (p, r) => p.x >= r.x && p.y >= r.y && p.x < r.x + (r.w || 272) && p.y < r.y + (r.h || 35);
  const at = (e) => { const r = cv.getBoundingClientRect(), d = cv.width / (r.width || 1);
    return { x: ((e.clientX - r.left) * d - view.x) / view.s, y: ((e.clientY - r.top) * d - view.y) / view.s }; };
  function hit(p) {
    if (loading) return null;
    for (const k of ['dir', 'files', 'enter', 'discord']) if (inside(p, BTN[k])) return k;
    if (offerAgain && !ready() && inside(p, againBox())) return 'again';
    return null;
  }
  function say(s, c = 'tan', ms = 2600) { msg = [s, c]; msgUntil = performance.now() + ms; }
  async function add(src) {
    const before = FILES.filter(have).length;
    const p = col.add(src); paint(); await p; paint();
    const got = FILES.filter(have).length - before;
    if (got > 0) say(`Found ${got} file${got > 1 ? 's' : ''}`, 'white', 1600);
    if (!art && !artTried && ['d2data', 'd2exp', 'patch_d2'].every((r) => col.state.slots[r]?.have)) {
      artTried = true;
      loadArchiveArt(col).then((a) => { art ??= a; }).catch((e) => console.warn('front door art:', e.message));
    }
  }
  function paint() { cv.style.cursor = ''; }   // the canvas repaints every frame; kept for the test hook
  async function act(h) {
    if (h === 'discord') window.open(DISCORD, '_blank', 'noopener');
    if (h === 'dir') add(await pickFolder());
    if (h === 'files') add(await pickFiles());
    if (h === 'again') { offerAgain = false; const r = await restore(true); await add({ handles: r.handles }); }
    if (h === 'enter') {
      const miss = FILES.filter((x) => !have(x));
      if (col.state.busy) say('Still reading your files…');
      else if (miss.length) say(miss.length === 1 ? `Still missing ${miss[0].f}` : `Still missing ${miss.length} files`, 'red');
      else if (!ready()) say(col.state.issues[0]?.text || 'These files cannot start the game.', 'red', 4000);
      else enter();
    }
  }
  const onMove = (e) => { const p = at(e), h = hit(p);
    hover = loading ? null : h === 'again' ? 'again' : FILES.find((x) => inside(p, entryBox(x))) || null;
    cv.style.cursor = h ? 'pointer' : ''; };
  const onUp = (e) => { const h = hit(at(e)); if (h && h === down) act(h); down = null; };
  const onOver = (e) => e.preventDefault();
  const onDrop = async (e) => { e.preventDefault(); if (!loading) add(await fromDrop(e.dataTransfer)); };
  const onKey = (e) => { if (e.key === 'Enter' && !loading && e.target === document.body) act('enter'); };
  cv.addEventListener('pointermove', onMove);
  cv.addEventListener('pointerleave', () => { hover = null; });
  cv.addEventListener('pointerdown', (e) => { down = hit(at(e)); });
  addEventListener('pointerup', onUp);
  addEventListener('dragover', onOver);
  addEventListener('drop', onDrop);
  addEventListener('keydown', onKey);

  // A returning visitor on Chromium: the browser remembered the handles; re-open them quietly if it
  // still allows, else offer one click that may prompt.
  restore(false).then(async ({ count, handles }) => {
    if (handles.length) await add({ handles });
    if (count && !col.state.ready) offerAgain = true;
  });

  let resolveDoor;
  function enter() {
    removeEventListener('pointerup', onUp); removeEventListener('dragover', onOver);
    removeEventListener('drop', onDrop); removeEventListener('keydown', onKey);
    hover = null; down = null; cv.style.cursor = '';
    const t = performance.now();
    loading = { t0: t, last: t, p: 0.1, full: 0, readyAt: 0 };
    root.classList.add('loading');
    document.body.classList.remove('door');   // the game lays out underneath while the door stays on top
    resolveDoor(col.mounts());
  }
  function close() { done = true; root.remove(); style.remove(); }

  requestAnimationFrame(frame);
  return new Promise((resolve) => {
    resolveDoor = resolve;
    window.__d2Door = { collection: col, add, paint, forget, enter: () => act('enter') };   // test hook: feed files without a picker
    if (onReady) onReady(col);
  });
}
