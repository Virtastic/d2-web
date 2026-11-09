import test from 'node:test';
import assert from 'node:assert/strict';
import { packets, sendWindow, createSendGate, createRealmInput, UNIT } from './realm-input.mjs';
import { createMonsterTable } from './realm-tables.mjs';

// The 1.13c client packet lengths (src/core/realm/d2gs_session.cpp kCsSize113c).
const SIZE = { 0x01: 5, 0x02: 9, 0x03: 5, 0x04: 9, 0x05: 5, 0x06: 9, 0x08: 5, 0x09: 9, 0x0c: 5, 0x0d: 9, 0x0f: 5, 0x10: 9,
  0x13: 9, 0x16: 13, 0x17: 5, 0x18: 17, 0x19: 5, 0x1a: 9, 0x1b: 9, 0x1c: 3, 0x1d: 9, 0x1f: 17, 0x20: 13, 0x26: 13,
  0x2f: 9, 0x30: 9, 0x38: 13, 0x3c: 9, 0x45: 9, 0x49: 9, 0x60: 1 };

test('every builder emits its opcode at the table length', () => {
  const made = [packets.walkTo(1, 2), packets.walkToUnit(1, 3), packets.runTo(1, 2), packets.runToUnit(1, 3),
    packets.leftSkillAt(1, 2), packets.leftSkillOnUnit(1, 3), packets.leftSkillAt(1, 2, true), packets.leftSkillOnUnit(1, 3, true),
    packets.rightSkillAt(1, 2), packets.rightSkillOnUnit(1, 3), packets.rightSkillAt(1, 2, true), packets.rightSkillOnUnit(1, 3, true),
    packets.interact(2, 7), packets.pickUp(4, 9), packets.dropCursor(5), packets.toGrid(5, 1, 2, 0), packets.fromGrid(5),
    packets.equip(5, 4), packets.swapOneForTwoHanded(5, 4), packets.unequip(4), packets.swapWithBody(5, 4),
    packets.swapCursorBuffer(1, 2, 3, 4), packets.useItem(5, 10, 11), packets.useBeltItem(5), packets.npcChatStart(1, 5),
    packets.npcChatEnd(1, 5), packets.npcAction(1, 2, 3), packets.selectSkill(36, false), packets.enterPortal(1, 2),
    packets.waypoint(1, 2), packets.swapWeapons()];
  for (const p of made) assert.equal(p.length, SIZE[p[0]], `opcode 0x${p[0].toString(16)}`);
});

test('field layout is little-endian as the server reads it', () => {
  assert.deepEqual([...packets.walkTo(0x1234, 0x5678)], [1, 0x34, 0x12, 0x78, 0x56]);
  assert.deepEqual([...packets.interact(UNIT.MONSTER, 0x01020304)], [0x13, 1, 0, 0, 0, 4, 3, 2, 1]);
  assert.deepEqual([...packets.pickUp(UNIT.ITEM, 7)], [0x16, 4, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0]);
  assert.deepEqual([...packets.selectSkill(54, true)], [0x3c, 54, 0, 0, 0x80, 255, 255, 255, 255]);
  assert.deepEqual([...packets.selectSkill(54, false)], [0x3c, 54, 0, 0, 0, 255, 255, 255, 255]);
});

test('the send windows are the retail table', () => {
  for (const op of [5, 6, 7, 8, 9, 10, 12, 13, 14, 15, 16, 17]) assert.equal(sendWindow(op), 50, op);
  for (const op of [1, 3, 4, 11, 18, 0x13, 0x20, 0x39, 0x3b, 0x3c, 0x60]) assert.equal(sendWindow(op), 200, op);
  assert.equal(sendWindow(0x3a), 0);
});

test('the gate drops an equal packet inside its window, shares one store, and never gates 0x3A', () => {
  const sent = []; const gate = createSendGate((b) => sent.push(Array.from(b)));
  assert.ok(gate.send(packets.walkTo(5, 5), 1000));
  assert.equal(gate.send(packets.walkTo(5, 5), 1199), false);
  assert.ok(gate.send(packets.walkTo(5, 5), 1200));
  assert.ok(gate.send(packets.rightSkillAt(5, 5), 1250));
  assert.equal(gate.send(packets.rightSkillAt(5, 5), 1299), false);
  assert.ok(gate.send(packets.rightSkillAt(5, 5), 1300));
  assert.ok(gate.send(packets.walkTo(6, 6), 1301));        // different bytes
  assert.ok(gate.send(Uint8Array.of(0x3a, 1, 0, 0, 0, 0, 0), 2000));
  assert.ok(gate.send(Uint8Array.of(0x3a, 1, 0, 0, 0, 0, 0), 2000));
  assert.equal(sent.length, 7);
});

