import test from 'node:test';
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import { importCharacterFiles, validateD2s } from '../play/assets/character-import.mjs';

const good = new Uint8Array(readFileSync(new URL('./fixtures/TestPally.d2s', import.meta.url)));
const fakeStore = () => {
  const rows = new Map(), aux = new Map();
  return { rows, aux,
    async get(n) { return rows.get(n) ?? null; },
    async put(b) { rows.set('TestPally', Uint8Array.from(b)); },
    async putAux(k, b) { aux.set(k, Uint8Array.from(b)); } };
};

test('a good file validates: class and level come from the header', () => {
  const r = validateD2s(good);
  assert.equal(r.ok, true);
  assert.deepEqual([r.header.name, r.header.className, r.header.level], ['TestPally', 'Paladin', 50]);
});

test('a good file is stored', async () => {
  const store = fakeStore();
  const r = await importCharacterFiles([{ name: 'TestPally.d2s', bytes: good }], { store });
  assert.equal(r.status, 'imported');
  assert.deepEqual(store.rows.get('TestPally'), good);
});

test('a bad checksum is refused and nothing is stored', async () => {
  const bad = Uint8Array.from(good); bad[100] ^= 1;
  const store = fakeStore();
  const r = await importCharacterFiles([{ name: 'x.d2s', bytes: bad }], { store });
  assert.deepEqual([r.status, r.error], ['error', 'bad .d2s checksum']);
  assert.equal(store.rows.size, 0);
});

test('a wrong signature is refused and nothing is stored', async () => {
  const bad = Uint8Array.from(good); bad[0] = 0;
  const store = fakeStore();
  const r = await importCharacterFiles([{ name: 'x.d2s', bytes: bad }], { store });
  assert.equal(r.status, 'error'); assert.match(r.error, /signature/);
  assert.equal(store.rows.size, 0);
});

test('a truncated file is refused on size', () => {
  assert.match(validateD2s(good.slice(0, 600)).error, /size/);
});

test('a name clash needs the confirm', async () => {
  const store = fakeStore(); store.rows.set('TestPally', new Uint8Array([1]));
  const no = await importCharacterFiles([{ name: 'a.d2s', bytes: good }], { store, confirmOverwrite: () => false });
  assert.equal(no.status, 'cancelled'); assert.deepEqual([...store.rows.get('TestPally')], [1]);
  const yes = await importCharacterFiles([{ name: 'a.d2s', bytes: good }], { store, confirmOverwrite: () => true });
  assert.equal(yes.status, 'imported'); assert.deepEqual(store.rows.get('TestPally'), good);
});

test('sidecars ride along when dropped together', async () => {
  const store = fakeStore();
  await importCharacterFiles([{ name: 'TestPally.d2s', bytes: good }, { name: 'TestPally.d2x', bytes: new Uint8Array([7]) },
    { name: 'shared.sss', bytes: new Uint8Array([8]) }], { store });
  assert.deepEqual([...store.aux.keys()], ['_LOD_TestPally.d2x', '_LOD_SharedStashSave.sss']);
});
