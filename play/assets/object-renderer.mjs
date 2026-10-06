import { decodeDatPalette } from './d2-formats.mjs';
import { composeCofFrame, decodeCof, decodeDcc } from './unit-formats.mjs';
import { dccDirection, indexedToRgba } from './unit-renderer.mjs';
import { fourcc } from './monster-renderer.mjs';
import { createBlender } from './frontend.mjs';
import { highlightMap } from './unit-highlight.mjs';
import { createUnitShadowRenderer } from './unit-shadow-renderer.mjs';
import { createObjectShadowPolicy } from './object-shadow-policy.mjs';

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

/**
 * `shadow: true` draws each of the object's shadow-carrying COF layers under its sprite first, the way
 * retail orders it: a `shadow dcc:` immediately before the matching `cel dcc:` of the same path, frame
 * and anchor.
 * Retail shadows these objects through that draw: the torches, banners, rocks, the waypoint, the
 * rogue's campfire. The gate is the layer's own COF Shadow byte and not its transparency byte; the
 * counts and the negative control are
 */
export function createObjectRenderer(read, documentObject = globalThis.document, { shadow = false } = {}) {
  const animations = new Map(), failures = new Set(), rasters = new Map(), palettes = new Map(), blenders = new Map(), highlights = new Map();
  const shadows = shadow ? createUnitShadowRenderer(read) : null;
  const shadowPolicy = shadow ? createObjectShadowPolicy(read) : null;
  let lastEvidence = null, measuring = false;
  const palette = (act) => { const a = Math.min(4, Math.max(0, (act | 0) - 1));
    if (!palettes.has(a)) palettes.set(a, decodeDatPalette(read(`data\\global\\palette\\${PALETTE[a]}\\pal.dat`))); return [a, palettes.get(a)]; };
  const blender = (act0, pal) => {
    if (!blenders.has(act0)) { let value = null;
      try { value = createBlender(read(`data\\global\\palette\\${PALETTE[act0]}\\pal.pl2`), pal); } catch {}
      blenders.set(act0, value);
    }
    return blenders.get(act0);
  };
  // Where a hit test paints when it needs the sprite's own box: a 1x1 scratch canvas, drawn once
  // and never read, so measuring an object never paints the frame the player sees.
  let scratch = null;
  const scratchContext = () => { if (!scratch) { scratch = documentObject.createElement('canvas'); scratch.width = 1; scratch.height = 1; } return scratch.getContext('2d'); };
  const highlightedIndices = (indices, opaque, act0) => {
    if (!highlights.has(act0)) highlights.set(act0, highlightMap(read(`data\\global\\palette\\${PALETTE[act0]}\\pal.dat`)));
    const map = highlights.get(act0), out = indices.slice();
    for (let i = 0; i < out.length; i++) if (opaque[i]) out[i] = map[out[i]];
    return out;
  };
  const renderer = { draw(ctx, x, y, object, act, highlight = false) {
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
    // The shadow goes under the sprite, at the object's own anchor, one blit per COF layer whose Shadow
    // byte is set -- retail's own order (shadow records precede every cel of the same frame).
    const shadowDecision = shadowPolicy ? shadowPolicy.decide(object) : { draw: false, reason: 'shadow-disabled', drawUnder: null };
    const shadowBlit = shadows && shadowDecision.draw && !measuring ? shadows.draw(ctx, Math.round(x), Math.round(y),
      { cof: animation.cof, components: animation.components, direction: animation.direction, frame },
      { kind: 'object', id: object.id, classId: object.classId, token: object.token, direction: animation.direction, frame, act }) : null;
    const composed = composeCofFrame(animation.cof, animation.direction, frame, animation.components), [act0, pal] = palette(act);
    const key = `${base}:${frame}:${act0}:${highlight ? 'highlight' : 'normal'}`; let canvas = rasters.get(key);
    if (!canvas) { canvas = documentObject.createElement('canvas'); canvas.width = composed.width; canvas.height = composed.height;
      if (canvas.width && canvas.height) canvas.getContext('2d').putImageData(new ImageData(indexedToRgba(
        highlight ? highlightedIndices(composed.indexed, composed.opaque, act0) : composed.indexed, composed.opaque, pal), canvas.width, canvas.height), 0, 0); rasters.set(key, canvas); }
    // COF effects apply to individual layers.  Flattening the object first lost the additive
    // flames/glows/smoke and rendered their source indices as opaque black or grey (F28/F41).
    const layerEvidence = [];
    const hasBlend = animation.cof.layerRecords.some((layer) => layer.transparent && (layer.drawEffect === 3 || layer.drawEffect === 4));
    if (hasBlend) {
      const blend = blender(act0, pal);
      for (const componentId of animation.cof.priority[animation.direction][frame]) {
        const dcc = animation.components.get(componentId); if (!dcc) continue;
        const layer = animation.cof.layerRecords.find((value) => value.component === componentId);
        const one = composeCofFrame(animation.cof, animation.direction, frame, new Map([[componentId, dcc]]));
        if (!one.width || !one.height) continue;
        const left = Math.round(x + one.xOffset), top = Math.round(y + one.yOffset);
        const requestedMode = layer?.transparent && (layer.drawEffect === 3 || layer.drawEffect === 4) ? layer.drawEffect : 5;
        // Evidence states what reached the canvas. A missing PL2 must never be reported as a blend.
        const mode = highlight ? 7 : (requestedMode !== 5 && blend ? requestedMode : 5);
        if (highlight) {
          const oneCanvas = documentObject.createElement('canvas'); oneCanvas.width = one.width; oneCanvas.height = one.height;
          oneCanvas.getContext('2d').putImageData(new ImageData(indexedToRgba(
            highlightedIndices(one.indexed, one.opaque, act0), one.opaque, pal), one.width, one.height), 0, 0);
          ctx.drawImage(oneCanvas, left, top);
        } else if (mode !== 5 && blend) blend(ctx, { width: one.width, height: one.height, indices: one.indexed, opaque: one.opaque }, left, top, mode);
        else {
          const oneCanvas = documentObject.createElement('canvas'); oneCanvas.width = one.width; oneCanvas.height = one.height;
          oneCanvas.getContext('2d').putImageData(new ImageData(indexedToRgba(one.indexed, one.opaque, pal), one.width, one.height), 0, 0);
          ctx.drawImage(oneCanvas, left, top);
        }
        const component = COMPONENT[componentId];
        const path = component && layer ? `objects\\${object.token}\\${component}\\${object.token}${component}lit${animation.mode}${layer.weaponClass}`.toLowerCase() : null;
        layerEvidence.push({ component, path, mode, transparent: Boolean(layer?.transparent), drawEffect: layer?.drawEffect ?? 0, shadow: Boolean(layer?.shadow) });
        if (Array.isArray(globalThis.__d2DrawList) && path) globalThis.__d2DrawList.push({ op: 'dcc', path, frame, x: Math.round(x), y: Math.round(y), mode });
      }
    } else if (canvas.width && canvas.height) {
      ctx.drawImage(canvas, Math.round(x + composed.xOffset), Math.round(y + composed.yOffset));
      for (const componentId of animation.cof.priority[animation.direction][frame]) {
        const layer = animation.cof.layerRecords.find((value) => value.component === componentId), component = COMPONENT[componentId];
        if (!layer || !component || !animation.components.has(componentId)) continue;
        const path = `objects\\${object.token}\\${component}\\${object.token}${component}lit${animation.mode}${layer.weaponClass}`.toLowerCase();
        layerEvidence.push({ component, path, mode: highlight ? 7 : 5, transparent: Boolean(layer.transparent), drawEffect: layer.drawEffect, shadow: Boolean(layer.shadow) });
        if (Array.isArray(globalThis.__d2DrawList)) globalThis.__d2DrawList.push({ op: 'dcc', path, frame, x: Math.round(x), y: Math.round(y), mode: highlight ? 7 : 5 });
      }
    }
    // PERF (F238): the FNV hash and opaque count are pure functions of the composed plane, and
    // composeCofFrame memoises planes, so both are computed ONCE per plane and shared with the monster
    // pass (same plane object, same F198 formula). Recomputing them per object per frame walked every
    // composed plane's pixels every frame; the town carries dozens of torch/banner objects.
    if (composed.pixelHash === undefined) { let h = 0x811c9dc5; for (const value of composed.indexed) { h ^= value; h = Math.imul(h, 0x01000193); } composed.pixelHash = h; composed.opaqueCount = composed.opaque.reduce((a, v) => a + v, 0); }
    lastEvidence = { id: object.id, classId: object.classId, token: object.token, mode: animation.mode, direction: animation.direction,
      frame, components: animation.components.size, width: composed.width, height: composed.height,
      xOffset: composed.xOffset, yOffset: composed.yOffset,
      opaque: composed.opaqueCount, pixelHash: composed.pixelHash >>> 0, layers: layerEvidence,
      // The shadow blit this draw made (or null).  `layers[].shadow` is the COF byte as retail shipped
      // it; `shadow.layers`/`shadow.components` is what the pass decided to draw -- so a row can assert
      // both a blit for a Shadow-set layer and none for a layer whose bytes say otherwise.
      shadow: shadowBlit, shadowLayers: shadowBlit ? shadowBlit.layers : 0, shadowDecision }; return lastEvidence;
  },
  // The box this object's sprite occupies when drawn at (x, y), or null when nothing would be drawn
  // (no token, a COF/DCC that will not decode). Retail's hover is the drawn sprite, not a radius
  // around the object's origin, so the hit test asks the SAME draw that paints the frame -- into the
  // scratch canvas, with the draw list and the last evidence left exactly as the frame left them.
  measure(object, act) {
    if (!object?.token) return null;
    const keepEvidence = lastEvidence, keepList = globalThis.__d2DrawList;
    globalThis.__d2DrawList = null; measuring = true;   // the scratch canvas is 1x1: a shadow there is all clip
    try { const drawn = renderer.draw(scratchContext(), 0, 0, object, act);
      return drawn ? { width: drawn.width, height: drawn.height, xOffset: drawn.xOffset, yOffset: drawn.yOffset } : null; }
    catch { return null; }
    finally { lastEvidence = keepEvidence; globalThis.__d2DrawList = keepList; measuring = false; }
  },
  get lastEvidence(){return lastEvidence&&{...lastEvidence};}, get decoded(){return animations.size;}, get failures(){return failures.size;} };
  return renderer;
}
