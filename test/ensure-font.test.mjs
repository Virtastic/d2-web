import assert from 'node:assert/strict';
import { ensureFont } from '../play/assets/centred-text.mjs';
const f = { draw() {} };
assert.equal(ensureFont(f, () => assert.fail('no rebuild')), f);
assert.equal(ensureFont(null, () => { throw new Error('no mpq'); }), null);
assert.equal(ensureFont(null, () => f), f);
console.log('ok');
