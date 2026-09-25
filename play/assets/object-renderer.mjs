import { decodeDatPalette } from './d2-formats.mjs';
import { composeCofFrame, decodeCof, decodeDcc } from './unit-formats.mjs';
import { dccDirection, indexedToRgba } from './unit-renderer.mjs';
import { fourcc } from './monster-renderer.mjs';

const COMPONENT = ['HD','TR','LG','RA','LA','RH','LH','SH','S1','S2','S3','S4','S5','S6','S7','S8'];
const MODE = ['NU','OP','ON','S1','S2','S3','S4','S5'];
const PALETTE = ['ACT1','ACT2','ACT3','ACT4','ACT5'];

export function parseObjectPresentations(bytes, count) {
  const view = bytes instanceof DataView ? bytes : new DataView(bytes.buffer, bytes.byteOffset || 0, bytes.byteLength);
  if (!Number.isInteger(count) || count < 0 || view.byteLength < count * 40) throw new RangeError('truncated object presentation records');
  return Array.from({ length: count }, (_, row) => { const at = row * 40; return {
    id: view.getUint32(at, true), classId: view.getUint32(at + 4, true), token: fourcc(view.getUint32(at + 8, true)),
    mode: view.getUint32(at + 12, true), x: view.getInt32(at + 16, true), y: view.getInt32(at + 20, true),
    framePrecise: view.getUint32(at + 24, true), frameCountPrecise: view.getUint32(at + 28, true),
    componentMask: view.getUint32(at + 32, true), animSpeed: view.getUint16(at + 36, true), direction: view.getUint8(at + 38), selectable: view.getUint8(at + 39) !== 0,
  }; });
}

export function createObjectRenderer(read, documentObject = globalThis.document) {
  const animations = new Map(), failures = new Set(), rasters = new Map(), palettes = new Map(); let lastEvidence = null;
  const palette = (act) => { const a = Math.min(4, Math.max(0, (act | 0) - 1));
    if (!palettes.has(a)) palettes.set(a, decodeDatPalette(read(`data\\global\\palette\\${PALETTE[a]}\\pal.dat`))); return [a, palettes.get(a)]; };
  return { draw(ctx, x, y, object, act) {
    const mode = MODE[object.mode] || 'NU', base = `${object.token}:${mode}:${object.direction}:${object.componentMask}`;
    if (!object.token || failures.has(base)) return null;
    let animation = animations.get(base);
    if (!animation) try {
      const cof = decodeCof(read(`data\\global\\OBJECTS\\${object.token}\\COF\\${object.token}${mode}HTH.COF`));
      const direction = dccDirection(object.direction, cof.directions), components = new Map();
      for (const layer of cof.layerRecords) if (object.componentMask & (1 << layer.component)) {
        const component = COMPONENT[layer.component]; if (!component) continue;
        const path = `data\\global\\OBJECTS\\${object.token}\\${component}\\${object.token}${component}LIT${mode}${layer.weaponClass.toUpperCase()}.DCC`;
        try { components.set(layer.component, decodeDcc(read(path), { directions: [direction] })); } catch {}
      }
      if (!components.size) throw new Error('no DCC components');
      animation = { cof, direction, components, mode }; animations.set(base, animation);
    } catch { failures.add(base); return null; }
    const frame = (object.framePrecise >>> 8) % animation.cof.framesPerDirection;
    const composed = composeCofFrame(animation.cof, animation.direction, frame, animation.components), [act0, pal] = palette(act);
    const key = `${base}:${frame}:${act0}`; let canvas = rasters.get(key);
    if (!canvas) { canvas = documentObject.createElement('canvas'); canvas.width = composed.width; canvas.height = composed.height;
      if (canvas.width && canvas.height) canvas.getContext('2d').putImageData(new ImageData(indexedToRgba(composed.indexed, composed.opaque, pal), canvas.width, canvas.height), 0, 0); rasters.set(key, canvas); }
    if (canvas.width && canvas.height) ctx.drawImage(canvas, Math.round(x + composed.xOffset), Math.round(y + composed.yOffset));
    let pixelHash = 0x811c9dc5; for (const value of composed.indexed) { pixelHash ^= value; pixelHash = Math.imul(pixelHash, 0x01000193); }
    lastEvidence = { id: object.id, classId: object.classId, token: object.token, mode: animation.mode, direction: animation.direction,
      frame, components: animation.components.size, width: composed.width, height: composed.height,
      opaque: composed.opaque.reduce((a,v)=>a+v,0), pixelHash: pixelHash >>> 0 }; return lastEvidence;
  }, get lastEvidence(){return lastEvidence&&{...lastEvidence};}, get decoded(){return animations.size;}, get failures(){return failures.size;} };
}
