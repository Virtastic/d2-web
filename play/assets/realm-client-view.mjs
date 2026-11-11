import { popupRows } from './belt-popup.mjs';
import { BIG_BANK_PAGE_1, TRANSMOGRIFY_BOX_PAGE_1 } from './stash-panel.mjs';
import { hirelingSlots } from './hireling-panel.mjs';
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
// The five town levels (D2Common's own town test compares the level id against these).
const MONSTER_MODE_DEATH = 0, MONSTER_MODE_DEAD = 12;
const TOWN_LEVELS = new Set([1, 40, 75, 103, 109]);
const precise = (u) => (u.preciseX != null && u.preciseY != null ? { preciseX: u.preciseX, preciseY: u.preciseY } : null);
// `warps`, `rooms` and `quests` are d2gsClientWarps(), d2gsClientRooms() and d2gsClientQuests(): the level exits the
// server assigned (0x09), the rooms its map packets hold active (0x07, 0x08), and the quest records and log states
// it sent (0x28, 0x29, 0x52, 0x5D). Exits and rooms have the records of a local game; quests is null until one arrives.
// `classOf(monster)` sorts a monster as the client does ({ kind, hostile }, assets/realm-combat.mjs createRealmUnits):
// a town person is `npc`, and only an enemy is `hostile`. Without it nothing is known, so nothing is an enemy.
// `tick` is the game's own tick when the client can know it (a local game's server is in the page); a remote
// server sends none, and the frame then keeps the client's own count.
// `alerts` are the guids of the NPCs the server marked as having something to say (realmGameReads): the
// monster entity carries it as speechPending, the flag a local game's entity has.
export function realmViewSnapshot({ base: given, resources, monsters, missiles, players, level, actor, frame, world = null, classOf = null, objects = [], alerts = null, warps = null, rooms = null, quests = null, tick = null }) {
  const marked = new Set(alerts ?? []);
  const base = given ?? EMPTY;   // before the first PlayerAssign the snapshot is empty, as the single-player one is
  const entities = [];
  for (const m of monsters ?? []) {
    const cls = classOf?.(m) ?? null;
    // Dead is the unit's mode (death, dead), never its life share: the server rounds a nearly dead monster's life to 0%.
    const dead = m.mode === MONSTER_MODE_DEATH || m.mode === MONSTER_MODE_DEAD;
    entities.push({ kind: 'monster', type: 1, id: m.guid, classId: m.classId, mode: m.mode, x: m.x, y: m.y, dead,
      // The unit's own hit points against their maximum when the client unit holds them; else the life byte.
      ...(m.hpMax > 0 ? { hp: dead ? 0 : Math.max(1, m.hp), hpMax: m.hpMax } : { hp: m.hpPct == null || dead ? 0 : Math.max(1, m.hpPct), hpMax: m.hpPct == null ? 0 : 100 }),
      hostile: cls?.hostile === true, npc: cls?.kind === 'npc', unitClass: cls?.kind ?? null, roomOrdinal: 0, direction: m.direction, animation: actor(m, frame), speechPending: marked.has(m.guid), ...precise(m) });
  }
  for (const p of players ?? []) {
    const r = p.renderState;
    entities.push({ kind: 'player', type: 0, id: p.guid, classId: p.classId, mode: r.mode, x: p.x, y: p.y, hp: 0, hpMax: 0,
      hostile: false, roomOrdinal: 0, direction: r.direction, name: p.name, clientRoomLevel: p.clientRoomLevel, playerRender: r, ...precise(p) });
  }
  for (const u of missiles ?? []) {
    if (u.level !== level) continue;
    entities.push({ kind: 'missile', type: 3, id: u.id, classId: u.classId, mode: u.mode, x: Math.floor(u.x), y: Math.floor(u.y), hp: 0, hpMax: 0,   // the unit's whole position, as a monster's; the sub-tile one is preciseX
      hostile: false, roomOrdinal: 0, direction: u.direction, ...precise(u), missile: { mode: u.mode, direction: u.direction, framePrecise: u.framePrecise, trans: u.trans } });
  }
  const r = resources ?? {};
  return { ...base, tick: tick ?? base.tick, entities, objects, items: world?.itemViews() ?? [], warps: warps ?? [], rooms: rooms ?? [], quests: quests ?? null, units: [], town: TOWN_LEVELS.has(base.level),
    player: { ...base.player, weaponClass: world?.weaponClassOf() ?? 'hth',
      hp: r.hp ?? base.player.hp, hpMax: r.hpMax ?? base.player.hpMax, mana: r.mana ?? 0, manaMax: r.manaMax ?? 0 } };
}

