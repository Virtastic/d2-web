import test from 'node:test';
import assert from 'node:assert/strict';
import { createBnetLoginScreen } from './bnet-login-screen.mjs';
import { createBnetConnectNotice } from './bnet-connecting.mjs';

globalThis.ImageData ??= class { constructor(data, w, h) { Object.assign(this, { data, width: w, height: h }); } };
const ctx = new Proxy({}, { get: (_, k) => (k === 'getImageData' ? () => ({ data: new Uint8ClampedArray(4) }) : () => {}), set: () => true });
const documentObject = { createElement: () => ({ width: 0, height: 0, getContext: () => ctx }) };
const frame = { width: 1, height: 1, offsetX: 0, offsetY: 0, indices: new Uint8Array(1), opaque: new Uint8Array([1]) };
const tbl = new Uint8Array(12 + 95 * 14); tbl.set([0x57, 0x6f, 0x6f, 0x21]); tbl[10] = 10;
for (let c = 32; c < 127; c++) { const b = 12 + (c - 32) * 14; tbl[b] = c; tbl[b + 3] = 6; tbl[b + 4] = 10; }
const read = (path) => (path.endsWith('.tbl') ? tbl : new Uint8Array(1 << 22));
const decodeDc6 = () => ({ frames: [frame, frame, frame, frame, frame, frame] });
const decodeDatPalette = () => new Uint8Array(1024);
const string = (i) => `s${i}`;
const tick = () => new Promise((r) => setImmediate(r));
const transport = (message) => Object.assign(new Error(message), { transport: true });
const screen = (hooks = {}) => createBnetLoginScreen({ read, decodeDc6, decodeDatPalette, string, documentObject, now: () => 0,
  lastAccount: 'Ama', ...hooks });
const type = (s, text) => { for (const k of text) s.typeKey(`Key${k}`, k); };

test('CREATE NEW ACCOUNT connects for the terms first and opens the create form only on AGREE', async () => {
  let asked = 0;
  const s = screen({ onTermsOfService: async () => { asked++; return ''; } });
  s.activate('create');
  assert.equal(s.busy, true);
  await tick();
  assert.equal(asked, 1);
  assert.equal(s.mode, 'terms');
  assert.deepEqual(s.evidence.boxes.map((b) => b.id), ['terms-agree', 'terms-cancel']);
  s.activate('terms-agree');
  assert.equal(s.mode, 'create');
});

test('the terms screen keeps the typed fields for the create form that AGREE opens', async () => {
  const s = screen({ onTermsOfService: async () => '', lastAccount: '' });
  type(s, 'ama'); s.typeKey('Tab', 'Tab'); type(s, 'pw');
  s.activate('create'); await tick();
  s.activate('terms-agree');
  assert.equal(s.mode, 'create');
  assert.equal(s.evidence.account, 'ama');
  assert.equal(s.evidence.passwordLength, 2);
});

test('CANCEL on the terms returns to the login screen with no form', async () => {
  const s = screen({ onTermsOfService: async () => 'terms text' });
  s.activate('create'); await tick();
  assert.equal(s.mode, 'terms');
  s.activate('terms-cancel');
  assert.equal(s.mode, 'login');
});

test('Escape on the terms returns to the login screen', async () => {
  const s = screen({ onTermsOfService: async () => '' });
  s.activate('create'); await tick();
  s.typeKey('Escape', 'Escape');
  assert.equal(s.mode, 'login');
});

test('without a realm to ask, CREATE NEW ACCOUNT opens the create form directly', () => {
  const s = screen();
  s.activate('create');
  assert.equal(s.mode, 'create');
});

test('a terms request that never reaches the realm raises the disconnect notice (5347), OK leaves the menu', async () => {
  let left = 0;
  const s = screen({ onTermsOfService: async () => { throw transport('not open'); }, onExit: () => { left++; } });
  s.activate('create'); await tick();
  assert.equal(s.mode, 'login');
  assert.equal(s.evidence.message, null);
  assert.equal(s.evidence.notice.body, 's5347');
  s.activate('notice-ok');
  assert.equal(left, 1);
  assert.equal(s.evidence.notice, null);
});

test('a transport failure at LOG IN raises notice 5189 rather than the error text', async () => {
  let left = 0;
  const s = screen({ onAuthenticate: async () => { throw transport('socket closed'); }, onExit: () => { left++; } });
  type(s, 'ama'); s.typeKey('Tab', 'Tab'); type(s, 'pw');
  s.activate('login'); await tick();
  assert.equal(s.evidence.message, null);
  assert.equal(s.evidence.notice.body, 's5189');
  s.typeKey('Enter', 'Enter');
  assert.equal(left, 1);
});

test('a logon refusal still raises the login failure box (5244), not the notice', async () => {
  const s = screen({ onAuthenticate: async () => { throw Object.assign(new Error('x'), { logonResult: 1 }); } });
  type(s, 'ama'); s.typeKey('Tab', 'Tab'); type(s, 'pw');
  s.activate('login'); await tick();
  assert.equal(s.evidence.notice, null);
  assert.equal(s.evidence.message.title, 's5244');
  assert.equal(s.evidence.message.body, 's5208');
});

test('a non-transport error keeps the login failure box with the status-0x12 box (5248 / 5347), not its English text', async () => {
  const s = screen({ onAuthenticate: async () => { throw new Error('bad state'); } });
  type(s, 'ama'); s.typeKey('Tab', 'Tab'); type(s, 'pw');
  s.activate('login'); await tick();
  assert.equal(s.evidence.notice, null);
  assert.equal(s.evidence.message.body, 's5347');
  assert.equal(s.evidence.message.title, 's5248');
});

test('the connect failure notice draws its frame and OK, and its OK runs the callback', () => {
  let ok = 0;
  const n = createBnetConnectNotice({ read, decodeDc6, decodeDatPalette, string, text: 's5189', onOk: () => { ok++; }, documentObject });
  n.draw(ctx);
  assert.equal(n.evidence.text, 's5189');
  assert.equal(n.press(0, 0), null);
  assert.equal(n.press(360, 320), 'ok');
  assert.equal(n.release('ok', 360, 320), true);
  assert.equal(ok, 1);
  assert.equal(n.key('Escape'), true);
  assert.equal(ok, 2);
});

test('each logon and create refusal shows the failure table box', async () => {
  const { loginErrorMessage, createRefusalMessage } = await import('./bnet-login-screen.mjs');
  assert.deepEqual(loginErrorMessage({ logonResult: 3 }, 'a', string), { title: 's5244', body: 's5209' });
  assert.deepEqual(loginErrorMessage({ logonResult: 6, logonReason: 'banned' }, 'a', string), { title: 's5244', body: 'banned' });
  assert.deepEqual(loginErrorMessage({ authResult: 0x200 }, 'a', string), { title: 's5247', body: 's5186', big: true, detail: 's5194' });
  for (const [r, id] of [[1, 5231], [2, 5232], [3, 5233], [4, 5239], [5, 5234], [6, 5235], [7, 5236], [8, 5237], [9, 5231]])
    assert.deepEqual(createRefusalMessage(r, string), { title: 's5249', body: `s${id}` });
});
