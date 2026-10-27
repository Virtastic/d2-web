import {readFileSync} from 'node:fs';
import assert from 'node:assert';
import {test} from 'node:test';
test('D2Common+0x17760 keeps outdoor/vis/warp(-1) guards and preset flag', () => {
  const s = readFileSync(new URL('../src/core/d2moo/src/DrlgOutPlace.cpp', import.meta.url), 'utf8');
  const i = s.indexOf('// 1.13c D2Common+0x17760');
  assert(i > 0);
  const b = s.slice(i, i + 1800);
  for (const t of ['DRLGTYPE_OUTDOOR', 'j < 8', 'pVisArray[j] && pWarpIdArray[j] == -1', 'DRLGROOM_AddOrth', '== DRLGTYPE_PRESET'])
    assert(b.includes(t), t);
});
