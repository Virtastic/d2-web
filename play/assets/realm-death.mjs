// A joined Battle.net game's death loop, the parts the page supplies. The client units themselves (the
// hero falling, lying dead, standing up in town; a corpse and its owner) are kept by the engine
// (src/core/realm/d2gs_client_player_death.h); this shapes what the death screen and the corpse click read.
//
// The loop on the wire, as the 1.13c client and server run it:
//   the hero dies      S 0x0D selector 8 (mode DEATH), then selector 9 (mode DEAD); S 0x1D-0x1F stat 175, the gold lost;
//                      S 0x59 + 0x0D selector 9 for the corpse (a player unit of the same class and name);
//                      S 0x8E flag 1 naming corpse and owner; S 0x74 flag 1, which takes the worn items off the owner
//   Esc on the screen  C 0x41 (one byte), accepted only in mode DEAD
//   softcore           S 0x1D-0x1F / 0x95 life, nonzero: the hero stands up in town; S 0x15 puts it there
//   hardcore           the server drops the connection instead; nothing stands up
//   the corpse         C 0x02/0x04 to the corpse while walking, C 0x13 (type 0, the corpse GUID) on arrival;
//                      S 0x9C/0x9D give the items back, S 0x8E flag 0 and S 0x0A remove the empty corpse

const HARDCORE = 0x04, EXPANSION = 0x20;

/**
 * The client flags of the character playing: the realm's character-list flags carry the same two bits the death
 * screen and the area banner read (0x04 hardcore, 0x20 expansion). A character the list does not hold reads as a
 * softcore expansion one, as the screen drew before it had this.
 */
export function realmHeroFlags(characters, name) {
  const wanted = String(name ?? '').toLowerCase();
  const c = wanted ? (characters ?? []).find((x) => String(x.name).toLowerCase() === wanted) : null;
  return c ? (c.flags & (HARDCORE | EXPANSION)) : EXPANSION;
}

/**
 * The corpses lying in view: the server's corpse list joined to the player units on screen.
 *   pairs    [{ owner, corpse }] (d2gsClientCorpses)
 *   players  the other player units drawn ([{ guid, x, y, renderState: { mode } }])
 *   ownGuid  the hero's GUID
 *   roster   the players in the game (bnet-roster.mjs), optional
 * A corpse whose unit is not in view (or not yet assigned) is left out: there is nothing to click.
 * With a roster, the client's own rule for a click on a dead player unit applies (1.13c D2Client 0x2AFB0):
 *   - the corpse resolves to its owner's roster entry (0x9D630 searches each entry's corpse list); an owner who
 *     is not in the game has no entry (0x9E280 frees the entry and its corpse list when the player leaves; a
 *     corpse named before its owner's entry goes to a side list the lookup never reads), and the corpse is then
 *     not a target at all;
 *   - `lootable`: the hero's own corpse, or one whose owner holds the loot relation toward the hero (0x4FD90,
 *     mask 1: the flags of S 0x8C from the owner to the hero);
 *   - `name`: the owner's roster name, which is what a pointed-at corpse is labelled with (0xBCEA0 reads the
 *     name of the entry the unit's GUID resolves to).
 * Without a roster (a caller that has none) every corpse is listed and only the hero's own is lootable.
 */
export function realmCorpseViews(pairs, players, ownGuid, roster = null) {
  const out = [], me = ownGuid == null ? null : ownGuid >>> 0;
  for (const pair of pairs ?? []) {
    const unit = (players ?? []).find((p) => p.guid === pair.corpse);
    if (!unit) continue;
    const owner = pair.owner >>> 0, own = me != null && owner === me;
    const entry = roster ? roster.get(owner) : null;
    if (roster && !entry) continue;
    const view = { guid: pair.corpse >>> 0, owner, own, mode: unit.renderState?.mode ?? unit.mode, x: unit.x, y: unit.y };
    if (roster) { view.lootable = own || (me != null && (roster.relation(owner, me) & LOOT) !== 0); view.name = entry.name; }
    out.push(view);
  }
  return out;
}
/** A corpse the hero may open: its own, or one its owner has opened to the hero. A view built without a roster carries no answer and falls back to "own". */
export const corpseLootable = (c) => (c.lootable ?? c.own) === true;

