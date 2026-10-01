import { decodeWav } from './wav.mjs';
import { resolveUnitSound, soundRow } from './unit-sound-resolver.mjs';

// S2: drained engine sound events (d2_session_take_sounds sextets) -> resolver -> Sounds.txt file -> Web Audio.
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
  let volume = 1;
  // A suspended context still schedules source.start(); it sounds once a trusted gesture resumes it.
  if (context.state === 'suspended' && typeof gestureTarget?.addEventListener === 'function') {
    const ac = new AbortController();
    const unlock = () => { context.resume().catch(() => {}); ac.abort(); };
    for (const t of ['pointerdown', 'keydown', 'touchstart', 'gamepadconnected']) gestureTarget.addEventListener(t, unlock, { signal: ac.signal, passive: true });
  }
  let soundsTxt = null, started = 0;
  const table = () => soundsTxt ??= new TextDecoder('latin1').decode(read('data\\global\\excel\\Sounds.txt'));
  const buffer = (path, rec) => {
    let b = buffers.get(path);
    if (b) rec.path = b.path;
    if (!b) { let bytes, err; for (const root of ROOTS) { try { bytes = read(root + path); rec.path = root + path; break; } catch (e) { err = e; } } if (!bytes) throw err; decodeWav(bytes); b = context.decodeAudioData(bytes.buffer.slice(bytes.byteOffset, bytes.byteOffset + bytes.byteLength)); b.path = rec.path; buffers.set(path, b); }
    return b;
  };
  /** One drained event {unitType, unitId, classId, soundEvent, updateUnitType, updateUnitId}; localId = the hero's unit id. */
  async function play(e, localId) {
    // Audience: -1 = every client, else only the client of that unit.
    if (e.updateUnitType !== -1 && e.updateUnitId !== localId) return null;
    const row = resolveUnitSound({ unitType: e.unitType, classId: e.classId, soundEvent: e.soundEvent, isLocalPlayer: e.unitType === 0 && e.unitId === localId });
    const rec = { ...e, row, handle: null, path: null, started: false, error: '' };
    events.push(rec);
    if (row == null) return rec;
    try {
      const r = soundRow(table(), row);
      if (!r?.file) throw new Error(`Sounds.txt row ${row} has no file`);
      rec.handle = r.handle;
      context.resume().catch(() => {});
      const source = context.createBufferSource(); source.buffer = await buffer(r.file, rec); source.connect(out);
      source.onended = () => source.disconnect();
      source.start(); started++; rec.started = true;
    } catch (error) { rec.error = String(error); }
    return rec;
  }
  return { play, events, setVolume(v) { volume = Math.max(0, Math.min(1, Number(v) || 0)); if (gain) gain.gain.value = volume; },
    get volume() { return gain ? gain.gain.value : volume; },
    evidence: () => ({ started, events: events.map((x) => ({ ...x })), state: context.state }), close: () => context.close?.() };
}
