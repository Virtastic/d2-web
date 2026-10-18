import test from 'node:test';
import assert from 'node:assert/strict';
import { groundItemsUnderPointerAt } from '../play/gameplay-model.mjs';
import { UNIT_CENTRE_Y, worldToScreen } from '../play/assets/session-terrain.mjs';

// The shell's static-grid projection with the player camera parked at subtile (10, 10).
const CAMERA = worldToScreen(10, 10), VIEW_X = 400, VIEW_Y = 300 + UNIT_CENTRE_Y;
const project = (wx, wy) => { const p = worldToScreen(wx, wy); return [p.x - CAMERA.x + VIEW_X, p.y - CAMERA.y + VIEW_Y]; };
const BOX = { xOffset: -5, yOffset: -30, width: 20, height: 32 };
const measure = () => BOX;
const centre = (item) => { const [sx, sy] = project(item.x, item.y);
  return [sx + BOX.xOffset + (BOX.width >> 1), sy + BOX.yOffset + (BOX.height >> 1)]; };

// S286/F14: hovering a ground item is the padded cel rectangle of the cel it is drawn from, for ANY
// item -- a piece of armour (non-gold) resolves exactly like a gold pile under the same rule.
test('ground item hover hit test resolves a non-gold item under its drawn sprite', () => {
  const cap = { id: 9, code: 0x706163, flippyFile: 'flpcap', mode: 3, invPage: 255, x: 12, y: 9 };
  const gold = { id: 10, code: 0x646c67, flippyFile: 'flpgld', mode: 3, invPage: 255, x: 14, y: 7 };
  const items = [cap, gold];
  const [capX, capY] = centre(cap), [goldX, goldY] = centre(gold);

  assert.equal(groundItemsUnderPointerAt(items, capX, capY, project, measure)[0], cap,
    'the armour under the pointer is the hit, not gold and not nothing');
  assert.equal(groundItemsUnderPointerAt(items, goldX, goldY, project, measure)[0], gold,
    'gold follows the same rule');
  assert.deepEqual(groundItemsUnderPointerAt(items, capX, capY + BOX.height + 60, project, measure), [],
    'far off the drawn sprite is not a hit');
  assert.deepEqual(groundItemsUnderPointerAt([{ ...cap, invPage: 0 }], capX, capY, project, measure), [],
    'an item that is not on the ground (inventory page set) is not a hover candidate');
});
