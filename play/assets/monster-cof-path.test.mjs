import assert from 'node:assert/strict';
import { loadMonsterAnimation } from './monster-renderer.mjs';

// An actor carrying the engine's COF path asks for exactly that file: its token and weapon class, not the page's own.
const asked = [];
const read = (path) => { asked.push(path); throw new Error('absent'); };
assert.throws(() => loadMonsterAnimation(read, { code: 'zz', mode: 13, direction: 0, components: [],
  cofPath: 'DATA\\GLOBAL\\MONSTERS\\ZZ\\COF\\ZZGH1HS.COF' }), /no retail COF for monster zz GH/);
assert.deepEqual(asked, ['data\\global\\MONSTERS\\zz\\COF\\zzGH1HS.COF']);
console.log('monster-cof-path ok');
