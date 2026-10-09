import test from 'node:test';
import assert from 'node:assert/strict';
import { createNpcMenu, npcMenuEntries } from '../play/assets/npc-menu.mjs';

function makeNpcMenu() {
  const fontTbl = new Uint8Array(12);
  fontTbl.set([0x57, 0x6f, 0x6f, 0x21]); // "Woo!", accepted empty Font16 table for hit-test-only coverage.
  return createNpcMenu({
    read: () => fontTbl,
    decodeDatPalette: () => new Uint8Array(1024),
    decodeDc6: () => ({ frames: [{ height: 1 }] }),
    string: (id) => String(id),
  });
}

test('F199: Asheara Hire opens the recovered seven-row layout at 800x600 and widescreen coordinates', () => {
  assert.deepEqual(npcMenuEntries(252).map(({ action }) => action), ['talk', 'hire', 'trade', 'cancel']);

  const menu = makeNpcMenu();
  const offers = Array.from({ length: 10 }, (_, i) => ({
    name: `Merc${i}`, level: 20 + i, life: 100 + i, def: 30 + i, cost: 200 + i, skill: 'Combat',
  }));
  menu.showHire(252, offers, 5000);

  // The native window's seven visible rows have baselines 145 + 34*n. The
  // overlay's hit region is exercised at those pixels, without claiming that
  // retail's own unrecorded mouse hit-box predicate has been recovered.
  for (let row = 0; row < 7; row++) {
    assert.deepEqual(menu.hireAt(200, 145 + 34 * row), { row });
    assert.deepEqual(menu.hireAt(440, 205 + 34 * row, 1280, 720), { row });
  }

  assert.deepEqual(menu.hireAt(400, 421), { cancel: true });
  assert.deepEqual(menu.hireAt(640, 481, 1280, 720), { cancel: true });
  assert.equal(menu.hireAt(200, 120), null, 'points above the first native row do not select an offer');
  assert.equal(menu.hireAt(174, 145), null, 'a point one pixel left of the row does not select an offer');
  assert.equal(menu.hireAt(200, 145, 1280, 720), null, 'the old unshifted coordinate is outside the centered widescreen row');
});
