import test from 'node:test';
import assert from 'node:assert/strict';
import { packets } from './realm-input.mjs';
import { npcPackets } from './realm-npc-packets.mjs';

// Bytes the 1.13c client hands to its send thunk for fixed inputs (recorded headlessly, deterministic across two passes).
const hex = (s) => Uint8Array.from(s.trim().split(/\s+/).map((h) => parseInt(h, 16)));
const same = (ours, retail) => assert.deepEqual([...ours], [...hex(retail)]);

test('belt use 0x26: item guid, onMerc flag, zero word', () => {
  same(packets.useBeltItem(0x0a0b0c0d, false), '26 0d 0c 0b 0a 00 00 00 00 00 00 00 00');
  same(packets.useBeltItem(0x0a0b0c0d, true), '26 0d 0c 0b 0a 01 00 00 00 00 00 00 00');
});

test('belt lift 0x24 and place 0x23 and swap 0x25', () => {
  same(packets.beltLift(0x0a0b0c0d), '24 0d 0c 0b 0a');
  same(packets.beltPlace(0x0a0b0c0d, 3), '23 0d 0c 0b 0a 03 00 00 00');
  same(packets.beltSwap(0x0a0b0c0d, 0x0e0f1011), '25 0d 0c 0b 0a 11 10 0f 0e');
});

test('skill select 0x3c, right hand: skill word then owner -1', () => {
  same(packets.selectSkill(5, false, 0xffffffff), '3c 05 00 00 00 ff ff ff ff');
});

// Retail sends the left-hand skill as the movsx word of a short whose bit 15 is the hand flag: ECX = 0xFFFF8005
// (vectors/d2client/fn/14910.wire.bin, tuple 0x3C 0xFFFF8005 0xFFFFFFFF) gives 3c 05 80 ff ff ff ff ff ff.
test('skill select 0x3c, left hand: sign-extended word 0xFFFF8005, owner -1', () => {
  same(packets.selectSkill(5, true, 0xffffffff), '3c 05 80 ff ff ff ff ff ff');
});

test('drop gold 0x50: hero guid then amount 500', () => {
  same(packets.dropGold(0x01020304, 500), '50 04 03 02 01 f4 01 00 00');
});

test('NPC buy 0x32 and sell 0x33: 17 bytes, four little-endian dwords after the opcode', () => {
  same(npcPackets.buy(0x01020304, 0x0a0b0c0d, 2, 0x1234, { gamble: true, multi: true }),
    '32 04 03 02 01 0d 0c 0b 0a 02 00 02 80 34 12 00 00');
  same(npcPackets.sell(7, 9, 4), '33 07 00 00 00 09 00 00 00 04 00 00 00 00 00 00 00');
});
