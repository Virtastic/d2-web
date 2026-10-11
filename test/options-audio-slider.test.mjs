import test from 'node:test';
import assert from 'node:assert/strict';
import { nativeAudioSliderValue } from '../play/assets/options-pages.mjs';

test('native Sound/Music pointer positions select discrete integer percentages', () => {
  // Native sound/master records have 21 positions; hit travel is x352..617.
  for (const [x, expected] of [[341,0],[352,0],[358,0],[359,0.05],[500,0.55],[560,0.8],[610,0.95],[611,1],[617,1],[629,1]]) {
    assert.equal(nativeAudioSliderValue(x), expected, `pointer x=${x}`);
  }
  const values = new Set(Array.from({length:266}, (_,dx) => nativeAudioSliderValue(352+dx)));
  assert.equal(values.size,21);
  assert.ok(!values.has((500-341)/289), 'continuous historical value is rejected');
});
