import { test } from 'node:test';
import assert from 'node:assert/strict';
import { createHudHelp, HUD_HELP_HITBOX, hudHelpButtonContains } from '../play/assets/hud-help.mjs';
import { bindHudHelpPointer } from '../play/assets/hud-help-pointer.mjs';

function eventHost() {
  const listeners = new Map(), captures = [];
  return {
    captures,
    addEventListener(type, listener) { listeners.set(type, listener); },
    setPointerCapture(pointerId) { captures.push(pointerId); },
    dispatch(type, values = {}) {
      const event = { button: 0, pointerId: 1, x: 725, y: 404, preventDefault() { this.defaultPrevented = true; }, ...values };
      listeners.get(type)?.(event);
      return event;
    },
  };
}

function dc6Fixture(frameIndices, width = 2, height = 2) {
  const table = 24 + frameIndices.length * 4;
  const encodedLength = height * (width + 2), stride = 32 + encodedLength + 3;
  const bytes = new Uint8Array(table + stride * frameIndices.length);
  const view = new DataView(bytes.buffer);
  view.setInt32(0, 6, true); view.setUint32(16, 1, true); view.setUint32(20, frameIndices.length, true);
  for (let i = 0; i < frameIndices.length; i++) {
    const at = table + stride * i;
    const frame = Uint8Array.from(Array.from({ length: height }, () => [width, ...Array(width).fill(frameIndices[i]), 0x80]).flat());
    view.setUint32(24 + i * 4, at, true);
    view.setUint32(at + 4, width, true); view.setUint32(at + 8, height, true);
    view.setUint32(at + 28, frame.length, true); bytes.set(frame, at + 32);
  }
  return bytes;
}

function fontTable() {
  const chars = [...new Set('Help (H)')], bytes = new Uint8Array(12 + chars.length * 14);
  bytes.set(new TextEncoder().encode('Woo!')); bytes[10] = 2; bytes[11] = 2;
  const view = new DataView(bytes.buffer);
  chars.forEach((char, i) => {
    const at = 12 + i * 14;
    view.setUint16(at, char.charCodeAt(0), true); bytes[at + 3] = 2; bytes[at + 4] = 2;
  });
  return bytes;
}

function fixtureRenderer() {
  const assets = {
    'data\\local\\font\\latin\\font16.dc6': dc6Fixture([17]),
    'data\\local\\font\\latin\\font16.tbl': fontTable(),
    'data\\global\\ui\\PANEL\\levelsocket.dc6': dc6Fixture([21, 22], 35, 36),
    'data\\global\\ui\\PANEL\\level.dc6': dc6Fixture([31, 41, 51], 30, 30),
    'data\\global\\palette\\ACT1\\pal.dat': new Uint8Array(768),
  };
  const requests = [];
  const read = (path) => { requests.push(path); if (!(path in assets)) throw new Error(`unexpected asset ${path}`); return assets[path]; };
  const documentObject = { createElement() { return { width: 0, height: 0, getContext: () => ({ putImageData() {} }) }; } };
  return { renderer: createHudHelp(read, documentObject), requests };
}

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

test('Help button uses socket frame 1 while held and keeps Level gem on frame 0', (t) => {
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
  assert.equal(pressed.button[0].frame, 1, 'the socket uses its native held frame');
  assert.equal(pressed.button[1].frame, 0, 'the Help draw routine keeps the gem at frame zero');
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
