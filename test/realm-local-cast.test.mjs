// Single player's own casts through the packet path: the select (0x3C) echoes as 0x23, the right-hand cast (0x0C)
// spends mana on the server, the client's own start shows the missile, and Teleport moves the player and says so (0x15).
import test from 'node:test';
import assert from 'node:assert/strict';
import { localGameSkip, openLocalGame } from './helpers/local-game.mjs';

test('local game: select, cast and Teleport through the realm client path', { skip: localGameSkip() }, async () => {
  const g = await openLocalGame({ level: 2, setup: (call) => { for (const s of [45, 54]) call('d2_session_fixture_skill_level', ['number', 'number'], [s, 3]); } });
  const mana = () => g.call('d2_session_player_stat', ['number', 'number'], [8, 0]);
  try {
    // Ice Bolt
    g.send(g.protocol.d2gsSelectSkill(45, false));
    assert.ok((await g.step(2)).some((p) => p.op === 0x23), 'the select is echoed as 0x23');
    assert.equal(g.protocol.d2gsSelectedSkills().right, 45);
    const me = g.server();
    const before = mana();
    assert.equal(g.Module._d2_realm_client_local_skill_at(1, me.x + 10, me.y + 2), 1);
    g.send(g.protocol.d2gsRightSkillAt(false, me.x + 10, me.y + 2));
    let seen = 0;
    for (let i = 0; i < 20; i++) { await g.step(1); seen = Math.max(seen, g.protocol.d2gsClientMissiles().length); }
    assert.ok(mana() < before, 'the server spent mana');
    assert.ok(seen >= 1, 'the client showed the missile');
    // Teleport
    g.send(g.protocol.d2gsSelectSkill(54, false));
    await g.step(2);
    const at = g.server();
    const start = g.packets.length;
    g.send(g.protocol.d2gsRightSkillAt(false, at.x + 10, at.y + 2));
    await g.step(12);
    const now = g.server();
    assert.ok(now.x !== at.x || now.y !== at.y, 'the server moved the player');
    assert.ok(g.packets.slice(start).some((p) => p.op === 0x15), 'the move is sent as a reassign');
    assert.deepEqual(g.rejected, []);
    assert.deepEqual(g.refused(), []);
  } catch (error) { console.error(error); process.exit(1); }   // the engine keeps the process alive; a failure must still fail the run
  process.exit(0);
});