// --- the hero's own death ---------------------------------------------------------------------------------------
const LOOT = 1, DEAD_FLAG = 0x08;
export const PLAYER_DEATH = 0, PLAYER_DEAD = 17;
// Both rules are the engine's: _d2_hero_is_down is 1.13c D2Client 0xA5440 (the player modes the client treats as
// dead), _d2_death_closes_panel is 0xBE350 run as the hero's own "die" action runs it (the client's panel table,
// panel 10 forced). The page binds the engine once it is up; nothing here answers without it.
let engine = null;
export function bindDeathRules(Module) { engine = Module; }
const rules = () => engine ?? globalThis.Module ?? (() => { throw new Error('death rules: engine not bound'); })();
/** Falling or lying dead. */
export const heroIsDown = (mode) => Number.isInteger(mode) && mode >= 0 && rules()._d2_hero_is_down(0, mode, 0) !== 0;
/** A panel the hero's death closes (panel number 0..0x25). */
export const closedOnDeath = (panel) => Number.isInteger(panel) && panel >= 0 && rules()._d2_death_closes_panel(panel) !== 0;
// GUESS (panel numbers): which number is which panel is the community's numbering, not read from the client;
// settled by reading each panel's draw routine from the toggle's switch. Only the numbers below are used.
export const DEATH_PANEL = Object.freeze({ inventory: 1, character: 2, skillSelect: 3, skillTree: 4, chat: 5, npcMenu: 8, gameMenu: 9, automap: 10,
  shop: 0x0c, quests: 0x0f, waypoint: 0x14, miniPanel: 0x15, party: 0x16, trade: 0x17, messageLog: 0x18, stash: 0x19, cube: 0x1a, help: 0x1f, hireling: 0x24 });
/** The named panels the hero's death closes, from the table. */
export const panelsClosedOnDeath = () => Object.keys(DEATH_PANEL).filter((name) => closedOnDeath(DEATH_PANEL[name]));

/**
 * Whether an outgoing game packet may leave while the hero is dead. `what` is the input layer's name for it.
 * Read from the client: dead, no panel opens and nothing toggles except the chat line (0xC2790), and Esc sends the
 * restart only once the hero lies in mode DEAD, not while it falls (0xC1A80, 0x65760).
 * GUESS (world clicks and item/skill keys while dead): refused here as a whole; the client's click and key handlers
 * were not each read for their dead test. capture: session <clicks and belt/skill keys pressed on the death screen,
 * the outgoing packets recorded>.
 */
export const deathSendAllowed = (mode, what) => !heroIsDown(mode) || (what === 'respawn' && mode === PLAYER_DEAD);

const DEAD_KEY_ACTIONS = new Set(['Chat', 'Run', 'Stand Still', 'Show Items']);
/**
 * Whether a key press is swallowed while the hero is dead: everything but Esc (the restart), the chat key and the
 * held modifiers, which send nothing. (Typing into an open chat line is handled before this is asked.)
 */
export const deathBlocksKey = (mode, code, action) => heroIsDown(mode) && code !== 'Escape' && !DEAD_KEY_ACTIONS.has(action);

/** The character list with the named hardcore characters marked dead (flag 0x08), as the client sets its own flag when a dead hardcore hero leaves its game. */
export function markCharactersDead(characters, deadNames) {
  if (!deadNames?.size) return characters;
  return (characters ?? []).map((c) => (deadNames.has(String(c.name).toLowerCase()) && (c.flags & HARDCORE) ? { ...c, flags: c.flags | DEAD_FLAG } : c));
}

export const TRADE_CANCEL = Object.freeze([0x4f, 0x02, 0, 0, 0, 0, 0]);

/**
 * The hero's death, in one place.
 *   hardcore()        true for a hardcore hero
 *   respawn()         sends the restart (C 0x41)
 *   leave()           Save and Exit's own path (C 0x69, then back to the lobby)
 *   close(names)      closes the named panels (panelsClosedOnDeath) and drops the walk-to interaction
 *   trade()           the trade state ({ open }) or null
 *   send(bytes)       a packet on the game connection
 *   markDead()        the character is dead from here on
 * observe(mode) is fed the hero's mode whenever it is read; escape(mode) is the Esc key. Both return true when they acted.
 *
 * As the 1.13c client runs it:
 *   the fall (S 0x0D selector 8 on the hero): the table's panels close; an open trade is cancelled with the trade
 *     window's own cancel (C 4F 02 00 00 00 00 00, what closing panel 0x17 sends: 0xC21F0 then 0x59740); the pending
 *     walk-to interaction is cleared (0x16A0).
 *   Esc while falling: nothing, and no panel opens.
 *   Esc lying dead, softcore: C 0x41; the game menu does not open.
 *   Esc lying dead, hardcore: C 0x41, then C 0x69, the character's dead flag (0x08) is set and the game is left.
 * Entering the game already dead closes nothing (only the "die" action does), and nothing can be open then.
 */
export function createRealmDeath({ hardcore, respawn, leave, close, trade = () => null, send = () => {}, markDead = () => {} }) {
  let down = false, leaving = false;
  return {
    observe(mode) {
      const now = heroIsDown(mode), fell = now && !down;
      down = now;
      if (!now) leaving = false;
      if (!fell) return false;
      const t = trade();
      if (t?.open) { send(TRADE_CANCEL.slice()); t.open = false; }
      close(panelsClosedOnDeath());
      return true;
    },
    escape(mode) {
      if (!heroIsDown(mode)) return false;
      if (mode !== PLAYER_DEAD || leaving) return true;   // swallowed: the game menu never opens on a dead hero
      respawn();
      if (hardcore()) { leaving = true; markDead(); leave(); }
      return true;
    },
    get leaving() { return leaving; },
  };
}