// The frame reads the HUD makes of the engine, for a game that has no local engine session. Each key is a
// snapOr() key of drawFrame; a key that is absent here is read as the single-player engine reads it (and
// answers "nothing" with no session).
//   characterScreen: slots 4 and 5 are the stamina and its maximum (whole points), what the stamina bar reads.
//   advancement:     the server's level, experience and attributes (assets/realm-hud-state.mjs); null, so the experience bar
//                    and the character and skill panels stay empty, until the server has sent its level.
//   hero:            the wasm client's reads of the selected player's unit ({ screen, stat }): the character screen and the
//                    attribute totals computed from its stat list with the worn items merged in, by the function the
//                    single-player panel reads. Without it the screen shows the server's raw stats.
//   questWords:      the player's own quest record, the 96 bytes of the server's 0x28 (not the game's record of 0x29).
//   runToggle:       absent, so the HUD's run button reads the engine's run/walk flag the R key and the button flip.
//   hotkeys / belt:  nothing is worn, bound or held yet (a hotkey slot of -1 is unbound, as the engine answers).
export function realmFrameReads(resources, world = null, tables = null, playerClass = 0, hud = null, skillLevels = null, hotkeys = null, skillBonuses = null, hero = null, quests = null) {
  const whole = (v) => (v ?? 0) >> 8;
  const screen = new Array(15).fill(0);
  screen[4] = whole(resources?.stamina); screen[5] = whole(resources?.staminaMax);
  screen[11] = whole(resources?.hp); screen[12] = whole(resources?.hpMax); screen[13] = whole(resources?.mana); screen[14] = whole(resources?.manaMax);
  // The character screen shows the server's stats; with no level from the server it has nothing to show.
  const advancement = hud?.advancement(playerClass) ?? null;
  // The hero unit's own screen once the server has sent its level (the panel stays shut before, as it does with raw stats).
  const heroScreen = advancement && hero?.screen ? hero.screen.slice() : null;
  // Life, mana and stamina tick per packet: their current values stay the ones the orbs and the stamina bar read.
  if (heroScreen && resources) { heroScreen[4] = screen[4]; heroScreen[11] = screen[11]; heroScreen[13] = screen[13]; }
  const reads = { characterScreen: heroScreen ?? (resources ? (hud ? hud.characterSlots(playerClass, screen) && screen : screen.slice(0, 11)) : null), advancement,
    attributeTotals: (heroScreen && hero.stat ? { strength: hero.stat(0), energy: hero.stat(1), dexterity: hero.stat(2), vitality: hero.stat(3) } : hud?.attributeTotals()) ?? null, questWords: quests?.player ?? null,
    classSkills: hud?.classSkills(playerClass, skillLevels, skillBonuses) ?? null, waypointTable: hud?.waypointTable() ?? null,
    hotkeys: Array.from({ length: 8 }, (_, i) => ({ skill: hotkeys?.[i]?.skill ?? -1, left: Boolean(hotkeys?.[i]?.left) })), beltIndex: 2, beltHudBoxes: null, beltBoxCount: 0 };
  if (!world || !tables) return reads;
  // The panels' geometry is Inventory.txt / Belts.txt read through the shell's asset reader, in the numeric
  // shapes the single-player engine hands the same readers.
  const row = tables.inventory.playerRow(playerClass, true);
  const grid = row && tables.inventory.grid(row), components = row && tables.inventory.components(row);
  if (grid && components) reads.inventoryLayout = { ...tables.inventoryLayoutFromEngine(grid, components), row: playerClass };
  // The stash and cube rows at this renderer's 800x600, the rows a local game reads (record + 16).
  const stash = tables.inventory.gridAt(BIG_BANK_PAGE_1, 1), cube = tables.inventory.gridAt(TRANSMOGRIFY_BOX_PAGE_1, 1);
  if (stash) reads.stashLayout = tables.stashGeometry(stash);
  if (cube) reads.cubeLayout = tables.stashGeometry(cube);
  // The trade screen's two grids at 800x600: the partner's offer above (page 1), the player's own below (page 2).
  const theirs = tables.inventory.grid('Trade Page 1-2'), own = tables.inventory.grid('Trade Page 2-2');
  if (theirs && own) reads.tradeLayout = { theirs: tables.stashGeometry(theirs), own: tables.stashGeometry(own) };
  const record = world.beltRecord(), boxes = tables.belt.boxes(record) ?? [];
  let bottom = boxes.length ? boxes[0].bottom : 0;
  for (const b of boxes) if (b.bottom > bottom) bottom = b.bottom;
  const hudBoxes = boxes.filter((b) => b.bottom === bottom);
  reads.beltIndex = record; reads.beltBoxCount = boxes.length; reads.beltHudBoxes = hudBoxes.length ? hudBoxes : null;
  // The popup shows the rows above the HUD row, as many as the single-player read opens for the same record.
  const rows = popupRows(record, boxes.length);
  reads.beltBoxes = boxes.length ? boxes : null;   // every box of the record: the popup's hover walks them
  reads.beltShownfalse = reads.beltHudBoxes;
  reads.beltShowntrue = rows ? boxes.slice(0, 4 * (rows + 1)) : reads.beltHudBoxes;
  return reads;
}

