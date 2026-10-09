import test from 'node:test';
import assert from 'node:assert/strict';
import { createRenderCache } from '../play/assets/render-cache.mjs';

test('render cache retains only the newest entries and refreshes hits', () => {
  const cache = createRenderCache(2);
  const first = {};
  cache.set('a', first);
  cache.set('b', 2);
  assert.equal(cache.get('a'), first);
  cache.set('c', 3);
  assert.equal(cache.size, 2);
  assert.equal(cache.get('a'), first, 'a hit refreshes its position');
  assert.equal(cache.get('b'), undefined, 'the least recently used entry was evicted');
  assert.equal(cache.get('c'), 3);
});

test('a cached undefined value still refreshes its LRU position', () => {
  const cache = createRenderCache(2);
  cache.set('undefined', undefined);
  cache.set('b', 'b');
  assert.equal(cache.get('undefined'), undefined);
  cache.set('c', 'c');
  assert.equal(cache.size, 2);
  assert.equal(cache.get('b'), undefined, 'the untouched entry was evicted');
  assert.equal(cache.get('c'), 'c');
  assert.equal(cache.get('undefined'), undefined, 'the cached entry survived as a hit');
});

test('render cache validates a finite positive entry limit', () => {
  assert.throws(() => createRenderCache(0), /positive integer/);
  assert.throws(() => createRenderCache(1.5), /positive integer/);
});

test('render cache evicts by retained byte budget and skips an oversized value', () => {
  const cache = createRenderCache(10, 5, (value) => value.bytes);
  cache.set('a', { bytes: 3 });
  cache.set('b', { bytes: 2 });
  cache.set('c', { bytes: 3 });
  assert.equal(cache.get('a'), undefined);
  assert.deepEqual(cache.get('b'), { bytes: 2 });
  assert.deepEqual(cache.get('c'), { bytes: 3 });
  cache.set('large', { bytes: 6 });
  assert.equal(cache.size, 2);
  assert.equal(cache.get('large'), undefined);
});
