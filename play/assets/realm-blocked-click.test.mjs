import test from 'node:test';
import assert from 'node:assert/strict';
import { existsSync, readFileSync } from 'node:fs';
import { fileURLToPath } from 'node:url';
import { blockedClicks, endDiffers, QUIET } from './realm-blocked-click.mjs';

const click = (tick, op, x, y) => { const b = Buffer.alloc(5); b[0] = op; b.writeUInt16LE(x, 1); b.writeUInt16LE(y, 3); return { tick, kind: 'C', text: b.toString('hex') }; };
const hero = (tick, x, y, mode) => ({ tick, kind: 'H', text: `${x},${y},${mode},0,0,100,110,100,00000001` });

test('a run that stands short of its click is a blocked click; one that arrives is not', () => {
  const short = [click(10, 3, 100, 100), hero(11, 90, 100, 3), hero(12, 95, 100, 3), hero(13, 96, 100, 1)];
  assert.deepEqual(blockedClicks(short).map((c) => [c.tick, c.x, c.y, c.stopX, c.stopY]), [[10, 100, 100, 96, 100]]);
  assert.deepEqual(blockedClicks([click(10, 3, 100, 100), hero(11, 99, 100, 3), hero(12, 100, 101, 1)]), []);
});

test('a dragged run, a click another action took over, and a walk that never moved are not blocked clicks', () => {
  const drag = [click(10, 3, 100, 100), hero(11, 90, 100, 3), click(10 + QUIET - 1, 3, 120, 100), hero(50, 96, 100, 3), hero(51, 96, 100, 1)];
  assert.deepEqual(blockedClicks(drag).map((c) => c.tick), [10 + QUIET - 1]);   // only the last click of the burst can be blocked
  assert.ok(!blockedClicks(drag).some((c) => c.tick === 10), 'the first click of a drag is superseded, not blocked');
  const taken = [click(10, 3, 100, 100), hero(11, 90, 100, 3), { tick: 12, kind: 'C', text: '0401000000' + '01000000' }, hero(13, 94, 100, 1)];
  assert.deepEqual(blockedClicks(taken), []);
  assert.deepEqual(blockedClicks([click(10, 1, 100, 100), hero(11, 90, 100, 1)]), []);
});

test('the end of the walk is compared, with the records that settle after it', () => {
  const r = [click(10, 3, 100, 100), hero(11, 90, 100, 3), hero(12, 96, 100, 1), hero(13, 96, 100, 1)];
  const [c] = blockedClicks(r);
  const same = new Map(r.filter((x) => x.kind === 'H').map((x) => { const [px, py] = x.text.split(',').map(Number); return [x, [px, py, px, py]]; }));
  assert.deepEqual(endDiffers(r, same, c), []);
  const past = new Map(same); past.set(r[3], [96, 100, 96, 98]);   // ours walked on past retail's end
  assert.deepEqual(endDiffers(r, past, c), [13]);
  assert.deepEqual(endDiffers(r, new Map(), c), [12, 13]);   // no sample = a miss, never a pass
});

// The committed retail-client recordings: the detector reads real walks. Retail's own H lines are the reference.
const dir = fileURLToPath(new URL('../../vectors/session/bnet-play/', import.meta.url));
test('the recorded field streams hold blocked clicks whose retail walk ended 2-4 cells short', { skip: !existsSync(dir + 'field-a.txt') && 'recordings absent' }, () => {
  const parse = (f) => readFileSync(dir + f, 'utf8').split('\n').flatMap((l) => { const m = /^(\d+) ([SCH]) (\S+)/.exec(l); return m ? [{ tick: +m[1], kind: m[2], text: m[3] }] : []; });
  const a = blockedClicks(parse('field-a.txt')), b = blockedClicks(parse('field-b.txt'));
  assert.deepEqual(a.map((c) => [c.tick, c.stopX, c.stopY]), [[623, 4655, 4625]]);
  assert.deepEqual(b.map((c) => c.tick), [2283]);
});
