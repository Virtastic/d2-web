import test from 'node:test';
import assert from 'node:assert/strict';
import { bncsPacket, enterChatPacket, joinChannelPacket, chatCommandPacket, parseChatEvent, chatLine, createBnetChat } from './bnet-chat.mjs';
import { webSocketTransport } from './realm-client.mjs';

const hex = (b) => Buffer.from(b).toString('hex');
const ev = (event, name, text) => { const b = Buffer.alloc(24); b.writeUInt32LE(event, 0); return Buffer.concat([b, Buffer.from(`${name}\0${text}\0`)]); };
const tick = () => new Promise((r) => setImmediate(r));

test('composers: framing and bodies of ENTERCHAT, JOINCHANNEL, CHATCOMMAND', () => {
  assert.equal(hex(bncsPacket(0x25, [1, 2, 3, 4])), 'ff25080001020304');
  assert.equal(hex(enterChatPacket('Ama', 'R')), 'ff0a0a00416d6100520' + '0');
  assert.equal(hex(joinChannelPacket('D')), 'ff0c0a000500000044' + '00');
  assert.equal(hex(chatCommandPacket('hi\nthere')), 'ff0e0d006869207468657265' + '00');
});

test('CHATEVENT parse and display lines', () => {
  const e = parseChatEvent(ev(5, 'Bob', 'hello'));
  assert.deepEqual([e.event, e.username, e.text], [5, 'Bob', 'hello']);
  assert.deepEqual(chatLine(e), { text: '<Bob> hello', colour: 0 });
  assert.equal(chatLine(parseChatEvent(ev(3, 'Bob', ''))).text, 'Bob has left the channel');
  assert.equal(parseChatEvent(new Uint8Array(10)), null);
});

// A transport + reader pair the way the realm client builds them: a socket the test feeds.
function pair() {
  const sent = []; let push; const waiters = []; let buf = new Uint8Array(0), cancelled = false;
  const pump = () => { for (let i = waiters.length - 1; i >= 0; i--) if (buf.length >= waiters[i].n) { const w = waiters.splice(i, 1)[0]; w.resolve(buf.subarray(0, w.n)); } };
  const reader = { want: (n) => new Promise((resolve, reject) => { if (cancelled) reject(new Error('x')); waiters.push({ n, resolve, reject }); pump(); }),
    skip: (n) => { buf = buf.subarray(n); }, cancel: () => { cancelled = true; waiters.splice(0).forEach((w) => w.reject(new Error('x'))); } };
  push = (d) => { const o = new Uint8Array(buf.length + d.length); o.set(buf); o.set(d, buf.length); buf = o; pump(); };
  return { transport: { send: (b) => { sent.push(hex(b)); return true; }, isOpen: true, close() {} }, reader, sent, push };
}

test('session: ENTERCHAT, join after its reply, ping echo, lines in, a typed line out', async () => {
  const p = pair(), lines = [];
  const chat = createBnetChat({ ...p, character: 'Ama', realm: 'R', onLine: (l) => lines.push(l) });
  chat.enter(); chat.join();
  assert.deepEqual(p.sent, ['ff0a0a00416d6100520' + '0']);
  p.push(bncsPacket(0x0a, [...Buffer.from('Ama\0s\0acct\0')])); await tick(); await tick();
  assert.equal(chat.entered, true);
  assert.deepEqual(p.sent.slice(1).map((s) => s.slice(0, 4)), ['ff0b', 'ff0c']);
  p.push(bncsPacket(0x25, [9, 0, 0, 0])); await tick();
  assert.equal(p.sent.at(-1), 'ff25080009000000');
  // two events in one chunk, one split across chunks
  const a = bncsPacket(0x0f, ev(7, '', 'Diablo II 1')), b = bncsPacket(0x0f, ev(5, 'Bob', 'hello'));
  p.push(Buffer.concat([a, b.subarray(0, 10)])); await tick(); p.push(b.subarray(10)); await tick(); await tick();
  assert.deepEqual(lines.map((l) => l.text), ['Joined channel Diablo II 1', '<Bob> hello']);
  assert.equal(chat.say('hi'), true);
  assert.equal(p.sent.at(-1), 'ff0e07006869' + '00');
  chat.close();
});