function sceneOf(extra = {}) {
  return () => ({ ready: true, player: { x: 100, y: 100 }, run: false, selected: { left: 0, right: 36 },
    monsters: [], objects: [], items: [], isNpc: (c) => c === 99, hoverBox: (dx, dy) => Math.abs(dx) < 10 && Math.abs(dy) < 20,
    toScreen: (x, y) => ({ x: x * 2, y: y }), pointerWorld: (px, py) => ({ x: px / 2, y: py }), ...extra });
}
const harness = (extra) => { const out = []; const input = createRealmInput({ transmit: (b) => out.push(Array.from(b)), scene: sceneOf(extra), now: () => 0 });
  return { out, input }; };

test('a click on bare ground walks, or runs with the run toggle', () => {
  let h = harness(); h.input.leftClick(200, 120);
  assert.deepEqual(h.out, [[1, 100, 0, 120, 0]]);
  h = harness({ run: true }); h.input.leftClick(200, 120);
  assert.deepEqual(h.out, [[3, 100, 0, 120, 0]]);
});

test('left on a monster is the left skill on that unit; on a person, a talk; right, the right skill', () => {
  const mon = (guid, classId) => ({ guid, classId, mode: 1, x: 101, y: 101, anchorX: 200, anchorY: 120, drawn: true });
  let h = harness({ monsters: [mon(77, 5)] }); h.input.leftClick(200, 120);
  assert.deepEqual(h.out, [[6, 1, 0, 0, 0, 77, 0, 0, 0]]);
  h = harness({ monsters: [mon(78, 99)] }); h.input.leftClick(200, 120);
  assert.deepEqual(h.out, [[0x13, 1, 0, 0, 0, 78, 0, 0, 0]]);
  h = harness({ monsters: [mon(77, 5)] }); h.input.rightClick(200, 120);
  assert.deepEqual(h.out, [[0x0d, 1, 0, 0, 0, 77, 0, 0, 0]]);
  h = harness(); h.input.rightClick(200, 120);
  assert.deepEqual(h.out, [[0x0c, 100, 0, 120, 0]]);
  h = harness({ monsters: [{ ...mon(77, 5), mode: 12 }] }); h.input.leftClick(200, 120);
  assert.equal(h.out[0][0], 1);                                   // a corpse is not a target
});

// The server never echoes the owner's own walk, so the click itself must start the local route -- and, as
// in retail, the local start comes first and the packet goes out only when the player could start it.
test('a ground click starts the local route first and sends only what the player could start', () => {
  const moves = [], order = [];
  let answer = 1;
  const make = (extra, transmit = (b) => { order.push(`send:${b[0]}`); return true; }) => createRealmInput({ transmit, scene: sceneOf(extra), now: () => 0,
    move: (x, y, run) => { moves.push([x, y, run]); order.push('move'); return answer; } });
  make().leftClick(200, 120);
  make({ run: true }).leftClick(200, 120);
  assert.deepEqual(moves, [[100, 120, false], [100, 120, true]]);
  assert.deepEqual(order, ['move', 'send:1', 'move', 'send:3']);
  moves.length = 0; order.length = 0;
  make({ monsters: [{ guid: 77, classId: 5, mode: 1, x: 101, y: 101, anchorX: 200, anchorY: 120, drawn: true }] }).leftClick(200, 120);
  assert.deepEqual(moves, []);                                     // a monster is a skill, not a walk
  answer = 0;                                                      // the local start refuses the route: nothing is sent
  order.length = 0; assert.equal(make().leftClick(200, 120), false); assert.equal(make().step(1, 0), false);
  assert.deepEqual(order, ['move', 'move']);
  answer = -1;                                                     // no engine answer: sent as before
  order.length = 0; make().leftClick(200, 120); assert.deepEqual(order, ['move', 'send:1']);
  answer = 1;
  const input = make(); input.leftClick(200, 120); input.leftClick(200, 120);  // the gate drops the repeat packet only
  assert.equal(order.filter((o) => o === 'send:1').length, 2);
  assert.equal(input.evidence.localMove, 1);
});

