// D41: an inventory tooltip with a vendor open ends with string 3331 + the engine's sell price (D2Client+0x46230).
import assert from 'node:assert';
import { itemTooltipSpec } from './item-tooltip.mjs';
const base = { name: 'Hand Axe', props: ['One-Hand Damage: 3 to 6'], sellLabel: 'Sell value: ' };
assert.deepStrictEqual(itemTooltipSpec({ ...base, sell: 62 }).lines, ['Hand Axe', 'One-Hand Damage: 3 to 6', 'Sell value: 62']);
assert.deepStrictEqual(itemTooltipSpec({ ...base, sell: 0 }).lines.at(-1), 'Sell value: 0');
assert.deepStrictEqual(itemTooltipSpec({ ...base, sell: -1 }).lines, ['Hand Axe', 'One-Hand Damage: 3 to 6']);
assert.deepStrictEqual(itemTooltipSpec(base).lines.length, 2);
console.log('item-tooltip D41: ok');
