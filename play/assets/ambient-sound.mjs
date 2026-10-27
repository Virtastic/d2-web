import { decodeWav } from './wav.mjs';
import { resolveLevelSoundEnvironment } from './act-music.mjs';
import { retailSoundChannelVolume, sliderPercent } from './sound-channel-volume.mjs';

const ROOTS = ['data\\global\\sfx\\', 'data\\local\\sfx\\'];

/** Sounds.txt line whose Index column is `id` -> { id, file, volume }, or null for id 0 / no file. */
export function soundByIndex(soundsTxt, id) {
  if (!(id > 0)) return null;
  const lines = soundsTxt.replace(/\r/g, '').split('\n');
  const cols = lines[0].split('\t'), ix = cols.indexOf('Index'), fx = cols.indexOf('FileName'), vx = cols.indexOf('Volume');
  if (ix < 0 || fx < 0 || vx < 0) throw new Error('Sounds.txt is missing Index/FileName/Volume');
  const rows = lines.slice(1).map((l) => l.split('\t')).filter((c) => Number(c[ix]) === id);
  if (rows.length !== 1) throw new Error(`expected one Sounds.txt row for ${id}, found ${rows.length}`);
  const volume = Number(rows[0][vx]);
  return rows[0][fx]?.trim() ? { id, file: rows[0][fx].trim(), volume: Number.isInteger(volume) ? volume : 255 } : null;
}

/** The engine's raw period of day (D2Common #10396 value: 0 day, 1..3 dusk/night/dawn = night ambience); 0 on an engine without the export. */
export function ambientCycleOf(Module) {
  return Module?._d2_session_period_of_day ? Module.ccall('d2_session_period_of_day', 'number', [], []) | 0 : 0;
}

/** 1.13c area-event cadence: E + R - floor(E/3), R in [0, 2*floor(E/3)]. */
export function nextEventInterval(delay, rand) {
  const d = Math.floor(delay / 3);
  return delay + Math.floor(rand() * (2 * d + 1)) - d;
}

/**
 * Looping area ambience plus jittered one-shot events. One loop source at a time: the same ambience id
 * is a no-op, a different id (level or day/night change) stops the old loop first, so loops never stack.
 * `cycle` is the raw D2Common-10396 value (1..3 = night), read via ambientCycleOf(Module).
 */
export function createAmbientSound(read, AudioContextClass = globalThis.AudioContext || globalThis.webkitAudioContext, rand = Math.random) {
  if (typeof read !== 'function') throw new TypeError('retail asset reader is required');
  if (!AudioContextClass) throw new Error('Web Audio is unavailable');
  const context = new AudioContextClass(), buffers = new Map();
  const gain = typeof context.createGain === 'function' ? context.createGain() : null;
  if (gain) gain.connect(context.destination);
  const out = gain ?? context.destination;
  let tables = null, sliders = { sound: 1, music: 1 }, volume = 1;
  let loop = null, loopId = 0, loopVolume = 0, serial = 0, key = '', env = null, countdown = 0, error = '', events = 0;
  const level = (v) => retailSoundChannelVolume(v, sliderPercent(sliders.sound), 100, false) / 255;
  const load = async (snd) => {
    let b = buffers.get(snd.file);
    if (b) return b;
    let bytes, err;
    for (const r of ROOTS) { try { bytes = read(r + snd.file); break; } catch (e) { err = e; } }
    if (!bytes) throw err;
    decodeWav(bytes);
    b = await context.decodeAudioData(bytes.buffer.slice(bytes.byteOffset, bytes.byteOffset + bytes.byteLength));
    buffers.set(snd.file, b); return b;
  };
  const stopLoop = () => { if (loop) { try { loop.src.stop(); } catch {} loop.src.disconnect(); loop.node?.disconnect(); loop = null; } loopId = 0; };
  const start = (buffer, snd, looping) => {
    const src = context.createBufferSource(); src.buffer = buffer; src.loop = looping;
    const node = typeof context.createGain === 'function' ? context.createGain() : null;
    if (node) { node.gain.value = level(snd.volume); src.connect(node); node.connect(out); } else src.connect(out);
    src.start(); return { src, node };
  };
  function read_() {
    return tables ??= ['Levels', 'SoundEnviron', 'Sounds'].map((n) => new TextDecoder('latin1').decode(read(`data\\global\\excel\\${n}.txt`)));
  }
  return {
    /** Select the level's ambience for `cycle`; idempotent per (level, ambience). */
    async sync(levelId, cycle = 0) {
      const [lv, se, sn] = read_();
      const sel = resolveLevelSoundEnvironment(levelId, cycle, lv, se);
      const k = `${sel.environmentId}:${sel.ambienceId}:${sel.eventId}:${sel.eventDelay}`;
      if (k === key) return sel;
      key = k; const mine = ++serial;
      env = sel.eventId > 0 && sel.eventDelay > 0 ? { snd: soundByIndex(sn, sel.eventId), delay: sel.eventDelay } : null;
      countdown = env ? nextEventInterval(env.delay, rand) : 0;
      const snd = soundByIndex(sn, sel.ambienceId);
      if (loopId === sel.ambienceId && loop) return sel;
      stopLoop();
      if (!snd) return sel;
      try {
        const [b] = await Promise.all([load(snd), context.resume?.()]);
        if (mine !== serial) return sel;          // a newer level superseded this request
        stopLoop(); loop = start(b, snd, true); loopId = sel.ambienceId; loopVolume = snd.volume;
      } catch (e) { error = String(e); key = ''; throw e; }
      return sel;
    },
    /** Advance the event countdown by `n` engine ticks. */
    async tick(n = 1) {
      if (!env?.snd || !loop) return;
      countdown -= n;
      if (countdown > 0) return;
      countdown = nextEventInterval(env.delay, rand);
      try { start(await load(env.snd), env.snd, false); events++; } catch (e) { error = String(e); }
    },
    stop() { serial++; key = ''; env = null; stopLoop(); },
    close() { this.stop(); return context.close?.(); },
    setVolume(v, next) {
      volume = Math.max(0, Math.min(1, Number(v) || 0));
      sliders = next ? { sound: next.sound, music: next.music } : { sound: volume, music: 1 };
      if (loop?.node) loop.node.gain.value = level(loopVolume);
    },
    get volume() { return volume; },
    get evidence() { return { loopId, events, error, state: context.state }; },
  };
}
