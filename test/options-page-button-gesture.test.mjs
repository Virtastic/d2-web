import test from 'node:test';
import assert from 'node:assert/strict';
import { createGameMenu } from '../play/assets/game-menu.mjs';
import { createOptionsPages } from '../play/assets/options-pages.mjs';

function menu() {
  const frame = { width: 300, height: 50, offsetX: 0, offsetY: 0, indices: new Uint8Array(300 * 50), opaque: new Uint8Array(300 * 50) };
  const previousImageData = globalThis.ImageData;
  globalThis.ImageData = class ImageData { constructor(data, width, height) { Object.assign(this, { data, width, height }); } };
  const instance = createGameMenu({
    read: () => new Uint8Array(), decodeDatPalette: () => new Uint8Array(1024),
    decodeDc6: () => ({ frames: [frame, frame] }),
    documentObject: { createElement: () => ({ getContext: () => ({ putImageData() {} }) }) },
  });
  return { instance, restore() {
    if (previousImageData === undefined) delete globalThis.ImageData;
    else globalThis.ImageData = previousImageData;
  } };
}

function openOptionsPage(instance, pageLabel) {
  instance.toggle();
  let point = instance.targets(800, 600).OPTIONS;
  instance.pointerDown(...point, 800, 600, 1, 0);
  instance.pointerUp(...point, 800, 600, 1, 0);
  point = instance.targets(800, 600)[pageLabel];
  instance.pointerDown(...point, 800, 600, 2, 0);
  instance.pointerUp(...point, 800, 600, 2, 0);
}

test('Sound and Video option rows activate only on matching release and keep the pressed marker', () => {
  const { instance, restore } = menu();
  try {
    openOptionsPage(instance, 'SOUND OPTIONS');
    const npcSpeech = instance.targets(800, 600).npcspeech;
    assert.equal(instance.evidence.sub.state.npcspeech, 2);
    assert.equal(instance.pointerDown(...npcSpeech, 800, 600, 7, 0), true);
    assert.equal(instance.evidence.sub.selected, 'npcspeech');
    assert.equal(instance.evidence.sub.state.npcspeech, 2, 'press selects feedback without applying the choice');
    instance.hover(...instance.targets(800, 600).previous, 800, 600);
    assert.equal(instance.evidence.sub.selected, 'npcspeech', 'movement does not move the held marker');
    instance.pointerUp(...npcSpeech, 800, 600, 7, 0);
    assert.equal(instance.evidence.sub.state.npcspeech, 1); // retail record (s2134): first click from default is Text Only

    const previous = instance.targets(800, 600).previous;
    assert.equal(instance.pointerDown(...previous, 800, 600, 8, 0), true);
    assert.equal(instance.evidence.sub.open, true);
    assert.equal(instance.pointerUp(...previous, 800, 600, 8, 0), 'previous');
    assert.equal(instance.evidence.sub.open, false, 'Previous closes on release');
  } finally { restore(); }
});

test('Sound/Video choice release on another row cancels and sliders retain down-edge updates', () => {
  const applied = [];
  const { instance, restore } = menu();
  try {
    openOptionsPage(instance, 'VIDEO OPTIONS');
    const lighting = instance.targets(800, 600).lighting;
    const shadows = instance.targets(800, 600).shadows;
    assert.equal(instance.pointerDown(...lighting, 800, 600, 9, 0), true);
    assert.equal(instance.pointerUp(...shadows, 800, 600, 9, 0), true);
    assert.equal(instance.evidence.sub.state.lighting, 0, 'a different-row release cancels');

  } finally { restore(); }

  const page = createOptionsPages({ read: () => new Uint8Array(), decodeDc6: () => ({ frames: [] }),
    decodeDatPalette: () => new Uint8Array(1024), documentObject: { createElement: () => ({}) },
    initialVolume: { sound: 0.25 }, onVolume: (key, value) => applied.push([key, value]) });
  page.open('sound');
  assert.equal(page.pointerDown(631, 159, 800, 600, 10, 0), false, 'slider remains on immediate click path');
  assert.deepEqual(page.click(631, 159, 800, 600), { row: 'sound', value: 1 });
  assert.deepEqual(applied, [['sound', 1]]);
});
