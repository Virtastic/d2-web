import { decodeWav } from './wav.mjs';
import { unlockOnGesture } from './audio-unlock.mjs';
import { sliderPercent } from './sound-channel-volume.mjs';
import { nativeSoundChannel, soundFadeLevel } from './sound-mixer-rules.mjs';

// The mixer tick (D2Client 0x5F020, once per client frame): fades count these.
const MIXER_TICK_SECONDS = 0.04;

const ROOTS = ['data\\global\\sfx\\', 'data\\local\\sfx\\'];

/** Sounds.txt line whose Index column is `id` -> { id, file, volume }, or null for id 0 / no file. */
export function soundByIndex(soundsTxt, id) {
  if (!(id > 0)) return null;
  const lines = soundsTxt.replace(/\r/g, '').split('\n');
  const cols = lines[0].split('\t'), ix = cols.indexOf('Index'), fx = cols.indexOf('FileName'), vx = cols.indexOf('Volume');
  if (ix < 0 || fx < 0 || vx < 0) throw new Error('Sounds.txt is missing Index/FileName/Volume');
  const rows = lines.slice(1).map((l) => l.split('\t')).filter((c) => Number(c[ix]) === id);
  if (rows.length !== 1) throw new Error(`expected one Sounds.txt row for ${id}, found ${rows.length}`);
  const volume = Number(rows[0][vx]), byte = (name) => { const v = Number(rows[0][cols.indexOf(name)]); return Number.isInteger(v) && v > 0 ? v : 0; };
  return rows[0][fx]?.trim() ? { id, file: rows[0][fx].trim(), volume: Number.isInteger(volume) ? volume : 255, fadeIn: byte('Fade In'), fadeOut: byte('Fade Out') } : null;
}

/** The ticks of the day/night cross-fade (0x4F860 asks 0x88230 for 250; the mixer takes the larger of that and the row's fade). */
export const AMBIENT_SWAP_TICKS = 250;

/** The engine's raw period of day (D2Common #10396 value: 0 day, 1..3 dusk/night/dawn = night ambience); 0 on an engine without the export. */
export function ambientCycleOf(Module) {
  return Module?._d2_session_period_of_day ? Module.ccall('d2_session_period_of_day', 'number', [], []) | 0 : 0;
}

/** The engine's SoundEnviron selection of a level for a period (d2_client_ambient_select over the tables 0x5DFD0 loads): { environmentId, ambienceId, eventId, eventDelay }. */
export function nativeAmbientSelect(Module) {
  return (levelId, cycle) => {
    const f = (field) => Number(Module._d2_client_ambient_select(levelId, cycle, field)) | 0;
    const sel = { levelId, environmentId: f(0), cycleState: cycle, ambienceId: f(1), eventId: f(2), eventDelay: f(3) };
    if (sel.environmentId < 0 || sel.ambienceId < 0 || sel.eventId < 0 || sel.eventDelay < 0) throw new Error(`no SoundEnviron row for level ${levelId}`);
    return sel;
  };
}

/**
 * The area ambience loop (one-shot events and the weather loop come from the engine's 0x4F860). One loop source at a time:
 * the same ambience id is a no-op, a different id (level or day/night change) fades the old loop out as the new one fades in.
 * `select(level, cycle)` is the engine's SoundEnviron row (nativeAmbientSelect); `cycle` is the raw D2Common-10396 value (1..3 = night).
 */
