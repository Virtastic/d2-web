import test from 'node:test';
import assert from 'node:assert/strict';
import { questIconCel, QUEST_DONE_ART, QUEST_ART } from './quest-log.mjs';
import { questTabActs } from './client-rules.mjs';
import { bindStubRules } from './client-rules-stub.mjs';

bindStubRules();

test('icon cels follow the engine rule for each record mode', () => {
  assert.deepEqual(questIconCel(1, 1, 'done', false), { path: QUEST_ART.icon(1, 1), frame: 24 });
  assert.deepEqual(questIconCel(2, 3, 'done', true), { path: QUEST_DONE_ART, frame: 6 + 2 });
  assert.deepEqual(questIconCel(1, 2, 'progress', false).frame, 0);
  assert.deepEqual(questIconCel(1, 2, 'progress', true).frame, 25);
  assert.deepEqual(questIconCel(1, 2, 'locked', true).frame, 26);
  assert.deepEqual(questIconCel(1, 2, 'done', false, 40).frame, 24);
  assert.deepEqual(questIconCel(1, 2, 'done', false, 7).frame, 7);
});

test('tabs: act 1 and the selected act always, later acts once the act before is completed', () => {
  assert.deepEqual(questTabActs(1, true, [0, 0, 0, 0]), [1]);
  assert.deepEqual(questTabActs(3, true, [1, 0, 0, 0]), [1, 2, 3]);
  assert.deepEqual(questTabActs(1, true, [1, 1, 1, 1]), [1, 2, 3, 4, 5]);
  assert.deepEqual(questTabActs(1, false, [1, 1, 1, 1]), [1, 2, 3, 4]);
});