test('the right skill is started locally first; a refused cast sends nothing', () => {
  const calls = [];
  const make = (extra, result) => { const out = []; return { out, input: createRealmInput({ transmit: (b) => out.push(Array.from(b)), scene: sceneOf(extra), now: () => 0,
    castAt: (x, y) => { calls.push(['at', x, y]); return result; }, castOnUnit: (t, g) => { calls.push(['unit', t, g]); return result; } }) }; };
  const mon = { guid: 77, classId: 5, mode: 1, x: 101, y: 101, anchorX: 200, anchorY: 120, drawn: true };
  let h = make({}, 1); h.input.rightClick(200, 120); assert.deepEqual(h.out, [[0x0c, 100, 0, 120, 0]]);
  h = make({ monsters: [mon] }, 1); h.input.rightClick(200, 120); assert.deepEqual(h.out, [[0x0d, 1, 0, 0, 0, 77, 0, 0, 0]]);
  h = make({}, 0); assert.equal(h.input.rightClick(200, 120), false); assert.deepEqual(h.out, []);
  h = make({ monsters: [mon] }, 0); assert.equal(h.input.rightClick(200, 120), false); assert.deepEqual(h.out, []);
  assert.deepEqual(calls, [['at', 100, 120], ['unit', 1, 77], ['at', 100, 120], ['unit', 1, 77]]);
});

// The server ignores a talk from 9 or more tiles; a far person is approached and talked to on arrival.
test('a far person is approached first and talked to once in range; items and objects walk there locally', () => {
  const moves = []; let scene = sceneOf({ monsters: [{ guid: 78, classId: 99, mode: 1, x: 120, y: 100, anchorX: 200, anchorY: 120, drawn: true }] })();
  const out = [];
  const input = createRealmInput({ transmit: (b) => out.push(Array.from(b)), scene: () => scene, now: () => 0,
    move: (x, y, run) => { moves.push([x, y, run]); return 1; } });
  input.leftClick(200, 120);
  assert.deepEqual(out, [[0x02, 1, 0, 0, 0, 78, 0, 0, 0]]);
  assert.deepEqual(moves, [[120, 100, false]]);
  input.tick(); assert.equal(out.length, 1);                      // still far: nothing more
  scene = { ...scene, player: { x: 116, y: 100 } };
  input.tick(); assert.deepEqual(out.at(-1), [0x13, 1, 0, 0, 0, 78, 0, 0, 0]);
  input.tick(); assert.equal(out.length, 2);                      // one talk only
  moves.length = 0;
  const h = createRealmInput({ transmit: () => true, scene: sceneOf({ items: [{ id: 9, mode: 3, x: 100, y: 120 }] }), now: () => 0,
    move: (x, y, run) => { moves.push([x, y, run]); return 1; } });
  h.leftClick(200, 120);
  assert.deepEqual(moves, [[100, 120, false]]);
});

test('arrow keys walk a step from the player; the interact key talks to the nearest person', () => {
  const moves = [];
  const make = (extra) => { const out = []; return { out, input: createRealmInput({ transmit: (b) => out.push(Array.from(b)), scene: sceneOf(extra), now: () => 0,
    move: (x, y, run) => { moves.push([x, y, run]); return 1; } }) }; };
  let h = make(); h.input.step(1, -1);
  assert.deepEqual(h.out, [[1, 101, 0, 99, 0]]); assert.deepEqual(moves, [[101, 99, false]]);
  h = make({ monsters: [{ guid: 5, classId: 99, mode: 1, x: 103, y: 100 }, { guid: 6, classId: 5, mode: 1, x: 101, y: 100 }],
    objects: [{ id: 12, x: 100, y: 101 }] });
  h.input.interactNearest();
  assert.deepEqual(h.out, [[0x13, 1, 0, 0, 0, 5, 0, 0, 0]]);
  h = make({ objects: [{ id: 12, x: 100, y: 101 }] }); h.input.interactNearest();
  assert.deepEqual(h.out, [[0x13, 2, 0, 0, 0, 12, 0, 0, 0]]);
});

test('the realm shell routes arrow keys, the interact key and the pending talk', async () => {
  const { readFile } = await import('node:fs/promises');
  const shell = await readFile(new URL('../gameplay-shell.mjs', import.meta.url), 'utf8');
  const branch = shell.slice(shell.indexOf("if (realmTransport.state === 'session-open') {\n    // The same commands"), shell.indexOf('if (!session) {\n    const advanced'));
  assert.match(branch, /realmInput\(\)\.step\(command\.x, command\.y\)/);
  assert.match(branch, /realmInput\(\)\.interactNearest\(\)/);
  assert.match(branch, /realmInput\(\)\.tick\(\)/);
});

