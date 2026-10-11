import { decodeWav } from './wav.mjs';

export const ACT_MUSIC = Object.freeze(['Act1\\town1.wav', 'Act2\\town2.wav', 'Act3\\town3.wav', 'Act4\\town4.wav', 'Act5\\xtown.wav']);

const MUSIC_VOLUME_MAX = 255;

/** Resolve the native source's Sounds.txt base volume for one retail music file. */
export function resolveActMusicSourceVolume(soundsTxt, path) {
  if (typeof soundsTxt !== 'string' || typeof path !== 'string') throw new TypeError('Sounds.txt and music path are required');
  const lines = soundsTxt.split(/\r?\n/);
  const columns = lines[0]?.split('\t') ?? [];
  const fileColumn = columns.indexOf('FileName'), volumeColumn = columns.indexOf('Volume'), nameColumn = columns.indexOf('Sound'), musicColumn = columns.indexOf('Music Vol');
  if (fileColumn < 0 || volumeColumn < 0 || nameColumn < 0 || musicColumn < 0) throw new Error('Sounds.txt is missing music source columns');
  const wanted = path.replaceAll('\\', '/').toLowerCase();
  const rows = lines.slice(1).map((line) => line.split('\t')).filter((row) =>
    row[fileColumn]?.replaceAll('\\', '/').toLowerCase() === wanted && row[nameColumn]?.startsWith('music_'));
  if (rows.length !== 1) throw new Error(`expected one music Sounds.txt row for ${path}, found ${rows.length}`);
  const volume = Number(rows[0][volumeColumn]);
  if (!Number.isInteger(volume) || volume < 0 || volume > MUSIC_VOLUME_MAX || Number(rows[0][musicColumn]) !== 1) {
    throw new Error(`invalid music source row for ${path}`);
  }
  return volume;
}

/** D2Client applies Music then Master with signed integer division before D2Sound's /255 scale. */
export function retailMusicChannelGain(sourceVolume, music, master) {
  const percent = (value) => Math.max(0, Math.min(100, Math.trunc(Number(value) * 100 + 1e-7)));
  let volume = Math.trunc(sourceVolume * percent(music) / 100);
  volume = Math.trunc(volume * percent(master) / 100);
  return volume / MUSIC_VOLUME_MAX;
}

// Every browser starts a fresh AudioContext 'suspended' until a user gesture (
// "Support matrix": iOS always; Chrome/Firefox once autoplay heuristics decide there was none). This
// resumes it on the FIRST pointer, key, touch or gamepad activity anywhere on the page, rather than waiting for whatever later UI click happens to call `play()` --
// so town music can start the instant a level loads, not on some second, unrelated click.
function unlockOnGesture(context, target = globalThis.window) {
  if (typeof target?.addEventListener !== 'function' || context.state !== 'suspended') return;
  const controller = new AbortController();
  let resuming = false;
  const unlock = () => {
    if (resuming || context.state !== 'suspended') return;
    resuming = true;
    let resumed;
    try { resumed = Promise.resolve(context.resume()); }
    catch { resuming = false; return; }
    resumed.then(() => {
      resuming = false;
      // A browser can reject resume() when the first event was not a qualifying user gesture.
      // Keep the listeners installed in that case so a later real click/key/touch can unlock audio.
      if (context.state !== 'suspended') controller.abort();
    }, () => { resuming = false; });
  };
  const opts = { signal: controller.signal, passive: true };
  for (const type of ['pointerdown', 'keydown', 'touchstart', 'gamepadconnected']) target.addEventListener(type, unlock, opts);
}

export function createActMusicPlayer(read, AudioContextClass = globalThis.AudioContext || globalThis.webkitAudioContext, gestureTarget = globalThis.window) {
  if (typeof read !== 'function') throw new TypeError('retail asset reader is required');
  if (!AudioContextClass) throw new Error('Web Audio is unavailable');
  const context = new AudioContextClass(), buffers = new Map();
  unlockOnGesture(context, gestureTarget);
  // F235: the Music slider's value reaches the loop through this one node. Web Audio's GainNode is
  // the only way to attenuate it; a context without createGain (a minimal implementation, or a unit
  // test's double) still plays, at full gain.
  const gain = typeof context.createGain === 'function' ? context.createGain() : null;
  if (gain) gain.connect(context.destination);
  const out = gain ?? context.destination;
  let volume = 1, sourceVolume = 255, sliders = { sound: 1, music: 1 }, soundsTxt = null;
  const applyGain = () => { if (gain) gain.gain.value = retailMusicChannelGain(sourceVolume, sliders.music, sliders.sound); };
  let source = null, activeAct = -1;
  async function prepare(act0) {
    const act = Math.min(4, Math.max(0, Math.trunc(Number(act0) || 0)));
    if (soundsTxt === null) soundsTxt = new TextDecoder('latin1').decode(read('data\\global\\excel\\Sounds.txt'));
    sourceVolume = resolveActMusicSourceVolume(soundsTxt, ACT_MUSIC[act]); applyGain();
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
      source = context.createBufferSource(); source.buffer = prepared.buffer; source.loop = true; source.connect(out); source.start(); activeAct = prepared.act0;
      return prepared;
    },
    prepare,
    /** The Music slider's value, 0..1 (F235). */
    setVolume(v, nextSliders) {
      volume = Math.max(0, Math.min(1, Number(v) || 0));
      if (nextSliders) sliders = { sound: nextSliders.sound, music: nextSliders.music };
      else sliders = { sound: 1, music: volume };
      applyGain();
    },
    /** The live gain node's own value -- what the loop is actually played through, not what was asked. */
    get volume() { return gain ? gain.gain.value : volume; },
    stop() { if (source) { source.stop(); source.disconnect(); source = null; } activeAct = -1; },
    close() { this.stop(); return context.close?.(); },
    get evidence() { return { activeAct, decodedActs: buffers.size, sourceVolume, sourceGain: sourceVolume / MUSIC_VOLUME_MAX, channelGain: gain ? gain.gain.value : null, state: context.state }; },
  };
}
