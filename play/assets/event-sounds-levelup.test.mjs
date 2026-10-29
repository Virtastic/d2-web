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
const texts = ['New Stats', 'New Skill'];
assert.deepEqual(levelButtonTooltip(0, 800, 600, texts), { bar: 'stats', text: 'New Stats', x: 221, y: 550, color: 0 });   // D2Client+0x50430: W/2-179, H-50
assert.deepEqual(levelButtonTooltip(1, 800, 600, texts), { bar: 'skill', text: 'New Skill', x: 578, y: 550, color: 0 });   // D2Client+0x501F0: W/2+178
assert.deepEqual(levelButtonTooltip(0, 1920, 1080, texts), { bar: 'stats', text: 'New Stats', x: 781, y: 1030, color: 0 }); // widescreen: follows W/2, not 800
assert.equal(levelButtonTooltip(-1, 800, 600, texts), null);
// deliberate-failure control: the assertion machinery rejects a wrong expectation
assert.throws(() => assert.equal(run([{ level: 1 }, { level: 2 }]).length, 0));
console.log('ok');
