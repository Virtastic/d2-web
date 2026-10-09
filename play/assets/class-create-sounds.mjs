import { decodeWav } from './wav.mjs';
import { soundRowByFile } from './unit-sound-resolver.mjs';
import { sliderPercent, soundSourceGain } from './sound-channel-volume.mjs';

// Retail 1.13c `data/global/excel/Sounds.txt` entries 17..26. OpenDiablo2's
// SelectHeroClass cross-checks the outgoing-deselect / incoming-select transition.
export const CLASS_CREATE_SOUNDS = Object.freeze({
  amazon:      Object.freeze({ select: 'Cursor\\intro\\amazon select.wav',      deselect: 'Cursor\\intro\\amazon deselect.wav' }),
  assassin:    Object.freeze({ select: 'Cursor\\intro\\assassin select.wav',    deselect: 'Cursor\\intro\\assassin deselect.wav' }),
  necromancer: Object.freeze({ select: 'Cursor\\intro\\necromancer select.wav', deselect: 'Cursor\\intro\\necromancer deselect.wav' }),
  barbarian:   Object.freeze({ select: 'Cursor\\intro\\barbarian select.wav',   deselect: 'Cursor\\intro\\barbarian deselect.wav' }),
  paladin:     Object.freeze({ select: 'Cursor\\intro\\paladin select.wav',     deselect: 'Cursor\\intro\\paladin deselect.wav' }),
  sorceress:   Object.freeze({ select: 'Cursor\\intro\\sorceress select.wav',   deselect: 'Cursor\\intro\\sorceress deselect.wav' }),
  druid:       Object.freeze({ select: 'Cursor\\intro\\druid select.wav',       deselect: 'Cursor\\intro\\druid deselect.wav' }),
});

/**
 * Queue one-shot class effects in input order. A WAV's decode time is data-dependent, so starting
 * sources from independent promises can reverse a rapid hover/click transition. `play()` records
 * its request immediately and serializes `source.start()`; evidence has the resulting monotonic
 * start index and AudioContext time, not merely a successful decode.
 */
export function createClassSelectSoundPlayer(read, AudioContextClass = globalThis.AudioContext || globalThis.webkitAudioContext) {
  if (typeof read !== 'function') throw new TypeError('retail asset reader is required');
  if (!AudioContextClass) throw new Error('Web Audio is unavailable');
  const context = new AudioContextClass(), buffers = new Map(), sources = new Set(), events = [];
  // F235: the Sound slider drives every SFX player through one gain node (see audio-volume.mjs).
  const gain = typeof context.createGain === 'function' ? context.createGain() : null;
  if (gain) gain.connect(context.destination);
  const out = gain ?? context.destination;
  let volume = 1, sliders = { sound: 1, music: 1 }, soundsTxt = null;
  const activeSources = new Map();
  const table = () => soundsTxt ??= new TextDecoder('latin1').decode(read('data\\global\\excel\\Sounds.txt'));
  const sourceGainFor = (row) => {
    const master = sliderPercent(sliders.sound), music = sliderPercent(sliders.music);
    const channel = soundSourceGain(row.volume, master, music, row.music);
    return master === 0 ? 0 : channel / (master / 100);
  };
  let queue = Promise.resolve(), nextStart = 0;
  function prepare(path) {
    let pending = buffers.get(path);
    if (!pending) {
      const bytes = read(`data\\global\\sfx\\${path}`);
      decodeWav(bytes);
      const copy = bytes.buffer.slice(bytes.byteOffset, bytes.byteOffset + bytes.byteLength);
      pending = context.decodeAudioData(copy);
      buffers.set(path, pending);
    }
    return pending;
  }
  return {
    play(hero, trigger) {
      const sounds = CLASS_CREATE_SOUNDS[hero];
      if (!sounds) return Promise.reject(new RangeError(`unknown character class sound: ${hero}`));
      const kind = trigger === 'deselect' ? 'deselect' : 'select';
      const path = sounds[kind];
      const event = { hero, trigger, path, asset: `data\\global\\sfx\\${path}`, requested: events.length, started: false, startIndex: null, startTime: null, sourceVolume: null };
      events.push(event);
      // This call stays in the trusted pointer handler. The queued work below may run after the
      // gesture and after an MPQ decode, but Chrome has already received the resume request.
      // `source.start()` is valid on a suspended context; it schedules in the exact call order and
      // becomes audible as soon as this real pointer gesture's resume resolves. Do not await
      // resume here: headless evidence dispatches synthetic pointers, which rightly cannot grant
      // autoplay permission and would otherwise strand the entire FIFO before its first start.
      context.resume().catch(() => {});
      queue = queue.catch(() => {}).then(async () => {
        const buffer = await prepare(path);
        const source = context.createBufferSource(); source.buffer = buffer;
        const row = soundRowByFile(table(), path); event.sourceVolume = row?.volume ?? null;
        const sourceGain = row && typeof context.createGain === 'function' ? context.createGain() : null;
        if (sourceGain) { sourceGain.gain.value = sourceGainFor(row); sourceGain.connect(out); source.connect(sourceGain); activeSources.set(sourceGain, row); }
        else source.connect(out);
        source.onended = () => { sources.delete(source); source.disconnect(); if (sourceGain) { sourceGain.disconnect(); activeSources.delete(sourceGain); } };
        source.start(); sources.add(source);
        event.started = true; event.startIndex = nextStart++; event.startTime = context.currentTime;
        return event;
      });
      return queue.then((started) => ({ ...started }));
    },
    get evidence() { return { events: events.map((event) => ({ ...event })), decoded: [...buffers.keys()], state: context.state }; },
    setVolume(v, nextSliders) { volume = Math.max(0, Math.min(1, Number(v) || 0)); sliders = nextSliders ? { sound: nextSliders.sound, music: nextSliders.music } : { sound: volume, music: 1 }; if (gain) gain.gain.value = volume; for (const [node, row] of activeSources) node.gain.value = sourceGainFor(row); },
    get volume() { return gain ? gain.gain.value : volume; },
    close() { for (const source of sources) { source.stop(); source.disconnect(); } sources.clear(); return context.close?.(); },
  };
}
