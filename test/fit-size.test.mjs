import test from 'node:test';
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import { fitSize } from '../play/assets/fit-size.mjs';

// CSS viewport sizes (portrait) and their device pixel ratios.
const PHONES = [[320, 568, 2], [360, 640, 3], [375, 667, 2], [390, 844, 3], [412, 915, 2.625], [430, 932, 3]];
const TABLETS = [[768, 1024, 2], [820, 1180, 2], [1024, 1366, 2], [800, 1280, 1.5], [884, 1104, 2.5]];

for (const [kind, list] of [['phone', PHONES], ['tablet', TABLETS]]) {
  for (const [a, b, dpr] of list) {
    for (const [cssW, cssH, o] of [[a, b, 'portrait'], [b, a, 'landscape']]) {
      test(`touch ${kind} ${cssW}x${cssH}@${dpr} ${o}: at least 800x600, box aspect, one edge exact`, () => {
        const f = fitSize({ cssW, cssH, dpr, touch: true });
        assert.ok(f.w >= 800 && f.h >= 600, `${f.w}x${f.h}`);
        const aspect = cssW / cssH, got = f.w / f.h;
        assert.ok(Math.abs(got - aspect) / aspect < 0.01, `aspect ${got} vs ${aspect}`);
        assert.ok(f.w === 800 || f.h === 600, `${f.w}x${f.h} touches neither minimum`);
        // The bitmap times S fills the device box.
        assert.ok(Math.abs(f.w * f.S - cssW * dpr) <= f.S, 'width fills the box');
        assert.ok(Math.abs(f.h * f.S - cssH * dpr) <= f.S, 'height fills the box');
      });
    }
  }
}

test('touch: a landscape phone gets exactly 600 rows, a portrait tablet exactly 800 columns', () => {
  assert.equal(fitSize({ cssW: 915, cssH: 412, dpr: 2.625, touch: true }).h, 600);
  assert.equal(fitSize({ cssW: 820, cssH: 1180, dpr: 2, touch: true }).w, 800);
  // 21:9 landscape: wide, 600 rows.
  const ultra = fitSize({ cssW: 2560, cssH: 1080, dpr: 1, touch: true });
  assert.equal(ultra.h, 600); assert.ok(ultra.w > 1400);
});

test('desktop: whole-number scale, common screens unchanged', () => {
  for (const [cssW, cssH, S] of [[1920, 1080, 1], [2560, 1440, 2], [3840, 2160, 3], [1366, 768, 1], [1280, 720, 1]]) {
    const f = fitSize({ cssW, cssH, dpr: 1 });
    assert.equal(f.S, S, `${cssW}x${cssH}`);
    assert.equal(f.w, Math.floor(cssW / S)); assert.equal(f.h, Math.floor(cssH / S));
  }
  // A narrow window keeps 800 columns: 1000x1400 would be S=2 by height alone (500 columns).
  const tall = fitSize({ cssW: 1000, cssH: 1400, dpr: 1 });
  assert.equal(tall.S, 1); assert.equal(tall.w, 1000);
  // Smaller than 800x600: S stays 1, the view is the window.
  assert.deepEqual(fitSize({ cssW: 640, cssH: 480, dpr: 1 }), { w: 640, h: 480, S: 1 });
});

test('an empty or unlaid box gives null, so the caller keeps its size', () => {
  assert.equal(fitSize({ cssW: 0, cssH: 600, touch: true }), null);
  assert.equal(fitSize({ cssW: 800, cssH: 0 }), null);
  assert.equal(fitSize({ cssW: NaN, cssH: 600 }), null);
  // NEGATIVE CONTROL: a bad dpr falls back to 1 rather than dividing the box away.
  assert.deepEqual(fitSize({ cssW: 800, cssH: 600, dpr: 0 }), { w: 800, h: 600, S: 1 });
});

test('fill mode: the viewport is sized by its safe-area insets, not also by 100%', () => {
  const html = readFileSync(new URL('../play/index.html', import.meta.url), 'utf8');
  const rule = html.match(/html\.fill #viewport\{[^}]*\}/)[0];
  assert.match(rule, /width:auto;height:auto/);
  assert.match(rule, /left:env\(safe-area-inset-left/);
  // A browser without dvh still gets a viewport height.
  assert.match(html, /--d2-vh:100vh\} @supports \(height:100dvh\)\{:root\{--d2-vw:100dvw;--d2-vh:100dvh\}\}/);
});

test('fitView measures the canvas box through fitSize', () => {
  const src = readFileSync(new URL('../play/gameplay-shell.mjs', import.meta.url), 'utf8');
  assert.match(src, /import \{ fitSize \} from '\.\/assets\/fit-size\.mjs'/);
  assert.match(src, /canvas\.getBoundingClientRect\(\)[\s\S]{0,200}fitSize\(/);
});
