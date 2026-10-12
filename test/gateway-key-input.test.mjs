import test from 'node:test';
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import vm from 'node:vm';
import { createGatewaySelector, GATEWAY_LAYOUT } from '../play/assets/bnet-servers.mjs';

const servers = [
  { region: 'U.S. East', host: 'east.local', ws: 'ws://east.local' },
  { region: 'Europe', host: 'europe.local', ws: 'ws://europe.local' },
];
function setup() {
  const commits = [];
  const selector = createGatewaySelector({ servers, onCommit: (server) => commits.push(server) });
  selector.openDialog();
  const row = GATEWAY_LAYOUT.rows;
  const x = row.x + 10, y = row.hitTop + row.pitch + 10;
  selector.release(selector.press(x, y), x, y);
  assert.equal(selector.evidence.highlighted, 'Europe');
  assert.equal(selector.selected.region, 'U.S. East');
  return { selector, commits };
}

for (const code of ['Enter', 'NumpadEnter']) {
  test(`${code} commits the highlighted gateway once and closes the dialog`, () => {
    const { selector, commits } = setup();
    assert.equal(selector.key(code), true);
    assert.equal(selector.open, false);
    assert.equal(selector.gatewayBar(), 'GATEWAY: EUROPE');
    assert.deepEqual(commits, [servers[1]]);
    assert.equal(selector.key(code), false);
    assert.equal(commits.length, 1);
  });
}

test('Escape cancels the staged gateway without calling its commit action', () => {
  const { selector, commits } = setup();
  assert.equal(selector.key('Escape'), true);
  assert.equal(selector.open, false);
  assert.equal(selector.selected.region, 'U.S. East');
  assert.deepEqual(commits, []);
  selector.openDialog();
  assert.equal(selector.evidence.highlighted, 'U.S. East');
});

test('unbound keys leave the modal choice intact; keyboard cancellation invalidates a pointer arm', () => {
  const { selector, commits } = setup();
  assert.equal(selector.key('KeyI'), false);
  assert.equal(selector.open, true);
  const b = GATEWAY_LAYOUT.button;
  const x = b.ok.x + 10, y = b.y + 10;
  const token = selector.press(x, y);
  selector.key('Escape');
  assert.equal(selector.release(token, x, y), -1);
  assert.deepEqual(commits, []);
});

test('pointer OK and Cancel retain the same commit and rollback behavior', () => {
  for (const key of ['ok', 'cancel']) {
    const { selector, commits } = setup();
    const b = GATEWAY_LAYOUT.button;
    const x = b[key].x + 10, y = b.y + 10;
    const result = selector.release(selector.press(x, y), x, y);
    assert.equal(selector.open, false);
    assert.equal(result, key === 'ok' ? 1 : -1);
    assert.deepEqual(commits, key === 'ok' ? [servers[1]] : []);
    assert.equal(selector.selected.region, key === 'ok' ? 'Europe' : 'U.S. East');
  }
});

test('production keydown forwards menu keys through the modal before the gameplay gate', () => {
  const source = readFileSync(new URL('../play/gameplay-shell.mjs', import.meta.url), 'utf8');
  const start = source.indexOf("addEventListener('keydown', (e) => {");
  const end = source.indexOf('  // F218:', start);
  assert.ok(start >= 0 && end > start);
  const { selector, commits } = setup();
  let handler, prevented = 0, renders = 0, titleMusicStarts = 0;
  const context = {
    addEventListener: (_type, callback) => { handler = callback; },
    screen: 'title', SCREEN: { TITLE: 'title', GAME: 'game' }, titlePhase: 'menu',
    actMusic: { playTitle: () => { titleMusicStarts++; return Promise.resolve(); } }, musicError: '',
    bootMovieSpec: () => null, titleScreen: { key: (code) => selector.key(code) },
    render: () => { renders++; },
  };
  vm.runInNewContext(source.slice(start, end) + '});', context);
  handler({ code: 'Enter', preventDefault() { prevented++; } });
  assert.deepEqual(commits, [servers[1]]);
  assert.equal(prevented, 1);
  assert.equal(renders, 1);
  assert.equal(titleMusicStarts, 1, 'menu keyboard input also retries music unlock from a user gesture');
  handler({ code: 'Escape', preventDefault() { prevented++; } });
  assert.equal(prevented, 1, 'closed gateway does not bind keys to title menu entries');
});