// What the server's game-level packets built in the client (assets/realm-client.mjs), in the shapes the
// single-player frame reads:
//   alerts          the NPCs carrying the speech mark, for realmViewSnapshot. The NPC whose menu is open
//                   (`npcGuid`) is told to the client first, which does not mark it.
//   clientFlags     the game flags in the client-flag layout (hardcore 0x04, expansion 0x20, ladder 0x40);
//                   absent until the server has sent them.
//   hirelingRoster  the player's own mercenary entry of the pet list, as the hireling roster of a local game.
//   hirelingSource  the mercenary's stats, worn items and panel slots, read by the hireling panel's merc().
const HIRELING = 7, MODE_DEATH = 0, MODE_DEAD = 12;
export function realmGameReads({ protocol, world = null, tables = null, npcGuid = null, ownGuid = null, monsters = null, string = () => '' }) {
  const reads = {};
  if (!protocol) return { alerts: [], marks: [], ui: null, reads };
  protocol.d2gsClientSetActiveNpc?.(npcGuid);
  const alerts = protocol.d2gsClientNpcAlerts?.() ?? [];
  const info = protocol.d2gsClientGameInfo?.() ?? null;
  if (info) reads.clientFlags = info.flags;
  const stat = (guid, n) => protocol.d2gsClientPetStat?.(guid, n) ?? null;
  reads.hirelingRoster = (protocol.d2gsClientPets?.() ?? []).filter((pet) => pet.kind === HIRELING && (ownGuid == null || pet.owner === (ownGuid >>> 0))).map((pet) => {
    const mode = (monsters ?? []).find((m) => m.guid === pet.guid)?.mode;
    return { guid: pet.guid, classId: pet.classId, nameId: pet.nameId, name: string(pet.nameId), seed: pet.seed,
      flags: mode === MODE_DEATH || mode === MODE_DEAD ? 1 : 0, hirelingId: -1, level: stat(pet.guid, 12) ?? 0,
      life: stat(pet.guid, 6) ?? 0, maxLife: stat(pet.guid, 7) ?? 0, lifePct: pet.lifePct, unitPresent: pet.unitPresent };
  });
  reads.hirelingSource = {
    stat,
    items: (guid) => (world?.wornBy(guid) ?? []).map((item) => ({ bodyLoc: item.bodyLoc, invFile: item.invFile, invWidth: item.invWidth || 1, invHeight: item.invHeight || 1, twoHanded: Boolean(item.twoHanded) })),
    slots: () => { const c = tables?.inventory.components('Hireling2') ?? tables?.inventory.components('Hireling'); return c ? hirelingSlots(c) : {}; },
  };
  // `marks`: the other units carrying the mark, the own hero among them; `ui`: the panel flags of the last game start.
  return { alerts, marks: protocol.d2gsClientUnitMarks?.() ?? [], ui: protocol.d2gsClientGameUi?.() ?? null, reads };
}
