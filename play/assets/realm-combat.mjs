// Combat in a realm game: the small rules the realm input, hover and sound paths share. Nothing here draws or
// plays anything itself; each helper shapes the server's unit views for the single-player renderer or trigger.

import { hoverLife, monsterNameColour } from './hover-lifebar.mjs';

const RANGED_WEAPON_CLASSES = new Set(['bow', 'xbw']);

/**
 * Whether the left button's skill swings at arm's length: the plain Attack (skill 0) with anything but a bow
 * or crossbow in hand. GUESS: the other melee skills (Bash, Zeal, ...) read as ranged here, so they are sent
 * without a local walk; the engine's own skill start is the rule once it is exported.
 */
export const realmLeftMelee = (leftSkill, weaponClass) => leftSkill === 0 && !RANGED_WEAPON_CLASSES.has(String(weaponClass ?? 'hth').toLowerCase());

/**
 * The hover target of a realm frame, in the shape the single-player hover label and monster highlight read.
 *   hit    : the realm input's pick under the pointer ({ kind, unit }); only a live monster is named here
 *   views  : the server's monster views (guid, hpPct: the life byte of its 0x0C/0x69 packets, typeFlags)
 *   nameOf : (classId) => { stringIndex, label } from the engine's string tables
 * The life is the plate's fill, 0..128, by the same rule the single-player plate uses.
 */
export function realmHoverTarget(hit, views, nameOf) {
  if (hit?.kind !== 'monster') return null;
  const unit = hit.unit, view = (views ?? []).find((m) => m.guid === unit.guid);
  const { stringIndex = -1, label = '' } = nameOf(unit.classId) ?? {};
  if (!label) return null;
  return { kind: 'unit', id: unit.guid, classId: unit.classId, stringIndex, label, colour: monsterNameColour(view?.typeFlags, unit.classId),
    x: unit.x, y: unit.y, life: hoverLife({ type: 1, mode: view?.mode ?? unit.mode, hpPct: view?.hpPct }) };
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
