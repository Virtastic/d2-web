// Single player's skill point and resource updates through the packet path: spending a point (0x3B) answers with
// the skill update (0x21) and the points update (0x1D), and the client's mana is the whole-point 0x95 value of the
// server's fractional mana, not the server's raw value.
import test from 'node:test';
import assert from 'node:assert/strict';
import { localGameSkip, openLocalGame } from './helpers/local-game.mjs';

test('local game: a skill point answers 0x21, and mana reaches the client as the whole-point 0x95 value', { skip: localGameSkip() }, async () => {
  const g = await openLocalGame({ level: 2, charClass: 1, setup: (call) => call('d2_session_fixture_skill_level', ['number', 'number'], [45, 3]) });
  const stat = (id) => g.call('d2_session_player_stat', ['number', 'number'], [id, 0]);
  try {
    assert.equal(g.call('d2_session_set_stat', ['number', 'number'], [5, 2]), 1);
    await g.step(2);
    const start = g.packets.length;
    g.send(Uint8Array.of(0x3b, 36, 0));
    await g.step(4);
    const got = g.packets.slice(start);
    assert.ok(got.some((p) => p.op === 0x21), 'the skill update arrives');
    assert.ok(got.some((p) => p.op === 0x1d), 'the points update arrives');
    assert.equal(stat(5), 1, 'one point is spent');

    g.send(g.protocol.d2gsSelectSkill(45, false));
    await g.step(2);
    const me = g.server();
    g.send(g.protocol.d2gsRightSkillAt(false, me.x + 10, me.y + 2));
    let checked = 0;
    for (let i = 0; i < 40; i++) {
      const sent = (await g.step(1)).some((p) => p.op === 0x95);
      if (sent) { assert.equal(g.protocol.d2gsClientResources().mana, stat(8) & ~0xff, 'the client holds the server mana rounded down to whole points'); checked++; }
    }
    assert.ok(checked >= 1, 'a 0x95 arrived after the cast');
    assert.ok(g.protocol.d2gsClientResources().mana < 8960, 'the cast cost reached the client');
  } catch (error) { console.error(error); process.exit(1); }   // the engine keeps the process alive; a failure must still fail the run
  process.exit(0);
});
