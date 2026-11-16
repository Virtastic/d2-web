// Instrument for the platform overlays' lettering (the controller's key grids, settings panel and skill wheel, the
// touch name box, the error notices): every one of them draws its text from retail's Font16 cels, laid out the way
// retail lays a line out, and none names a CSS font family.
//
//  1. The layout rule against retail: every Font16 line in retail's own draw lists (vectors/drawlist, private) is
//     re-laid by the overlay painter and compared glyph for glyph -- the font cel and the pen step from the line's
//     first glyph. Fails unless the count is 0; a font table with one advance changed must make it differ.
//  2. Each overlay site, driven through its own module with a stand-in DOM, paints exactly the glyphs that rule
//     gives for its text (the expectation is computed here from the raw font table, not by the painter).
//  3. No overlay source names a font family any more.
// Without the retail data the first two skip.
import test from 'node:test';
import assert from 'node:assert/strict';
import { readFileSync, existsSync, readdirSync } from 'node:fs';
const W = new URL('..', import.meta.url).pathname.replace(/\/$/, '');
globalThis.ImageData = class ImageData { constructor(data, width, height) { Object.assign(this, { data, width, height }); } };

const FONT = 'data\\local\\font\\latin\\font16';
const have = existsSync(`${W}/retail/D2/d2data.mpq`) && existsSync(`${W}/vectors/drawlist/a1-kashya-menu-noquest.txt`);

function fakeElement() {
  // As in a page: setting the text drops the children, and an element holding one canvas reads as its fallback text.
  let text = '';
  return { style: {}, children: [], appendChild(c) { this.children.push(c); return c; }, replaceChildren(...c) { this.children = c; }, setAttribute() {},
    get textContent() { return this.children.length ? this.children.map((c) => c.textContent).join('') : text; },
    set textContent(v) { text = String(v); this.children = []; } };
}
function fakeDocument() {
  return { createElement: (tag) => (tag === 'canvas'
    ? { ...fakeElement(), width: 1, height: 1, draws: [], getContext() { const c = this; return { drawImage: (img, x, y) => c.draws.push({ img, x, y }), putImageData() {} }; } }
    : fakeElement()) };
}

async function retailFont(mutate = (tbl) => tbl) {
  const { createFont, decodeFontTbl } = await import(`${W}/play/assets/frontend.mjs`);
  const { decodeDc6, decodeDatPalette } = await import(`${W}/play/assets/d2-formats.mjs`);
  const { openMpq } = await import(`${W}/tools/mpq-node.mjs`);
  const mpqs = ['patch_d2.mpq', 'd2exp.mpq', 'd2data.mpq', 'd2char.mpq'].map((n) => openMpq(`${W}/retail/D2/${n}`));
  const read = (p) => { for (const m of mpqs) { const b = m.read(p); if (b) return new Uint8Array(b); } throw new Error(`missing ${p}`); };
  const tbl = mutate(decodeFontTbl(read(`${FONT}.tbl`)));
  const font = createFont({ dc6: decodeDc6(read(`${FONT}.dc6`)), tbl, palette: decodeDatPalette(read('data\\global\\palette\\Units\\pal.dat')),
    documentObject: fakeDocument(), path: `${FONT}.dc6` });
  return { font, tbl };
}

// The rule, from the raw table: glyph i is the cel its code point maps to, at the sum of the advances before it.
function expected(tbl, text, lineHeight) {
  const out = [];
  String(text).split('\n').forEach((line, row) => {
    let pen = 0;
    for (const ch of line) {
      const m = tbl.chars.get(ch.codePointAt(0));
      if (!m) continue;
      out.push({ index: m.imageIndex, x: pen, y: row * lineHeight });   // the space is a cel too, as in retail's lists
      pen += m.width;
    }
  });
  return out;
}

