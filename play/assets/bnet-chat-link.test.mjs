import test from 'node:test';
import assert from 'node:assert/strict';
import { bncsPacket, createBnetChat } from './bnet-chat.mjs';
import { createChatLink } from './bnet-chat-link.mjs';

const hex = (b) => Buffer.from(b).toString('hex');
const tick = async (n = 4) => { for (let i = 0; i < n; i++) await new Promise((r) => setImmediate(r)); };
const ev = (event, name, text) => { const b = Buffer.alloc(24); b.writeUInt32LE(event, 0); return Buffer.concat([b, Buffer.from(`${name}\0${text}\0`)]); };

// A BNCS socket the test feeds and can kill, as the realm client's reader sees one.
function socket() {
  const sent = []; const waiters = []; let buf = new Uint8Array(0), dead = false;
  const pump = () => { for (let i = waiters.length - 1; i >= 0; i--) {
    if (buf.length >= waiters[i].n) { const w = waiters.splice(i, 1)[0]; w.resolve(buf.subarray(0, w.n)); }
    else if (dead) waiters.splice(i, 1)[0].reject(new Error('closed')); } };
  const reader = { want: (n) => new Promise((resolve, reject) => { waiters.push({ n, resolve, reject }); pump(); }),
    skip: (n) => { buf = buf.subarray(n); }, cancel: () => { dead = true; pump(); } };
  const transport = { isOpen: true, sent,
    send(b) { if (dead) return false; sent.push(hex(b)); return true; },
    close() { transport.isOpen = false; dead = true; pump(); } };
  return { transport, reader, sent,
    push(d) { const o = new Uint8Array(buf.length + d.length); o.set(buf); o.set(d, buf.length); buf = o; pump(); },
    kill() { transport.isOpen = false; dead = true; pump(); } };
}
const enterReply = () => bncsPacket(0x0a, [...Buffer.from('Ama\0')]);
const channelEvent = (name) => bncsPacket(0x0f, ev(7, '', name));
const NULL = 'ff000400', ENTER = hex(bncsPacket(0x0a, [...Buffer.from('Ama\0R\0')]));

function rig({ failDials = 0 } = {}) {
  const socks = [], log = [], states = [], lines = []; let dials = 0, failed = 0;
  const dial = async () => { dials++; if (failed < failDials) { failed++; throw new Error('refused'); } const s = socket(); socks.push(s); return s; };
  const link = createChatLink({ dial, createChat: (o) => createBnetChat({ ...o, keepAliveMs: 0 }),
    onState: (on) => states.push(on), onFail: (e) => log.push(`fail:${e.message}`) });
  const user = { character: 'Ama', realm: 'R', onLine: (l) => lines.push(l.text) };
  const first = socket(); socks.push(first);
  link.attach({ transport: first.transport, reader: first.reader }, user);
  return { link, user, socks, states, lines, log, get dials() { return dials; } };
}
async function inChannel(r, name = 'Diablo II 3') {
  r.socks[0].push(enterReply()); await tick();
  r.link.join(); await tick();
  r.socks[0].push(channelEvent(name)); await tick();
}

test('close mid-game: the dead socket is noticed, the return redials, re-enters chat and rejoins the last channel', async () => {
  const r = rig(); await inChannel(r, 'Clan Room');
  assert.deepEqual(r.socks[0].sent.map((s) => s.slice(0, 4)), ['ff0b', 'ff0a', 'ff0c']);
  r.socks[0].kill(); await tick();
  assert.equal(r.link.session.open, false); assert.deepEqual(r.states, [true, false]);
  assert.equal(r.dials, 0, 'no redial while in the game');
  const back = r.link.ensure(r.user); const back2 = r.link.ensure(r.user);
  assert.equal(back, back2, 'single flight');
  await back; assert.equal(r.dials, 1);
  const s = r.socks[1]; assert.deepEqual(s.sent, ['ff0b080050583244', ENTER]);
  s.push(enterReply()); await tick();
  assert.deepEqual(s.sent, ['ff0b080050583244', ENTER, hex(bncsPacket(0x0c, [2, 0, 0, 0, ...Buffer.from('Clan Room\0')]))]);
  assert.equal(r.link.session.open, true); assert.deepEqual(r.states, [true, false, false, true]);
});

