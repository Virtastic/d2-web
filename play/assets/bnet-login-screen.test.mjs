import test from 'node:test';
import assert from 'node:assert/strict';
import { createBnetLoginScreen, BNET_LOGIN_LAYOUT as L, changeEmailError, changePasswordError } from './bnet-login-screen.mjs';
import { bncsChangePassword, bncsResetPassword, bncsChangeEmail } from './realm-client.mjs';

globalThis.ImageData ??= class { constructor(data, w, h) { Object.assign(this, { data, width: w, height: h }); } };
const ctx = new Proxy({}, { get: (_, k) => (k === 'getImageData' ? () => ({ data: new Uint8ClampedArray(4) }) : () => {}), set: () => true });
const documentObject = { createElement: () => ({ width: 0, height: 0, getContext: () => ctx }) };
const frame = { width: 1, height: 1, offsetX: 0, offsetY: 0, indices: new Uint8Array(1), opaque: new Uint8Array([1]) };
const tbl = new Uint8Array(12 + 95 * 14); tbl.set([0x57, 0x6f, 0x6f, 0x21]); tbl[10] = 10;
for (let c = 32; c < 127; c++) { const b = 12 + (c - 32) * 14; tbl[b] = c; tbl[b + 3] = 6; tbl[b + 4] = 10; }
const screen = (hooks = {}) => createBnetLoginScreen({ read: (path) => (path.endsWith('.tbl') ? tbl : new Uint8Array(1 << 22)),
  decodeDc6: () => ({ frames: [frame, frame, frame, frame, frame, frame] }), decodeDatPalette: () => new Uint8Array(1024),
  string: (i) => `s${i}`, documentObject, now: () => 0, lastAccount: 'Ama', ...hooks });
const tick = () => new Promise((r) => setImmediate(r));
const type = (s, text) => { for (const k of text) s.typeKey(`Key${k}`, k); };
const rect = (s, id) => s.evidence.boxes.find((b) => b.id === id);

test('ACCOUNT SETTINGS opens the three buttons and CANCEL; each button opens its form', () => {
  const s = screen();
  assert.ok(s.activate('settings'));
  assert.equal(s.mode, 'settings');
  assert.deepEqual(s.evidence.boxes.map((b) => b.id), ['change-password', 'get-password', 'change-email', 'settings-cancel']);
  assert.deepEqual(rect(s, 'settings-cancel'), { id: 'settings-cancel', x: 33, y: 538, w: 128, h: 35 });
  for (const [id, mode] of [['change-password', 'cpass'], ['get-password', 'gpass'], ['change-email', 'cemail']]) {
    s.activate(id); assert.equal(s.mode, mode);
    s.escape(); assert.equal(s.mode, 'settings');
  }
  s.activate('settings-cancel'); assert.equal(s.mode, 'login');
});

test('form hit rects follow the descriptors, bottom-left anchored', () => {
  const s = screen();
  s.activate('settings'); s.activate('change-password');
  assert.deepEqual(s.evidence.boxes.filter((b) => b.id.startsWith('field-')).map((b) => [b.id, b.x, b.y, b.w, b.h]),
    [['field-account', 319, 322, 169, 26], ['field-old', 319, 376, 169, 26], ['field-password', 319, 430, 169, 26], ['field-verify', 319, 484, 169, 26]]);
  assert.deepEqual(['cpass-ok', 'cpass-exit'].map((id) => rect(s, id).x), [627, 33]);
  s.escape(); s.activate('get-password');
  assert.deepEqual(s.evidence.boxes.filter((b) => b.id.startsWith('field-')).map((b) => [b.id, b.x, b.y, b.w]), [['field-account', 248, 402, 300], ['field-email', 248, 452, 300]]);
  s.escape(); s.activate('change-email');
  assert.equal(s.evidence.boxes.filter((b) => b.id.startsWith('field-')).length, 4);
  assert.deepEqual(['cemail-ok', 'cemail-cancel'].map((id) => rect(s, id).x), [627, 33]);
});

