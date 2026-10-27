import { createHudHelp } from '../../play/assets/hud-help.mjs';

export function eventHost() {
  const listeners = new Map(), captures = [];
  return {
    captures,
    addEventListener(type, listener) { listeners.set(type, listener); },
    setPointerCapture(pointerId) { captures.push(pointerId); },
    dispatch(type, values = {}) {
      const event = { button: 0, pointerId: 1, x: 725, y: 404, preventDefault() { this.defaultPrevented = true; }, ...values };
      listeners.get(type)?.(event);
      return event;
    },
  };
}

function dc6Fixture(frameIndices, width = 2, height = 2) {
  const table = 24 + frameIndices.length * 4;
  const encodedLength = height * (width + 2), stride = 32 + encodedLength + 3;
  const bytes = new Uint8Array(table + stride * frameIndices.length);
  const view = new DataView(bytes.buffer);
  view.setInt32(0, 6, true); view.setUint32(16, 1, true); view.setUint32(20, frameIndices.length, true);
  for (let i = 0; i < frameIndices.length; i++) {
    const at = table + stride * i;
    const frame = Uint8Array.from(Array.from({ length: height }, () => [width, ...Array(width).fill(frameIndices[i]), 0x80]).flat());
    view.setUint32(24 + i * 4, at, true);
    view.setUint32(at + 4, width, true); view.setUint32(at + 8, height, true);
    view.setUint32(at + 28, frame.length, true); bytes.set(frame, at + 32);
  }
  return bytes;
}

function fontTable() {
  const chars = [...new Set('Help (H)')], bytes = new Uint8Array(12 + chars.length * 14);
  bytes.set(new TextEncoder().encode('Woo!')); bytes[10] = 2; bytes[11] = 2;
  const view = new DataView(bytes.buffer);
  chars.forEach((char, i) => {
    const at = 12 + i * 14;
    view.setUint16(at, char.charCodeAt(0), true); bytes[at + 3] = 2; bytes[at + 4] = 2;
  });
  return bytes;
}

export function fixtureRenderer() {
  const assets = {
    'data\\local\\font\\latin\\font16.dc6': dc6Fixture([17]),
    'data\\local\\font\\latin\\font16.tbl': fontTable(),
    'data\\global\\ui\\PANEL\\levelsocket.dc6': dc6Fixture([21], 35, 36),
    'data\\global\\ui\\PANEL\\level.dc6': dc6Fixture([31, 41, 51], 30, 30),
    'data\\global\\palette\\ACT1\\pal.dat': new Uint8Array(768),
  };
  const requests = [];
  const read = (path) => { requests.push(path); if (!(path in assets)) throw new Error(`unexpected asset ${path}`); return assets[path]; };
  const documentObject = { createElement() { return { width: 0, height: 0, getContext: () => ({ putImageData() {} }) }; } };
  return { renderer: createHudHelp(read, documentObject), requests };
}

