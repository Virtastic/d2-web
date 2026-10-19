import test from 'node:test';
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import { resolve } from 'node:path';

test('D2Common 0x0D540 resets path coordinates from its 1.13c globals', () => {
  const source = readFileSync(resolve('src/core/d2moo/src/Units/UnitRoom.cpp'), 'utf8');
  assert.match(source, /constexpr int dword_6FDDD458 = 0;/);
  assert.match(source, /constexpr int dword_6FDDD45C = 0;/);
  const start = source.indexOf('static void UNITROOM_ResetDynamicPath');
  assert.notEqual(start, -1, 'reset helper must remain present');
  const end = source.indexOf('\n}', start);
  assert.notEqual(end, -1, 'reset helper must have a closing brace');
  const body = source.slice(start, end);

  // Retail decomp 0xD540.c loads DAT_6fddd458/45c; 0x3C870.c repeats
  // the same pair for the room teardown reset sequence.
  assert.match(body, /dwPrecisionX\s*=\s*dword_6FDDD458\s*;/);
  assert.match(body, /dwPrecisionY\s*=\s*dword_6FDDD45C\s*;/);
  assert.match(body, /dwClientCoordX\s*=\s*dword_6FDDD458\s*;/);
  assert.match(body, /dwClientCoordY\s*=\s*dword_6FDDD45C\s*;/);
  assert.doesNotMatch(body, /dword_6FDD258[04]/);
});