test('the typed account carries in; CHANGE PASSWORD focuses the current password, the others the account', () => {
  const s = screen();
  s.activate('settings'); s.activate('change-password');
  assert.equal(s.evidence.form.account, 'Ama'); assert.equal(s.focus, 'old');
  s.escape(); s.activate('get-password'); assert.equal(s.focus, 'account');
});

test('every form draws without throwing', () => {
  const s = screen(); const out = ctx;
  for (const id of ['change-password', 'get-password', 'change-email']) { s.activate('settings'); s.activate(id); type(s, 'abc'); s.draw(out, 800, 600); s.escape(); s.escape(); }
});

test('validation order: change password and change email', () => {
  assert.notEqual(changePasswordError('A', 'xx', 'xx'), null);
  assert.notEqual(changePasswordError('Ama', 'abc', 'abd'), null);
  assert.equal(changePasswordError('Ama', 'abc', 'ABC'), null);
  assert.equal(changeEmailError('a@b', 'c@d', 'x@y'), 11122);
  assert.equal(changeEmailError('a@b', 'a@b', 'a@b'), 11121);
  assert.equal(changeEmailError('a@b', 'c@d', 'c@d'), null);
});

test('CHANGE PASSWORD: a rule failure raises the notice whose OK rebuilds the form; success logs on with the new password', async () => {
  const calls = [], logons = [];
  const s = screen({ onChangePassword: async (r) => { calls.push(r); return 0; }, onAuthenticate: async (r) => { logons.push(r); } });
  s.activate('settings'); s.activate('change-password');
  type(s, 'old1'); s.typeKey('Tab'); type(s, 'new1'); s.typeKey('Tab'); type(s, 'new2');
  s.activate('cpass-ok'); await tick();
  assert.ok(s.evidence.notice); assert.equal(calls.length, 0);
  s.activate('notice-ok'); assert.equal(s.mode, 'cpass'); assert.equal(s.evidence.form.account, 'Ama');
  type(s, 'old1'); s.typeKey('Tab'); type(s, 'new1'); s.typeKey('Tab'); type(s, 'new1');
  s.typeKey('Enter'); await tick(); await tick();
  assert.deepEqual(calls, [{ account: 'Ama', password: 'old1', newPassword: 'new1' }]);
  assert.deepEqual(logons, [{ account: 'Ama', password: 'new1' }]);
});

test('CHANGE PASSWORD refused by the realm reopens the form; a dead socket shows the disconnect notice', async () => {
  let result = 1;
  const s = screen({ onChangePassword: async () => { if (result === 'x') throw new Error('down'); return result; } });
  const fill = () => { s.activate('settings'); s.activate('change-password'); type(s, 'o1'); s.typeKey('Tab'); type(s, 'n1'); s.typeKey('Tab'); type(s, 'n1'); s.activate('cpass-ok'); };
  fill(); await tick(); await tick();
  assert.ok(s.evidence.message); s.activate('ok'); assert.equal(s.mode, 'cpass');
  s.escape(); s.escape(); s.reset(); result = 'x'; fill(); await tick(); await tick();
  assert.equal(s.evidence.notice.body, 's5347'); s.activate('notice-ok'); assert.equal(s.mode, 'login');
});

test('GET NEW PASSWORD sends account and email, shows the receipt, OK returns to login', async () => {
  const calls = [];
  const s = screen({ onResetPassword: async (r) => { calls.push(r); } });
  s.activate('settings'); s.activate('get-password');
  s.typeKey('Backspace'); type(s, 'Z'); s.typeKey('Tab'); type(s, 'a@b.c');
  s.activate('gpass-ok'); await tick(); await tick();
  assert.deepEqual(calls, [{ account: 'AmZ', email: 'a@b.c' }]);
  assert.deepEqual(s.evidence.message, { title: '', body: 's11104', then: s.evidence.message.then });
  s.activate('ok'); assert.equal(s.mode, 'login');
});

