import { decodeWav } from './wav.mjs';
import { soundRowByHandle } from './unit-sound-resolver.mjs';
import { sliderPercent, soundSourceGain } from './sound-channel-volume.mjs';

// Sounds.txt row 4: cursor_button_click. This is retail UI feedback, not a
// browser-generated substitute (reference/libd2/.../Sounds.txt).
export const UI_BUTTON_CLICK = Object.freeze({
  handle: 'cursor_button_click',
  path: 'data\\global\\sfx\\cursor\\button.wav',
});

// The shell consumes world clicks too; they issue a walk, attack or interaction
// and are not button activations. The source metadata is attached by the shell's
// earlier pointer listener so this player can distinguish the actual target.
const WORLD_ACTIONS = new Set(['world', 'corpse', 'warp', 'npc', 'object', 'weaponTab']); // weaponTab: D2Client +0x90F60 plays cursor_pass itself, not the button click
export function isUiButtonClick(event) {
  return event?.button === 0 && event.defaultPrevented && !WORLD_ACTIONS.has(event.__d2ClickRecord?.spent);
}

// Match act-music: an AudioContext may only be resumed from a trusted browser
// gesture.  Keep that policy at the edge rather than treating a test runner-made
// event as if it were player input.
function unlockOnGesture(context, target = globalThis.window) {
  if (typeof target?.addEventListener !== 'function' || context.state !== 'suspended') return;
  const controller = new AbortController();
  const unlock = () => { context.resume().catch(() => {}); controller.abort(); };
  const options = { signal: controller.signal, passive: true };
  for (const type of ['pointerdown', 'keydown', 'touchstart', 'gamepadconnected']) target.addEventListener(type, unlock, options);
}

/** Play the retail button sound after the shell consumes a canvas click. */
export function createUiClickSoundPlayer(read, canvas, AudioContextClass = globalThis.AudioContext || globalThis.webkitAudioContext, gestureTarget = globalThis.window) {
  if (typeof read !== 'function') throw new TypeError('retail asset reader is required');
  if (!canvas?.addEventListener) throw new TypeError('canvas event target is required');
  if (!AudioContextClass) throw new Error('Web Audio is unavailable');
  const context = new AudioContextClass();
  unlockOnGesture(context, gestureTarget);
  // F235: the Sound slider drives every SFX player through one gain node (see audio-volume.mjs).
  const gain = typeof context.createGain === 'function' ? context.createGain() : null;
  if (gain) gain.connect(context.destination);
  const out = gain ?? context.destination;
  let volume = 1, sliders = { sound: 1, music: 1 }, soundsTxt = null, sourceRow = null;
  const activeSources = new Map();
  const table = () => soundsTxt ??= new TextDecoder('latin1').decode(read('data\\global\\excel\\Sounds.txt'));
  const clickRow = () => sourceRow ??= soundRowByHandle(table(), UI_BUTTON_CLICK.handle);
  const sourceGainFor = (row) => {
    const master = sliderPercent(sliders.sound), music = sliderPercent(sliders.music);
    const channel = soundSourceGain(row.volume, master, music, row.music);
    return master === 0 ? 0 : channel / (master / 100);
  };
  let buffer = null, preparing = null, requested = 0, started = 0, lastError = '';
  async function prepare() {
    if (buffer) return buffer;
    if (!preparing) preparing = (async () => {
      const bytes = read(UI_BUTTON_CLICK.path);
      decodeWav(bytes);
      const copy = bytes.buffer.slice(bytes.byteOffset, bytes.byteOffset + bytes.byteLength);
      buffer = await context.decodeAudioData(copy);
      return buffer;
    })().catch((error) => { lastError = String(error); preparing = null; throw error; });
    return preparing;
  }
  async function play() {
    requested++;
    // Resume while the trusted pointer handler still has user activation;
    // decoding may complete after that activation has returned to the browser.
    await context.resume();
    const decoded = await prepare();
    const source = context.createBufferSource();
    source.buffer = decoded;
    const row = clickRow();
    if (!row || row.file.toLowerCase() !== 'cursor\\button.wav') throw new Error('Sounds.txt cursor_button_click source row is missing or mismatched');
    const sourceGain = typeof context.createGain === 'function' ? context.createGain() : null;
    if (sourceGain) { sourceGain.gain.value = sourceGainFor(row); sourceGain.connect(out); source.connect(sourceGain); activeSources.set(sourceGain, row); }
    else source.connect(out);
    source.onended = () => { source.disconnect(); if (sourceGain) { sourceGain.disconnect(); activeSources.delete(sourceGain); } };
    source.start(); started++;
    return evidence();
  }
  function evidence() { return { ...UI_BUTTON_CLICK, decoded: Boolean(buffer), requested, started, state: context.state, error: lastError }; }
  // This listener is installed during bootstrap, after gameplay-shell's input
  // listener. A raw event cannot claim a sound: the real shell must consume it.
  canvas.addEventListener('pointerdown', (event) => {
    if (isUiButtonClick(event)) play().catch(() => {});
  });
  const player = { prepare, play, evidence, setVolume(v, nextSliders) { volume = Math.max(0, Math.min(1, Number(v) || 0)); sliders = nextSliders ? { sound: nextSliders.sound, music: nextSliders.music } : { sound: volume, music: 1 }; if (gain) gain.gain.value = volume; for (const [node, row] of activeSources) node.gain.value = sourceGainFor(row); },
    get volume() { return gain ? gain.gain.value : volume; }, close: () => context.close?.() };
  // Diagnostic evidence stays with the rendered target, avoiding a second
  // shell state store solely for this browser capability.
  Object.defineProperty(canvas, '__d2UiClickSound', { value: player });
  return player;
}
