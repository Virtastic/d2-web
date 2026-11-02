// The hero's stat updates as retail's client sees them: D2Client+0xAEDF0 writes each stat the server sends
// and runs D2Client+0xAEB10, whose callees (d2_client_stat_update_actions) say what the change re-checks.
// Platform layer only: this polls the stats and reports the union of retail's actions for the ones that moved.
export const STAT_UPDATE = { EQUIPPED: 1, UNIT: 2, INVENTORY: 4, DEAD_HP: 8 };
export const ITEM_RECHECK = STAT_UPDATE.EQUIPPED | STAT_UPDATE.UNIT | STAT_UPDATE.INVENTORY;

/**
 * readStat(stat) -> the hero's current value; actions(stat, value) -> the D2Client+0xAEB10 mask for the
 * local hero. The first call only records; later calls return the mask for every stat that changed.
 */
export function createStatUpdateWatch(readStat, actions, stats = Array.from({ length: 16 }, (_, i) => i)) {
  let prev = null;
  return () => {
    const cur = stats.map(readStat);
    let mask = 0;
    if (prev) for (let i = 0; i < stats.length; i++) if (cur[i] !== prev[i]) mask |= actions(stats[i], cur[i]);
    prev = cur;
    return mask;
  };
}
