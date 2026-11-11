// The desktop app's front door (play/assets/byo-door.mjs, hosted): the desktop host serves the player's own archives,
// so the door reads nothing, shows every file found, and boot waits for ENTER SANCTUARY. Run in node on recording
// stubs of the DOM; no frame is ever requested, so the door's state and its promise are all that is observed.
import { test } from 'node:test';
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';

const listeners = new Map();
const node = () => {
  const e = {
    children: [], className: '', id: '', textContent: '', style: {}, width: 0, height: 0,
    classList: { add() {}, remove() {} }, setAttribute() {}, remove() {}, addEventListener() {}, removeEventListener() {},
    getBoundingClientRect: () => ({ left: 0, top: 0, width: 800, height: 600 }), getContext: () => new Proxy({}, { get: (t, k) => (k in t ? t[k] : () => {}), set: (t, k, v) => { t[k] = v; return true; } }),
    append(...c) { e.children.push(...c); }, appendChild(c) { e.children.push(c); return c; },
  };
  return e;
};
const body = node();
globalThis.window = globalThis;
globalThis.document = { body, head: node(), createElement: () => node() };
globalThis.innerWidth = 800; globalThis.innerHeight = 600; globalThis.devicePixelRatio = 1;
globalThis.requestAnimationFrame = () => 0;
globalThis.fetch = () => Promise.reject(new Error('no network in a unit test'));
globalThis.addEventListener = (type, fn) => { if (!listeners.has(type)) listeners.set(type, new Set()); listeners.get(type).add(fn); };
globalThis.removeEventListener = (type, fn) => listeners.get(type)?.delete(fn);
const dispatch = (type, ev) => { for (const fn of [...(listeners.get(type) || [])]) fn(ev); };
// The door's staged-art and backdrop timers would hold node's event loop for 10 s; they are not what is under test.
const realTimeout = globalThis.setTimeout;
globalThis.setTimeout = (fn, ms, ...a) => { const t = realTimeout(fn, ms, ...a); t.unref?.(); return t; };
const tick = () => new Promise((r) => realTimeout(r, 0));

const { frontDoor } = await import('../play/assets/byo-door.mjs');

test('a hosted door waits for ENTER SANCTUARY, then resolves; no drop target and no remembered-handle restore', async () => {
  let resolved = null;
  frontDoor({ hosted: true }).then((mounts) => { resolved = { mounts }; });
  assert.equal(listeners.has('drop'), false, 'a hosted door takes no dropped files');
  assert.equal(listeners.has('dragover'), false);
  assert.equal(listeners.get('keydown')?.size, 1, 'the one keydown listener is ENTER');
  await tick();
  assert.equal(resolved, null, 'boot does not start before ENTER');
  dispatch('keydown', { key: 'a', target: body });
  await tick();
  assert.equal(resolved, null, 'a key other than ENTER keeps the door shut');
  dispatch('keydown', { key: 'Enter', target: body });
  await tick();
  assert.deepEqual(resolved, { mounts: [] }, 'ENTER resolves with no mounts: the host serves the archives under /retail/');
  assert.deepEqual(window.__d2PlugyFiles, {}, 'no PlugY files come from the player: the host serves D2SE/ itself');
});

test('index.html: the desktop host gets the hosted door, keyed on window.d2Desktop; the byo door call is unchanged', () => {
  const html = readFileSync(new URL('../play/index.html', import.meta.url), 'utf8');
  assert.match(html, /if \(byo\) retail\.push\(\.\.\.await \(await import\('\.\/assets\/byo-door\.mjs\?v=' \+ VER\)\)\.frontDoor\(\)\);/);
  assert.match(html, /else if \(window\.d2Desktop && wantRetail && !noRetail\) await \(await import\('\.\/assets\/byo-door\.mjs\?v=' \+ VER\)\)\.frontDoor\(\{ hosted: true \}\);/);
});
