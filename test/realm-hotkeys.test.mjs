import {test} from 'node:test';
import assert from 'node:assert/strict';
import {bindEngineProtocol} from '../play/assets/realm-client.mjs';

function bind() {
  const heap = new Uint8Array(200000); let next = 32;
  const Module = {HEAPU8: heap, _malloc: (n) => { const p = next; next += n; return BigInt(p); }, _free: () => {},
    ccall: (name, _r, _t, args) => {
      if (name !== 'd2_realm_d2gs_select_skill') throw new Error(name);
      const [skill, left, owner, out] = args, v = new DataView(heap.buffer);
      v.setUint8(Number(out), 0x3c); v.setUint32(Number(out) + 1, (skill | (left ? 0x80000000 : 0)) >>> 0, true);
      v.setUint32(Number(out) + 5, owner, true); return 9;
    }};
  return bindEngineProtocol(Module);
}
const pkt = (slot, word, owner) => Uint8Array.of(0x7b, slot, word & 255, word >> 8, owner & 255, (owner >>> 8) & 255, (owner >>> 16) & 255, owner >>> 24);

test('server hotkey table starts unbound and F-keys send nothing', () => {
  const p = bind();
  assert.equal(p.d2gsHotkeys().length, 16);
  assert.ok(p.d2gsHotkeys().every((h) => h.skill === -1));
  assert.equal(p.d2gsHotkeySelect(0), null);
  p.free();
});

test('0x7B fills a slot with skill, hand and owner; F-key sends 0x3C with them', () => {
  const p = bind();
  assert.equal(p.d2gsApplyHotkey(pkt(2, 0x8000 | 36, 0x12345678), 400), true);
  assert.deepEqual(p.d2gsHotkeys()[2], {skill: 36, left: true, owner: 0x12345678});
  assert.deepEqual([...p.d2gsHotkeySelect(2)], [0x3c, 36, 0, 0, 0x80, 0x78, 0x56, 0x34, 0x12]);
  p.d2gsApplyHotkey(pkt(3, 54, 0xffffffff), 400);
  assert.deepEqual([...p.d2gsHotkeySelect(3)], [0x3c, 54, 0, 0, 0, 255, 255, 255, 255]);
  p.free();
});

test('0x7B skill above the skill count unbinds the slot; slot past 15 is ignored', () => {
  const p = bind();
  p.d2gsApplyHotkey(pkt(1, 36, 7), 400);
  p.d2gsApplyHotkey(pkt(1, 0x7ff, 7), 400);
  assert.equal(p.d2gsHotkeys()[1].skill, -1);
  assert.equal(p.d2gsHotkeySelect(1), null);
  assert.equal(p.d2gsApplyHotkey(pkt(16, 36, 7), 400), false);
  assert.throws(() => p.d2gsApplyHotkey(Uint8Array.of(0x23, 0, 0, 0, 0, 0, 0, 0)), RangeError);
  p.free();
});
