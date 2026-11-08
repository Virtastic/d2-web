import test from 'node:test';
import assert from 'node:assert/strict';
import { createSessionGames } from './bnet-session-games.mjs';
import { parseGameList, filterGames, encodeGameList } from './bnet-gamelist.mjs';

test('a game this session made is listed under an empty realm reply, newest first, one row per name', () => {
  const s = createSessionGames();
  s.setRealm(parseGameList(encodeGameList([])));
  assert.deepEqual(s.list(), []);
  assert.equal(s.remember({ name: 'Cows', password: false, difficulty: 0, description: 'a' }), true);
  s.remember({ name: 'Hell', password: true, difficulty: 2, description: '' });
  s.remember({ name: 'Cows', password: false, difficulty: 0, description: 'b' });
  assert.deepEqual(s.list().map((g) => [g.name, g.difficulty, g.description]), [['Cows', 0, 'b'], ['Hell', 2, '']]);
});

test('a realm row with the same name wins over the remembered one, other realm rows come first', () => {
  const s = createSessionGames();
  s.remember({ name: 'Cows', difficulty: 0, description: 'mine' });
  s.remember({ name: 'Mine', difficulty: 1, description: '' });
  s.setRealm([{ name: 'Realm', difficulty: 3, description: 'x', password: false }, { name: 'Cows', difficulty: 4, description: 'theirs', password: true }]);
  assert.deepEqual(s.list().map((g) => [g.name, g.difficulty, g.description]), [['Realm', 3, 'x'], ['Cows', 4, 'theirs'], ['Mine', 1, '']]);
});

test('a typed join with no listed details is not remembered, since the panel could not filter it', () => {
  const s = createSessionGames();
  assert.equal(s.remember({ name: 'Typed' }), false);
  assert.equal(s.remember(null), false);
  assert.deepEqual(s.list(), []);
});

test('the remembered games pass the panel filter: softcore sees digits up to its reach, hardcore 3 .. 3 + reach', () => {
  const s = createSessionGames();
  s.remember({ name: 'Normal', difficulty: 0 });
  s.remember({ name: 'Hell', difficulty: 2 });
  s.remember({ name: 'HC Normal', difficulty: 3 });
  s.remember({ name: 'HC Hell', difficulty: 5 });
  assert.deepEqual(filterGames(s.list(), { progression: 0 }).map((g) => g.name), ['Normal']);
  assert.deepEqual(filterGames(s.list(), { progression: 5 }).map((g) => g.name), ['Hell', 'Normal']);
  assert.deepEqual(filterGames(s.list(), { progression: 0, hardcore: true }).map((g) => g.name), ['HC Normal']);
});

test('list() returns copies, so the panel cannot change the session', () => {
  const s = createSessionGames();
  s.remember({ name: 'Cows', difficulty: 0 });
  s.list()[0].name = 'changed';
  assert.equal(s.list()[0].name, 'Cows');
});
