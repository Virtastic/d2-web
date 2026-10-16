import test from 'node:test';
import assert from 'node:assert/strict';
import { createOptionsPages, nativeAudioSliderValue, optionsPageDrawOrder } from '../play/assets/options-pages.mjs';

test('native Sound/Music pointer positions select discrete integer percentages', () => {
  // Native sound/master records have 21 positions; hit travel is x352..617.
  for (const [x, expected] of [[341,0],[352,0],[358,0],[359,0.05],[500,0.55],[560,0.8],[610,0.95],[611,1],[617,1],[629,1]]) {
    assert.equal(nativeAudioSliderValue(x), expected, `pointer x=${x}`);
  }
  const values = new Set(Array.from({length:266}, (_,dx) => nativeAudioSliderValue(352+dx)));
  assert.equal(values.size,21);
  assert.ok(!values.has((500-341)/289), 'continuous historical value is rejected');
});

test('Video Resolution cycles modes and emits the display transition request', () => {
  const transitions = [];
  const frame = { width: 2, height: 2, offsetX: 0, offsetY: 0, indices: new Uint8Array(4), opaque: new Uint8Array(4) };
  const documentObject = { createElement: () => ({ width: 0, height: 0, getContext: () => ({ putImageData() {} }) }) };
  const previousImageData = globalThis.ImageData;
  globalThis.ImageData = class ImageData { constructor(data, width, height) { Object.assign(this, { data, width, height }); } };
  try {
    const page = createOptionsPages({ read: () => new Uint8Array([0]), decodeDc6: () => ({ frames: [frame] }),
      decodeDatPalette: () => new Uint8Array(1024), documentObject, onResolution: (mode) => transitions.push(mode) });
    page.open('video');
    assert.equal(page.state.resolution, 1);
    assert.ok(optionsPageDrawOrder('video', page.state).some((draw) => draw.path === '800x600'));

    assert.equal(page.pointerDown(400, 150, 800, 600, 7, 0), true);
    assert.deepEqual(transitions, [], 'pressing does not apply the display mode');
    assert.equal(page.pointerUp(400, 150, 800, 600, 7, 0), true);
    assert.deepEqual(transitions, [{ name: '640x480', width: 640, height: 480 }]);
    assert.equal(page.state.resolution, 0);
    assert.ok(!optionsPageDrawOrder('video', page.state).some((draw) => draw.path === '800x600' || draw.path === '640x480'),
      'unknown 640x480 cel identity stays out of the draw list');

    assert.equal(page.pointerDown(400, 150, 800, 600, 8, 0), true);
    assert.equal(page.pointerUp(400, 200, 800, 600, 8, 0), true, 'a release on another row consumes without applying');
    assert.equal(page.state.resolution, 0);
    assert.equal(transitions.length, 1);
    assert.equal(page.pointerDown(400, 150, 800, 600, 9, 0), true);
    assert.equal(page.pointerUp(400, 150, 800, 600, 10, 0), false, 'another pointer cannot finish the press');
    assert.equal(page.pointerCancel(9), true);
    assert.equal(page.pointerUp(400, 150, 800, 600, 9, 0), false, 'a canceled press cannot apply later');
    assert.equal(transitions.length, 1);

    assert.deepEqual(page.click(400, 150, 800, 600), { row: 'resolution', value: '800x600', width: 800, height: 600 });
    assert.equal(page.state.resolution, 1);
    assert.deepEqual(transitions[1], { name: '800x600', width: 800, height: 600 });

    assert.equal(page.click(400, 290, 800, 600), null, 'the dimmed Perspective row stays inert');
    assert.equal(page.state.resolution, 1);
    assert.equal(page.evidence.rows.find((row) => row.key === 'perspective').mode, 1);
  } finally {
    if (previousImageData === undefined) delete globalThis.ImageData;
    else globalThis.ImageData = previousImageData;
  }
});

test('Video Resolution initializes its displayed state from the saved choice', () => {
  const frame = { width: 2, height: 2, offsetX: 0, offsetY: 0, indices: new Uint8Array(4), opaque: new Uint8Array(4) };
  const documentObject = { createElement: () => ({ width: 0, height: 0, getContext: () => ({ putImageData() {} }) }) };
  const previousImageData = globalThis.ImageData;
  globalThis.ImageData = class ImageData { constructor(data, width, height) { Object.assign(this, { data, width, height }); } };
  try {
    const page = createOptionsPages({ read: () => new Uint8Array([0]), decodeDc6: () => ({ frames: [frame] }),
      decodeDatPalette: () => new Uint8Array(1024), documentObject, initialResolution: 0 });
    page.open('video');
    assert.equal(page.state.resolution, 0);
    assert.ok(!optionsPageDrawOrder('video', page.state).some((draw) => draw.path === '800x600' || draw.path === '640x480'));
  } finally {
    if (previousImageData === undefined) delete globalThis.ImageData;
    else globalThis.ImageData = previousImageData;
  }
});

test('NPC Speech follows retail native values and one-click Audio Only selection', () => {
  const transitions = [];
  const frame = { width: 2, height: 2, offsetX: 0, offsetY: 0, indices: new Uint8Array(4), opaque: new Uint8Array(4) };
  const documentObject = { createElement: () => ({ width: 0, height: 0, getContext: () => ({ putImageData() {} }) }) };
  const previousImageData = globalThis.ImageData;
  globalThis.ImageData = class ImageData { constructor(data, width, height) { Object.assign(this, { data, width, height }); } };
  try {
    const page = createOptionsPages({ read: () => new Uint8Array([0]), decodeDc6: () => ({ frames: [frame] }),
      decodeDatPalette: () => new Uint8Array(1024), documentObject, onNpcSpeech: (mode) => transitions.push(mode) });
    page.open('sound');
    assert.equal(page.state.npcspeech, 2, 'retail default is Audio and Text');
    assert.ok(optionsPageDrawOrder('sound', page.state).some((draw) => draw.path === 'AudioText'));

    // Retail record (s2134): one click from the default gives Text Only; cycle is 2 -> 1 -> 0 -> 2.
    for (const [value, path] of [[1, 'TextOnly'], [0, 'AudioOnly'], [2, 'AudioText']]) {
      assert.deepEqual(page.click(400, 384, 800, 600), { row: 'npcspeech', value });
      assert.ok(optionsPageDrawOrder('sound', page.state).some((draw) => draw.path === path));
    }
    assert.deepEqual(transitions, [1, 0, 2]);
  } finally {
    if (previousImageData === undefined) delete globalThis.ImageData;
    else globalThis.ImageData = previousImageData;
  }
});
