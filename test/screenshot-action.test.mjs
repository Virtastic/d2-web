import test from 'node:test';
import assert from 'node:assert/strict';
import { createScreenshotAction } from '../play/assets/screenshot-action.mjs';

test('Print Screen downloads numbered JPEGs and applies the native one-second throttle', async () => {
  let now = 5000;
  const downloads = [], revoked = [], created = [];
  const urlObject = {
    createObjectURL(blob) { created.push(blob); return `blob:${created.length}`; },
    revokeObjectURL(url) { revoked.push(url); },
  };
  const documentObject = { createElement(tag) {
    assert.equal(tag, 'a');
    const link = { click() { downloads.push({ href: link.href, filename: link.download }); } };
    return link;
  } };
  const capture = createScreenshotAction({ documentObject, urlObject, now: () => now });
  const canvas = { toBlob(callback, type, quality) {
    assert.equal(type, 'image/jpeg');
    assert.equal(quality, 1);
    callback(new Blob([new Uint8Array([0xff, 0xd8, 0xff, 0xd9])], { type }));
  } };

  assert.deepEqual(capture(canvas), { captured: true, filename: 'Screenshot000.jpg' });
  assert.deepEqual(capture(canvas), { captured: false, reason: 'rate-limited' });
  now += 999;
  assert.deepEqual(capture(canvas), { captured: false, reason: 'rate-limited' });
  now += 1;
  assert.deepEqual(capture(canvas), { captured: true, filename: 'Screenshot001.jpg' });
  assert.deepEqual(downloads, [
    { href: 'blob:1', filename: 'Screenshot000.jpg' },
    { href: 'blob:2', filename: 'Screenshot001.jpg' },
  ]);
  assert.equal(created.length, 2);
  assert.ok(created.every((blob) => blob.type === 'image/jpeg'));
  assert.ok(created.every((blob) => blob.size === 4));
  for (const blob of created) {
    const bytes = new Uint8Array(await blob.arrayBuffer());
    assert.deepEqual([...bytes], [0xff, 0xd8, 0xff, 0xd9]);
  }
  await new Promise((resolve) => setTimeout(resolve, 5));
  assert.deepEqual(revoked, ['blob:1', 'blob:2']);
});

test('Print Screen rejects a missing canvas without consuming the next valid capture', () => {
  const capture = createScreenshotAction({ now: () => 5000 });
  assert.deepEqual(capture(null), { captured: false, reason: 'canvas-unavailable' });
});
