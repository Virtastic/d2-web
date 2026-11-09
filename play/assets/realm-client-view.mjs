import { popupRows } from './belt-popup.mjs';
// The joined Battle.net game's side of the shell's clientView. The game screen is drawn by the same
// drawFrame a local game uses; this shapes what the D2GS client state knows (assets/realm-client.mjs)
// into the snapshot that function reads, and into the few engine-side reads the HUD makes, so no part
// of the screen has a second, Battle.net-only drawing path.

// Life, mana and stamina are the selected player's own stats (8.8 fixed point, as in a local game).
// `base` is d2gsClientSnapshot(), `resources` is d2gsClientResources(); either may be missing early in the join.
const EMPTY = Object.freeze({ tick: 0, seed: 0, act: 1, level: 0, difficulty: 0, mode: 1, flags: 0, playerClass: 0, playerId: 0,
  monsterCount: 0, hash: 0, area: '', player: Object.freeze({ x: 0, y: 0, hp: 0, hpMax: 0, mana: 0, manaMax: 0 }) });
// `players` are the other players in the room (the selected player is drawn as the hero): each carries the render
// state the player renderer reads (mode, direction, frame, worn items, weapon class), shaped here into a
// kind 'player' entity of the same list the monsters and missiles are in. A unit that carries its path's
// 16.16 position (preciseX/preciseY) is drawn there, as an offline unit is, so it glides between subtiles.
const precise = (u) => (u.preciseX != null && u.preciseY != null ? { preciseX: u.preciseX, preciseY: u.preciseY } : null);
export function realmViewSnapshot({ base: given, resources, monsters, missiles, players, level, actor, frame, world = null }) {
  const base = given ?? EMPTY;   // before the first PlayerAssign the snapshot is empty, as the single-player one is
  const entities = [];
  for (const m of monsters ?? []) {
    entities.push({ kind: 'monster', type: 1, id: m.guid, classId: m.classId, mode: m.mode, x: m.x, y: m.y, hp: 0, hpMax: 0,
      hostile: true, roomOrdinal: 0, direction: m.direction, animation: actor(m, frame), ...precise(m) });
  }
  for (const p of players ?? []) {
    const r = p.renderState;
    entities.push({ kind: 'player', type: 0, id: p.guid, classId: p.classId, mode: r.mode, x: p.x, y: p.y, hp: 0, hpMax: 0,
      hostile: false, roomOrdinal: 0, direction: r.direction, name: p.name, clientRoomLevel: p.clientRoomLevel, playerRender: r, ...precise(p) });
  }
  for (const u of missiles ?? []) {
    if (u.level !== level) continue;
    entities.push({ kind: 'missile', type: 3, id: u.id, classId: u.classId, mode: u.mode, x: u.x, y: u.y, hp: 0, hpMax: 0,
      hostile: false, roomOrdinal: 0, direction: u.direction, missile: { mode: u.mode, direction: u.direction, framePrecise: u.framePrecise, trans: u.trans } });
  }
  const r = resources ?? {};
  return { ...base, entities, objects: world?.objectViews() ?? [], items: world?.itemViews() ?? [], warps: [], rooms: [], quests: [], units: [], town: false,
    player: { ...base.player, weaponClass: world?.weaponClassOf() ?? 'hth',
      hp: r.hp ?? base.player.hp, hpMax: r.hpMax ?? base.player.hpMax, mana: r.mana ?? 0, manaMax: r.manaMax ?? 0 } };
}

// The frame reads the HUD makes of the engine, for a game that has no local engine session. Each key is a
// snapOr() key of drawFrame; a key that is absent here is read as the single-player engine reads it (and
// answers "nothing" with no session).
//   characterScreen: slots 4 and 5 are the stamina and its maximum (whole points), what the stamina bar reads.
//   advancement:     the server's level, experience and attributes (assets/realm-hud-state.mjs); null, so the experience bar
//                    and the character and skill panels stay empty, until the server has sent its level.
//   runToggle / hotkeys / belt: nothing is worn, bound or held yet (a hotkey slot of -1 is unbound, as the engine answers).
export function realmFrameReads(resources, world = null, tables = null, playerClass = 0, hud = null, skillLevels = null, hotkeys = null, skillBonuses = null) {
  const whole = (v) => (v ?? 0) >> 8;
  const screen = new Array(15).fill(0);
  screen[4] = whole(resources?.stamina); screen[5] = whole(resources?.staminaMax);
  screen[11] = whole(resources?.hp); screen[12] = whole(resources?.hpMax); screen[13] = whole(resources?.mana); screen[14] = whole(resources?.manaMax);
  // The character screen shows the server's stats; with no level from the server it has nothing to show.
  const advancement = hud?.advancement(playerClass) ?? null;
  const reads = { characterScreen: resources ? (hud ? hud.characterSlots(playerClass, screen) && screen : screen.slice(0, 11)) : null, advancement,
    attributeTotals: hud?.attributeTotals() ?? null, questWords: hud?.questWords ?? null,
    classSkills: hud?.classSkills(playerClass, skillLevels, skillBonuses) ?? null, waypointTable: hud?.waypointTable() ?? null,
    runToggle: false, hotkeys: Array.from({ length: 8 }, (_, i) => ({ skill: hotkeys?.[i]?.skill ?? -1, left: Boolean(hotkeys?.[i]?.left) })), beltIndex: 2, beltHudBoxes: null, beltBoxCount: 0 };
  if (!world || !tables) return reads;
  // The panels' geometry is Inventory.txt / Belts.txt read through the shell's asset reader, in the numeric
  // shapes the single-player engine hands the same readers.
  const row = tables.inventory.playerRow(playerClass, true);
  const grid = row && tables.inventory.grid(row), components = row && tables.inventory.components(row);
  if (grid && components) reads.inventoryLayout = { ...tables.inventoryLayoutFromEngine(grid, components), row: playerClass };
  const stash = tables.inventory.grid('Big Bank Page 1'), cube = tables.inventory.grid('Transmogrify Box Page 1');
  if (stash) reads.stashLayout = tables.stashGeometry(stash);
  if (cube) reads.cubeLayout = tables.stashGeometry(cube);
  const record = world.beltRecord(), boxes = tables.belt.boxes(record) ?? [];
  let bottom = boxes.length ? boxes[0].bottom : 0;
  for (const b of boxes) if (b.bottom > bottom) bottom = b.bottom;
  const hudBoxes = boxes.filter((b) => b.bottom === bottom);
  reads.beltIndex = record; reads.beltBoxCount = boxes.length; reads.beltHudBoxes = hudBoxes.length ? hudBoxes : null;
  // The popup shows the rows above the HUD row, as many as the single-player read opens for the same record.
  const rows = popupRows(record, boxes.length);
  reads.beltShownfalse = reads.beltHudBoxes;
  reads.beltShowntrue = rows ? boxes.slice(0, 4 * (rows + 1)) : reads.beltHudBoxes;
  return reads;
}