test('the realm shell wires the local route to the engine export', async () => {
  const { readFile } = await import('node:fs/promises');
  const root = new URL('../../', import.meta.url);
  const shell = await readFile(new URL('play/gameplay-shell.mjs', root), 'utf8');
  const call = shell.slice(shell.indexOf('createRealmInput({'), shell.indexOf('createRealmInput({') + 2000);
  assert.match(call, /move:[^\n]*d2_realm_client_local_move/);
  const link = await readFile(new URL('wasm-build/link-d2.sh', root), 'utf8');
  assert.match(link, /_d2_realm_client_local_move[,\s]/);
});

// A click on a far item or object walks to the unit; the pickup or the use goes out only on arrival.
test('ground items and objects are walked to by guid and used only on arrival', () => {
  for (const [extra, type, id, use] of [
    [{ items: [{ id: 9, mode: 3, x: 100, y: 120 }] }, 4, 9, [0x16, 4, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0]],
    [{ objects: [{ id: 12, x: 100, y: 120, selectable: true }] }, 2, 12, [0x13, 2, 0, 0, 0, 12, 0, 0, 0]]]) {
    let scene = sceneOf(extra)(); const out = [];
    const input = createRealmInput({ transmit: (b) => out.push(Array.from(b)), scene: () => scene, now: () => 0 });
    input.leftClick(200, 120);
    assert.deepEqual(out, [[0x02, type, 0, 0, 0, id, 0, 0, 0]]);
    assert.deepEqual(input.pending, { type, guid: id });
    input.tick(); assert.equal(out.length, 1);                    // still walking
    scene = { ...scene, player: { x: 100, y: 119 } };
    input.tick(); assert.deepEqual(out.at(-1), use);
    assert.equal(input.pending, null);
  }
  const h = harness({ items: [{ id: 9, mode: 3, x: 100, y: 101 }] }); h.input.leftClick(200, 101);  // in reach: at once
  assert.deepEqual(h.out, [[0x16, 4, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0]]);
});

test('the engine answers reach, the approach and the end of the walk when it can', () => {
  let done = false, inReach = false; const out = [], approaches = [];
  const input = createRealmInput({ transmit: (b) => out.push(Array.from(b)), now: () => 0,
    scene: sceneOf({ objects: [{ id: 12, x: 100, y: 120 }] }),
    approachUnit: (t, g, run) => { approaches.push([t, g, run]); return 1; }, reach: () => (inReach ? 1 : 0), actionDone: () => done });
  input.leftClick(200, 120);
  assert.deepEqual(approaches, [[2, 12, false]]);
  input.tick(); assert.equal(out.length, 1);                      // the walk is not over
  done = true; input.tick();                                       // over, still short: walk again
  assert.deepEqual(approaches.length, 2); assert.equal(out.length, 1);  // the gate drops the equal 0x02
  inReach = true; input.tick(); assert.deepEqual(out.at(-1), [0x13, 2, 0, 0, 0, 12, 0, 0, 0]);
  const refused = createRealmInput({ transmit: (b) => out.push(Array.from(b)), now: () => 0,
    scene: sceneOf({ objects: [{ id: 12, x: 100, y: 120 }] }), approachUnit: () => 0 });
  out.length = 0; assert.equal(refused.leftClick(200, 120), false);
  assert.deepEqual(out, []); assert.deepEqual(refused.pending, { type: 2, guid: 12 });  // 2AA40 keeps the action waiting
});

test('the Compelling Orb and the open Hell Forge are used only with their quest item on the cursor', () => {
  const run = (object, code) => { const h = harness({ objects: [{ x: 100, y: 101, selectable: true, ...object }], cursorCode: () => code });
    h.input.leftClick(200, 101); return h.out; };
  assert.deepEqual(run({ id: 3, classId: 404, mode: 0 }, 0), []);
  assert.deepEqual(run({ id: 3, classId: 404, mode: 0 }, 0x20326671), [[0x13, 2, 0, 0, 0, 3, 0, 0, 0]]);
  assert.deepEqual(run({ id: 4, classId: 376, mode: 2 }, 0), []);
  assert.deepEqual(run({ id: 4, classId: 376, mode: 2 }, 0x20686668), [[0x13, 2, 0, 0, 0, 4, 0, 0, 0]]);
  assert.deepEqual(run({ id: 4, classId: 376, mode: 0 }, 0), [[0x13, 2, 0, 0, 0, 4, 0, 0, 0]]);
});

