// Combat in a realm game: the small rules the realm input, hover and sound paths share. Nothing here draws or
// plays anything itself; each helper shapes the server's unit views for the single-player renderer or trigger.

import { hoverLife, monsterNameColour } from './hover-lifebar.mjs';

/**
 * The units of a realm game as the client sorts them: who is a town person, an enemy, a hireling or a pet,
 * which name plate each gets and which cursor the pointer takes over it. One instance is the data source of
 * the snapshot's `hostile`/`npc`, the hover label and the cursor, so the three cannot disagree.
 *   rowOf(classId)      : the MonStats.txt row's { npc, interact, killable, align, named } (realm-tables.mjs)
 *   hostilePlayer(guid) : whether that player is hostile to ours (the roster's relation); absent, nobody is
 *
 * The rules, as the 1.13c client applies them:
 *   plate    a unit gets the monster plate (HUD top centre, life bar, type line) when its MonStats row is not
 *            `interact`, is `killable` and has a name; every other unit gets the bare name above its head.
 *   colour   a unique is gold and a champion blue (hover-lifebar.mjs monsterNameColour); nothing else colours a name.
 *   owner    the pet packet (0x81) lists a pet under its owner: byte 1 the pet kind (7 is the hireling, of which
 *            an owner has one), dword 4 the owner, dword 8 the pet. An owned unit takes its owner's side.
 *   hostile  GUESS: an unowned monster is an enemy when its MonStats `Align` is 0 (evil). The client asks the
 *            unit's alignment STAT, which the server can change (a converted or revived monster); the stat
 *            list of the monster assign is parsed by the engine but not handed to the page. Settled by
 *            exporting that stat (and the assign packet's own owner field) in the monster view.
 *   cursor   the same three the local game draws: attack over an enemy, talk over a person, the hand otherwise.
 */
const PET_HIRELING = 7;
export function createRealmUnits({ rowOf, hostilePlayer = () => false }) {
  const pets = new Map();   // pet guid: { owner, kind }
  const flag = (v) => v === true || v === 1 || v === '1';
  function classOf(unit) {
    const row = rowOf(unit.classId) ?? {}, pet = pets.get((unit.guid ?? unit.id) >>> 0) ?? null;
    const plate = !flag(row.interact) && flag(row.killable) && row.named !== false ? 'monster' : 'npc';
    if (pet) return { kind: pet.kind === PET_HIRELING ? 'hireling' : 'pet', owner: pet.owner, hostile: Boolean(hostilePlayer(pet.owner)), plate };
    if (flag(row.interact)) return { kind: 'npc', owner: 0, hostile: false, plate };
    const hostile = Number(row.align || 0) === 0 && flag(row.killable);
    return { kind: hostile ? 'monster' : 'friendly', owner: 0, hostile, plate };
  }
  return {
    classOf,
    /** One 0x81 packet. A hireling replaces the owner's previous one. */
    applyPet(packet) {
      if (packet?.[0] !== 0x81 || packet.length < 12) return null;
      const view = new DataView(packet.buffer, packet.byteOffset, packet.byteLength);
      const entry = { kind: packet[1], owner: view.getUint32(4, true) }, guid = view.getUint32(8, true);
      if (entry.kind === PET_HIRELING) for (const [g, p] of pets) if (p.kind === PET_HIRELING && p.owner === entry.owner) pets.delete(g);
      pets.set(guid, entry);
      return { guid, ...entry };
    },
    clear() { pets.clear(); },
    /**
     * The hover target of a realm frame, in the shape the single-player hover label and monster highlight read.
     *   hit    : the realm input's pick under the pointer ({ kind, unit }); a live monster or person is named here
     *   views  : the server's monster views (guid, hpPct: the life byte of its 0x0C/0x69 packets, typeFlags)
     *   nameOf : (classId) => { stringIndex, label } from the engine's string tables
     * The life is the plate's fill, 0..128, by the same rule the single-player plate uses. `npc: true` asks for
     * the bare name above the unit.
     * GUESS: the name is the class name. A unique, super unique or hireling carries its own name in the monster
     * assign (name bytes and a name id), which the engine parses but does not hand to the page; so does the
     * unique's enchantment line. Settled by exporting those fields in the monster view.
     */
    hoverTarget(hit, views, nameOf) {
      if (hit?.kind !== 'monster' && hit?.kind !== 'npc') return null;
      const unit = hit.unit, view = (views ?? []).find((m) => m.guid === unit.guid);
      const { stringIndex = -1, label = '' } = nameOf(unit.classId) ?? {};
      if (!label) return null;
      const cls = classOf(unit), base = { kind: 'unit', id: unit.guid, classId: unit.classId, stringIndex, label, unitClass: cls.kind };
      if (cls.plate === 'npc') return { ...base, npc: true, x: unit.x, y: unit.y, preciseX: unit.preciseX, preciseY: unit.preciseY };
      return { ...base, colour: monsterNameColour(view?.typeFlags, unit.classId),
        x: unit.x, y: unit.y, life: hoverLife({ type: 1, mode: view?.mode ?? unit.mode, hpPct: view?.hpPct }) };
    },
    /** The pointer's context over that pick, as the local game's cursor reads it. */
    cursorContext(hit) {
      if (hit?.kind !== 'monster' && hit?.kind !== 'npc') return { kind: 'hand' };
      const cls = classOf(hit.unit), target = { id: hit.unit.guid, type: 1 };
      if (cls.hostile) return { kind: 'attack', target };
      return cls.kind === 'npc' ? { kind: 'talk', target } : { kind: 'hand' };
    },
  };
}

/**
 * One game tick of a realm game for the single-player combat sound trigger (combat-sound-trigger.mjs), which
 * plays a unit's attack, hit, death and cast cue when its mode changes.
 *   snapshot    : the realm view snapshot (monsters and the other players as entities)
 *   views       : the server's monster views, for the champion/unique flags that pick a sound profile
 *   hero        : the selected player's render source ({ source, guid, classId, renderState }); it is not an entity of the snapshot
 *   castSkillId : the selected skill of the hand that last issued a command
 * The server sends a life byte, never hit points, so no entity carries hp: the trigger then keys on modes alone.
 */
export function realmCombatSoundFrame(snapshot, views, hero, castSkillId) {
  const flags = new Map((views ?? []).map((m) => [m.guid, m.typeFlags]));
  const entities = (snapshot?.entities ?? []).filter((e) => e.type === 0 || e.type === 1)
    .map((e) => (e.type === 1 ? { ...e, hp: undefined, typeFlags: flags.get(e.id) ?? 0 } : { ...e, hp: undefined }));
  const mine = hero?.source === 'realm' && hero.renderState;
  if (mine) entities.push({ type: 0, id: hero.guid, classId: hero.classId, mode: hero.renderState.mode });
  return { entities, frame: { ...snapshot, playerId: mine ? hero.guid : undefined,
    player: { ...snapshot?.player, castSkillId, framePrecise: mine ? hero.renderState.framePrecise : undefined } } };
}
