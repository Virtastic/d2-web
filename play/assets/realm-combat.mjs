// Combat in a realm game: the small rules the realm input, hover and sound paths share. Nothing here draws or
// plays anything itself; each helper shapes the server's unit views for the single-player renderer or trigger.

import { hoverLife, monsterNameColour } from './hover-lifebar.mjs';

/**
 * One unit's record from the engine (d2_realm_client_unit_info), as the page reads it.
 *   selectable / enemy / enemyIfHostile : whether the hero may attack the unit: where the hero stands, and
 *       outside town with the roster's relation toward `rosterGuid` friendly and hostile
 *   rosterGuid : the player whose relation decides it (the target player, or a pet's owner); 0xFFFFFFFF for none
 *   isPet, petType (7 is the hireling), petOwner, petDead : the unit's pet list entry
 *   name : the plate's name (a super unique's, a champion's, a unique's rolled name, a hireling's own)
 *   line : the line under a unique's or a minion's name (its type and enchantments)
 */
export function parseRealmUnitInfo(bytes) {
  const view = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
  if (bytes.length < 468 || view.getUint32(0, true) !== 1 || view.getUint32(4, true) !== 468) throw new Error('invalid realm unit record');
  const u = (at) => view.getUint32(at, true), text = (at, n) => { const part = bytes.subarray(at, at + n), end = part.indexOf(0); return new TextDecoder().decode(part.subarray(0, end < 0 ? n : end)); };
  return { unitType: u(8), guid: u(12), selectable: u(16) !== 0, enemy: u(20) !== 0, enemyIfHostile: u(24) !== 0, rosterGuid: u(28),
    alignment: view.getInt32(32, true), isPet: u(36) !== 0, petType: u(40), petOwner: u(44), petDead: u(48) !== 0, petNameId: u(52),
    typeFlags: u(56), superUnique: view.getInt32(60, true), nameSeed: u(64), ownerGuid: u(68), umods: Array.from(bytes.subarray(72, 81)),
    name: text(84, 128), line: text(212, 256) };
}

/**
 * The units of a realm game as the client sorts them: who is a town person, an enemy, a hireling or a pet,
 * which name plate each gets and which cursor the pointer takes over it. One instance is the data source of
 * the snapshot's `hostile`/`npc`, the hover label and the cursor, so the three cannot disagree.
 *   rowOf(classId)        : the MonStats.txt row's { npc, interact, killable, named } (realm-tables.mjs)
 *   unitInfo(type, guid)  : the engine's record of the unit (parseRealmUnitInfo), null when it holds none
 *   hostilePlayer(guid)   : whether that player is hostile to ours (the roster's relation); absent, nobody is
 *
 * The rules, as the 1.13c client applies them:
 *   plate    a unit gets the monster plate (HUD top centre, life bar, type line) when its MonStats row is not
 *            `interact`, is `killable` and has a name; every other unit gets the bare name above its head.
 *   colour   a unique is gold and a champion blue (hover-lifebar.mjs monsterNameColour); nothing else colours a name.
 *   owner    the pet packets (0x81, 0x7A) keep the engine's pet list: a listed unit is its owner's, and type 7
 *            is the hireling. A pet takes its owner's side; my own pets are never targets.
 *   hostile  the engine's attack-target test: a MonStats2 flag can force either answer, a player or a player's
 *            pet follows the roster's hostile relation, and every other monster is an enemy unless its
 *            alignment (a state the server sets, so a converted monster changes sides) matches the hero's.
 *   name     the engine composes it from the monster assignment; the class name stands when it has none.
 *   cursor   the same three the local game draws: attack over an enemy, talk over a person, the hand otherwise.
 */