test('a repeated talk is dropped only by the send gate: the same packet inside 200 ms', () => {
  let t = 0; const out = [];
  const input = createRealmInput({ transmit: (b) => out.push(Array.from(b)), now: () => t,
    scene: sceneOf({ monsters: [{ guid: 5, classId: 99, mode: 1, x: 101, y: 100 }] }) });
  input.interactNearest(); t = 300; input.interactNearest(); t = 350; input.interactNearest();
  assert.equal(out.length, 2);
});

test("the server's talk answer opens the chat with 0x2F, or ends it with 0x30 for a unit the client lost", () => {
  const h = harness({ monsters: [{ guid: 5, classId: 99, mode: 1, x: 101, y: 100 }] });
  h.input.npcInteraction([0x28, 1, 5, 0, 0, 0, 7]);
  h.input.npcInteraction([0x28, 1, 6, 0, 0, 0, 7]);
  assert.equal(h.input.npcInteraction([0x28, 6, 5, 0, 0, 0, 7]), null);
  assert.deepEqual(h.out, [[0x2f, 1, 0, 0, 0, 5, 0, 0, 0], [0x30, 7, 0, 0, 0, 6, 0, 0, 0]]);
});

test('a belt key uses the item in its own slot, not a higher one in the column', () => {
  const h = harness({ items: [{ id: 1, mode: 2, gridX: 4 }, { id: 2, mode: 2, gridX: 0 }, { id: 3, mode: 2, gridX: 1 }] });
  assert.ok(h.input.useBelt(0));
  assert.deepEqual(h.out, [[0x26, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]]);
  assert.equal(h.input.useBelt(3), false);
  const upper = harness({ items: [{ id: 1, mode: 2, gridX: 4 }] });
  assert.equal(upper.input.useBelt(0), false);
});

test('a belt key does nothing while dead or with an item on the cursor', () => {
  const items = [{ id: 2, mode: 2, gridX: 0 }];
  assert.equal(harness({ items, playerMode: 17 }).input.useBelt(0), false);
  assert.equal(harness({ items, cursorId: 9 }).input.useBelt(0), false);
  assert.ok(harness({ items, playerMode: 1 }).input.useBelt(0));
});

test('a belt key stops when the scene reports the use routine refusing the item', () => {
  const items = [{ id: 2, mode: 2, gridX: 0 }];
  const h = harness({ items, beltRefused: (item) => item.id === 2 });
  assert.equal(h.input.useBelt(0), false);
  assert.deepEqual(h.out, []);
  assert.ok(harness({ items, beltRefused: () => false }).input.useBelt(0));
});

test('a belt key with Shift down names the mercenary as the target', () => {
  const h = harness({ items: [{ id: 2, mode: 2, gridX: 0 }] });
  assert.ok(h.input.useBelt(0, true));
  assert.deepEqual(h.out, [[0x26, 2, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0]]);
});

test('MonStats: the npc and interact columns name the people', () => {
  const text = 'id\tnpc\tinteract\nskeleton\t\t\ncharsi\t1\t1\n';
  const t = createMonsterTable(() => new TextEncoder().encode(text));
  assert.equal(t.isNpc(0), false); assert.equal(t.isNpc(1), true); assert.equal(t.isNpc(9), false);
});

test('a talk that goes out is reported to the NPC state', () => {
  const talked = [], out = [];
  const input = createRealmInput({ transmit: (b) => out.push(Array.from(b)), now: () => 0, onTalk: (g) => talked.push(g),
    scene: sceneOf({ monsters: [{ guid: 78, classId: 99, mode: 1, x: 101, y: 101, anchorX: 200, anchorY: 120, drawn: true }] }) });
  input.leftClick(200, 120);
  assert.deepEqual(out, [[0x13, 1, 0, 0, 0, 78, 0, 0, 0]]);
  assert.deepEqual(talked, [78]);
});

test('an item action asks the leave wait first: refused actions send nothing, allowed ones go out', () => {
  const out = []; let allow = false, asked = 0;
  const input = createRealmInput({ transmit: (b) => out.push(Array.from(b)), scene: sceneOf(), now: () => 0,
    leaveWait: { itemAction: () => { asked++; return allow; } } });
  assert.equal(input.lift(7), false);
  assert.equal(input.equip(7, 1), false);
  assert.equal(input.dropCursor(7), false);
  assert.deepEqual(out, []);
  allow = true;
  assert.ok(input.lift(7));
  assert.equal(asked, 4);
  assert.deepEqual(out, [Array.from(packets.fromGrid(7))]);
});
