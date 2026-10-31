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

test('link log: debug continuation lines and duplicate symbols name their probe', () => {
  const log = join(dir, 'dbg.log');
  writeFileSync(log, [
    'wasm-ld: error: undefined symbol: Port',
    '>>> referenced by x.cpp:12 (src/core/fn/x.cpp:12)',
    '>>>               build-wasm/obj/ab/src_core_fn_a_1.o:(f)',
    'wasm-ld: error: duplicate symbol: Dup',
    '>>> defined in build-wasm/obj/ab/src_core_d2game_dup.o',
    '>>> defined in build-wasm/obj/ab/src_core_fn_b_2.o',
  ].join('\n'));
  assert.equal(sh(`probe_objs_from_link_log ${log}`), 'src_core_fn_a_1.o\nsrc_core_fn_b_2.o\n');
});

// Drives probe_link_retry with a scripted link_engine: each call pops one scenario step.
const retry = (steps, objs) => {
  const d = mkdtempSync(join(tmpdir(), 'probe-retry-'));
  const out = sh(`
    BUILD=${d}; LINKLOG=$BUILD/link.log; SKIPPED_FILE=$BUILD/skipped.txt; : > $SKIPPED_FILE
    OBJS="${objs}"; calls=0; STEPS="${steps.join(' ')}"
    link_engine() {
      calls=$((calls+1)); local step; step=$(echo $STEPS | cut -d' ' -f$calls)
      case "$step" in ok) return 0;; engine) echo 'wasm-ld: error: undefined symbol: e'; echo '>>> referenced by o/src_core_a.o:(f)'; return 1;;
        *) echo "wasm-ld: error: o/$step: undefined symbol: s"; echo ">>> referenced by o/$step:(f)"; return 1;; esac
    }
    probe_link_retry 2>/dev/null && rc=0 || rc=1
    echo "rc=$rc calls=$calls objs=$(echo $OBJS | tr ' ' ',') skipped=$(tr '\\n' ',' < $SKIPPED_FILE)"
  `);
  return out.split('\n').filter((l) => l.startsWith('PROBE-SKIP') || l.startsWith('rc='));
};

test('link retry: one bad probe is dropped and the relink succeeds', () => {
  const r = retry(['src_core_fn_a_1.o', 'ok'], 'o/src_core_a.o o/src_core_fn_a_1.o o/src_core_fn_b_2.o');
  assert.match(r[0], /^PROBE-SKIP o\/src_core_fn_a_1\.o: .*undefined symbol: s.*\(link\)$/);
  assert.equal(r[1], 'rc=0 calls=2 objs=o/src_core_a.o,o/src_core_fn_b_2.o skipped=a_1,');
});

test('link retry: a cascade is dropped round by round', () => {
  const r = retry(['src_core_fn_a_1.o', 'src_core_fn_b_2.o', 'ok'], 'o/src_core_a.o o/src_core_fn_a_1.o o/src_core_fn_b_2.o');
  assert.equal(r.at(-1), 'rc=0 calls=3 objs=o/src_core_a.o skipped=a_1,b_2,');
});

test('link retry: an engine-only error fails at once without relinking', () => {
  const r = retry(['engine', 'ok'], 'o/src_core_a.o o/src_core_fn_a_1.o');
  assert.equal(r.at(-1), 'rc=1 calls=1 objs=o/src_core_a.o,o/src_core_fn_a_1.o skipped=');
});

test('compile: a probe that fails to compile is skipped, an engine source fails the build', () => {
  const d = mkdtempSync(join(tmpdir(), 'probe-cc-'));
  writeFileSync(join(d, 'em++'), '#!/bin/sh\necho "boom: error: no port" >&2; exit 1\n', { mode: 0o755 });
  const o = join(d, 'src_core_fn_x.o');
  const out = sh(`
    PATH=${d}:$PATH; CXXFLAGS=; dependency_digest() { :; }
    compile_one "src/core/fn/x.cpp|${o}"; echo "probe rc=$?"
    compile_one "src/core/a.cpp|${d}/src_core_a.o"; echo "engine rc=$?"; [ -e ${o} ] && echo present || echo absent
  `);
  assert.match(out, /^PROBE-SKIP src\/core\/fn\/x\.cpp: boom: error: no port\nprobe rc=0\nengine rc=1\nabsent\n/);
});

test('link log: the object named in the error header counts, a warning block does not', () => {
  const log = join(dir, 'hdr.log');
  writeFileSync(log, [
    'wasm-ld: warning: function signature mismatch: F',
    '>>> defined as (i64) -> i32 in o/src_core_fn_warn_1.o',
    '>>> defined as () -> i32 in o/src_core_x.o',
    '',
    'wasm-ld: error: o/ab/src_core_fn_d2cmp_10b70.o: undefined symbol: d2cmp_10b70',
    'wasm-ld: error: o/ab/src_core_fn_d2cmp_10b70.o: undefined symbol: other(int, void*)',
    'wasm-ld: error: o/ab/src_core_d2common_y.o: undefined symbol: engine_only',
    'em++: error: wasm-ld failed',
  ].join('\n'));
  assert.equal(sh(`probe_objs_from_link_log ${log}`), 'src_core_fn_d2cmp_10b70.o\n');
});
