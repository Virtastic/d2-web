// The engine path of the level-button and mini-panel hover plates: the hit test and plate position come from the engine callbacks.
import assert from 'node:assert/strict';
import { levelButtonTooltip } from './hud-bar-tooltip.mjs';
import { miniPanelTooltip } from './minipanel-tooltip.mjs';

const texts = ['New Stats', 'New Skill'];
const calls = [];
const engine = (w, h, mx, my) => { calls.push([w, h, mx, my]); return { side: 1, x: 999, y: 111 }; };
// the engine's side and plate win over the caller's own index; a button not drawn lit shows nothing
assert.deepEqual(levelButtonTooltip(-1, 800, 600, texts, { engine, mx: 5, my: 6, lit: [false, true] }), { bar: 'skill', text: 'New Skill', x: 999, y: 111, color: 0 });
assert.deepEqual(calls, [[800, 600, 5, 6]]);
assert.equal(levelButtonTooltip(1, 800, 600, texts, { engine, lit: [true, false] }), null);
assert.equal(levelButtonTooltip(1, 800, 600, texts, { engine: () => null, lit: [true, true] }), null);

const lang = (id) => `s${id}`;
const mini = (engineFn) => miniPanelTooltip(0, 100, 800, 600, 110, 540, { lang, bindings: [], slotOf: () => -1, engine: engineFn });
assert.deepEqual(mini((...a) => (assert.deepEqual(a, [800, 600, 110, 540, 100]), { x: 7, y: 8 })), { text: 's4169', x: 7, y: 8, color: 0 });
assert.equal(mini(() => null), null);
assert.deepEqual(mini(null), { text: 's4169', x: 97, y: 524, color: 0 });   // fallback rectangle
console.log('ok');

// the bar text goes through the engine's formatter when one is given, the JS replace otherwise
import { hudBarTooltip } from './hud-bar-tooltip.mjs';
{
  const bar = () => ({ bar: 1, x: 1, y: 2, color: 0, cur: 0 });
  const base = { experience: 5, nextExperience: 9, stamina: 1, staminaMax: 2, fmtExp: 'E %u / %u', fmtStamina: 'S %d / %d', engine: bar };
  assert.equal(hudBarTooltip(800, 600, 0, 0, base).text, 'E 5 / 9');
  const seen = [];
  assert.equal(hudBarTooltip(800, 600, 0, 0, { ...base, formatEngine: (id, a, b) => (seen.push([id, a, b]), 'eng') }).text, 'eng');
  assert.deepEqual(seen, [[4163, 5, 9]]);
  assert.equal(hudBarTooltip(800, 600, 0, 0, { ...base, formatEngine: () => null }).text, 'E 5 / 9');
}