// Retail's Font16 lines: a run of glyph records on one baseline, left to right. A gap wider than two cells is
// another piece of text; inside a run, a gap is spaces.
function retailLines(tbl) {
  const byFrame = new Map([...tbl.chars].map(([code, m]) => [m.imageIndex, code]).reverse());
  const space = tbl.chars.get(32).width, lines = [];
  for (const name of readdirSync(`${W}/vectors/drawlist`).filter((n) => n.endsWith('.txt'))) {
    let run = null;
    for (const row of readFileSync(`${W}/vectors/drawlist/${name}`, 'utf8').split('\n')) {
      const m = /^celcolor \S*Font16\.dc6 frame=(\d+) dir=0 x=(-?\d+) y=(-?\d+) /i.exec(row);
      if (!m) { run = null; continue; }
      const g = { frame: +m[1], x: +m[2], y: +m[3] };
      const last = run?.glyphs.at(-1);
      if (!last || g.y !== last.y || g.x <= last.x || g.x - last.x > 28) { run = { name, glyphs: [], text: '' }; lines.push(run); }
      else {
        const gap = g.x - last.x - tbl.chars.get(byFrame.get(last.frame)).width;
        if (gap > 0 && gap % space === 0) run.text += ' '.repeat(gap / space);
      }
      run.text += String.fromCodePoint(byFrame.get(g.frame));
      run.glyphs.push(g);
    }
  }
  const seen = new Set();
  return lines.filter((l) => l.glyphs.length > 1 && !seen.has(l.text) && seen.add(l.text));
}

async function retailDiffs(mutate) {
  const { paintOverlayLines } = await import(`${W}/play/assets/overlay-text.mjs`);
  const { font, tbl } = await retailFont();
  const lines = retailLines(tbl);
  const painter = mutate ? (await retailFont(mutate)).font : font;
  let diffs = 0, glyphs = 0;
  for (const line of lines) {
    const ours = paintOverlayLines({ drawImage() {} }, painter, [line.text]);
    const n = Math.max(ours.length, line.glyphs.length);
    glyphs += line.glyphs.length;
    for (let i = 0; i < n; i++) {
      const r = line.glyphs[i], o = ours[i];
      const same = r && o && o.index === r.frame && o.x - ours[0].x === r.x - line.glyphs[0].x;
      if (!same) { diffs++; if (!mutate && diffs <= 10) console.log(`  differing: ${line.name} ${JSON.stringify(line.text)} glyph ${i}`, r, o); }
    }
  }
  return { diffs, glyphs, lines: lines.length };
}

test('the overlay painter lays a Font16 line out as retail does (every Font16 line in the retail draw lists)', { skip: !have }, async () => {
  const r = await retailDiffs();
  console.log(`overlay-text vs retail Font16 lines: ${r.lines} distinct lines, ${r.glyphs} glyphs, ${r.diffs} differ`);
  assert.ok(r.lines >= 20 && r.glyphs >= 200, 'the vectors hold retail Font16 text');
  const wrong = await retailDiffs((tbl) => { tbl.chars.get(97).width += 1; return tbl; });
  assert.ok(wrong.diffs > 0, 'the comparison cannot fail');
  assert.equal(r.diffs, 0);
});

