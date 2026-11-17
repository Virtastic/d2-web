// The engine's automap layout export (ports of D2Client 0x5F9F0 + 0x608D0) against the JS arithmetic it replaces,
// built natively with g++ and compared over a grid of screen sizes, modes, panels and selectors.
import test from 'node:test';
import assert from 'node:assert/strict';
import { spawnSync } from 'node:child_process';
import { mkdtempSync, writeFileSync, rmSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join, dirname } from 'node:path';
import { fileURLToPath } from 'node:url';
import { automapViewport, automapScrollOffset, bindAutomapLayout } from './automap-overlay.mjs';

const root = join(dirname(fileURLToPath(import.meta.url)), '..', '..', 'src', 'core', 'd2client');
const have = spawnSync('g++', ['--version']).status === 0;

test('native layout wrapper matches the JS automap viewport and scroll', { skip: !have && 'no g++' }, () => {
  const dir = mkdtempSync(join(tmpdir(), 'amlayout-'));
  try {
    writeFileSync(join(dir, 'emscripten.h'), '#define EMSCRIPTEN_KEEPALIVE\n');
    writeFileSync(join(dir, 'main.cpp'), `#include <cstdio>
#include <cstdint>
extern "C" void d2_client_automap_layout(int32_t,int32_t,int32_t,int32_t,int32_t,int32_t,int32_t*);
int main(){ int w,h,m,p,s,c; while (scanf("%d %d %d %d %d %d",&w,&h,&m,&p,&s,&c)==6){ int32_t o[8];
  d2_client_automap_layout(w,h,m,p,s,c,o); for(int i=0;i<8;i++) printf("%d ",o[i]); printf("\\n"); } }
`);
    const exe = join(dir, 'amtest');
    const build = spawnSync('g++', ['-std=c++17', '-I', dir, '-I', root, '-o', exe, join(dir, 'main.cpp'),
      join(root, 'automap_layout.cpp'), join(root, 'batch_b46.cpp'), join(root, 'batch_b48_automap.cpp')], { encoding: 'utf8' });
    assert.equal(build.status, 0, build.stderr);
    const cases = [];
    for (const [w, h] of [[640, 480], [800, 600], [1024, 768], [1280, 720], [1920, 1080], [801, 601]])
      for (const mode of [0, 1, 2]) for (const left of [1, 0]) for (const sel of [0, 1, 2]) cases.push([w, h, mode, left, sel]);
    const run = spawnSync(exe, [], { input: cases.map((c) => `${c.join(' ')} 1\n`).join(''), encoding: 'utf8' });
    assert.equal(run.status, 0);
    const rows = run.stdout.trim().split('\n').map((l) => l.trim().split(' ').map(Number));
    assert.equal(rows.length, cases.length);
    cases.forEach(([w, h, mode, left, sel], i) => {
      const jsMode = mode === 1 ? 'mini' : 'full';
      const v = automapViewport(w, h, { mode: jsMode, left: !!left, selector284: sel });
      const o = rows[i], tag = JSON.stringify([w, h, mode, left, sel]);
      assert.deepEqual(o.slice(0, 2), [v.anchorX, v.anchorY], `anchor ${tag}`);
      assert.deepEqual(o.slice(2, 6), [v.viewport.left, v.viewport.top, v.viewport.right, v.viewport.bottom], `viewport ${tag}`);
      const s = automapScrollOffset(w, h, { mode: jsMode, center: 'yes', anchorX: v.anchorX, anchorY: v.anchorY });
      assert.deepEqual(o.slice(6, 8), [s.x, s.y], `scroll ${tag}`);
    });
  } finally { rmSync(dir, { recursive: true, force: true }); }
});

test('bindAutomapLayout leaves the JS fallback when the export is missing', () => {
  assert.equal(bindAutomapLayout({}), false);
  assert.equal(automapViewport(800, 600, { mode: 'mini' }).anchorY, 96);
});
