import { decodeWav } from './wav.mjs';
import { unlockOnGesture } from './audio-unlock.mjs';
import { resolveUnitSound, soundRow, soundRowByHandle } from './unit-sound-resolver.mjs';
import { sliderPercent } from './sound-channel-volume.mjs';
import { nativeUnitSoundSpatial, unitSoundSourcePosition, nativeSoundPanBalance } from './unit-sound-spatial.mjs';
import { soundClientOffset, soundInStartRange, soundBeyondCutoff, nativeSoundChannel, soundLoops, soundFadeLevel } from './sound-mixer-rules.mjs';

// S2: drained engine sound events (d2_session_take_sounds septets) -> resolver -> Sounds.txt file -> Web Audio.
// The engine decides the event, D2Client's resolver picks the row, Sounds.txt names the WAV; nothing is invented.
const ROOTS = ['data\\global\\sfx\\', 'data\\local\\sfx\\']; // voices/speech live under the local tree
// The mixer tick (D2Client 0x5F020) runs once per client frame; Fade In/Fade Out/Duration count these ticks.
export const MIXER_TICK_SECONDS = 0.04;

// `fetched(names)`, optional: resolves once the first of those archive paths that exists can be read without a network stall.
// `options.variant(row)`, optional: the group variant a row played outside the engine's sound queue sounds (default: the
// engine's 0x5EE70 export). `options.ended(handle)`, optional: a queued entry's source ended or never sounded (default:
// the engine's queue export). A start the queue made (event.queueHandle) brings its variant (event.variantRow).
export function createUnitSoundPlayer(read, AudioContextClass = globalThis.AudioContext || globalThis.webkitAudioContext, gestureTarget = globalThis.window, fetched = null, options = {}) {
  if (typeof read !== 'function') throw new TypeError('retail asset reader is required');
  if (!AudioContextClass) throw new Error('Web Audio is unavailable');
  const context = new AudioContextClass(), buffers = new Map(), events = [];
  // Bounded: a long session would otherwise grow this log forever.
  events.push = function (...xs) { const n = Array.prototype.push.apply(this, xs); if (n > 512) this.splice(0, n - 512); return this.length; };
  const starts = []; starts.push = events.push;
  // F235: the Sound slider drives every SFX player through one gain node (see audio-volume.mjs).
  const gain = typeof context.createGain === 'function' ? context.createGain() : null;
  if (gain) gain.connect(context.destination);
  const out = gain ?? context.destination;
  let volume = 1, sliders = { sound: 1, music: 1 };
  const activeSources = new Map();
  // The listener and unit positions of the last refresh; a source submitted between refreshes is placed against them.
  let lastSnapshot = null, lastListener = null;
  const variant = typeof options.variant === 'function' ? options.variant
    : (row) => (typeof globalThis.Module?._d2_client_play_sound_variant === 'function' ? Number(globalThis.Module._d2_client_play_sound_variant(row)) | 0 : row);
  const ended = typeof options.ended === 'function' ? options.ended
    : (h) => globalThis.Module?._d2_client_play_sound_ended?.(h);
  const release = (rec) => { if (rec.queueHandle && !rec.released) { rec.released = true; ended(rec.queueHandle); } };
  const sourceGainFor = (row, spatialGainByte = 255, offset = null, level = 255) => {
    const master = sliderPercent(sliders.sound), music = sliderPercent(sliders.music);
    const channel = nativeSoundChannel(row ?? { volume: 255 }, master, music, offset, level);
    const attenuated = Math.trunc(channel * spatialGainByte / 255);
    // The shared player node carries Master. This source node carries the remaining
    // factor so their product is exactly D2Sound's integer channel / 255 result.
    return master === 0 ? 0 : (attenuated / 255) / (master / 100);
  };
  // The source's place against the listener: its client-pixel offset (Falloff) and ordinal 10047's gain/pan.
  const placement = (rec, snapshot = lastSnapshot, listener = lastListener) => {
    if (!rec || !listener) return null;
    const position = rec.position ?? unitSoundSourcePosition(rec, snapshot);
    const offset = position ? soundClientOffset(position, listener) : null;
    return offset ? { offset, spatial: nativeUnitSoundSpatial(position, listener) } : null;
  };
  // The source's live level (entry +0x1C: 255 at submit) and its fade (0x88DD0 / 0x88230: from, to, start and end in
  // mixer ticks, the level stepping once per tick by soundFadeLevel). The node holds the channel of each tick's level.
  const fadeEnd = (f) => f.t0 + f.ticks * MIXER_TICK_SECONDS;
  const levelNow = (state, now = context.currentTime ?? 0) => {
    const f = state.fade;
    if (!f) return state.level ?? 255;
    return soundFadeLevel(f.from, f.to, 0, f.ticks, Math.max(0, Math.floor((now - f.t0) / MIXER_TICK_SECONDS)));
  };
  const applyGain = (node, state) => {
    const g = node?.gain;
    if (!g) return;
    const now = context.currentTime ?? 0, f = state.fade;
    const at = (level) => sourceGainFor(state.row, state.spatialGainByte, state.offset, level);
    if (f && now >= fadeEnd(f)) { state.level = f.to; state.fade = null; }
    if (!state.fade || typeof g.setValueAtTime !== 'function') { g.value = at(state.fade ? f.to : state.level ?? 255); return; }
    g.cancelScheduledValues?.(now);
    const k0 = Math.max(0, Math.floor((now - f.t0) / MIXER_TICK_SECONDS));
    for (let k = k0; k <= f.ticks; k++) g.setValueAtTime(at(soundFadeLevel(f.from, f.to, 0, f.ticks, k)), Math.max(now, f.t0 + k * MIXER_TICK_SECONDS));
  };
  const connectSource = (source, row, state = {}) => {
    const node = typeof context.createGain === 'function' ? context.createGain() : null;
    if (node) {
      const full = { row, spatialGainByte: 255, offset: null, rec: null, panCode: null, level: 255, fade: null, ...state, source };
      applyGain(node, full);
      const balance = full.withBalance && typeof context.createChannelMerger === 'function' ? createBalance(node, full.rec, full.panCode ?? 128) : null;
      if (!balance) node.connect(out);
      source.connect(node);
      activeSources.set(node, { ...full, balance });
    }
    else source.connect(out);
    return node;
  };
  const applyBalance = (balance, rec, code) => {
    const value = nativeSoundPanBalance(code);
    balance.left.gain.value = value.left;
    balance.right.gain.value = value.right;
    rec.appliedSpatialPanCode = code;
    rec.appliedSpatialPanLevel = value.level;
  };
  const createBalance = (node, rec, code) => {
    // A mono buffer: duplicate its signal into explicit left/right merger inputs;
    // equal-power StereoPanner would attenuate both at center.
    const left = context.createGain(), right = context.createGain();
    const merger = context.createChannelMerger(2);
    node.channelCount = 1; node.channelCountMode = 'explicit';
    node.connect(left); node.connect(right);
    left.connect(merger, 0, 0); right.connect(merger, 0, 1); merger.connect(out);
    const balance = { left, right, merger };
    applyBalance(balance, rec, code);
    return balance;
  };
  const disconnectSource = (node) => {
    if (node) {
      const balance = activeSources.get(node)?.balance;
      node.disconnect();
      if (balance) for (const part of Object.values(balance)) part.disconnect();
      activeSources.delete(node);
    }
  };
  // 0x88550/0x88230: a stop request ramps the level to 0 over the row's Fade Out ticks, then stops;
  // 0x880E0 (out of range, Duration) stops at once.
  const stopSource = (node, state, fade, reason) => {
    if (state.stopping) return;
    state.stopping = true;
    const ticks = fade ? state.row.fadeOut : 0, now = context.currentTime ?? 0;
    state.rec.stopped = reason;
    state.rec.fadeOutSeconds = ticks * MIXER_TICK_SECONDS;
    if (ticks > 0 && typeof node?.gain?.setValueAtTime === 'function') {
      // The fade starts from the level the source has this tick.
      state.fade = { from: levelNow(state, now), to: 0, t0: now, ticks };
      applyGain(node, state);
      try { state.source.stop?.(now + ticks * MIXER_TICK_SECONDS); } catch {}
    } else {
      try { state.source.stop?.(); } catch {}
    }
  };
  // 0x880E0 on a playing looping row (Duration 0) that left its cutoff: the entry goes back to the queue holding the
  // play position (+0x41), and starts again from there with a 3-tick fade once it is inside its cutoff.
  const parked = new Set();
  const park = (node, state) => {
    const now = context.currentTime ?? 0, length = state.source.buffer?.duration ?? 0;
    const at = (state.resumeFrom ?? 0) + (now - (state.startedAt ?? now));
    state.rec.parked = true;
    stopSource(node, state, false, 'out-of-range');
    parked.add({ rec: state.rec, resume: length > 0 ? at % length : 0, fadeTicks: 3 });
  };
  // A suspended context still schedules source.start(); it sounds once a gesture resumes it.
  unlockOnGesture(context, gestureTarget);
  let soundsTxt = null, started = 0, dialogueGeneration = 0, dialogueSource = null;
  let dialogueDone = Promise.resolve(), finishDialogue = null;
  const table = () => soundsTxt ??= new TextDecoder('latin1').decode(read('data\\global\\excel\\Sounds.txt'));
  // PERF (freeze): the first play of a WAV waits for its bytes to arrive in the background before the synchronous
  // read, instead of spinning the main thread on the fetch. Callers already await the decode.
  const load = async (path) => {
    if (fetched) await fetched(ROOTS.map((root) => root + path));
    let bytes, at, err; for (const root of ROOTS) { try { bytes = read(root + path); at = root + path; break; } catch (e) { err = e; } } if (!bytes) throw err; decodeWav(bytes);
    return { at, audio: await context.decodeAudioData(bytes.buffer.slice(bytes.byteOffset, bytes.byteOffset + bytes.byteLength)) };
  };
  const buffer = async (path, rec) => {
    let b = buffers.get(path);
    if (!b) { buffers.set(path, b = load(path)); b.catch(() => buffers.delete(path)); }
    const { at, audio } = await b; rec.path = at; return audio;
  };
  async function playTableRow(row, rec) {
    try {
      const t = table();
      const asked = soundRow(t, row);
      if (!asked?.file) throw new Error(`Sounds.txt row ${row} has no file`);
      rec.handle = asked.handle;
      // 0x88A70: a row with Volume 0 is never queued.
      if (asked.volume === 0) { rec.silent = true; return rec; }
      // A source attached to a unit (or given a position) is placed against the listener; one without sits on it.
      const place = placement(rec);
      rec.clientOffset = place?.offset ?? null;
      // 0x88DD0: a queued source starts only while it is inside its Falloff cutoff. Out of it, a one-shot is dropped
      // and a looping row attached to a unit stays queued until its unit comes inside the cutoff.
      if (!soundInStartRange(place?.offset, asked.falloff)) {
        rec.outOfRange = true;
        if (soundLoops(asked) && place && !rec.position && Number.isInteger(rec.unitType) && Number.isInteger(rec.unitId)) {
          rec.parked = true;
          parked.add({ rec, resume: 0, fadeTicks: 0 });
        }
        return rec;
      }
      await begin(rec, row, asked, t);
    } catch (error) { rec.error = String(error); }
    // a queued entry that never started (silent, out of range, deferred, failed) leaves the client sound list at once
    finally { if (!rec.started && !rec.parked) release(rec); }
    return rec;
  }
  // The start (0x88DD0 -> 0x5D7A0) of a queued entry inside its cutoff. `resume` = the play position a requeued loop
  // kept; `fadeTicks` = its 3-tick restart fade (0 = the row's Fade In).
  async function begin(rec, row, asked, t, resume = 0, fadeTicks = 0) {
    {
      const place = placement(rec);
      rec.clientOffset = place?.offset ?? null;
      // 0x88DD0 -> 0x87C30: a Defer Inst or Stop Inst row looks for the earliest-started playing source of its group
      // (the loader's group base). Defer Inst then drops the new entry; Stop Inst stops that source at once and starts.
      // A start the client sound list brings (rec.queueHandle) has made this test in the engine already.
      if ((asked.deferInst || asked.stopInst) && !rec.queueHandle && !fadeTicks) {
        let old = null;
        for (const entry of activeSources)
          if (!entry[1].stopping && entry[1].groupBase === asked.groupBase && (!old || entry[1].startedAt < old[1].startedAt)) old = entry;
        if (old && !asked.stopInst) { rec.deferred = true; return; }
        if (old) stopSource(old[0], old[1], false, 'stop-inst');
      }
      // 0x5D7A0 -> 0x5EE70: a row with a Group Size plays one of its variants, avoiding its last picks (the engine's
      // pick; a queued start already made it).
      let r = asked;
      const pick = Number.isInteger(rec.variantRow) ? rec.variantRow : asked.groupSize > 1 ? variant(row) : row;
      if (pick !== row || asked.groupSize > 1) {
        rec.variantRow = pick;
        const v = soundRow(t, pick);
        if (v?.file) r = v;
        rec.handle = r.handle;
      }
      context.resume().catch(() => {});
      const source = context.createBufferSource(); source.buffer = await buffer(r.file, rec);
      // 0x5D1D0: every row takes the Falloff level; a mono row (mixer mode 0) also takes ordinal 10047's
      // radial gain and pan (10028). Tracking rows refresh their offset each tick; others keep the submit offset.
      const mono = !r.stereo;
      const gainByte = !mono ? 255 : place ? place.spatial.gainByte
        : Number.isInteger(rec.nativeSpatialGainByte) ? rec.nativeSpatialGainByte : 255;
      const panCode = !mono ? null : place ? place.spatial.panCode
        : Number.isInteger(rec.nativeSpatialPanCode) ? rec.nativeSpatialPanCode : null;
      // A source attached to a unit (type and id, no fixed position) follows that unit while its row is Tracking.
      const attached = !rec.position && Number.isInteger(rec.unitType) && Number.isInteger(rec.unitId);
      const tracking = r.tracking && attached;
      rec.appliedSpatialGainByte = gainByte;
      if (panCode != null) rec.nativeSpatialPanCode = panCode;
      // Loop restarts the buffer (Duration 0). A looping source ends through its unit's stop request, so only a
      // source attached to a unit the listener can see loops here; one with nothing to stop it plays once.
      const loops = soundLoops(r) && (rec.held === true || attached && Boolean(place));
      if (soundLoops(r) && !loops) rec.loopHeld = true;
      rec.looping = loops;
      source.loop = loops;
      const now = context.currentTime ?? 0;
      // Fade In (entry +0x3F): the level steps from 0 to 255 over the row's Fade In ticks; a requeued loop restarts
      // over 3 ticks instead.
      const ticks = fadeTicks || r.fadeIn;
      if (ticks > 0) rec.fadeInSeconds = ticks * MIXER_TICK_SECONDS;
      if (fadeTicks) rec.resumedAt = resume;
      const node = connectSource(source, r, { rec, spatialGainByte: gainByte, offset: place?.offset ?? null, panCode,
        withBalance: mono && (panCode != null || tracking), tracking, attached, mono, looping: loops,
        level: rec.level ?? 255, fade: ticks > 0 ? { from: 0, to: rec.level ?? 255, t0: now, ticks } : null, groupBase: asked.groupBase, startedAt: now, resumeFrom: resume });
      source.onended = () => { source.disconnect(); disconnectSource(node); if (!rec.parked) release(rec); };
      if (resume > 0) source.start(now, resume); else source.start();
      started++; rec.started = true; rec.parked = false;
      // Duration: the tick stops a source once more than Duration ticks have passed since it started.
      if (r.duration > 0) {
        rec.durationSeconds = (r.duration + 1) * MIXER_TICK_SECONDS;
        try { source.stop?.(now + rec.durationSeconds); } catch {}
      }
    }
  }
  /** One drained event {unitType, unitId, classId, soundEvent, updateUnitType, updateUnitId}; localId = the hero's unit id. */
  async function play(e, localId) {
    // Audience: -1 = every client, else only the client of that unit.
    if (e.updateUnitType !== -1 && e.updateUnitId !== localId) return null;
    const row = resolveUnitSound({ unitType: e.unitType, classId: e.classId, soundEvent: e.soundEvent, isLocalPlayer: e.unitType === 0 && e.unitId === localId,
      monsterTaunt: e.monsterTaunt ?? (e.soundEvent === 0x10 ? e.nativeSelectedRow : 0),
      skillStartSoundRow: e.skillStartSoundRow ?? (e.soundEvent === 0x0c ? e.nativeSelectedRow : 0) });
    const rec = { ...e, row, handle: null, path: null, started: false, error: '' };
    events.push(rec);
    if (row == null) return rec;
    return playTableRow(row, rec);
  }
  /** Play a Sounds.txt row selected by a table-driven engine event, such as missile HitSound; {unitType, unitId} or {position} places it. */
  /** A cue submitted to the client sound list (0x88A70): recorded now, whether or when the list starts it. */
  function noteSubmit(row, event = {}) {
    const rec = { ...event, kind: 'submit', row, handle: null, path: null, started: false, error: '' };
    events.push(rec);
    return rec;
  }
  async function playRow(row, event = {}) {
    const rec = { ...event, kind: 'table-row', row, handle: null, path: null, started: false, error: '' };
    // A start the client sound list brings (event.queueHandle) was recorded at its submit (noteSubmit), as retail's
    // record is the 0x88A70 submit; its start goes to `starts`.
    (event.queueHandle ? starts : events).push(rec);
    if (!Number.isInteger(row) || row < 0) { rec.error = `invalid Sounds.txt row ${row}`; return rec; }
    // A delayed submit (event.delay, in 25 Hz game ticks, as retail's submit offset) is recorded now and starts its audio later.
    if (event.delay > 0) { setTimeout(() => playTableRow(row, rec), event.delay * 40); return rec; }
    return playTableRow(row, rec);
  }
  function stopDialogue() {
    dialogueGeneration++;
    const source = dialogueSource; dialogueSource = null;
    finishDialogue?.(); finishDialogue = null;
    if (source) { try { source.stop(); } catch {} source.disconnect(); }
  }
  /** 0x88C20 on a freed unit: its looping sources stop through their Fade Out (one-shots finish). */
  function stopUnitSounds(unitType, unitId) {
    for (const [node, state] of activeSources)
      if (state.looping && state.rec?.unitType === unitType && (state.rec?.unitId >>> 0) === (unitId >>> 0)) stopSource(node, state, true, 'unit-freed');
  }
  function updateSpatialGain(snapshot, listener) {
    // FUN_6FB38DD0 refreshes Tracking sources' offsets against the listener, then FUN_6FB0D1D0
    // reapplies the channel controls. Refresh the existing source without restarting it.
    lastSnapshot = snapshot; lastListener = listener;
    for (const [node, state] of activeSources) {
      if (!state.attached || state.stopping || !listener) continue;
      const place = placement(state.rec, snapshot, listener);
      if (!place) {
        // The unit is gone from the client: a looping source gets its stop request.
        if (state.looping) stopSource(node, state, true, 'unit-freed');
        continue;
      }
      if (!state.tracking) continue;
      // Beyond the Falloff cutoff the tick stops the source at once; a looping one goes back to the queue.
      if (soundBeyondCutoff(place.offset, state.row.falloff)) {
        if (state.looping) park(node, state); else stopSource(node, state, false, 'out-of-range');
        continue;
      }
      state.offset = place.offset;
      state.rec.clientOffset = place.offset;
      if (state.mono) {
        state.spatialGainByte = place.spatial.gainByte;
        state.rec.nativeSpatialGainByte = place.spatial.gainByte;
        state.rec.appliedSpatialGainByte = place.spatial.gainByte;
      }
      applyGain(node, state);
      if (state.balance && state.mono && place.spatial.panCode !== state.rec.appliedSpatialPanCode) {
        state.rec.nativeSpatialPanCode = place.spatial.panCode;
        applyBalance(state.balance, state.rec, place.spatial.panCode);
      }
    }
    // A queued loop starts (or resumes) once its unit is inside the cutoff; its unit gone, it leaves the queue.
    if (!listener) return;
    for (const entry of [...parked]) {
      const { rec } = entry;
      const place = placement(rec, snapshot, listener);
      if (!place) { parked.delete(entry); rec.parked = false; rec.stopped = 'unit-freed'; release(rec); continue; }
      const t = table(), asked = soundRow(t, rec.row);
      if (!asked || !soundInStartRange(place.offset, asked.falloff)) continue;
      parked.delete(entry);
      rec.outOfRange = false;
      begin(rec, rec.row, asked, t, entry.resume, entry.fadeTicks)
        .catch((error) => { rec.error = String(error); })
        .finally(() => { if (!rec.started) { rec.parked = false; release(rec); } });
    }
  }
  // The weather sound (0x4F860's kind-1 handle): one held loop whose level the caller sets each tick. It starts
  // with the row's Fade In to its first level (the mixer keeps its own ramp while a fade runs), follows each later
  // level, and stops through the row's Fade Out when the level reaches 0 (0x88BF0).
  let weather = null;
  function setWeatherLevel(row, level) {
    const l = Math.max(0, Math.min(255, level | 0));
    if (!weather) {
      if (l === 0 || !Number.isInteger(row)) return null;
      weather = { kind: 'weather', row, level: l, held: true, handle: null, path: null, started: false, error: '' };
      events.push(weather);
      playTableRow(row, weather);
      return weather;
    }
    const rec = weather;
    rec.level = l;
    for (const [node, state] of activeSources) {
      if (state.rec !== rec || state.stopping) continue;
      if (l === 0) { stopSource(node, state, true, 'weather-ended'); continue; }
      if (state.fade) { if ((context.currentTime ?? 0) < fadeEnd(state.fade)) continue; state.fade = null; }
      state.level = l; applyGain(node, state);
    }
    if (l === 0) weather = null;
    return rec;
  }
  async function playDialogue(handle, fileOverride = null) {
    stopDialogue();
    const token = dialogueGeneration;
    let resolveDone;
    dialogueDone = new Promise((resolve) => { resolveDone = resolve; finishDialogue = resolve; });
    const finish = () => { resolveDone(); if (finishDialogue === resolveDone) finishDialogue = null; };
    const row = handle ? soundRowByHandle(table(), handle) : null;
    const file = fileOverride || row?.file;
    const rec = { kind: 'dialogue', row: row?.row ?? null, handle, path: null, started: false, error: '' };
    events.push(rec);
    if (!file) { rec.error = `Sounds.txt has no voice handle ${handle}`; finish(); return rec; }
    try {
      context.resume().catch(() => {});
      const audio = await buffer(file, rec);
      if (token !== dialogueGeneration) { finish(); return rec; }
      const source = context.createBufferSource(); source.buffer = audio; const node = connectSource(source, row ?? { volume: 255, music: false }, { rec });
      source.onended = () => {
        source.disconnect(); disconnectSource(node);
        if (dialogueSource === source) dialogueSource = null;
        if (token === dialogueGeneration) finish();
      };
      source.start(); dialogueSource = source; started++; rec.started = true;
    } catch (error) { rec.error = String(error); finish(); }
    return rec;
  }
  return { play, playRow, noteSubmit, starts, playDialogue, stopDialogue, stopUnitSounds, setWeatherLevel, whenDialogueDone: () => dialogueDone, updateSpatialGain, events, setVolume(v, nextSliders) { volume = Math.max(0, Math.min(1, Number(v) || 0)); if (nextSliders) sliders = { sound: nextSliders.sound, music: nextSliders.music }; else sliders = { sound: volume, music: 1 }; if (gain) gain.gain.value = volume; for (const [node, state] of activeSources) if (!state.stopping || state.fade) applyGain(node, state); },
    get volume() { return gain ? gain.gain.value : volume; },
    evidence: () => ({ started, dialogueActive: Boolean(dialogueSource), events: events.map((x) => ({ ...x })), state: context.state }), close: () => context.close?.() };
}
