import test from 'node:test';
import assert from 'node:assert/strict';
import { CONTROL_LAYOUT, HUD_LAYOUT, hudAimPoint, resourceRatio } from '../play/assets/control-panel.mjs';

test('life and mana fill ratios clamp authoritative snapshot values', () => {
  assert.deepEqual([resourceRatio(50, 100), resourceRatio(-2, 100), resourceRatio(200, 100), resourceRatio(4, 0)], [.5, 0, 1, 0]);
});

test('the console is centred between the orbs, which is where retail puts it', () => {
  // Not a pinned magic number: derive the offsets from 800ctrlpnl7's own frame widths. The orbs
  // are 117, the console's pieces 128/128/128/86, so the console is 470 in a 566 gap and sits 48
  // in from each orb. Cycle 110 read those margins as seams and filled them with frame 6, which is
  // 39% opaque and put holes in the bar instead.
  const orb = 117, console_ = 128 + 128 + 128 + 86, screen = 800;
  const between = screen - 2 * orb;
  assert.equal(CONTROL_LAYOUT.centerX, orb + (between - console_) / 2);
  assert.equal(CONTROL_LAYOUT.manaX, screen - orb);
  assert.equal(CONTROL_LAYOUT.healthX, 0);
  assert.equal(CONTROL_LAYOUT.height, 104);
});

// The finger reach of the HUD's small controls. Canvas 800x600: retail coordinates are canvas coordinates.
const run = { left: 255, top: 571, w: 16, h: 20 };

test('a near miss of a small HUD control lands on its centre; on it, or out of reach, it does not', () => {
  assert.deepEqual(hudAimPoint(245, 580, { hud: { runButton: run } }), [263, 581]);
  assert.equal(hudAimPoint(260, 580, { hud: { runButton: run } }), null);
  assert.equal(hudAimPoint(215, 580, { hud: { runButton: run } }), null);
});

test('a point on a belt box or an open panel is never pulled to a HUD control', () => {
  const belt = [{ left: 230, top: 570, right: 252, bottom: 592 }];
  assert.equal(hudAimPoint(250, 580, { hud: { runButton: run }, belt }), null);
  assert.equal(hudAimPoint(250, 580, { hud: { runButton: run }, extra: [{ left: 200, top: 500, w: 60, h: 100 }] }), null);
});

test('a grey level button takes no click, so it takes no snap; a lit one does', () => {
  const at = (frame) => ({ levelButtons: [{ left: 206, top: 563, w: 30, h: 30, frame }] });
  assert.equal(hudAimPoint(200, 578, { hud: at(HUD_LAYOUT.levelButtons.disabledFrame) }), null);
  assert.deepEqual(hudAimPoint(200, 578, { hud: at(HUD_LAYOUT.levelButtons.litFrame) }), [221, 578]);
});

test('on a wider canvas the snap is returned in canvas pixels', () => {
  assert.deepEqual(hudAimPoint(445, 680, { width: 1200, height: 700, hud: { runButton: run } }), [463, 681]);
});