test('every overlay site paints its text from the retail font cels', { skip: !have }, async () => {
  const { setOverlayFont, setOverlayText, flushOverlayText } = await import(`${W}/play/assets/overlay-text.mjs`);
  const { font, tbl } = await retailFont();
  const doc = fakeDocument();
  globalThis.document = doc;
  const sites = [];   // { name, el, text }
  const viewport = fakeElement();

  // Before the font exists an element keeps its plain text and waits.
  setOverlayFont(() => null);
  const early = doc.createElement('div');
  assert.equal(setOverlayText(early, 'OK'), false);
  assert.equal(early.textContent, 'OK');
  assert.equal(early.children.length, 0);
  assert.equal(flushOverlayText(), 1);
  setOverlayFont(() => font);   // the font arriving paints what waited
  assert.equal(flushOverlayText(), 0);
  sites.push({ name: 'a label set before the font was mounted', el: early, text: 'OK' });

  // The controller's key grids: one label per cell, and the hint line under them.
  const { CHAT_ROWS, hintLine, padLayout } = await import(`${W}/play/assets/pad-menus.mjs`);
  const NAME_ROWS = ['ABCDEFG', 'HIJKLMN', 'OPQRSTU', 'VWXYZ-_', ['DEL', 'OK']], GOLD_ROWS = ['123', '456', '789', ['DEL', '0', 'OK']];
  const controllerSource = readFileSync(`${W}/play/assets/input-controller.mjs`, 'utf8');
  assert.ok(controllerSource.includes(`const NAME_ROWS = ['ABCDEFG', 'HIJKLMN', 'OPQRSTU', 'VWXYZ-_', ['DEL', 'OK']];`) && controllerSource.includes(`const GOLD_ROWS = ['123', '456', '789', ['DEL', '0', 'OK']];`), 'the grids under test are the controller\'s');
  for (const [grid, rows] of [['name', NAME_ROWS], ['gold', GOLD_ROWS], ['chat', CHAT_ROWS]]) {
    for (const row of rows) for (const label of Array.from(row)) { const el = doc.createElement('div'); setOverlayText(el, label); sites.push({ name: `${grid} grid ${JSON.stringify(label)}`, el, text: label }); }
  }
  for (const layout of ['xbox', 'playstation', 'nintendo']) {
    let text; try { text = hintLine(padLayout?.(layout) ?? layout, [[0, 'Select'], [1, 'Delete'], [3, 'Hide']]); } catch { continue; }
    const el = doc.createElement('div'); setOverlayText(el, text); sites.push({ name: `grid hint (${layout})`, el, text });
  }

  // The settings panel and the skill overlay, through their own modules.
  const { createSettingsPanel, createSettings, ROWS } = await import(`${W}/play/assets/pad-settings.mjs`);
  const store = new Map(), storage = { getItem: (k) => store.get(k) ?? null, setItem: (k, v) => store.set(k, v) };
  const panelHost = fakeElement();
  const panel = createSettingsPanel(panelHost, createSettings(storage));
  panel.toggle(true);
  assert.equal(panelHost.children.length, 1);
  assert.ok(panelHost.children[0].textContent.startsWith('Controller settings\n') && ROWS.every((r) => panelHost.children[0].textContent.includes(r.label)));
  sites.push({ name: 'controller settings panel', el: panelHost.children[0], text: panelHost.children[0].textContent });

  const { createSkillOverlay } = await import(`${W}/play/assets/pad-skill-overlay.mjs`);
  const skillHost = fakeElement();
  const overlay = createSkillOverlay(skillHost);
  const [hint, wheel, strip] = skillHost.children;
  overlay.render({ ui: { hint: 'Attack', at: 0, actions: false, strip: 0, feedback: '' }, uiMode: 'none' }, [], () => '');
  sites.push({ name: 'skill hint', el: { children: [...hint.children], __overlayText: hint.__overlayText }, text: 'Attack' });   // as painted now: the wheel clears it next
  overlay.render({ ui: { hint: '', at: 1, actions: false, strip: 0, feedback: 'set' }, uiMode: 'wheel' }, [36, 37, 38], (id) => ({ 36: 'Fire Bolt', 37: 'Warmth', 38: 'Charged Bolt' })[id]);
  assert.equal(wheel.children.length, 4);
  wheel.children.forEach((el, i) => sites.push({ name: `skill wheel ${i}`, el, text: el.textContent }));
  assert.deepEqual(wheel.children.map((c) => c.textContent), ['Skills - set', 'Fire Bolt', 'Warmth', 'Charged Bolt']);
  overlay.render({ ui: { hint: '', at: 0, actions: false, strip: 1, feedback: '' }, uiMode: 'strip' }, [], () => '');
  assert.ok(strip.textContent.includes('['));
  sites.push({ name: 'panel strip', el: strip, text: strip.textContent });

  // The error notices.
  const { setFrontdoorError, savedCharacterReadError } = await import(`${W}/play/assets/frontdoor-error.mjs`);
  const alert = { ...doc.createElement('pre'), hidden: true };
  setFrontdoorError(alert, savedCharacterReadError(new Error('IndexedDB read failed')));
  assert.equal(alert.hidden, false);
  sites.push({ name: 'front-door error', el: alert, text: 'Character select: Unable to read saved characters: IndexedDB read failed' });
  const notice = doc.createElement('div');
  const long = 'Widescreen HUD art missing (Error: missing data\\global\\ui\\panel\\wide.dc6 in every mounted archive, tried four); the wide bar and panel filler are not drawn.';
  setOverlayText(notice, long, { wrap: 780 });
  const { wrapOverlayText } = await import(`${W}/play/assets/overlay-text.mjs`);
  const wrapped = wrapOverlayText(font, long, 780);
  assert.ok(wrapped.length > 1 && wrapped.every((l) => font.measure(l) <= 780) && wrapped.join(' ') === long, 'a long notice wraps at spaces inside its width');
  sites.push({ name: 'missing-art notice', el: notice, text: wrapped.join('\n') });

  let diffs = 0, glyphs = 0;
  for (const s of sites) {
    const canvas = s.el.children[0], want = expected(tbl, s.text, tbl.header.height);
    const got = (canvas?.draws ?? []).map((d) => ({ index: [...tbl.chars.values()].find((m) => font.glyph([...tbl.chars].find(([, v]) => v === m)[0])?.canvas === d.img)?.imageIndex, x: d.x, y: d.y }));
    glyphs += want.length;
    let bad = Math.abs(got.length - want.length);
    for (let i = 0; i < Math.min(got.length, want.length); i++) if (got[i].index !== want[i].index || got[i].x !== want[i].x || got[i].y !== want[i].y) bad++;
    if (!want.length || s.el.children.length !== 1 || canvas.textContent !== s.el.__overlayText) bad++;
    if (bad) console.log(`  differing: ${s.name}: ${bad}`);
    diffs += bad;
  }
  console.log(`overlay sites in the retail font: ${sites.length} sites, ${glyphs} glyphs, ${diffs} differ`);
  assert.ok(sites.length >= 100 && glyphs >= 400);
  assert.equal(diffs, 0);
  delete globalThis.document;
});

