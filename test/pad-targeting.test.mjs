import { test } from 'node:test';
import assert from 'node:assert/strict';
import { rankTargets, pickTarget } from '../play/assets/pad-targeting.mjs';
import { pad, B, install, tap, drain, poll } from './pad-fixture.mjs';

// Screen = world * 10 around the player at (400,300), so a unit's offset is its screen offset / 10.
const project = (x, y) => [400 + x * 10, 300 + y * 10];
const mon = (id, x, y, extra = {}) => ({ id, type: 1, mode: 1, hp: 50, hostile: true, x, y, ...extra });
const snap = (o = {}) => ({ player: { x: 0, y: 0, hpMax: 100 }, playerId: 1, level: 5, entities: [], items: [], objects: [], warps: [], ...o });
const top = (s, aim) => { const r = rankTargets(s, { aim, project }); return r[0] && `${r[0].kind}${r[0].id}`; };

test('priority: monster > item > object > player > corpse, whatever the distance', () => {
  const s = snap({
    entities: [{ id: 9, type: 1, mode: 12, hp: 0, x: 1, y: 0 }, { id: 8, type: 0, x: 1, y: 1 }, mon(2, 10, 0)],
    items: [{ id: 5, mode: 3, invPage: 255, x: 2, y: 0 }], objects: [{ id: 6, x: 1, y: 1 }],
  });
  assert.deepEqual(rankTargets(s, { project }).map((t) => t.kind), ['unit', 'item', 'object', 'player', 'corpse']);
  s.entities.splice(2, 1);
  assert.equal(top(s), 'item5');
});

test('the cone follows the stick: a farther monster in the aim beats a nearer one behind it', () => {
  const s = snap({ entities: [mon(2, -3, 0), mon(3, 8, 0)] });
  assert.equal(top(s, null), 'unit2');
  assert.equal(top(s, [1, 0]), 'unit3');
  assert.equal(top(s, [-1, 0]), 'unit2');
  assert.equal(top(snap({ entities: [mon(2, -3, 0)] }), [1, 0]), undefined, 'outside the cone is no target');
});

test('turn-to-face tie-break: fewer 45-degree turns from the aim wins over nearer', () => {
  const s = snap({ entities: [mon(2, 3, 3), mon(3, 9, 0)] });   // 45 degrees off, near; dead ahead, far
  assert.equal(top(s, [1, 0]), 'unit3');
  assert.equal(top(s, [1, 1]), 'unit2');
});

test('stairs are targeted at their click box; a warp to the current level is not', () => {
  const warp = { slot: 4, enabled: true, destinationLevel: 6, x: 4, y: 0, originX: 4, originY: 0, select: { x: -10, y: -20, dx: 20, dy: 20 } };
  const r = rankTargets(snap({ warps: [warp, { ...warp, slot: 5, destinationLevel: 5 }] }), { project });
  assert.deepEqual(r.map((t) => [t.kind, t.id, t.screen]), [['warp', 4, [440, 290]]]);
});

test('cycling wraps through the ranked list', () => {
  const r = rankTargets(snap({ entities: [mon(2, 1, 0), mon(3, 2, 0)] }), { project });
  assert.deepEqual([0, 1, 2, -1].map((n) => pickTarget(r, n).id), [2, 3, 2, 3]);
});

test('pad: A clicks the cone target, the right stick points at another, B with nothing casts ahead', async () => {
  const s = snap({ entities: [mon(2, 3, 0), mon(3, 6, 0)] });
  await install({ screen: 'game', uiTargets: () => ({}), snapshot: s, project, controlCodeFor: () => null });
  poll(); drain();
  tap(B.A);
  assert.deepEqual(drain(), ['pointermove:430.5,300.5', 'pointerdown:430.5,300.5', 'pointerup:430.5,300.5']);
  // The pointer starts at the last target clicked; two full-deflection polls move it 28 px right, to the second monster's
  // side, and A clicks at the pointer, not at the picker's target.
  pad.axes[2] = 1; poll(); poll(); pad.axes[2] = 0; poll();
  assert.deepEqual(drain(), ['pointermove:444.5,300.5', 'pointermove:458.5,300.5']);
  tap(B.A);
  assert.deepEqual(drain(), ['pointermove:458.5,300.5', 'pointerdown:458.5,300.5', 'pointerup:458.5,300.5']);
  pad.axes[1] = 1; poll(); pad.axes[1] = 0; poll();
  s.entities.length = 0; poll(); drain();
  tap(B.A);
  assert.deepEqual(drain(), [], 'A with nothing in reach is consumed');
  tap(B.B);
  assert.deepEqual(drain().filter((e) => e.startsWith('pointerdown')), ['pointerdown:400.5,460.5'], 'B casts at the ground ahead');
});
