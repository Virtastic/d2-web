// Row 7 (phones): every AudioContext the page makes starts suspended and resumes on a trusted gesture. The
// music, UI-click and unit players did; the ambience player only resumed inside sync(), which is not a gesture,
// so it now uses act-music's hook. Each player is fed a touchstart or a pointerdown on its gesture target and
// must resume its own suspended context, and nothing else: the contexts are not merged. On a phone only the LIFT
// (pointerup/touchend) grants activation, so a resume() from the touchstart stays pending and the touchend retries.
import test from 'node:test';
import assert from 'node:assert/strict';
import { createActMusicPlayer } from '../play/assets/act-music.mjs';
import { createAmbientSound } from '../play/assets/ambient-sound.mjs';
import { createUiClickSoundPlayer } from '../play/assets/ui-click-sound.mjs';
import { createUnitSoundPlayer } from '../play/assets/unit-sound-player.mjs';

/** A suspended AudioContext: resume() moves it to 'running' and counts the calls. */
function fakeContextClass() {
  const made = [];
  class Ctx {
    constructor() { this.state = 'suspended'; this.resumes = 0; made.push(this); }
    resume() { this.resumes++; this.state = 'running'; return Promise.resolve(); }
    createGain() { return { gain: { value: 1 }, connect() {}, disconnect() {} }; }
    createBufferSource() { return { buffer: null, connect() {}, disconnect() {}, start() {}, stop() {} }; }
    get destination() { return {}; }
    close() { return Promise.resolve(); }
  }
  return { Ctx, made };
}
const tick = () => new Promise((r) => setTimeout(r, 0));
// A reader that serves nothing: none of these players reads a file until a sound is asked for.
const read = () => { throw new Error('not read in this test'); };

const players = {
  'act-music': (AC, target) => createActMusicPlayer(read, AC, target, null),
  'ambient-sound': (AC, target) => createAmbientSound(read, AC, Math.random, null, target),
  'ui-click-sound': (AC, target) => createUiClickSoundPlayer(read, new EventTarget(), AC, target),
  'unit-sound-player': (AC, target) => createUnitSoundPlayer(read, AC, target, null),
};

for (const [name, make] of Object.entries(players)) {
  for (const type of ['pointerdown', 'pointerup', 'touchstart', 'touchend', 'keydown']) {
    test(`${name}: a ${type} resumes its own suspended context`, async () => {
      const { Ctx, made } = fakeContextClass();
      const target = new EventTarget();
      make(Ctx, target);
      assert.equal(made.length, 1, 'one context per player');
      assert.equal(made[0].state, 'suspended');
      target.dispatchEvent(new Event(type));
      await tick();
      assert.equal(made[0].resumes, 1);
      assert.equal(made[0].state, 'running');
      // a second gesture does not call resume() again on a running context
      target.dispatchEvent(new Event(type));
      await tick();
      assert.equal(made[0].resumes, 1);
    });
  }
}

test('ambient-sound: nothing resumes before a gesture (a level load alone does not unlock audio)', async () => {
  const { Ctx, made } = fakeContextClass();
  const target = new EventTarget();
  createAmbientSound(read, Ctx, Math.random, null, target);
  await tick();
  assert.equal(made[0].resumes, 0);
});

test('one gesture resumes each player context: two contexts, not merged', async () => {
  const { Ctx, made } = fakeContextClass();
  const target = new EventTarget();
  players['act-music'](Ctx, target);
  players['ambient-sound'](Ctx, target);
  assert.equal(made.length, 2, 'two players, two contexts');
  target.dispatchEvent(new Event('touchstart'));
  await tick();
  assert.deepEqual(made.map((c) => c.resumes), [1, 1], 'both listeners are on the same target');
  assert.notEqual(made[0], made[1]);
});

for (const [name, make] of Object.entries(players)) {
  test(`${name}: a touchstart whose resume stays pending does not block the touchend's retry (the phone case)`, async () => {
    const made = [];
    class PhoneCtx {
      constructor() { this.state = 'suspended'; this.resumes = 0; made.push(this); }
      // Chrome: resume() without user activation returns a promise that does not settle until one arrives.
      resume() { this.resumes++; if (this.resumes === 1) return new Promise(() => {}); this.state = 'running'; return Promise.resolve(); }
      createGain() { return { gain: { value: 1 }, connect() {}, disconnect() {} }; }
      createBufferSource() { return { buffer: null, connect() {}, disconnect() {}, start() {}, stop() {} }; }
      get destination() { return {}; }
      close() { return Promise.resolve(); }
    }
    const target = new EventTarget();
    make(PhoneCtx, target);
    target.dispatchEvent(new Event('touchstart'));
    await tick();
    assert.equal(made[0].state, 'suspended');
    target.dispatchEvent(new Event('touchend'));
    await tick();
    assert.equal(made[0].resumes, 2, 'the lift calls resume() again');
    assert.equal(made[0].state, 'running');
    target.dispatchEvent(new Event('pointerdown'));
    await tick();
    assert.equal(made[0].resumes, 2, 'listeners are gone once the context runs');
  });
}
