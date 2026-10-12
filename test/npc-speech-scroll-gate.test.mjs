import test from 'node:test';
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import { createNpcSpeechScrollGate } from '../play/assets/npc-speech-scroll-gate.mjs';

const deferred = () => {
  let resolve;
  const promise = new Promise((done) => { resolve = done; });
  return { promise, resolve };
};

test('voice gate withholds the combined speech box and text renderer', () => {
  const shell = readFileSync(new URL('../play/gameplay-shell.mjs', import.meta.url), 'utf8');
  const box = readFileSync(new URL('../play/assets/speech-box.mjs', import.meta.url), 'utf8');
  assert.match(shell, /waitForVoice\(token, playback, unitSounds\)/);
  assert.match(shell, /if \(npcSpeechHasText\(npcSpeechSetting\.mode\) && !shopOpen\(\) && ensureSpeechBox\(\) && npcSpeechScrollGate\.ready\)\s*pendingSpeech = \{ text: npcState\.greeting, messageInstance: npcVoiceGeneration, scrolling: true \}/);
  assert.match(shell, /npcSpeechHasText\(npcSpeechSetting\.mode\) && !shopOpen\(\) && ensureSpeechBox\(\) && npcSpeechScrollGate\.ready/);
  assert.match(shell, /speechBox\.draw\(ctx, pendingSpeech\.text, undefined,\s*\{ messageInstance: pendingSpeech\.messageInstance \}\)/);
  assert.doesNotMatch(shell, /speechT0/);
  // Native FUN_6FB2BB40 draws its container and text in one call; Text Only and Audio+Text share
  // that renderer, while Audio Only does not invoke it.
  const draw = box.slice(box.indexOf('draw(ctx, text'), box.indexOf('\n    },', box.indexOf('draw(ctx, text')));
  assert.ok(draw.indexOf('ctx.fillRect(') >= 0);
  assert.ok(draw.indexOf('ctx.fillRect(') < draw.indexOf('font.drawAnchored('));
});

test('speech clock stays unopened until playback and the active voice channel finish', async () => {
  const gate = createNpcSpeechScrollGate();
  const playback = deferred(), voice = deferred();
  const token = gate.begin();
  const waiting = gate.waitForVoice(token, playback.promise, { whenDialogueDone: () => voice.promise });

  assert.equal(gate.ready, false);
  playback.resolve({ started: true });
  await Promise.resolve();
  assert.equal(gate.ready, false);
  voice.resolve();
  await waiting;
  assert.equal(gate.ready, true);
});

test('unplayed and stale voices cannot hold or reopen a later speech clock', async () => {
  const gate = createNpcSpeechScrollGate();
  const stale = deferred();
  const oldToken = gate.begin();
  const waiting = gate.waitForVoice(oldToken, stale.promise, { whenDialogueDone: async () => {} });
  gate.cancel();
  const newToken = gate.begin();
  stale.resolve({ started: false });
  await waiting;
  assert.equal(gate.ready, false);

  await gate.waitForVoice(newToken, Promise.resolve({ started: false }));
  assert.equal(gate.ready, true);
});
