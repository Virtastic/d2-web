import test from 'node:test';
import assert from 'node:assert/strict';
import { execFileSync } from 'node:child_process';
import { mkdtempSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';

const sh = (body) => execFileSync('bash', ['-c', `. wasm-build/probe-skip.sh; ${body}`], { encoding: 'utf8' });
const dir = mkdtempSync(join(tmpdir(), 'probe-skip-'));

test('link log: only fn probe objects are named, once each', () => {
  const log = join(dir, 'link.log');
  writeFileSync(log, [
    'wasm-ld: error: undefined symbol: g_lastError',
    '>>> referenced by build-wasm/obj/ab12/src_core_fn_storm_26500.o:(f)',
    '>>> referenced by build-wasm/obj/ab12/src_core_fn_storm_26500.o:(g)',
    'wasm-ld: error: undefined symbol: g_list459CC',
    '>>> referenced by build-wasm/obj/ab12/src_core_fn_d2cmp_11890.o:(f)',
    'wasm-ld: error: undefined symbol: engine_only',
    '>>> referenced by build-wasm/obj/ab12/src_core_d2common_x.o:(f)',
  ].join('\n'));
  assert.equal(sh(`probe_objs_from_link_log ${log}`), 'src_core_fn_d2cmp_11890.o\nsrc_core_fn_storm_26500.o\n');
});

test('link log with only engine errors names no probe', () => {
  const log = join(dir, 'engine.log');
  writeFileSync(log, 'wasm-ld: error: undefined symbol: x\n>>> referenced by build-wasm/obj/ab/src_core_a.o:(f)\n');
  assert.equal(sh(`probe_objs_from_link_log ${log}`), '');
});

test('compile skip line carries the first error', () => {
  const log = join(dir, 'cc.log');
  writeFileSync(log, "In file included\nsrc/core/fn/x.cpp:3:10: fatal error: 'nope.h' file not found\n");
  assert.match(sh(`probe_skip_line src/core/fn/x.cpp ${log}`), /^PROBE-SKIP src\/core\/fn\/x\.cpp: .*nope\.h/);
});
