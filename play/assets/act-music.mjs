import { decodeWav } from './wav.mjs';

export const ACT_MUSIC = Object.freeze(['Act1\\town1.wav', 'Act2\\town2.wav', 'Act3\\town3.wav', 'Act4\\town4.wav', 'Act5\\xtown.wav']);

// Every browser starts a fresh AudioContext 'suspended' until a user gesture (
// "Support matrix": iOS always; Chrome/Firefox once autoplay heuristics decide there was none). This
// resumes it on the FIRST pointer, key, touch or gamepad activity anywhere on the page, rather than waiting for whatever later UI click happens to call `play()` --
// so town music can start the instant a level loads, not on some second, unrelated click.
function unlockOnGesture(context, target = globalThis.window) {
  if (typeof target?.addEventListener !== 'function' || context.state !== 'suspended') return;
  const controller = new AbortController();
  const unlock = () => { context.resume().catch(() => {}); controller.abort(); };
  const opts = { signal: controller.signal, passive: true };
  for (const type of ['pointerdown', 'keydown', 'touchstart', 'gamepadconnected']) target.addEventListener(type, unlock, opts);
}

export function createActMusicPlayer(read, AudioContextClass = globalThis.AudioContext || globalThis.webkitAudioContext, gestureTarget = globalThis.window) {
  if (typeof read !== 'function') throw new TypeError('retail asset reader is required');
  if (!AudioContextClass) throw new Error('Web Audio is unavailable');
  const context = new AudioContextClass(), buffers = new Map();
  unlockOnGesture(context, gestureTarget);
  let source = null, activeAct = -1;
  async function prepare(act0) {
    const act = Math.min(4, Math.max(0, Math.trunc(Number(act0) || 0)));
    let buffer = buffers.get(act);
    if (!buffer) {
      const bytes = read(`data\\global\\music\\${ACT_MUSIC[act]}`);
      decodeWav(bytes);
      const copy = bytes.buffer.slice(bytes.byteOffset, bytes.byteOffset + bytes.byteLength);
      buffer = await context.decodeAudioData(copy); buffers.set(act, buffer);
    }
    return { act0: act, path: ACT_MUSIC[act], buffer };
  }
  return {
    async play(act0) {
      const prepared = await prepare(act0);
      await context.resume();
      if (activeAct === prepared.act0 && source) return prepared;
      if (source) { source.stop(); source.disconnect(); }
      source = context.createBufferSource(); source.buffer = prepared.buffer; source.loop = true; source.connect(context.destination); source.start(); activeAct = prepared.act0;
      return prepared;
    },
    prepare,
    stop() { if (source) { source.stop(); source.disconnect(); source = null; } activeAct = -1; },
    close() { this.stop(); return context.close?.(); },
    get evidence() { return { activeAct, decodedActs: buffers.size, state: context.state }; },
  };
}
