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
