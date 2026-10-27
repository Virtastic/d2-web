import test from 'node:test';
import assert from 'node:assert/strict';
import { createItemPropertyCache, itemPropsCacheable } from '../play/assets/item-property-cache.mjs';

test('item tooltip cache keeps the most recently used IDs within its bound', () => {
  const cache = createItemPropertyCache(3);
  const a = { title: 'A', lines: ['one'] }, b = { title: 'B', lines: ['two'] };
  cache.set(1, a);
  cache.set(2, b);
  cache.set(3, { title: 'C', lines: [] });
  assert.equal(cache.get(1), a); // Access refreshes its recency.
  cache.set(4, { title: 'D', lines: [] });

  assert.equal(cache.size, 3);
  assert.equal(cache.get(2), undefined); // Least recently used entry was evicted.
  assert.equal(cache.get(1), a);
  assert.equal(cache.get(3).title, 'C');
  assert.equal(cache.get(4).title, 'D');
});

test('item tooltip cache re-reads a changed item under the same ID and keeps unchanged reads hot', () => {
  const cache = createItemPropertyCache(3);
  let engineValue = { title: 'Unidentified', lines: ['Durability: 20 of 20'], quality: 2 };
  let reads = 0;
  const readItem = (id) => cache.getOrRead(id, () => { reads++; return { ...engineValue }; });

  const first = readItem(41);
  assert.equal(readItem(41), first);
  assert.equal(reads, 1);

  // A successful engine mutation can preserve the GUID while changing the tooltip payload.
  engineValue = { title: 'Fine Sword', lines: ['Durability: 17 of 20', '+5 to Attack Rating'], quality: 4 };
  cache.clear();
  const updated = readItem(41);
  assert.notEqual(updated, first);
  assert.equal(updated.title, 'Fine Sword');
  assert.deepEqual(updated.lines, ['Durability: 17 of 20', '+5 to Attack Rating']);
  assert.equal(updated.quality, 4);
  assert.equal(readItem(41), updated);
  assert.equal(reads, 2);
});

test('item tooltip cache validates a positive integer capacity', () => {
  assert.throws(() => createItemPropertyCache(0), /positive/);
  assert.throws(() => createItemPropertyCache(1.5), /positive/);
});

test('unnamed quality 4+ reads are not cached, so identification shows the unique/rare name', () => {
  assert.equal(itemPropsCacheable({ lines: ['x'], title: '', quality: 7 }), false);
  assert.equal(itemPropsCacheable({ lines: ['x'], title: 'Stormshield', quality: 7 }), true);
  assert.equal(itemPropsCacheable({ lines: ['x'], title: '', quality: 2 }), true);
  assert.equal(itemPropsCacheable({ lines: [], title: 'A', quality: 4 }), false);
});
