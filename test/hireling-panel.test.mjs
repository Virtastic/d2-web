import test from 'node:test';
import assert from 'node:assert/strict';
import { hirelingPanelDrawList, serializedBodyLoc } from '../play/assets/hireling-panel.mjs';

test('hireling panel: no merc draws nothing, so O shuts it', () => {
  assert.deepEqual(hirelingPanelDrawList(null, { string: () => '', measure: () => 0 }), []);
});
test('hireling panel: the body location is bits 61..64 of the serialized item', () => {
  const bits = new Uint8Array(16);
  bits[7] = 0xa0;   // value 5 (left arm): bits 61 and 63 set, LSB first
  assert.equal(serializedBodyLoc(bits), 5);
});
test('hireling panel: the shell draws the left panel through the renderer, no canvas fonts', async () => {
  const src = (await import('node:fs')).readFileSync(new URL('../play/gameplay-shell.mjs', import.meta.url), 'utf8');
  const panel = src.slice(src.indexOf('hirelingPanelEvidence = null;\n  if (hirelingVisible'), src.indexOf("composition: 'retail-frame'"));
  assert.ok(panel.length > 100 && !/Georgia|fillRect|drawFrame|400/.test(panel));
  assert.ok(src.includes("inside({ left: 80, top: 60, width: 320, height: 432 })) return 'hireling'"));
});
