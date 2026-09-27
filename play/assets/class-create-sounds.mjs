import { decodeWav } from './wav.mjs';

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
      const event = { hero, trigger, path, asset: `data\\global\\sfx\\${path}`, requested: events.length, started: false, startIndex: null, startTime: null };
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
        const source = context.createBufferSource(); source.buffer = buffer; source.connect(context.destination);
        source.onended = () => { sources.delete(source); source.disconnect(); };
        source.start(); sources.add(source);
        event.started = true; event.startIndex = nextStart++; event.startTime = context.currentTime;
        return event;
      });
      return queue.then((started) => ({ ...started }));
    },
    get evidence() { return { events: events.map((event) => ({ ...event })), decoded: [...buffers.keys()], state: context.state }; },
    close() { for (const source of sources) { source.stop(); source.disconnect(); } sources.clear(); return context.close?.(); },
  };
}
