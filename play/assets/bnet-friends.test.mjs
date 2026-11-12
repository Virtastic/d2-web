// Battle.net friends requests (/f l, /f a name, /f r name) and the replies a client reads. Retail sends the friends
// text as an ordinary CHATCOMMAND (0x0E) line: BNClient +0xC400 frames the text, and D2Multi's slash dispatcher
// (0xB090) keeps only /r, /reply and /d2notify, so `/f ...` goes to the server as typed. The server's answers arrive as
// CHATEVENT (0x0F) lines. Retail's frame pump (BNClient +0xEA60) dispatches only ids below 0x5E, so 0x65-0x68 are skipped.
// The friends frames below are the bytes the original client put on the wire for these exact lines.
import test from 'node:test';
import assert from 'node:assert/strict';
import { bncsPacket, chatCommandPacket, createBnetChat } from './bnet-chat.mjs';
import { localSlash } from './bnet-slash.mjs';

const hex = (b) => Buffer.from(b).toString('hex');
const ev = (event, name, text) => { const b = Buffer.alloc(24); b.writeUInt32LE(event, 0); return Buffer.concat([b, Buffer.from(`${name}\0${text}\0`)]); };
const tick = () => new Promise((r) => setImmediate(r));

function pair() {
  const sent = []; let buf = new Uint8Array(0), cancelled = false; const waiters = [];
  const pump = () => { for (let i = waiters.length - 1; i >= 0; i--) if (buf.length >= waiters[i].n) { const w = waiters.splice(i, 1)[0]; w.resolve(buf.subarray(0, w.n)); } };
  const reader = { want: (n) => new Promise((resolve, reject) => { if (cancelled) reject(new Error('x')); waiters.push({ n, resolve, reject }); pump(); }),
    skip: (n) => { buf = buf.subarray(n); }, cancel: () => { cancelled = true; waiters.splice(0).forEach((w) => w.reject(new Error('x'))); } };
  const push = (d) => { const o = new Uint8Array(buf.length + d.length); o.set(buf); o.set(d, buf.length); buf = o; pump(); };
  return { transport: { send: (b) => { sent.push(hex(b)); return true; }, isOpen: true, close() {} }, reader, sent, push };
}

test('friends commands are CHATCOMMAND frames equal to the original client output', () => {
  // the original client's frames, verbatim
  const retail = {
    '/f l': 'ff0e09002f66206c00',
    '/f a Dunder': 'ff0e10002f6620612044756e64657200',
    '/f r Dunder': 'ff0e10002f6620722044756e64657200',
    '/friends list': 'ff0e12002f667269656e6473206c69737400',
    '/F R Dunder': 'ff0e10002f4620522044756e64657200',
    '/f a': 'ff0e09002f66206100',
    '/f': 'ff0e07002f6600',
  };
  for (const [line, frame] of Object.entries(retail)) assert.equal(hex(chatCommandPacket(line)), frame, line);
});

test('the slash dispatcher does not take /f lines: they reach the server as typed', () => {
  const state = { replyTo: 'Dunder', notify: false };
  for (const line of ['/f l', '/f a Dunder', '/F R Dunder', '/f']) assert.equal(localSlash(line, state), null, line);
  assert.equal(state.notify, false);
});

test('a C string ends at its first NUL; blanks and newlines are kept as typed', () => {
  assert.equal(hex(chatCommandPacket('a\0b')), 'ff0e0600' + '6100');
  // two spaces + NUL = 3 bytes: len 7; blanks are text, not empty
  assert.equal(hex(chatCommandPacket('  ')), 'ff0e0700' + '202000');
});

test('say("") sends nothing: BNClient +0xC400 returns before EB00 on an empty text', () => {
  const p = pair(), chat = createBnetChat({ ...p, character: 'Ama', keepAliveMs: 0 });
  assert.equal(chat.say(''), false);
  assert.equal(chat.say(undefined), false);
  assert.deepEqual(p.sent, []);
  assert.equal(chat.say(' '), true);
  assert.deepEqual(p.sent, ['ff0e06002000']);
  chat.close();
});

test('say("/f l") writes the friends frame; the server reply arrives as CHATEVENT lines', async () => {
  const p = pair(), lines = [];
  const chat = createBnetChat({ ...p, character: 'Ama', keepAliveMs: 0, onLine: (l) => lines.push(l) });
  assert.equal(chat.say('/f l'), true);
  assert.equal(p.sent.at(-1), 'ff0e09002f66206c00');
  // EID_INFO (0x12) carries the friends list text; EID_ERROR (0x13) a refused add.
  p.push(bncsPacket(0x0f, ev(0x12, '', 'Friends: Dunder (Online)')));
  p.push(bncsPacket(0x0f, ev(0x13, '', 'Cannot add: no such user')));
  await tick(); await tick();
  assert.deepEqual(lines, [{ text: 'Friends: Dunder (Online)', colour: 3 }, { text: 'Cannot add: no such user', colour: 1 }]);
  // a slash line is never echoed back as "<me> /f l"
  assert.equal(p.sent.length, 1);
  chat.close();
});

test('0x65-0x68 friends packets are not dispatched by retail: they are skipped and the session goes on', async () => {
  const p = pair(), lines = [];
  const chat = createBnetChat({ ...p, character: 'Ama', keepAliveMs: 0, onLine: (l) => lines.push(l) });
  for (const id of [0x5e, 0x65, 0x66, 0x67, 0x68]) p.push(bncsPacket(id, [1, 0, 0, 0, 0x44, 0x75, 0x6e, 0x00]));
  p.push(bncsPacket(0x0f, ev(0x12, '', 'still here')));
  await tick(); await tick();
  assert.deepEqual(p.sent, []);
  assert.deepEqual(lines, [{ text: 'still here', colour: 3 }]);
  assert.equal(chat.open, true);
  chat.close();
});
