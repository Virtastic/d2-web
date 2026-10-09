import test from 'node:test';
import assert from 'node:assert/strict';
import { createVideoResolutionSettings, VIDEO_RESOLUTION_STORAGE_KEY } from '../play/assets/video-resolution-settings.mjs';

test('resolution preference defaults without pinning a display mode', () => {
  const settings = createVideoResolutionSettings({ storage: null });
  assert.equal(settings.index, 1);
  assert.equal(settings.mode.name, '800x600');
  assert.equal(settings.selected, false);
});

test('resolution choice persists and restores the native two-choice setting', () => {
  const values = new Map();
  const storage = { getItem: (key) => values.get(key) ?? null, setItem: (key, value) => values.set(key, value) };
  const settings = createVideoResolutionSettings({ storage });
  assert.equal(settings.set(0).name, '640x480');
  assert.equal(values.get(VIDEO_RESOLUTION_STORAGE_KEY), '0');

  const restored = createVideoResolutionSettings({ storage });
  assert.equal(restored.index, 0);
  assert.equal(restored.mode.width, 640);
  assert.equal(restored.selected, true);
});

test('invalid or unavailable saved settings fall back to the default mode', () => {
  for (const raw of ['not-json', '2', '-1']) {
    const settings = createVideoResolutionSettings({ storage: { getItem: () => raw } });
    assert.equal(settings.index, 1);
    assert.equal(settings.selected, false);
  }
  const blocked = createVideoResolutionSettings({ storage: { getItem() { throw new Error('blocked'); } } });
  assert.equal(blocked.mode.name, '800x600');
  const writeBlocked = createVideoResolutionSettings({ storage: { getItem: () => null, setItem() { throw new Error('full'); } } });
  assert.equal(writeBlocked.set(0).name, '640x480');
  assert.equal(writeBlocked.selected, true, 'the live choice still takes effect if persistence fails');
});
