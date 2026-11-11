// A click the retail client sent (0x01 walk or 0x03 run to a point) whose walk ended short of the clicked point: the point was
// blocked or unreachable, so the server's path (and the client's own prediction of it, D2Client 0x2A7C0 then 0x01/0x03) ended
// where the path finder's nearest reachable cell is, not at the click. The recordings hold the reference client's own hero after
// every tick, so the end of such a walk is retail's answer to a blocked click.
//
// records: [{ tick, kind: 'S' | 'C' | 'H', text }] as a recording parses. A click counts only when no later click follows within
// QUIET ticks (a dragged run re-clicks every few ticks and says nothing about where one click ends), and the walk it started
// must have moved and then stood. Modes: 2 = walk, 3 = run, 1 and 5 = neutral.
export const QUIET = 30;
// Client packets that start or change an action (walk, run, use or cast on a point or a unit): the next one ends a click's walk.
const ACTION_OPS = new Set([0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14]);
const MOVING = new Set([2, 3]), STANDING = new Set([1, 5]);

export function blockedClicks(records) {
  const clicks = [];
  for (const [at, r] of records.entries()) {
    if (r.kind !== 'C' || r.text.length !== 10 || (r.text.slice(0, 2) !== '01' && r.text.slice(0, 2) !== '03')) continue;
    const b = Buffer.from(r.text, 'hex');
    clicks.push({ at, tick: r.tick, op: b[0], x: b.readUInt16LE(1), y: b.readUInt16LE(3) });
  }
  const found = [];
  for (const [i, c] of clicks.entries()) {
    const next = clicks[i + 1];
    if (next && next.tick - c.tick < QUIET) continue;
    let moved = false;
    for (let k = c.at + 1; k < records.length; k++) {
      const h = records[k];
      if (h.kind === 'C' && ACTION_OPS.has(parseInt(h.text.slice(0, 2), 16))) break;   // another action took over: not an unfinished walk
      if (h.kind !== 'H') continue;
      const [x, y, mode] = h.text.split(',').map(Number);
      if (MOVING.has(mode)) moved = true;
      else if (moved && STANDING.has(mode)) {
        if (Math.max(Math.abs(x - c.x), Math.abs(y - c.y)) > 1) found.push({ ...c, stopAt: k, stopTick: h.tick, stopX: x, stopY: y });
        break;
      }
    }
  }
  return found;
}

/**
 * Where our hero is not where retail's was once the blocked click's walk ended: the end tick and the SETTLE records after it.
 * (The ticks on the way differ by a cell for a tick or two when the hero's animation phase differs; the end is the answer.)
 * `samples` maps a recording's H record to [retail x, retail y, our x, our y]; a record with no sample counts as a miss.
 */
export const SETTLE = 4;
export function endDiffers(records, samples, click) {
  const bad = [];
  let seen = 0;
  for (let k = click.stopAt; k < records.length && seen <= SETTLE; k++) {
    if (records[k].kind !== 'H') continue;
    seen++;
    const s = samples.get(records[k]);
    if (!s || s[0] !== s[2] || s[1] !== s[3]) bad.push(records[k].tick);
  }
  return bad;
}
