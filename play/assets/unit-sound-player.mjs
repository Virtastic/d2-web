import { decodeWav } from './wav.mjs';
import { resolveUnitSound, soundRow, soundRowByHandle } from './unit-sound-resolver.mjs';
import { retailSoundChannelVolume, soundSourceGain, sliderPercent } from './sound-channel-volume.mjs';
import { unitSoundNativeSpatial, unitSoundNativePanCode, nativeSoundPanBalance } from './unit-sound-spatial.mjs';

// S2: drained engine sound events (d2_session_take_sounds septets) -> resolver -> Sounds.txt file -> Web Audio.
// The engine decides the event, D2Client's resolver picks the row, Sounds.txt names the WAV; nothing is invented.
const ROOTS = ['data\\global\\sfx\\', 'data\\local\\sfx\\']; // voices/speech live under the local tree

export function createUnitSoundPlayer(read, AudioContextClass = globalThis.AudioContext || globalThis.webkitAudioContext, gestureTarget = globalThis.window) {
  if (typeof read !== 'function') throw new TypeError('retail asset reader is required');
  if (!AudioContextClass) throw new Error('Web Audio is unavailable');
  const context = new AudioContextClass(), buffers = new Map(), events = [];
  // F235: the Sound slider drives every SFX player through one gain node (see audio-volume.mjs).
  const gain = typeof context.createGain === 'function' ? context.createGain() : null;
  if (gain) gain.connect(context.destination);
  const out = gain ?? context.destination;
  let volume = 1, sliders = { sound: 1, music: 1 };
  const activeSources = new Map();
  const sourceGainFor = (row, spatialGainByte = 255) => {
    const master = sliderPercent(sliders.sound), music = sliderPercent(sliders.music);
    const channel = retailSoundChannelVolume(row?.volume ?? 255, master, music, row?.music === true);
    const attenuated = Math.trunc(channel * spatialGainByte / 255);
    // The shared player node carries Master. This source node carries the remaining
    // factor so their product is exactly D2Sound's integer channel / 255 result.
    return master === 0 ? 0 : (attenuated / 255) / (master / 100);
  };
  const connectSource = (source, row, spatialGainByte = 255, spatialGainEvent = null, spatialPanEvent = null) => {
    const node = typeof context.createGain === 'function' ? context.createGain() : null;
    if (node) {
      node.gain.value = sourceGainFor(row, spatialGainByte);
      const balance = spatialPanEvent && typeof context.createChannelMerger === 'function' ? createBalance(node, spatialPanEvent) : null;
      if (!balance) node.connect(out);
      source.connect(node);
      activeSources.set(node, { row, spatialGainByte, spatialGainEvent, spatialPanEvent, balance });
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
  const createBalance = (node, rec) => {
    // The traced row is mono. Duplicate its mono signal into explicit left/right
    // merger inputs; equal-power StereoPanner would attenuate both at center.
    const left = context.createGain(), right = context.createGain();
    const merger = context.createChannelMerger(2);
    node.channelCount = 1; node.channelCountMode = 'explicit';
    node.connect(left); node.connect(right);
    left.connect(merger, 0, 0); right.connect(merger, 0, 1); merger.connect(out);
    const balance = { left, right, merger };
    applyBalance(balance, rec, Number.isInteger(rec.nativeSpatialPanCode) ? rec.nativeSpatialPanCode : 128);
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
  // A suspended context still schedules source.start(); it sounds once a trusted gesture resumes it.
  if (context.state === 'suspended' && typeof gestureTarget?.addEventListener === 'function') {
    const controller = new AbortController();
    let resuming = false;
    const unlock = () => {
      if (resuming) return;
      if (context.state !== 'suspended') { controller.abort(); return; }
      resuming = true;
      let resumed;
      try { resumed = Promise.resolve(context.resume()); }
      catch { resuming = false; return; }
      resumed.then(() => {
        resuming = false;
        // Browsers can reject resume(), or fulfill it while leaving the context suspended.
        // Keep listening until a trusted gesture actually unlocks this audio context.
        if (context.state !== 'suspended') controller.abort();
      }, () => { resuming = false; });
    };
    for (const t of ['pointerdown', 'keydown', 'touchstart', 'gamepadconnected'])
      gestureTarget.addEventListener(t, unlock, { signal: controller.signal, passive: true });
  }
  let soundsTxt = null, started = 0, dialogueGeneration = 0, dialogueSource = null;
  let dialogueDone = Promise.resolve(), finishDialogue = null;
  const table = () => soundsTxt ??= new TextDecoder('latin1').decode(read('data\\global\\excel\\Sounds.txt'));
  const buffer = (path, rec) => {
    let b = buffers.get(path);
    if (b) rec.path = b.path;
    if (!b) { let bytes, err; for (const root of ROOTS) { try { bytes = read(root + path); rec.path = root + path; break; } catch (e) { err = e; } } if (!bytes) throw err; decodeWav(bytes); b = context.decodeAudioData(bytes.buffer.slice(bytes.byteOffset, bytes.byteOffset + bytes.byteLength)); b.path = rec.path; buffers.set(path, b); }
    return b;
  };
  async function playTableRow(row, rec, { spatial = true } = {}) {
    try {
      const r = soundRow(table(), row);
      if (!r?.file) throw new Error(`Sounds.txt row ${row} has no file`);
      rec.handle = r.handle;
      context.resume().catch(() => {});
      const source = context.createBufferSource(); source.buffer = await buffer(r.file, rec);
      // Distance gain remains limited to the verified shrine row. Native pan applies
      // to resolved mono sources whose Sounds.txt Tracking flag is set.
      const tracksSpatialGain = spatial && !r.stereo && r.row === 2673 && rec.soundEvent === 0x0a;
      const tracksSourcePan = spatial && !r.stereo && r.tracking;
      const spatialGainByte = tracksSpatialGain
        && Number.isInteger(rec.nativeSpatialGainByte) ? rec.nativeSpatialGainByte : 255;
      rec.appliedSpatialGainByte = spatialGainByte;
      const spatialGainEvent = tracksSpatialGain ? rec : null;
      const spatialPanEvent = tracksSourcePan ? rec : null;
      const node = connectSource(source, r, spatialGainByte, spatialGainEvent, spatialPanEvent);
      source.onended = () => { source.disconnect(); disconnectSource(node); };
      source.start(); started++; rec.started = true;
    } catch (error) { rec.error = String(error); }
    return rec;
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
  /** Play a Sounds.txt row selected by a table-driven engine event, such as missile HitSound. */
  async function playRow(row, event = {}) {
    const rec = { ...event, kind: 'table-row', row, handle: null, path: null, started: false, error: '' };
    events.push(rec);
    if (!Number.isInteger(row) || row < 0) { rec.error = `invalid Sounds.txt row ${row}`; return rec; }
    return playTableRow(row, rec, { spatial: false });
  }
  function stopDialogue() {
    dialogueGeneration++;
    const source = dialogueSource; dialogueSource = null;
    finishDialogue?.(); finishDialogue = null;
    if (source) { try { source.stop(); } catch {} source.disconnect(); }
  }
  function updateSpatialGain(snapshot, listener) {
    // FUN_6FB38DD0 refreshes tracked source/listener tuples, then FUN_6FB0D1D0
    // reapplies the channel controls. Refresh the existing source without restarting it.
    for (const [node, state] of activeSources) {
      const gainEvent = state.spatialGainEvent;
      const gainSpatial = gainEvent ? unitSoundNativeSpatial(gainEvent, snapshot, listener) : null;
      if (gainSpatial && gainSpatial.gainByte !== state.spatialGainByte) {
        state.spatialGainByte = gainSpatial.gainByte;
        gainEvent.nativeSpatialGainByte = gainSpatial.gainByte;
        gainEvent.appliedSpatialGainByte = gainSpatial.gainByte;
        node.gain.value = sourceGainFor(state.row, gainSpatial.gainByte);
      }
      const panEvent = state.spatialPanEvent;
      if (state.balance && panEvent) {
        const panCode = gainSpatial && panEvent === gainEvent
          ? gainSpatial.panCode : unitSoundNativePanCode(panEvent, snapshot, listener);
        if (Number.isInteger(panCode) && panCode !== panEvent.appliedSpatialPanCode) {
          panEvent.nativeSpatialPanCode = panCode;
          applyBalance(state.balance, panEvent, panCode);
        }
      }
    }
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
      const source = context.createBufferSource(); source.buffer = audio; const node = connectSource(source, row ?? { volume: 255, music: false });
      source.onended = () => {
        source.disconnect(); disconnectSource(node);
        if (dialogueSource === source) dialogueSource = null;
        if (token === dialogueGeneration) finish();
      };
      source.start(); dialogueSource = source; started++; rec.started = true;
    } catch (error) { rec.error = String(error); finish(); }
    return rec;
  }
  return { play, playRow, playDialogue, stopDialogue, whenDialogueDone: () => dialogueDone, updateSpatialGain, events, setVolume(v, nextSliders) { volume = Math.max(0, Math.min(1, Number(v) || 0)); if (nextSliders) sliders = { sound: nextSliders.sound, music: nextSliders.music }; else sliders = { sound: volume, music: 1 }; if (gain) gain.gain.value = volume; for (const [node, state] of activeSources) node.gain.value = sourceGainFor(state.row, state.spatialGainByte); },
    get volume() { return gain ? gain.gain.value : volume; },
    evidence: () => ({ started, dialogueActive: Boolean(dialogueSource), events: events.map((x) => ({ ...x })), state: context.state }), close: () => context.close?.() };
}
