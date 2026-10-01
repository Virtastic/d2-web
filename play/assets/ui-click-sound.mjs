import { decodeWav } from './wav.mjs';

// Sounds.txt row 4: cursor_button_click. This is retail UI feedback, not a
// browser-generated substitute (reference/libd2/.../Sounds.txt).
export const UI_BUTTON_CLICK = Object.freeze({
  handle: 'cursor_button_click',
  path: 'data\\global\\sfx\\cursor\\button.wav',
});

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
  let volume = 1;
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
    source.buffer = decoded; source.connect(out); source.start(); started++;
    return evidence();
  }
  function evidence() { return { ...UI_BUTTON_CLICK, decoded: Boolean(buffer), requested, started, state: context.state, error: lastError }; }
  // This listener is installed during bootstrap, after gameplay-shell's input
  // listener. A raw event cannot claim a sound: the real shell must consume it.
  canvas.addEventListener('pointerdown', (event) => {
    if (event.button === 0 && event.defaultPrevented) play().catch(() => {});
  });
  const player = { prepare, play, evidence, setVolume(v) { volume = Math.max(0, Math.min(1, Number(v) || 0)); if (gain) gain.gain.value = volume; },
    get volume() { return gain ? gain.gain.value : volume; }, close: () => context.close?.() };
  // Diagnostic evidence stays with the rendered target, avoiding a second
  // shell state store solely for this browser capability.
  Object.defineProperty(canvas, '__d2UiClickSound', { value: player });
  return player;
}
