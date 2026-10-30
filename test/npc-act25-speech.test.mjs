// Act 2-5 town NPC speech: every Introduction/Gossip id in NPC_SPEECH_BY_NAME must name the same NPC in the
// D2MOO StringIds.h header (STR_IDX_<id>_<Name>). Speechless NPCs (no header string) must resolve to null.
import { test } from 'node:test';
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import { fileURLToPath } from 'node:url';
import { dirname, join } from 'node:path';
import { NPC_SPEECH_BY_NAME, npcSpeechByName } from '../play/assets/npc-topics.mjs';

const here = dirname(fileURLToPath(import.meta.url));
const header = readFileSync(join(here, '..', 'src/core/d2moo/include/DataTbls/StringIds.h'), 'utf8');
const headerName = new Map();
for (const m of header.matchAll(/STR_IDX_(\d+)_(\w+)=\1\b/g)) headerName.set(Number(m[1]), m[2]);
const stem = (s) => s.toLowerCase().replace(/[^a-z]/g, '');

// Act 2-5 entries, keyed by NPC name and act (a single-act entry is {intro, gossip}).
const rows = [];
for (const [name, row] of Object.entries(NPC_SPEECH_BY_NAME)) {
  if ('gossip' in row) rows.push([name, null, row]);
  else for (const [act, r] of Object.entries(row)) if (Number(act) >= 2) rows.push([name, Number(act), r]);
}

for (const [name, act, r] of rows) {
  test(`${name}${act ? ` (Act ${act})` : ''}: speech ids name this NPC in StringIds.h`, () => {
    const ids = [r.intro, r.gossip].filter((v) => v !== null && v !== undefined);
    assert.ok(ids.length > 0, 'at least one line');
    for (const id of ids) {
      const h = headerName.get(id);
      assert.ok(h, `id ${id} has a header name`);
      // The header spells some names differently from the display name (Greiz is GriezIntroGossip1).
      const spelled = { greiz: 'griez', 'nihlathak town': 'nihlathak' }[name] ?? name.replace(/^deckard /, '');
      const want = stem(spelled);
      assert.ok(stem(h).includes(want), `id ${id} (${h}) names ${name}`);
    }
    const got = npcSpeechByName(name, act ?? 1);
    assert.equal(got?.gossip ?? null, r.gossip ?? null);
  });
}

test('nihlathak town (class 514 display name) resolves to the same lines as nihlathak', () => {
  assert.deepEqual(npcSpeechByName('nihlathak town', 5), npcSpeechByName('nihlathak', 5));
  assert.notEqual(npcSpeechByName('nihlathak town', 5), null);
});

test('tyrael has no Act 2 Introduction string, only Gossip', () => {
  assert.equal(NPC_SPEECH_BY_NAME.tyrael[2].intro, null);
  assert.equal(typeof NPC_SPEECH_BY_NAME.tyrael[2].gossip, 'number');
});

test('speechless Act 5 NPCs (no header string) resolve to null', () => {
  for (const name of ['halbu', 'navi', 'drehya', 'jamella', 'malachai', 'izual', 'kaelan', 'tyrael act 5']) {
    assert.equal(npcSpeechByName(name, 5), null, name);
  }
});

test('unknown NPC names and acts without a line return null', () => {
  assert.equal(npcSpeechByName('nobody', 2), null);
});
