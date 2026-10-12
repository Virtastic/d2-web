import test from 'node:test';
import assert from 'node:assert/strict';
import { createNpcNameLabelCache } from '../play/assets/npc-name-label-cache.mjs';

test('paired automap label lookup preserves labels and reduces repeated engine resolutions', () => {
  const visibleNpcClasses = [155, 156, 210, 155, 210, 155, 156, 999, 999];
  const names = new Map([[155, 'Warriv'], [156, 'Akara'], [210, 'Atma']]);
  let baselineResolutions = 0, cachedResolutions = 0;
  const resolveBaseline = (classId) => { baselineResolutions++; return names.get(classId) ?? ''; };
  const resolveCached = (classId) => { cachedResolutions++; return names.get(classId) ?? ''; };

  const baseline = visibleNpcClasses.map(resolveBaseline);
  const cache = createNpcNameLabelCache(resolveCached);
  const cached = visibleNpcClasses.map((classId) => cache.get(classId));

  assert.deepEqual(cached, baseline, 'the same engine-resolved names, including unknown classes, are returned');
  assert.equal(baselineResolutions, visibleNpcClasses.length);
  assert.equal(cachedResolutions, 4, 'only the four distinct visible classes cross the resolver boundary');
  assert.equal(cache.size, 4);
});

test('NPC name cache promotes hits and bounds retained class names', () => {
  const resolved = [];
  const cache = createNpcNameLabelCache((classId) => { resolved.push(classId); return `name-${classId}`; }, 2);
  cache.get(1); cache.get(2); cache.get(1); cache.get(3);
  assert.equal(cache.size, 2);
  assert.equal(cache.get(1), 'name-1', 'a promoted class survives insertion of a newer class');
  assert.deepEqual(resolved, [1, 2, 3]);
  cache.get(2);
  assert.deepEqual(resolved, [1, 2, 3, 2], 'the least recently used class is evicted');
  cache.clear();
  assert.equal(cache.size, 0);
});
