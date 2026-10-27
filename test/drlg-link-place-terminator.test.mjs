import test from 'node:test';
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';

// D2Common+0x16E10: retail walks link tables to the nLevel==0 terminator, no 15 cap.
const src = readFileSync(new URL('../src/core/d2moo/src/DrlgOutPlace.cpp', import.meta.url), 'utf8');
test('sub_6FD823C0 is uncapped and every link table is terminated', () => {
  const fn = src.slice(src.indexOf('// 1.13c D2Common+0x16E10'), src.indexOf('// 1.13c D2Common+0x16CF0'));
  assert.ok(fn.length > 100 && !/i < 15 &&/.test(fn));
  for (const m of src.matchAll(/static D2DrlgLinkStrc g\w+DrlgLink\[15\] =\s*\{([\s\S]*?)\n\};/g))
    assert.match(m[1], /\{ NULL, 0, -1, -1 \}/);
});
