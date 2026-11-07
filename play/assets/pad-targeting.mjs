// Gamepad world targeting, pure geometry. No DOM, no engine: a snapshot, a projection and an aim in,
// a ranked candidate list out, so a test feeds it plain objects.
//
// D2R's developers (PlayStation Blog, 2021-08-13): the playfield is scanned with a large cone and the
// target is chosen by priority -- monsters, items, interactable objects, other players, corpses. The
// cone is centred on the stick's aim in SCREEN space (up on the stick is up on the screen, as for the
// walk). Inside a tier the target needing the fewest 45-degree turns from the aim wins, then the
// nearest. The engine's own facing is not in the snapshot, so with no aim the cone is the whole circle
// and only distance ranks (the turn bucket is 0 for all).
import { isPrimaryTargetEligible } from '../gameplay-model.mjs';

export const TARGET_RADIUS = 12;   // world subtiles, monsters / items / NPCs / players / corpses
export const OBJECT_RADIUS = 7;    // chebyshev, gameplay-shell.mjs's COMMAND.INTERACT nearest-object scan
export const CONE_HALF = Math.PI / 3;   // +-60 degrees: a 120-degree cone
const TURN = Math.PI / 4;
export const TIER = { monster: 0, item: 1, object: 2, player: 3, corpse: 4 };

const angleBetween = (a, b) => {
  const d = Math.abs(Math.atan2(a[1], a[0]) - Math.atan2(b[1], b[0])) % (2 * Math.PI);
  return d > Math.PI ? 2 * Math.PI - d : d;
};

/** Every candidate in reach of `snapshot.player`, best first. `aim` is a screen vector [ax, ay] or null. */
export function rankTargets(snapshot, { aim = null, project, origin = null } = {}) {
  const p = snapshot?.player;
  if (!p || !project) return [];
  const o = origin || project(p.x, p.y);
  const dist2 = (u) => (u.x - p.x) ** 2 + (u.y - p.y) ** 2;
  const out = [];
  const add = (tier, kind, u, screen, d) => {
    const at = screen || project(u.x, u.y);
    const v = [at[0] - o[0], at[1] - o[1]];
    let turns = 0;
    if (aim && (v[0] || v[1])) {
      const a = angleBetween(aim, v);
      if (a > CONE_HALF) return;
      turns = Math.round(a / TURN);
    }
    out.push({ tier, kind, id: u.id ?? u.slot, x: u.x, y: u.y, screen: at, turns, d });
  };
  const near = (u) => dist2(u) <= TARGET_RADIUS * TARGET_RADIUS;
  for (const e of snapshot.entities || []) {
    if (e.npc) { if (near(e)) add(TIER.object, 'npc', e, null, dist2(e)); continue; }
    if (e.type === 1 && (e.mode === 0 || e.mode === 12)) { if (near(e)) add(TIER.corpse, 'corpse', e, null, dist2(e)); continue; }
    if (e.type === 0) { if (e.id !== snapshot.playerId && near(e)) add(TIER.player, 'player', e, null, dist2(e)); continue; }
    if (isPrimaryTargetEligible(e) && near(e)) add(TIER.monster, 'unit', e, null, dist2(e));
  }
  for (const it of snapshot.items || []) {
    if (it.mode === 3 && it.invPage === 255 && near(it)) add(TIER.item, 'item', it, null, dist2(it));
  }
  for (const ob of snapshot.objects || []) {
    const c = Math.max(Math.abs(ob.x - p.x), Math.abs(ob.y - p.y));
    if (c < OBJECT_RADIUS) add(TIER.object, 'object', ob, null, c * c);
  }
  // Stairs, portals and the like: the warp's own click box, the one the shell's warp hover tests.
  for (const w of snapshot.warps || []) {
    if (!w.enabled || w.located === false || w.unitTile === false || !w.select || w.destinationLevel === snapshot.level) continue;
    const [ax, ay] = project(w.originX, w.originY);
    if (near(w)) add(TIER.object, 'warp', w, [Math.round(ax + w.select.x + w.select.dx / 2), Math.round(ay + w.select.y + w.select.dy / 2)], dist2(w));
  }
  return out.sort((a, b) => a.tier - b.tier || a.turns - b.turns || a.d - b.d || (a.id - b.id));
}

/** The chosen candidate: the best of the best tier, or, with `cycle` n, the nth in the ranked list (wrapping). */
export function pickTarget(ranked, cycle = 0) {
  if (!ranked.length) return null;
  const n = ranked.length;
  return ranked[((cycle % n) + n) % n];
}

// Opt-in gold pickup: the nearest gold pile on the ground within `radius` tiles of the player, or
// null. The pad clicks it like a player would; nothing about pickup itself changes.
export const GOLD_RADIUS = 3;
// The snapshot reports an item's code as its packed dword (gold 'gld ' = 0x20646C67 = 543452263); a
// freshly dropped pile is mode 5 (still dropping) and a settled one mode 3, and both are on the ground.
const GOLD_CODE = 0x20646C67;
export function goldInReach(snapshot, radius = GOLD_RADIUS) {
  const p = snapshot?.player;
  if (!p) return null;
  let best = null, bd = radius * radius;
  for (const it of snapshot.items || []) {
    if (it.mode !== 3 && it.mode !== 5 || it.invPage !== 255 || (it.code !== 'gld' && it.code !== GOLD_CODE)) continue;
    const d = (it.x - p.x) ** 2 + (it.y - p.y) ** 2;
    if (d <= bd) { bd = d; best = it; }
  }
  return best;
}
