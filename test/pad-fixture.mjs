// A fake DOM, a fake gamepad and the captured poll timer, so a test feeds button/axis samples to the real
// controller module and reads back the exact DOM events it dispatched. No engine, no browser.
export const events = [], polls = [];
const element = () => ({
  style: {}, children: [],
  appendChild(child) { this.children.push(child); },
  dispatchEvent(event) { events.push(event); return true; },
});
const viewport = element();
const canvas = {
  ...element(), width: 800, height: 600,
  offsetLeft: 0, offsetTop: 0, offsetWidth: 800, offsetHeight: 600,
  parentElement: viewport,
  getBoundingClientRect: () => ({ left: 0, top: 0, width: 800, height: 600 }),
};
globalThis.window = globalThis;
globalThis.document = { readyState: 'complete', body: viewport, getElementById: () => canvas, querySelector: () => canvas, createElement: element };
globalThis.getComputedStyle = () => ({ position: 'relative' });
globalThis.PointerEvent = globalThis.KeyboardEvent = globalThis.MouseEvent = class {
  constructor(type, init) { this.type = type; Object.assign(this, init); }
};
globalThis.dispatchEvent = (event) => { events.push(event); return true; };
globalThis.setInterval = (poll) => { polls.push(poll); };
export const pad = { index: 0, id: 'Standard Gamepad', mapping: 'standard', axes: [0, 0, 0, 0], buttons: Array.from({ length: 17 }, () => ({ pressed: false })) };
Object.defineProperty(globalThis, 'navigator', { value: { getGamepads: () => [pad] }, configurable: true });
export const B = { A: 0, B: 1, X: 2, Y: 3, LB: 4, RB: 5, LT: 6, RT: 7, BACK: 8, START: 9, L3: 10, R3: 11, UP: 12, DOWN: 13, LEFT: 14, RIGHT: 15 };
export function install(shell) {
  globalThis.__d2Shell = shell;
  return import('../play/assets/input-controller.mjs');
}
export const poll = () => polls[0]();
export const down = (...buttons) => { for (const b of buttons) pad.buttons[b].pressed = true; poll(); };
export const up = (...buttons) => { for (const b of buttons) pad.buttons[b].pressed = false; poll(); };
export const tap = (button) => { down(button); up(button); };
/** The events since the last call, as short strings: "keydown:KeyI", "pointerdown:400,300". */
export function drain() {
  const out = events.map((e) => (e.type.startsWith('key') ? `${e.type}:${e.code}` : `${e.type}:${e.clientX},${e.clientY}${e.shiftKey ? ':shift' : ''}${e.ctrlKey ? ':ctrl' : ''}`));
  events.length = 0;
  return out;
}
export const keyTaps = (...codes) => codes.flatMap((c) => [`keydown:${c}`, `keyup:${c}`]);
// Real (trusted) mouse/touch events reach the controller through window listeners; a test fires them here.
export const listeners = {};
globalThis.addEventListener = (type, fn) => { (listeners[type] ||= []).push(fn); };
export const userEvent = (type, pointerType = 'mouse') => (listeners[type] || []).forEach((fn) => fn({ type, pointerType, isTrusted: true }));
