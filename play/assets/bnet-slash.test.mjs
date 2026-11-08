import test from 'node:test';
import assert from 'node:assert/strict';
import { localSlash, NOBODY_TO_REPLY, NOTIFY_ON, NOTIFY_OFF } from './bnet-slash.mjs';
import { createBnetLobbyScreen } from './bnet-lobby-screen.mjs';

// D2Multi B090 -> ADC0 (/r, /reply) and AFA0 (/d2notify): the words match case-insensitively and end at a space.
test('/r with no whisper yet shows Nobody to reply to! and sends nothing', () => {
  assert.deepEqual(localSlash('/r hi', { replyTo: '', notify: false }), { handled: true, notice: NOBODY_TO_REPLY, colour: 1 });
  assert.deepEqual(localSlash('/REPLY', { replyTo: '', notify: false }), { handled: true, notice: NOBODY_TO_REPLY, colour: 1 });
});

test('/r and /reply send the raw rest of the line as a whisper to the last whisperer', () => {
  const st = { replyTo: 'Bob', notify: false };
  assert.deepEqual(localSlash('/r  see you  there', st), { handled: true, send: '/w *Bob see you  there' });
  assert.deepEqual(localSlash('/Reply hi', st), { handled: true, send: '/w *Bob hi' });
  assert.deepEqual(localSlash('/r', st), { handled: true, notice: 'Correct usage is: /r (message)', colour: 1 });
  assert.deepEqual(localSlash('/R   ', st), { handled: true, notice: 'Correct usage is: /r (message)', colour: 1 });
  assert.deepEqual(localSlash('/REPLY', st), { handled: true, notice: 'Correct usage is: /reply (message)', colour: 1 });
});

test('a word that only starts with a command is not local; other slash lines go to the server', () => {
  const st = { replyTo: 'Bob', notify: false };
  assert.equal(localSlash('/rx hi', st), null);
  assert.equal(localSlash('/replyall hi', st), null);
  assert.equal(localSlash('/me waves', st), null);
  assert.equal(localSlash('hello /r', st), null);
  assert.equal(st.notify, false);
});

test('/d2notify toggles the flag and says which way it went', () => {
  const st = { replyTo: '', notify: false };
  assert.deepEqual(localSlash('/d2notify', st), { handled: true, notice: NOTIFY_ON, colour: 3 });
  assert.equal(st.notify, true);
  assert.deepEqual(localSlash('/D2NOTIFY now', st), { handled: true, notice: NOTIFY_OFF, colour: 3 });
  assert.equal(st.notify, false);
});

const ctx = new Proxy({}, { get: (_, k) => (k === 'getImageData' ? () => ({ data: new Uint8ClampedArray(4) }) : () => {}), set: () => true });
const tbl = new Uint8Array(12 + 95 * 14); tbl.set([0x57, 0x6f, 0x6f, 0x21]); tbl[10] = 10;
for (let c = 32; c < 127; c++) { const b = 12 + (c - 32) * 14; tbl[b] = c; tbl[b + 3] = 6; tbl[b + 4] = 10; }
globalThis.ImageData ??= class { constructor(data, w, h) { Object.assign(this, { data, width: w, height: h }); } };
const frame = { width: 1, height: 1, offsetX: 0, offsetY: 0, indices: new Uint8Array(1), opaque: new Uint8Array([1]) };
const screenWith = (sent) => createBnetLobbyScreen({
  read: (p) => (p.endsWith('.tbl') ? tbl : new Uint8Array(1 << 22)), decodeDc6: () => ({ frames: [frame, frame, frame, frame, frame, frame] }),
  decodeDatPalette: () => new Uint8Array(1024), string: () => '', character: { name: 'Ama', charClass: 0, level: 3 },
  documentObject: { createElement: () => ({ width: 0, height: 0, getContext: () => ctx }) }, now: () => 0,
  onSendChat: (l) => { sent.push(l); return true; } });
const type = (s, text) => { for (const k of text) s.typeKey(k === ' ' ? 'Space' : `Key${k}`, k); };

test('screen: join and leave lines wait for /d2notify; a whisper sets the reply target', () => {
  const sent = [];
  const s = screenWith(sent);
  s.draw(ctx, 800, 600);
  s.typeKey('Enter'); s.chatLive(true);
  s.chatEvent({ event: 0x02, username: 'Bob', text: '', flags: 0 });
  s.chatEvent({ event: 0x03, username: 'Bob', text: '', flags: 0 });
  assert.deepEqual(s.evidence.chat, []);
  type(s, '/r'); s.typeKey('Enter');
  assert.equal(s.evidence.chat.at(-1), NOBODY_TO_REPLY);
  type(s, '/d2notify'); s.typeKey('Enter');
  assert.equal(s.evidence.chat.at(-1), NOTIFY_ON);
  s.chatEvent({ event: 0x02, username: 'Cy', text: '', flags: 0 });
  assert.equal(s.evidence.chat.at(-1), 'Cy has joined the channel.');
  s.chatEvent({ event: 0x04, username: '*Dee', text: 'hey', flags: 0 });
  type(s, '/r yo'); s.typeKey('Enter');
  assert.deepEqual(sent, ['/w *Dee yo']);
});

// D2Multi key filter 0xB760: Tab steps forward through the last ten slash lines (newest first), wrapping; non-slash lines are not stored.
test('screen: Tab recalls sent slash lines, newest first, wrapping', () => {
  const sent = [];
  const s = screenWith(sent);
  s.draw(ctx, 800, 600);
  s.typeKey('Enter'); s.chatLive(true);
  type(s, '/me a'); s.typeKey('Enter');
  type(s, 'plain'); s.typeKey('Enter');
  type(s, '/r'); s.typeKey('Enter');
  type(s, '/me b'); s.typeKey('Enter');
  const tab = () => { s.typeKey('Tab'); return s.editValue(); };
  assert.equal(tab(), '/me b');
  assert.equal(tab(), '/r');
  assert.equal(tab(), '/me a');
  assert.equal(tab(), '/me b');
  s.typeKey('ArrowUp'); assert.equal(s.editValue(), '/me b');
  s.typeKey('Enter');
  assert.equal(tab(), '/me b');
});

test('screen: empty /r shows the usage line once a whisper arrived; talk colours come from the user list', () => {
  const s = screenWith([]);
  s.draw(ctx, 800, 600);
  s.typeKey('Enter'); s.chatLive(true);
  s.chatEvent({ event: 0x04, username: '*Dee', text: 'hey', flags: 0 });
  type(s, '/reply'); s.typeKey('Enter');
  assert.equal(s.evidence.chat.at(-1), 'Correct usage is: /reply (message)');
  s.chatEvent({ event: 0x02, username: 'Op', text: '', flags: 3 });
  s.chatEvent({ event: 0x05, username: 'Op', text: 'hi', flags: 0 });
  assert.equal(s.evidence.chat.at(-1), '<Op> hi');
  s.chatEvent({ event: 0x0D, text: 'Void' });
  assert.match(s.evidence.chat.at(-1), /^The channel "Void" is full\. /);
  const n = s.evidence.chat.length;
  s.chatEvent({ event: 0x0E, text: 'Void' });
  assert.equal(s.evidence.chat.length, n);
  s.chatEvent({ event: 0x15, text: 'info' });
  assert.equal(s.evidence.chat.at(-1), 'info');
});
