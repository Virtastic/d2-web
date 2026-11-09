// An online unit and the camera use the path's 16.16 position, as an offline game does, so a walking
// NPC glides between subtiles instead of jumping a whole subtile while its walk cycle plays.
import { test } from 'node:test';
import assert from 'node:assert/strict';
import { realmViewSnapshot } from './realm-client-view.mjs';
import { realmCameraCentre, selectPlayerRenderSource } from './realm-player-state.mjs';
import { unitAnchor } from './unit-anchor.mjs';

const actor = () => null;

test('a walking online monster and player are drawn at their precise path position', () => {
  const snap = realmViewSnapshot({
    monsters: [{ guid: 7, classId: 146, mode: 2, x: 10, y: 20, direction: 0, preciseX: 10.75, preciseY: 20.25 }],
    players: [{ guid: 9, classId: 3, x: 30, y: 40, name: 'B', renderState: { mode: 2, direction: 0 }, preciseX: 30.5, preciseY: 40.875 }],
    actor, frame: 0, level: 1 });
  const monster = snap.entities.find((e) => e.kind === 'monster');
  const player = snap.entities.find((e) => e.kind === 'player');
  assert.deepEqual(unitAnchor(monster), [10.25, 19.75]);
  assert.deepEqual(unitAnchor(player), [30, 40.375]);
});

test('a unit with no path position keeps its whole-subtile anchor', () => {
  const snap = realmViewSnapshot({ monsters: [{ guid: 7, classId: 146, mode: 1, x: 10, y: 20, direction: 0 }], actor, frame: 0, level: 1 });
  assert.deepEqual(unitAnchor(snap.entities[0]), [10, 20]);
  assert.equal('preciseX' in snap.entities[0], false);
});

test('the camera follows the selected player precise position and falls back to the subtile centre', () => {
  const player = { active: true, clientRoomActive: true, guid: 1, x: 5, y: 6, classId: 0, preciseX: 5.25, preciseY: 6.75 };
  const source = selectPlayerRenderSource(null, null, true, player);
  assert.deepEqual(realmCameraCentre(source), { x: 5.25, y: 6.75 });
  const resting = selectPlayerRenderSource(null, null, true, { ...player, preciseX: undefined, preciseY: undefined });
  assert.deepEqual(realmCameraCentre(resting), { x: 5.5, y: 6.5 });
});
