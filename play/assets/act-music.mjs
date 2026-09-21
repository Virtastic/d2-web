import { decodeWav } from './wav.mjs';

export const ACT_MUSIC = Object.freeze(['Act1\\town1.wav', 'Act2\\town2.wav', 'Act3\\town3.wav', 'Act4\\town4.wav', 'Act5\\xtown.wav']);

export function createActMusicPlayer(read, AudioContextClass = globalThis.AudioContext || globalThis.webkitAudioContext) {
  if (typeof read !== 'function') throw new TypeError('retail asset reader is required');
  if (!AudioContextClass) throw new Error('Web Audio is unavailable');
  const context = new AudioContextClass(), buffers = new Map();
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
