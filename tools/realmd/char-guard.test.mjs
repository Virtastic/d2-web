import test from 'node:test';
import assert from 'node:assert/strict';
import { createCharGuard } from './char-guard.mjs';

const frame = (op, body) => { const f = Buffer.alloc(3 + body.length); f.writeUInt16LE(f.length, 0); f[2] = op; body.copy(f, 3); return f; };
const logon = (name) => {
  const g = Buffer.alloc(37); g[0] = 0x68; g.write(name, 21, 'latin1'); return g;
};
function session(guard, name) {
  const m = guard.mcpTap();
  m.fromClient(Buffer.concat([Buffer.from([1]), frame(0x07, Buffer.from(name + '\0'))]));
  m.fromRealm(frame(0x07, Buffer.alloc(4)));
  return m;
}
const join = () => frame(0x04, Buffer.alloc(10));
const mk = () => { let t = 1000; const g = createCharGuard({ graceMs: 1000, lockedMs: 5000, now: () => t }); return { g, adv: (n) => { t += n; } }; };

test('a live game refuses a second join with the failure reply', () => {
  const { g } = mk(); const d = g.d2gsTap(); d.fromClient(logon('Bob')); d.fromRealm(Buffer.from([1]));
  const r = session(g, 'bob').fromClient(join());
  assert.ok(r.reply); assert.equal(r.forward.length, 0);
});

test('a quick re-join after the game closes waits for the unlock, then goes through', () => {
  const { g, adv } = mk(); const d = g.d2gsTap(); d.fromClient(logon('Bob')); d.fromRealm(Buffer.from([1])); d.close(false);
  adv(300);
  const r = session(g, 'Bob').fromClient(join());
  assert.equal(r.reply, null); assert.equal(r.forward.length, 0); assert.equal(r.delayMs, 700); assert.deepEqual(r.later, join());
  adv(700);
  const r2 = session(g, 'Bob').fromClient(join());
  assert.deepEqual(r2.forward, join()); assert.equal(r2.delayMs, undefined);
});

test('a load the realm failed marks the character locked by something unseen, then ages out', () => {
  const { g, adv } = mk(); const d = g.d2gsTap(); d.fromClient(logon('Ann')); d.close(true);   // closed with no answer
  adv(2000);
  assert.ok(session(g, 'ann').fromClient(join()).reply);          // refused as the realm would
  adv(5000);
  assert.deepEqual(session(g, 'ann').fromClient(join()).forward, join());   // probe goes through
});

test('a browser abort before the answer is not a lock, and an answered load clears one', () => {
  const { g, adv } = mk(); const d = g.d2gsTap(); d.fromClient(logon('Cy')); d.close(false); adv(1000);
  assert.deepEqual(session(g, 'cy').fromClient(join()).forward, join());
  const e = g.d2gsTap(); e.fromClient(logon('Cy')); e.close(true);
  const f = g.d2gsTap(); f.fromClient(logon('Cy')); f.fromRealm(Buffer.from([1])); f.close(false); adv(1000);
  assert.deepEqual(session(g, 'cy').fromClient(join()).forward, join());
});