test('CHANGE EMAIL: mismatch and same-address popups reopen the form; a valid SEND returns to login', async () => {
  const calls = [];
  const s = screen({ onChangeEmail: async (r) => { calls.push(r); } });
  s.activate('settings'); s.activate('change-email');
  s.typeKey('Tab'); type(s, 'a@b'); s.typeKey('Tab'); type(s, 'c@d'); s.typeKey('Tab'); type(s, 'x@y');
  s.activate('cemail-ok'); await tick();
  assert.equal(s.evidence.message.body, 's11122'); s.activate('ok'); assert.equal(s.mode, 'cemail'); assert.equal(calls.length, 0);
  s.typeKey('Tab'); type(s, 'a@b'); s.typeKey('Tab'); type(s, 'a@b'); s.typeKey('Tab'); type(s, 'a@b');
  s.activate('cemail-ok'); await tick();
  assert.equal(s.evidence.message.body, 's11121'); s.activate('ok');
  s.typeKey('Tab'); type(s, 'a@b'); s.typeKey('Tab'); type(s, 'c@d'); s.typeKey('Tab'); type(s, 'c@d');
  s.activate('cemail-ok'); await tick(); await tick();
  assert.deepEqual(calls, [{ account: 'Ama', oldEmail: 'a@b', newEmail: 'c@d' }]);
  assert.equal(s.mode, 'login');
});

// A transport that answers each frame the client writes with the next queued reply, and the protocol's framing
// reduced to a four byte header (0xff, id, length) so the bodies can be compared exactly.
const run = async (request, replies) => {
  const frames = [], cbs = [], queue = [...replies];
  const transport = { send: (b) => { const bytes = Uint8Array.from(b); if (bytes[0] === 0xff) { frames.push([bytes[1], [...bytes.subarray(4)]]); const r = queue.shift(); if (r) setImmediate(() => cbs.forEach((cb) => cb(r))); } return true; },
    onData: (cb) => cbs.push(cb), onClose: () => {}, close() {}, isOpen: true };
  const protocol = { bncsSelector: () => [1], tickCount: () => 7, bncsAuthInfo: () => new Uint8Array(0),
    bncsFrame: (id, body) => [0xff, id, (body.length + 4) & 0xff, (body.length + 4) >> 8, ...body],
    bncsLogon: () => Uint8Array.from({ length: 32 }, (_, i) => (i < 28 ? i : 0x41 + (i - 28))) };
  const result = await request(transport, protocol);
  return { frames, result };
};
const reply = (id, body) => Uint8Array.from([0xff, id, (body.length + 4) & 0xff, (body.length + 4) >> 8, ...body]);
const authInfo = reply(0x50, [0, 0, 0, 0, 9, 0, 0, 0, ...new Array(12).fill(0), 0]);
const text = (v) => [...new TextEncoder().encode(v), 0];

test('request bytes: RESETPASSWORD and CHANGEEMAIL are C strings; CHANGEPASSWORD splices the new hash before the name', async () => {
  const reset = await run((t, p) => bncsResetPassword(t, p, { account: 'Ama', email: 'a@b.c' }), [authInfo]);
  assert.deepEqual(reset.frames.map((f) => f[0]), [0x50, 0x5a]);
  assert.deepEqual(reset.frames[1][1], [...text('Ama'), ...text('a@b.c')]);
  const email = await run((t, p) => bncsChangeEmail(t, p, { account: 'Ama', oldEmail: 'a@b', newEmail: 'c@d' }), [authInfo]);
  assert.deepEqual(email.frames.map((f) => f[0]), [0x50, 0x5b]);
  assert.deepEqual(email.frames[1][1], [...text('Ama'), ...text('a@b'), ...text('c@d')]);
  const change = await run((t, p) => bncsChangePassword(t, p, { account: 'Ama', password: 'old', newPassword: 'New' }), [authInfo, reply(0x31, [0, 0, 0, 0])]);
  assert.equal(change.result, 0);
  const body = change.frames[1][1];
  assert.equal(change.frames[1][0], 0x31);
  assert.deepEqual(body.slice(0, 28), Array.from({ length: 28 }, (_, i) => i));
  assert.equal(body.length, 28 + 20 + 4);
  assert.deepEqual(body.slice(48), [0x41, 0x42, 0x43, 0x44]);
  const denied = await run((t, p) => bncsChangePassword(t, p, { account: 'Ama', password: 'old', newPassword: 'x' }), [authInfo, reply(0x31, [1, 0, 0, 0])]);
  assert.equal(denied.result, 1);
});
