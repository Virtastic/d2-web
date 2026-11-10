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
 * A corpse whose unit is not in view (or not yet assigned) is left out: there is nothing to click.
 */
export function realmCorpseViews(pairs, players, ownGuid) {
  const out = [];
  for (const pair of pairs ?? []) {
    const unit = (players ?? []).find((p) => p.guid === pair.corpse);
    if (!unit) continue;
    out.push({ guid: pair.corpse >>> 0, owner: pair.owner >>> 0, own: ownGuid != null && (pair.owner >>> 0) === (ownGuid >>> 0),
      mode: unit.renderState?.mode ?? unit.mode, x: unit.x, y: unit.y });
  }
  return out;
}
