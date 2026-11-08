import test from 'node:test';
import assert from 'node:assert/strict';
import { bncsPacket, createBnetChat } from './bnet-chat.mjs';
import { createChatLink } from './bnet-chat-link.mjs';
import { createBnetLobbyScreen } from './bnet-lobby-screen.mjs';

globalThis.ImageData ??= class { constructor(data, w, h) { Object.assign(this, { data, width: w, height: h }); } };
const ctx = new Proxy({}, { get: (_, k) => (k === 'getImageData' ? () => ({ data: new Uint8ClampedArray(4) }) : () => {}), set: () => true });
const frame = { width: 1, height: 1, offsetX: 0, offsetY: 0, indices: new Uint8Array(1), opaque: new Uint8Array([1]) };
const tbl = new Uint8Array(12 + 95 * 14); tbl.set([0x57, 0x6f, 0x6f, 0x21]); tbl[10] = 10;
for (let c = 32; c < 127; c++) { const b = 12 + (c - 32) * 14; tbl[b] = c; tbl[b + 3] = 6; tbl[b + 4] = 10; }
const tick = async (n = 6) => { for (let i = 0; i < n; i++) await new Promise((r) => setImmediate(r)); };
const ev = (event, name, text) => { const b = Buffer.alloc(24); b.writeUInt32LE(event, 0); return Buffer.concat([b, Buffer.from(`${name}\0${text}\0`)]); };

// A BNCS server in the socket: answers the client's frames the way PvPGN does (echo = false: the speaker never hears itself).
function server(echo) {
  const got = []; const waiters = []; let buf = new Uint8Array(0);
  const pump = () => { for (let i = waiters.length - 1; i >= 0; i--) if (buf.length >= waiters[i].n) { const w = waiters.splice(i, 1)[0]; w.resolve(buf.subarray(0, w.n)); } };
  const push = (d) => { const o = new Uint8Array(buf.length + d.length); o.set(buf); o.set(d, buf.length); buf = o; pump(); };
  const reader = { want: (n) => new Promise((resolve) => { waiters.push({ n, resolve }); pump(); }), skip: (n) => { buf = buf.subarray(n); }, cancel() {} };
  const transport = { isOpen: true, close() {}, send(b) {
    const id = b[1], body = Buffer.from(b.subarray(4)); got.push({ id, body });
    if (id === 0x0a) push(bncsPacket(0x0a, [...Buffer.from('Ama*acct\0stat\0acct\0')]));
    if (id === 0x0c) { push(bncsPacket(0x0f, ev(7, '', 'Diablo II 1'))); push(bncsPacket(0x0f, ev(1, 'Ama*acct', 'stat'))); push(bncsPacket(0x0f, ev(1, 'Bob*b', 'stat'))); }
    if (id === 0x0e && echo) push(bncsPacket(0x0f, ev(5, 'Ama*acct', body.subarray(0, body.length - 1).toString())));
    return true; } };
  return { transport, reader, got, push };
}

function rig(echo) {
  const srv = server(echo); let link, s;
  s = createBnetLobbyScreen({ read: (p) => (p.endsWith('.tbl') ? tbl : new Uint8Array(1 << 22)), decodeDc6: () => ({ frames: [frame, frame, frame, frame, frame, { ...frame, width: 10 }] }),
    decodeDatPalette: () => new Uint8Array(1024), string: () => '', character: { name: 'Ama', charClass: 0, level: 3 },
    documentObject: { createElement: () => ({ width: 0, height: 0, getContext: () => ctx }) }, now: () => 0,
    onEnterChat: () => link.join(), onSendChat: (t) => link.say(t, user) === true });
  const user = { character: 'Ama', realm: '', onEvent: (e) => s.chatEvent(e), onLine: (l) => s.chatLine(l) };
  link = createChatLink({ dial: async () => { throw new Error('no redial'); }, createChat: (o) => createBnetChat({ ...o, keepAliveMs: 0 }), onState: (on) => s.chatLive(on) });
  link.attach({ transport: srv.transport, reader: srv.reader }, user);
  return { s, srv, link };
}
const type = (s, text) => { for (const k of text) s.typeKey(`Key${k.toUpperCase()}`, k); s.typeKey('Enter'); };

for (const echo of [false, true]) {
  test(`lobby chat loopback (server ${echo ? 'echoes' : 'does not echo'} the speaker): hello goes out as 0x0E and shows once`, async () => {
    const { s, srv } = rig(echo);
    await tick(); assert.deepEqual(srv.got.map((p) => p.id), [0x0b, 0x0a]);
    s.activate('enter-chat'); await tick();
    assert.ok(srv.got.some((p) => p.id === 0x0c), 'JOINCHANNEL sent');
    assert.equal(s.mode, 'chat');
    assert.ok(s.evidence.chat.includes('You have joined channel: Diablo II 1'));
    assert.deepEqual(s.chatState.users.map((u) => u.name), ['Ama*acct', 'Bob*b']);
    type(s, 'hello'); await tick();
    const say = srv.got.filter((p) => p.id === 0x0e);
    assert.equal(say.length, 1); assert.equal(say[0].body.toString('latin1'), 'hello\0');
    assert.equal(s.evidence.chat.filter((l) => l === '<Ama> hello').length, 1);
    srv.push(bncsPacket(0x0f, ev(5, 'Bob*b', 'yo'))); await tick();
    assert.equal(s.evidence.chat.at(-1), '<Bob> yo');
    type(s, 'hello'); await tick();   // the same words again are a new line
    assert.equal(s.evidence.chat.filter((l) => l === '<Ama> hello').length, 2);
  });
}