test('no overlay names a CSS font family', () => {
  const code = (f) => readFileSync(`${W}/play/assets/${f}.mjs`, 'utf8').replace(/^\s*\/\/.*$/gm, '');
  let named = 0;
  for (const f of ['input-touch', 'input-controller', 'pad-settings', 'pad-skill-overlay', 'frontdoor-error', 'overlay-text']) {
    const hits = code(f).match(/fontFamily|font-family|\bfont:\s|OVERLAY_FONT|monospace|serif|Georgia|Palatino/g) || [];
    if (hits.length) console.log(`  ${f}: ${hits.join(', ')}`);
    named += hits.length;
  }
  for (const f of ['input-controller', 'pad-settings', 'pad-skill-overlay', 'frontdoor-error']) {
    const raw = (code(f).match(/\.textContent\s*=[^=]/g) || []).length;
    if (raw) console.log(`  ${f}: ${raw} text set past the painter`);
    named += raw;
  }
  const page = readFileSync(`${W}/play/index.html`, 'utf8'), shell = readFileSync(`${W}/play/gameplay-shell.mjs`, 'utf8');
  const rule = /#frontdoor-error\{[^}]*\}/.exec(page)[0];
  if (/font:|font-family/.test(rule)) { console.log('  #frontdoor-error names a font'); named++; }
  const freeres = shell.split('\n').filter((l) => /freeres-notice/.test(l) || /Widescreen HUD art missing/.test(l) || /border-bottom:1px solid #493d27/.test(l)).join('\n');
  assert.ok(/setOverlayText\(d, 'Widescreen HUD art missing/.test(freeres), 'the missing-art notice goes through the painter');
  if (/font:|font-family|sans-serif/.test(freeres)) { console.log('  #freeres-notice names a font'); named++; }
  // The touch name box shows nothing of its own: the name under it is the one the screen draws.
  assert.match(code('input-touch'), /#d2-touch-name\{[^}]*'\s*\+\s*'font-size:16px;[^}]*background:transparent;color:transparent;/s);
  console.log(`overlay sources naming a font or setting raw text: ${named}`);
  assert.equal(named, 0);
});