const PET_HIRELING = 7, UNIQUE_OR_MINION = 0x18;
export function createRealmUnits({ rowOf, unitInfo = () => null, hostilePlayer = () => false }) {
  const flag = (v) => v === true || v === 1 || v === '1';
  const infoOf = (unit) => unitInfo(1, (unit.guid ?? unit.id) >>> 0) ?? null;
  function sort(unit, info) {
    const row = rowOf(unit.classId) ?? {};
    const plate = !flag(row.interact) && flag(row.killable) && row.named !== false ? 'monster' : 'npc';
    const hostile = Boolean(info && (info.rosterGuid !== 0xFFFFFFFF && hostilePlayer(info.rosterGuid) ? info.enemyIfHostile : info.enemy));
    if (info?.isPet) return { kind: info.petType === PET_HIRELING ? 'hireling' : 'pet', owner: info.petOwner, hostile, plate };
    if (flag(row.interact)) return { kind: 'npc', owner: 0, hostile, plate };
    return { kind: hostile ? 'monster' : 'friendly', owner: 0, hostile, plate };
  }
  const classOf = (unit) => sort(unit, infoOf(unit));
  return {
    classOf,
    /**
     * The hover target of a realm frame, in the shape the single-player hover label and monster highlight read.
     *   hit    : the realm input's pick under the pointer ({ kind, unit }); a live monster or person is named here
     *   views  : the server's monster views (guid, hpPct: the life byte of its 0x0C/0x69 packets, typeFlags)
     *   nameOf : (classId) => { stringIndex, label } from the engine's string tables
     * The life is the plate's fill, 0..128, by the same rule the single-player plate uses. `npc: true` asks for
     * the bare name above the unit. `line` is present for a unique or a minion only (it may be empty): it
     * replaces the class's type line under the name.
     */
    hoverTarget(hit, views, nameOf) {
      if (hit?.kind !== 'monster' && hit?.kind !== 'npc') return null;
      const unit = hit.unit, view = (views ?? []).find((m) => m.guid === unit.guid), info = infoOf(unit);
      const { stringIndex = -1, label: className = '' } = nameOf(unit.classId) ?? {};
      const label = info?.name || className;
      if (!label) return null;
      const cls = sort(unit, info), base = { kind: 'unit', id: unit.guid, classId: unit.classId, stringIndex, label, unitClass: cls.kind };
      if (cls.plate === 'npc') return { ...base, npc: true, x: unit.x, y: unit.y, preciseX: unit.preciseX, preciseY: unit.preciseY };
      const typeFlags = info?.typeFlags ?? view?.typeFlags;
      return { ...base, colour: monsterNameColour(typeFlags, unit.classId), ...(info && (info.typeFlags & UNIQUE_OR_MINION) ? { line: info.line } : {}),
        x: unit.x, y: unit.y, life: hoverLife(view?.hpMax > 0 ? { type: 1, mode: view.mode ?? unit.mode, hp: view.hp, maxHp: view.hpMax }
          : { type: 1, mode: view?.mode ?? unit.mode, hpPct: view?.hpPct }) };
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
 *   hitClassOf  : (type, guid) => the hit class the server last struck the unit with (0 none); it picks the impact sounds
 *   hitOf       : (type, guid) => the hit class of a hit the server reported for the unit this tick (undefined none)
 * A monster carries the hit points its client unit holds (its assign and hit records set them) and the mark of a
 * hit taken this tick, so the trigger plays the hit as it does in a local game; a unit with no life from the server
 * carries none and the trigger keys on its modes alone.
 */
/**
 * Whether the activations the client took this frame include a shrine's (0x4D on an object, shrine code 1..22): the shrine
 * effects (Storm, fire, exploding) remove hit points with no hit, so the life that drops in the same frame is no struck-unit
 * cue. The activations are the client's own (d2gsClientTakeObjectEvents), not a read of the engine's shrine queue.
 */
export const shrineDirectHpLoss = (events) => (events ?? []).some((e) => e.shrine >= 1 && e.shrine <= 22);
// The monster's mode and animation clock as the client unit holds them at this tick (the views are read every client
// tick), not as the draw snapshot last saw them: the snapshot is rebuilt once per drawn frame, and a frame that owes
// several ticks would hand the trigger the same frame clock each time -- a unit in get-hit whose clock has not moved
// reads as struck again (the trigger's re-entry rule), so one hit sounded once per owed tick (issue #2).
function fresh(e, view) {
  if (!view || view.framePrecise == null) return null;
  const flags = view.animationFlags ?? 0;
  return { mode: view.mode, animation: { ...e.animation, mode: view.mode, framePrecise: view.framePrecise, frameCountPrecise: view.frameCountPrecise,
    hasAnimSequence: (flags & 0x01) !== 0, seqMode: flags >> 4 } };
}
export function realmCombatSoundFrame(snapshot, views, hero, castSkillId, hitClassOf, hitOf, directHpLoss = false) {
  const byGuid = new Map((views ?? []).map((m) => [m.guid, m]));
  const struck = (type, id) => (hitOf?.(type, id) !== undefined ? { hitPacket: true } : null);
  const entities = (snapshot?.entities ?? []).filter((e) => e.type === 0 || e.type === 1)
    .map((e) => { const view = e.type === 1 ? byGuid.get(e.id) : null; return e.type === 1 ? { ...e, ...fresh(e, view), hp: view?.hpMax > 0 ? view.hp : undefined, typeFlags: view?.typeFlags ?? 0 } : { ...e, hp: undefined }; })
    .map((e) => ({ ...e, ...(hitClassOf ? { lastHitClass: hitClassOf(e.type, e.id) } : null), ...struck(e.type, e.id) }));
  const mine = hero?.source === 'realm' && hero.renderState;
  if (mine) entities.push({ type: 0, id: hero.guid, classId: hero.classId, mode: hero.renderState.mode, ...(hitClassOf ? { lastHitClass: hitClassOf(0, hero.guid) } : {}), ...struck(0, hero.guid) });
  return { entities, frame: { ...snapshot, directHpLoss, playerId: mine ? hero.guid : undefined,
    player: { ...snapshot?.player, castSkillId, framePrecise: mine ? hero.renderState.framePrecise : undefined } } };
}
