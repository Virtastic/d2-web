// D24: the client's level-up feedback, deterministic. Sounds.txt row cursor_level_up plays once per level
// gained whether the engine's own event (soundEvent 2) arrived or only the level poll saw it; the lit
// HUD buttons carry the "New Stats"/"New Skill" hover text (string.tbl 3986/3987).
import assert from 'node:assert/strict';
import { createEventSounds } from './event-sounds.mjs';
import { levelButtonTooltip } from './hud-bar-tooltip.mjs';
const sounds = 'Sound\tIndex\tFileName\nstub\t0\ta.wav\ncursor_level_up\t7\tcursor\\levelup.wav\n';
const read = (p) => new TextEncoder().encode(/Sounds/.test(p) ? sounds : 'code\tdropsound\n');
const run = (steps) => { const got = []; const es = createEventSounds(read, (row, e) => got.push([row, e.kind]));
  for (const s of steps) { if (s.server) es.noteServerLevelUp(); es.update({}, s.level); } return got.filter((g) => g[1] === 'levelup'); };
// poll alone (server event lost to the single attach slot) -> one sound
assert.equal(run([{ level: 1 }, { level: 1 }, { level: 2 }, { level: 2 }]).length, 1);
// server event already played the sound -> the poll must not repeat it
assert.equal(run([{ level: 1 }, { level: 2, server: true }, { level: 2 }]).length, 0);
// two levels, two server events -> none from the poll; a later server-less level -> one
assert.equal(run([{ level: 1 }, { level: 2, server: true }, { level: 3, server: true }, { level: 4 }]).length, 1);
// no level change -> silence
assert.equal(run([{ level: 5 }, { level: 5 }]).length, 0);
// buttons: lit button -> tooltip text; not hovering -> none
const box = { left: 206, top: 563, w: 30, h: 30 }, texts = ['New Stats', 'New Skill'];
assert.equal(levelButtonTooltip(0, box, texts).text, 'New Stats');
assert.equal(levelButtonTooltip(1, box, texts).text, 'New Skill');
assert.equal(levelButtonTooltip(-1, box, texts), null);
// deliberate-failure control: the assertion machinery rejects a wrong expectation
assert.throws(() => assert.equal(run([{ level: 1 }, { level: 2 }]).length, 0));
console.log('ok');
