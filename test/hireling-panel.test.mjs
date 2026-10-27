import test from 'node:test';
import assert from 'node:assert/strict';
import { hirelingTextRows } from '../play/assets/hireling-panel.mjs';
test('hireling rows: empty and one merc', () => {
  assert.deepEqual(hirelingTextRows([]).map((r) => r.text), ['No hireling']);
  const r = hirelingTextRows([{ name: 'Rogue', level: 3, life: 40, maxLife: 50 }]);
  assert.deepEqual(r.map((t) => [t.text, t.y]), [['Hireling', 18], ['Rogue', 58], ['Level 3', 80], ['40 / 50', 102]]);
});
test('hireling rows: two mercs stack by step, no canvas-font path in the shell panel', async () => {
  const r = hirelingTextRows([{ name: 'A', level: 1, life: 1, maxLife: 2 }, { name: 'B', level: 2, life: 3, maxLife: 4 }]);
  assert.equal(r[4].y - r[1].y, 72);
  const src = (await import('node:fs')).readFileSync(new URL('../play/gameplay-shell.mjs', import.meta.url), 'utf8');
  const panel = src.slice(src.indexOf('hirelingPanelEvidence = null;\n  if (hirelingVisible'), src.indexOf("composition: 'retail-frame'"));
  assert.ok(panel.length > 100 && !/Georgia|fillRect/.test(panel));
});