export function createAmbientSound(read, AudioContextClass = globalThis.AudioContext || globalThis.webkitAudioContext, select = null, fetched = null, gestureTarget = globalThis.window) {
  if (typeof read !== 'function') throw new TypeError('retail asset reader is required');
  if (typeof select !== 'function') throw new TypeError('the engine ambient selection is required');
  if (!AudioContextClass) throw new Error('Web Audio is unavailable');
  const context = new AudioContextClass(), buffers = new Map();
  unlockOnGesture(context, gestureTarget);   // the shared gesture unlock (a fresh context starts suspended)
  const gain = typeof context.createGain === 'function' ? context.createGain() : null;
  if (gain) gain.connect(context.destination);
  const out = gain ?? context.destination;
  let soundsTxt = null, sliders = { sound: 1, music: 1 }, volume = 1;
  let loop = null, loopId = 0, serial = 0, key = '', error = '';
  const fading = new Set();   // loops on their way out: each stops when its fade ends
  // A source's live level (entry +0x1C) steps once per mixer tick along its fade; its node holds that tick's channel.
  const channel = (snd, l) => nativeSoundChannel(snd, sliderPercent(sliders.sound), 100, null, l) / 255;
  const levelOf = (lp, now = context.currentTime ?? 0) => {
    const f = lp.fade;
    return f ? soundFadeLevel(f.from, f.to, 0, f.ticks, Math.max(0, Math.floor((now - f.t0) / MIXER_TICK_SECONDS))) : lp.level;
  };
  const apply = (lp) => {
    const g = lp.node?.gain;
    if (!g) return;
    const now = context.currentTime ?? 0, f = lp.fade;
    if (f && now >= f.t0 + f.ticks * MIXER_TICK_SECONDS) { lp.level = f.to; lp.fade = null; }
    if (!lp.fade || typeof g.setValueAtTime !== 'function') { g.value = channel(lp.snd, lp.fade ? f.to : lp.level); return; }
    g.cancelScheduledValues?.(now);
    for (let k = Math.max(0, Math.floor((now - f.t0) / MIXER_TICK_SECONDS)); k <= f.ticks; k++)
      g.setValueAtTime(channel(lp.snd, soundFadeLevel(f.from, f.to, 0, f.ticks, k)), Math.max(now, f.t0 + k * MIXER_TICK_SECONDS));
  };
  const fadeTo = (lp, to, ticks) => {
    const now = context.currentTime ?? 0;
    lp.fade = ticks > 0 ? { from: levelOf(lp, now), to, t0: now, ticks } : null;
    if (!lp.fade) lp.level = to;
    apply(lp);
  };
  const load = async (snd) => {
    let b = buffers.get(snd.file);
    if (b) return b;
    if (fetched) await fetched(ROOTS.map((r) => r + snd.file));   // PERF: the bytes arrive in the background, not in a sync stall
    let bytes, err;
    for (const r of ROOTS) { try { bytes = await (read.sliced ?? read)(r + snd.file); break; } catch (e) { err = e; } }
    if (!bytes) throw err;
    decodeWav(bytes);
    b = await context.decodeAudioData(bytes.buffer.slice(bytes.byteOffset, bytes.byteOffset + bytes.byteLength));
    buffers.set(snd.file, b); return b;
  };
  const end = (lp) => { try { lp.src.stop(); } catch {} lp.src.disconnect(); lp.node?.disconnect(); fading.delete(lp); };
  // 0x88550: the area loop's stop request fades it over its row's Fade Out (`ticks` overrides it), then stops it.
  // `fade` false = at once (the ambience is switched off).
  const stopLoop = (fade = false, ticks = null) => {
    const lp = loop; loop = null; loopId = 0;
    if (!lp) return;
    const n = ticks ?? lp.snd.fadeOut;
    if (!fade || !(n > 0) || typeof lp.node?.gain?.setValueAtTime !== 'function') { end(lp); return; }
    fading.add(lp);
    fadeTo(lp, 0, n);
    try { lp.src.stop((context.currentTime ?? 0) + n * MIXER_TICK_SECONDS); } catch {}
    lp.src.onended = () => end(lp);
  };
  // A loop starts with its row's Fade In (`ticks` overrides it); a one-shot starts at its level.
  const start = (buffer, snd, looping, ticks = null) => {
    const src = context.createBufferSource(); src.buffer = buffer; src.loop = looping;
    const node = typeof context.createGain === 'function' ? context.createGain() : null;
    const lp = { src, node, snd, level: 255, fade: null };
    if (node) {
      const n = looping ? ticks ?? snd.fadeIn : 0;
      if (n > 0) { lp.level = 0; fadeTo(lp, 255, n); } else apply(lp);
      src.connect(node); node.connect(out);
    } else src.connect(out);
    src.start(); return lp;
  };
  const sounds = () => soundsTxt ??= new TextDecoder('latin1').decode(read('data\\global\\excel\\Sounds.txt'));
  return {
    /** Select the level's ambience for `cycle`; idempotent per (level, ambience). */
    async sync(levelId, cycle = 0) {
      const sel = select(levelId, cycle);
      const k = `${sel.environmentId}:${sel.ambienceId}:${sel.eventId}:${sel.eventDelay}`;
      if (k === key) return sel;
      key = k; const mine = ++serial;
      const snd = soundByIndex(sounds(), sel.ambienceId);
      if (loopId === sel.ambienceId && loop) return sel;
      // 0x4F860: a change between the same area's day and night loops cross-fades them (the old to 0, the new from 0,
      // over the longer of 250 ticks and the row's fade); any other change fades the old loop out over its Fade Out
      // (0x88550) and starts the new one with its Fade In.
      const day = select(levelId, 0).ambienceId, night = select(levelId, 1).ambienceId;
      const swap = Boolean(loop) && sel.ambienceId > 0 && day !== night
        && ((loopId === day && sel.ambienceId === night) || (loopId === night && sel.ambienceId === day));
      stopLoop(true, swap ? Math.max(AMBIENT_SWAP_TICKS, loop.snd.fadeOut) : null);
      if (!snd) return sel;
      try {
        const [b] = await Promise.all([load(snd), context.resume?.()]);
        if (mine !== serial) return sel;          // a newer level superseded this request
        stopLoop(true); loop = start(b, snd, true, swap ? Math.max(AMBIENT_SWAP_TICKS, snd.fadeIn) : null); loopId = sel.ambienceId;
      } catch (e) { error = String(e); key = ''; throw e; }
      return sel;
    },
    stop() { serial++; key = ''; stopLoop(); for (const lp of [...fading]) end(lp); },
    close() { this.stop(); return context.close?.(); },
    setVolume(v, next) {
      volume = Math.max(0, Math.min(1, Number(v) || 0));
      sliders = next ? { sound: next.sound, music: next.music } : { sound: volume, music: 1 };
      if (loop) apply(loop);
      for (const lp of fading) apply(lp);
    },
    get volume() { return volume; },
    get evidence() { return { loopId, error, state: context.state }; },
  };
}
