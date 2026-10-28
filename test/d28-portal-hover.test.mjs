import test from 'node:test';
import assert from 'node:assert/strict';
import { objectPortalLabel } from '../play/assets/object-hover-label.mjs';
import { parseObjectPresentations } from '../play/assets/object-renderer.mjs';

test('D28 portal label appends the destination level name; other objects are unchanged', () => {
  assert.equal(objectPortalLabel('Portal to', 'Rogue Encampment'), 'Portal to Rogue Encampment');
  assert.equal(objectPortalLabel('Waypoint', ''), 'Waypoint');
});

test('D28 object presentation record is 44 bytes and carries the portal destination level', () => {
  const b = new Uint8Array(88); b[40] = 1; b[44 + 40] = 0;
  const [portal, plain] = parseObjectPresentations(b, 2);
  assert.equal(portal.portalLevel, 1);
  assert.equal(plain.portalLevel, 0);
});

test('D33 a locked chest is named by string 3262, an unlocked one keeps its Objects.txt name (D2Client+0xA5D90)', async () => {
  const { objectNameFor, STR_LOCKED_CHEST } = await import('../play/assets/object-hover-label.mjs');
  assert.equal(STR_LOCKED_CHEST, 0xcbe);
  assert.equal(objectNameFor({ locked: true }, 'Chest', 'Locked Chest'), 'Locked Chest');
  assert.equal(objectNameFor({ locked: false }, 'Chest', 'Locked Chest'), 'Chest');
  const b = new Uint8Array(88); b[41] = 1;
  const [locked, plain] = parseObjectPresentations(b, 2);
  assert.equal(locked.locked, true); assert.equal(plain.locked, false);
});
