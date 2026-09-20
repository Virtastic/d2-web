import { decodeDatPalette } from './d2-formats.mjs';
import { composeCofFrame, decodeCof, decodeDcc } from './unit-formats.mjs';
import { dccDirection, indexedToRgba } from './unit-renderer.mjs';

const COMPONENT = ['HD','TR','LG','RA','LA','RH','LH','SH','S1','S2','S3','S4','S5','S6','S7','S8'];
const MODE = ['DT','NU','WL','GH','A1','A2','BL','SC','S1','S2','S3','S4','DD','KB','SQ','RN'];
const PALETTE = ['ACT1','ACT2','ACT3','ACT4','ACT5'];

export function fourcc(value) {
  let text = '';
  for (let i = 0; i < 4; i++) { const c = (value >>> (i * 8)) & 0xff; if (c) text += String.fromCharCode(c); }
  return text.trim();
}

export function parseMonsterAnimations(bytes, count) {
  const view = bytes instanceof DataView ? bytes : new DataView(bytes.buffer, bytes.byteOffset || 0, bytes.byteLength);
  if (!Number.isInteger(count) || count < 0 || view.byteLength < count * 92) throw new RangeError('truncated monster animation records');
  return Array.from({ length: count }, (_, row) => {
    const at = row * 92;
    return { id: view.getUint32(at, true), classId: view.getUint32(at + 4, true), code: fourcc(view.getUint32(at + 8, true)),
      mode: view.getUint32(at + 12, true), framePrecise: view.getUint32(at + 16, true), frameCountPrecise: view.getUint32(at + 20, true),
      components: Array.from({ length: 16 }, (_, i) => fourcc(view.getUint32(at + 24 + i * 4, true))),
      animSpeed: view.getUint16(at + 88, true), direction: view.getUint8(at + 90) };
  });
}

export function monsterModeToken(mode) { return MODE[Math.trunc(Number(mode))] || 'NU'; }

export function loadMonsterAnimation(read, actor) {
  if (!/^[\x21-\x7e]{2,4}$/.test(actor.code)) throw new Error(`invalid monster code ${actor.code}`);
  const token = monsterModeToken(actor.mode);
  let cof, weaponClass;
  for (const candidate of ['HTH', '1HS', '1HT', '2HS', '2HT']) {
    try { cof = decodeCof(read(`data\\global\\MONSTERS\\${actor.code}\\COF\\${actor.code}${token}${candidate}.COF`)); weaponClass = candidate; break; }
    catch {}
  }
  if (!cof) throw new Error(`no retail COF for monster ${actor.code} ${token}`);
  const direction = dccDirection(actor.direction, cof.directions), components = new Map();
  for (const layer of cof.layerRecords) {
    const component = COMPONENT[layer.component], variant = actor.components[layer.component];
    if (!component || !variant) continue;
    const path = `data\\global\\MONSTERS\\${actor.code}\\${component}\\${actor.code}${component}${variant}${token}${layer.weaponClass.toUpperCase()}.DCC`;
    try { components.set(layer.component, decodeDcc(read(path), { directions: [direction] })); } catch {}
  }
  if (!components.size) throw new Error(`no retail DCC components for monster ${actor.code} ${token}/${weaponClass}`);
  return { cof, components, direction, token };
}

export function createMonsterRenderer(read, documentObject = globalThis.document) {
  const animations = new Map(), failures = new Set(), palettes = new Map(), rasters = new Map();
  let lastEvidence = null;
  const palette = (act) => {
    const act0 = Math.min(4, Math.max(0, Math.trunc(Number(act) || 1) - 1));
    if (!palettes.has(act0)) palettes.set(act0, decodeDatPalette(read(`data\\global\\palette\\${PALETTE[act0]}\\pal.dat`)));
    return [act0, palettes.get(act0)];
  };
  return {
    draw(ctx, x, y, actor, act) {
      const baseKey = `${actor.code}:${actor.mode}:${actor.direction}:${actor.components.join(',')}`;
      if (failures.has(baseKey)) return null;
      let animation = animations.get(baseKey);
      if (!animation) { try { animation = loadMonsterAnimation(read, actor); animations.set(baseKey, animation); } catch { failures.add(baseKey); return null; } }
      const frame = ((actor.framePrecise >>> 8) % animation.cof.framesPerDirection) >>> 0;
      const composed = composeCofFrame(animation.cof, animation.direction, frame, animation.components);
      const [act0, pal] = palette(act), key = `${baseKey}:${frame}:${act0}`;
      let canvas = rasters.get(key);
      if (!canvas) {
        canvas = documentObject.createElement('canvas'); canvas.width = composed.width; canvas.height = composed.height;
        if (canvas.width && canvas.height) canvas.getContext('2d').putImageData(
          new ImageData(indexedToRgba(composed.indexed, composed.opaque, pal), canvas.width, canvas.height), 0, 0);
        rasters.set(key, canvas);
      }
      if (canvas.width && canvas.height) ctx.drawImage(canvas, Math.round(x + composed.xOffset), Math.round(y + composed.yOffset));
      let pixelHash = 0x811c9dc5; for (const value of composed.indexed) { pixelHash ^= value; pixelHash = Math.imul(pixelHash, 0x01000193); }
      lastEvidence = { id: actor.id, classId: actor.classId, code: actor.code, token: animation.token,
        direction: animation.direction, frame, components: animation.components.size, width: composed.width,
        height: composed.height, opaque: composed.opaque.reduce((a, v) => a + v, 0), pixelHash: pixelHash >>> 0 };
      return lastEvidence;
    },
    get lastEvidence() { return lastEvidence && { ...lastEvidence }; },
    get decodedAnimations() { return animations.size; }, get failures() { return failures.size; },
  };
}
