// The HUD Help button (D2Client 0x37740 draws it; the pressed flag is the frame of its second cel, Level.dc6) and the H hotkey,
// driven as the shell wires them: pointerdown/up on the button, and the 'Help Screen' action resolved from the default key table.
import { test } from 'node:test';
import assert from 'node:assert/strict';
import { eventHost, fixtureRenderer } from './helpers/hud-help-fixture.mjs';
import { HUD_HELP_HITBOX } from '../play/assets/hud-help.mjs';
import { bindHudHelpPointer } from '../play/assets/hud-help-pointer.mjs';
import { CONFIGURE_CONTROLS_SLOTS, controlActionForKey } from '../play/assets/configure-controls.mjs';

test('mouse down shows the pressed frame, release opens help; H toggles it', (t) => {
  const prev = globalThis.ImageData;
  globalThis.ImageData = class ImageData { constructor(data, width, height) { Object.assign(this, { data, width, height }); } };
  t.after(() => { if (prev === undefined) delete globalThis.ImageData; else globalThis.ImageData = prev; });
  const { renderer } = fixtureRenderer(), host = eventHost(), ctx = { drawImage() {} };
  let helpVisible = false;
  const ptr = bindHudHelpPointer(host, { point: (e) => [e.x, e.y], hit: () => HUD_HELP_HITBOX, enabled: () => !helpVisible,
    canActivate: () => !helpVisible, activate: () => { helpVisible = true; } });
  const frames = () => renderer.draw(ctx, 1, { pressed: ptr.pressed }).button.map((b) => b.frame);
  assert.deepEqual(frames(), [0, 0]);
  host.dispatch('pointerdown');
  assert.deepEqual(frames(), [0, 1], 'held: the gem shows its pressed frame (the socket has only frame 0)');
  assert.equal(helpVisible, false, 'nothing opens on press');
  host.dispatch('pointerup');
  assert.deepEqual(frames(), [0, 0]);
  assert.equal(helpVisible, true);
  const bindings = CONFIGURE_CONTROLS_SLOTS.map((s) => s && [s[1], s[2]]);
  assert.equal(controlActionForKey(bindings, 'KeyH'), 'Help Screen');
});
