import test from 'node:test';
import assert from 'node:assert/strict';
import { playerWeaponClassToken } from '../play/assets/player-weapon-class.mjs';
import { createPlayerCompositeResolver } from '../play/assets/player-composite.mjs';

const tables = {
  'data\\global\\excel\\armor.txt': [
    'code\tcomponent\talternategfx\trarm\tlarm\ttorso\tlegs\trspad\tlspad',
    'kit\t7\tkit\t0\t0\t0\t0\t0\t0',
  ].join('\n'),
  'data\\global\\excel\\weapons.txt': [
    'code\tcomponent\talternategfx\twclass\t2handed',
    'hax\t5\thax\t1hs\t0',
    'ktr\t5\tktr\t1ht\t0',
    '2hs\t5\t2hs\t1hs\t1',
    '2ax\t5\t2ax\t1hs\t0',
    'sbw\t6\tsbw\tbow\t1',
  ].join('\n'),
  'data\\global\\excel\\ArmType.txt': 'token\nlit\nmed\nhvy',
};

test('engine weapon-class enum maps to retail animation tokens', () => {
  assert.deepEqual([2, 5, 10, 12, 13].map(playerWeaponClassToken), ['1hs', '2hs', '1ss', 'ht1', 'ht2']);
  assert.throws(() => playerWeaponClassToken(-1), /unknown engine weapon class id/);
});

test('composite uses engine class for animation and worn records only for hand art', () => {
  const resolver = createPlayerCompositeResolver((path) => new TextEncoder().encode(tables[path]));
  const dualAxes = [
    { id: 1, code: 'hax', mode: 1, bodyLoc: 4 },
    { id: 2, code: 'hax', mode: 1, bodyLoc: 5 },
  ];
  const barb = resolver.resolve(dualAxes, {}, '1ss', 1);
  assert.equal(barb.weaponClass, '1ss');
  assert.deepEqual([barb.components[5], barb.components[6]], ['hax', 'hax']);

  const axeAndShield = resolver.resolve([
    { code: 'hax', mode: 1, bodyLoc: 4 },
    { code: 'kit', mode: 1, bodyLoc: 5 },
  ], {}, '1hs');
  assert.equal(axeAndShield.weaponClass, '1hs');
  assert.deepEqual([axeAndShield.components[5], axeAndShield.components[7]], ['hax', 'kit']);

  const assassin = resolver.resolve([
    { id: 3, code: 'ktr', mode: 1, bodyLoc: 4 },
    { id: 4, code: 'ktr', mode: 1, bodyLoc: 5 },
  ], {}, 'ht2', 3);
  assert.equal(assassin.weaponClass, 'ht2');
  assert.deepEqual([assassin.components[5], assassin.components[6]], ['ktr', 'ktr']);

  // This is the engine's distinct two-hand class token. D2Game 0x1B setup is
  // covered separately by s2118; its current packet path refuses the fixture.
  const twoHand = resolver.resolve([{ code: '2hs', mode: 1, bodyLoc: 4 }], {}, '2hs');
  assert.equal(twoHand.weaponClass, '2hs');
  assert.equal(twoHand.components[5], '2hs');
});

test('composite rejects a missing engine weapon class', () => {
  const resolver = createPlayerCompositeResolver((path) => new TextEncoder().encode(tables[path]));
  assert.throws(() => resolver.resolve([], {}, undefined), /engine weapon class is required/);
});

test('weapon component and native selected hand drive hand-layer identity', () => {
  const resolver = createPlayerCompositeResolver((path) => new TextEncoder().encode(tables[path]));
  const bow = resolver.resolve([{ id: 31, code: 'sbw', mode: 1, bodyLoc: 4 }], {}, 'bow', 31);
  assert.deepEqual([bow.components[5], bow.components[6]], [undefined, 'sbw']);
  assert.deepEqual([bow.source[5], bow.source[6]], [undefined, 'sbw']);

  // The selected item can occupy either body location. It keeps its native
  // component; the other weapon in a dual pair is assigned to the LH layer.
  const dual = resolver.resolve([
    { id: 41, code: 'hax', mode: 1, bodyLoc: 4 },
    { id: 42, code: '2ax', mode: 1, bodyLoc: 5 },
  ], {}, '1ss', 42);
  assert.deepEqual([dual.components[5], dual.components[6]], ['2ax', 'hax']);
  assert.deepEqual([dual.source[5], dual.source[6]], ['2ax', 'hax']);
  assert.equal(dual.primaryWeaponId, 42);
  assert.deepEqual(dual.handAssignments.map(({ id, component, selected }) => ({ id, component, selected })), [
    { id: 41, component: 6, selected: false }, { id: 42, component: 5, selected: true },
  ]);

  const noSelectedHand = resolver.resolve([
    { id: 51, code: 'hax', mode: 1, bodyLoc: 4 },
    { id: 52, code: '2ax', mode: 1, bodyLoc: 5 },
  ], {}, '1ss', 0);
  assert.deepEqual(noSelectedHand.handAssignments.map(({ component, selected }) => ({ component, selected })), [
    { component: 6, selected: false }, { component: 6, selected: false },
  ]);
});

test('active equipment body slots outrank weapon-switch flags', () => {
  const resolver = createPlayerCompositeResolver((path) => new TextEncoder().encode(tables[path]));
  const swappedBack = resolver.resolve([
    // The native client selects drawable equipment by body-grid node/body
    // location, not these flag values.
    { id: 61, code: 'hax', mode: 1, bodyLoc: 4, itemFlags: 0xc0 },
    { id: 62, code: 'kit', mode: 1, bodyLoc: 5, itemFlags: 0xc0 },
    // The alternate set remains in SWRARM/SWLARM and must not replace RH/SH.
    { id: 63, code: '2ax', mode: 1, bodyLoc: 11, itemFlags: 0x80 },
    { id: 64, code: 'kit', mode: 1, bodyLoc: 12, itemFlags: 0x40 },
  ], {}, '1hs', 61);
  assert.equal(swappedBack.components[5], 'hax');
  assert.equal(swappedBack.source[5], 'hax');
  assert.equal(swappedBack.components[7], 'kit');
  assert.equal(swappedBack.source[7], 'kit');
  assert.equal(swappedBack.components[6], undefined);
  assert.equal(swappedBack.handAssignments.length, 1);
});

test('player composite resolves a large inventory in one source-list pass', () => {
  const resolver = createPlayerCompositeResolver((path) => new TextEncoder().encode(tables[path]));
  const inventory = [
    ...Array.from({ length: 80 }, (_, id) => ({ id, code: 'hax', mode: 0, bodyLoc: 4 })),
    { id: 100, code: 'kit', mode: 1, bodyLoc: 3 },
    { id: 101, code: 'hax', mode: 1, bodyLoc: 4 },
    { id: 102, code: '2ax', mode: 1, bodyLoc: 5 },
  ];
  let traversals = 0;
  const source = { *[Symbol.iterator]() { traversals++; yield* inventory; } };

  const composite = resolver.resolve(source, {}, '1ss', 102);

  assert.equal(traversals, 1);
  assert.deepEqual([composite.components[5], composite.components[6]], ['2ax', 'hax']);
  assert.deepEqual(composite.handAssignments.map(({ id, component, selected }) => ({ id, component, selected })), [
    { id: 101, component: 6, selected: false },
    { id: 102, component: 5, selected: true },
  ]);
});
