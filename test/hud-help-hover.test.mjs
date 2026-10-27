import { test } from 'node:test';
import assert from 'node:assert/strict';
import { createHudHelp, HUD_HELP_HITBOX, hudHelpButtonContains } from '../play/assets/hud-help.mjs';
import { eventHost, fixtureRenderer } from './helpers/hud-help-fixture.mjs';
import { bindHudHelpPointer } from '../play/assets/hud-help-pointer.mjs';

test('Help callback bounds preserve the native inclusive right and bottom edges', () => {
  const hit = HUD_HELP_HITBOX;
  assert.deepEqual(hit, { x: 725, y: 404, w: 36, h: 37 });
  assert.equal(hudHelpButtonContains(725, 404, hit), true);
  assert.equal(hudHelpButtonContains(760, 440, hit), true);
  assert.equal(hudHelpButtonContains(761, 440, hit), false);
  assert.equal(hudHelpButtonContains(760, 441, hit), false);
  assert.equal(hudHelpButtonContains(724, 404, hit), false);
  assert.equal(hudHelpButtonContains(725, 403, hit), false);
  assert.equal(hudHelpButtonContains(NaN, 420, hit), false);
});

test('Help button uses Level gem frame 1 while held and keeps the one-frame socket on frame 0', (t) => {
  const previousImageData = globalThis.ImageData;
  globalThis.ImageData = class ImageData { constructor(data, width, height) { Object.assign(this, { data, width, height }); } };
  t.after(() => { if (previousImageData === undefined) delete globalThis.ImageData; else globalThis.ImageData = previousImageData; });
  const { renderer } = fixtureRenderer();
  const draws = [];
  const ctx = { drawImage(image, x, y) { draws.push([x, y]); } };
  const rest = renderer.draw(ctx, 1);
  assert.equal(rest.pressed, false);
  assert.equal(rest.button[0].frame, 0);
  assert.equal(rest.button[1].frame, 0);
  const pressed = renderer.draw(ctx, 1, { pressed: true });
  assert.equal(pressed.pressed, true);
  assert.equal(pressed.button[0].frame, 0, 'D2Client 0x37740 draws the socket at frame 0');
  assert.equal(pressed.button[1].frame, 1, 'the held frame is the second cel (Level.dc6), the pressed flag of 0x37740');
  assert.equal(draws.length, 20, 'each call paints eight glyphs and two button cels');
});

test('Help pointer handling owns matching release and clears only its pointer on cancel', () => {
  const host = eventHost(), changes = [], activations = [];
  const help = bindHudHelpPointer(host, {
    point: (event) => [event.x, event.y], hit: () => HUD_HELP_HITBOX,
    enabled: () => true, canActivate: () => true,
    activate: () => activations.push('help'), changed: (pressed) => changes.push(pressed),
  });

  const down = host.dispatch('pointerdown', { pointerId: 7 });
  assert.equal(down.defaultPrevented, true);
  assert.equal(down.__d2HelpPointerHandled, true);
  assert.deepEqual(host.captures, [7]);
  assert.equal(help.pressed, true);
  assert.deepEqual(changes, [true]);

  const secondDown = host.dispatch('pointerdown', { pointerId: 8 });
  assert.equal(secondDown.__d2HelpPointerHandled, true, 'a second pointer is consumed without taking ownership');
  assert.equal(help.pressed, true);
  assert.deepEqual(host.captures, [7], 'capture remains with the first pointer');
  host.dispatch('pointerup', { pointerId: 8 });
  assert.equal(help.pressed, true, 'another pointer cannot release the armed Help socket');
  assert.deepEqual(activations, []);

  const up = host.dispatch('pointerup', { pointerId: 7, x: 760, y: 440 });
  assert.equal(up.__d2HelpPointerHandled, true);
  assert.equal(help.pressed, false);
  assert.deepEqual(activations, ['help'], 'the matching pointer activates at native inclusive edges');
  assert.deepEqual(changes, [true, false]);

  host.dispatch('pointerdown', { pointerId: 9 });
  host.dispatch('pointercancel', { pointerId: 10 });
  assert.equal(help.pressed, true, 'cancelling a different pointer leaves the press intact');
  host.dispatch('pointercancel', { pointerId: 9 });
  assert.equal(help.pressed, false, 'pointercancel clears the matching press');
  host.dispatch('pointerup', { pointerId: 9 });
  assert.deepEqual(activations, ['help'], 'a cancelled press cannot activate later');
  assert.deepEqual(changes, [true, false, true, false]);

  host.dispatch('pointerdown', { pointerId: 10 });
  host.dispatch('lostpointercapture', { pointerId: 10 });
  assert.equal(help.pressed, false, 'lost capture also clears a matching press');
  assert.deepEqual(changes, [true, false, true, false, true, false]);

  host.dispatch('pointerdown', { pointerId: 11 });
  const outside = host.dispatch('pointerup', { pointerId: 11, x: 761, y: 440 });
  assert.equal(outside.__d2HelpPointerHandled, true);
  assert.equal(help.pressed, false);
  assert.deepEqual(activations, ['help'], 'release outside the callback rectangle does not activate');
});

test('Help release rechecks current eligibility so a newly opened modal suppresses activation', () => {
  const host = eventHost(), activations = [];
  let enabled = true;
  const help = bindHudHelpPointer(host, {
    point: (event) => [event.x, event.y], hit: () => HUD_HELP_HITBOX,
    enabled: () => enabled, canActivate: () => true,
    activate: () => activations.push('help'),
  });

  host.dispatch('pointerdown', { pointerId: 12 });
  assert.equal(help.pressed, true);
  enabled = false; // Models a modal/control overlay taking pointer ownership before release.
  const up = host.dispatch('pointerup', { pointerId: 12, x: 740, y: 420 });
  assert.equal(up.__d2HelpPointerHandled, true);
  assert.equal(help.pressed, false, 'the completed press clears even when release is no longer eligible');
  assert.deepEqual(activations, [], 'release-time modal state prevents Help activation');
});
