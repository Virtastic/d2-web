import test from 'node:test';
import assert from 'node:assert/strict';
import { createGatewaySelector, GATEWAY_LAYOUT, GATEWAY_STORAGE_KEY, initialGatewayIndex } from './bnet-servers.mjs';

// Retail 1.13c: the GATEWAY bar opens the list, a row click highlights, OK commits and persists
// (D2Launch FUN_6FA58810 -> SetCurGateway + "Preferred Realm"), CANCEL keeps the prior choice.
const servers = [
  { region: 'Example.org', host: 'a.local', ws: 'wss://a.local' },
  { region: 'U.S. East', host: 'b.local', ws: 'wss://b.local' },
  { region: 'Europe', host: 'c.local', ws: 'wss://c.local' },
];
const memoryStorage = (init = {}) => {
  const m = new Map(Object.entries(init));
  return { getItem: (k) => (m.has(k) ? m.get(k) : null), setItem: (k, v) => m.set(k, String(v)), map: m };
};
const rowPoint = (i) => [GATEWAY_LAYOUT.rows.x + 10, GATEWAY_LAYOUT.rows.hitTop + i * GATEWAY_LAYOUT.rows.pitch + 10];
const buttonPoint = (key) => [GATEWAY_LAYOUT.button[key].x + 10, GATEWAY_LAYOUT.button.y + 10];
const click = (sel, [x, y]) => sel.release(sel.press(x, y), x, y);

test('boot picks the persisted region, else U.S. EAST, else the first; none configured is -1', () => {
  assert.equal(initialGatewayIndex(servers, memoryStorage({ [GATEWAY_STORAGE_KEY]: 'EUROPE' })), 2);
  assert.equal(initialGatewayIndex(servers, memoryStorage()), 1);
  assert.equal(initialGatewayIndex(servers, memoryStorage({ [GATEWAY_STORAGE_KEY]: 'Gone' })), 1);
  assert.equal(initialGatewayIndex([servers[0]], null), 0);
  assert.equal(initialGatewayIndex([], memoryStorage()), -1);
  const throwing = { getItem() { throw new Error('blocked'); } };
  assert.equal(initialGatewayIndex(servers, throwing), 1);
});

test('every row is clickable and moves the highlight, without committing', () => {
  const storage = memoryStorage();
  const sel = createGatewaySelector({ servers, storage });
  assert.equal(sel.openDialog(), true);
  for (const i of [0, 2, 1, 0]) {
    assert.equal(click(sel, rowPoint(i)), i);
    assert.equal(sel.evidence.highlighted, servers[i].region);
  }
  assert.equal(sel.selected.region, 'U.S. East');
  assert.equal(storage.map.size, 0);
  assert.equal(sel.open, true);
});

test('OK commits, persists and relabels the bar; a new selector boots on it', () => {
  const storage = memoryStorage();
  const commits = [];
  const sel = createGatewaySelector({ servers, storage, onCommit: (s) => commits.push(s.region) });
  sel.openDialog();
  click(sel, rowPoint(0));
  click(sel, buttonPoint('ok'));
  assert.equal(sel.open, false);
  assert.equal(sel.selected.region, 'Example.org');
  assert.equal(sel.gatewayBar(), 'GATEWAY: EXAMPLE.ORG');
  assert.equal(storage.getItem(GATEWAY_STORAGE_KEY), 'Example.org');
  assert.deepEqual(commits, ['Example.org']);
  const next = createGatewaySelector({ servers, storage });
  assert.equal(next.selected.region, 'Example.org');
  assert.equal(next.gatewayBar(), 'GATEWAY: EXAMPLE.ORG');
  assert.equal(next.open, false);
});

test('CANCEL closes and keeps the committed region and storage', () => {
  const storage = memoryStorage({ [GATEWAY_STORAGE_KEY]: 'Europe' });
  const sel = createGatewaySelector({ servers, storage });
  sel.openDialog();
  click(sel, rowPoint(0));
  click(sel, buttonPoint('cancel'));
  assert.equal(sel.open, false);
  assert.equal(sel.selected.region, 'Europe');
  assert.equal(storage.getItem(GATEWAY_STORAGE_KEY), 'Europe');
  sel.openDialog();
  assert.equal(sel.evidence.highlighted, 'Europe');
});

test('a storage write failure still commits in memory', () => {
  const storage = { getItem: () => null, setItem() { throw new Error('quota'); } };
  const sel = createGatewaySelector({ servers, storage });
  sel.openDialog();
  click(sel, rowPoint(2));
  click(sel, buttonPoint('ok'));
  assert.equal(sel.selected.region, 'Europe');
});