test('close on return before chat was ever joined: redial enters chat only', async () => {
  const r = rig(); r.socks[0].kill(); await tick();
  await r.link.ensure(r.user); r.socks[1].push(enterReply()); await tick();
  assert.deepEqual(r.socks[1].sent, ['ff0b080050583244', ENTER]);
});

test('close on send: the line is held, the socket redialled, the channel rejoined, then the line goes out once', async () => {
  const r = rig(); await inChannel(r);
  r.socks[0].kill();   // closed without the reader having noticed yet is the same path: send returns false
  assert.equal(r.link.say('hello there', r.user), true);
  await tick(); assert.equal(r.dials, 1);
  const s = r.socks[1]; s.push(enterReply()); await tick();
  assert.deepEqual(s.sent.map((x) => x.slice(0, 4)), ['ff0b', 'ff0a', 'ff0c']);
  s.push(channelEvent('Diablo II 3')); await tick(); await tick();
  assert.equal(s.sent.at(-1), hex(bncsPacket(0x0e, [...Buffer.from('hello there\0')])));
  assert.equal(s.sent.filter((x) => x.startsWith('ff0e')).length, 1);
  assert.equal(r.link.say('again', r.user), true);
  assert.equal(s.sent.at(-1), hex(bncsPacket(0x0e, [...Buffer.from('again\0')])));
});

test('a live send needs no redial; a refused dial reports and the next send tries again', async () => {
  const r = rig({ failDials: 1 }); await inChannel(r);
  assert.equal(r.link.say('hi', r.user), true); assert.equal(r.dials, 0);
  r.socks[0].kill(); await tick();
  r.link.say('lost', r.user); await tick(); await tick();
  assert.deepEqual(r.log, ['fail:refused']); assert.equal(r.dials, 1);
  r.link.say('next', r.user); await tick(); assert.equal(r.dials, 2);
  r.socks[1].push(enterReply()); await tick(); r.socks[1].push(channelEvent('Diablo II 3')); await tick(); await tick();
  assert.deepEqual(r.socks[1].sent.filter((x) => x.startsWith('ff0e')), [hex(bncsPacket(0x0e, [...Buffer.from('next\0')]))]);
});

test('closing the link during a redial drops the new socket and starts no session', async () => {
  const r = rig(); r.socks[0].kill(); await tick();
  const p = r.link.ensure(r.user); r.link.close(); assert.equal(await p, null);
  assert.equal(r.socks[1].transport.isOpen, false); assert.equal(await r.link.ensure(r.user), null);
});

test('keep-alive: SID_NULL on the interval, ping echoed, a closed socket ends it and reports', async (t) => {
  t.mock.timers.enable({ apis: ['setInterval'] });
  const s = socket(); let closed = 0;
  const chat = createBnetChat({ transport: s.transport, reader: s.reader, character: 'Ama', keepAliveMs: 180000, onClose: () => closed++ });
  t.mock.timers.tick(180000); t.mock.timers.tick(180000);
  assert.deepEqual(s.sent, [NULL, NULL]);
  s.push(bncsPacket(0x25, [7, 0, 0, 0])); await tick();
  assert.equal(s.sent.at(-1), 'ff25080007000000');
  s.transport.isOpen = true; s.kill(); await tick(); assert.equal(closed, 1);
  t.mock.timers.tick(180000); assert.equal(s.sent.length, 3); assert.equal(chat.open, false);
  chat.close();
});

test('a keep-alive that finds the socket closed marks the chat dead', async (t) => {
  t.mock.timers.enable({ apis: ['setInterval'] });
  const s = socket(); let closed = 0;
  const chat = createBnetChat({ transport: { ...s.transport, send: () => false, isOpen: true }, reader: s.reader, character: 'Ama', keepAliveMs: 1000, onClose: () => closed++ });
  t.mock.timers.tick(1000); assert.equal(closed, 1); assert.equal(chat.open, false); assert.equal(chat.say('x'), false);
  chat.close();
});

test('a channel picked in the CHANNEL panel is a forced join; the list can be asked for again', async () => {
  const r = rig(); await inChannel(r);
  r.link.join('Clan Room'); await tick();
  assert.equal(r.socks[0].sent.at(-1), hex(bncsPacket(0x0c, [2, 0, 0, 0, ...Buffer.from('Clan Room\0')])));
  assert.equal(r.link.channels(), true);
  assert.equal(r.socks[0].sent.at(-1), 'ff0b080050583244');
});
