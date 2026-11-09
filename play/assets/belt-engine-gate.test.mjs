import test from 'node:test';
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';

// The single-player belt command takes the belt slot as the key column exactly, and refuses a dead hero or a held item.
const source = readFileSync(new URL('../../src/core/monspawn/monspawn_check.cpp', import.meta.url), 'utf8');
const body = source.slice(source.indexOf('int d2_session_command_use_belt('), source.indexOf('d2_session_command_use_item('));

test('belt command refuses a dead hero and a held item before looking at the slot', () => {
  const gate = body.indexOf('PLRMODE_DEAD');
  assert.ok(gate > 0 && gate < body.indexOf('INVENTORY_GetItemFromBeltSlot'));
  assert.match(body, /INVENTORY_GetCursorItem\(g_targetPlayer->pInventory\)/);
});

test('belt command looks up the slot itself, with no scan of the cells above it', () => {
  assert.equal(body.match(/INVENTORY_GetItemFromBeltSlot/g).length, 1);
  assert.doesNotMatch(body, /for\s*\(/);
});

test('belt command mirrors the use routine refusals before the packet gate', () => {
  const packet = body.indexOf('sessionDispatchGate(0x26)');
  for (const needle of ['nUseable != 1', 'nQuest == 1', 'nUnique == 1', 'STATLIST_GetStatListFromUnitAndFlag(item, 2)', 'STATES_CheckState(g_targetPlayer, 0x36)']) {
    const at = body.indexOf(needle);
    assert.ok(at > 0 && at < packet, needle);
  }
});

test('input is drained and dropped while the game menu is open, so no belt key reaches the use path', () => {
  const shell = readFileSync(new URL('../gameplay-shell.mjs', import.meta.url), 'utf8');
  const menu = shell.indexOf('if (gameMenu?.open) { gameMenu.step(); controlsPanel?.step(); input.drain(); return; }');
  assert.ok(menu > 0 && menu < shell.indexOf('command.type === COMMAND.BELT'));
});
