import { decodeDatPalette, decodeDc6 } from './d2-formats.mjs';
import { composeCofFrame, decodeCof, decodeDcc } from './unit-formats.mjs';
import { dccDirection, indexedToRgba } from './unit-renderer.mjs';
import { createBlender } from './frontend.mjs';
import { createUnitShadowRenderer } from './unit-shadow-renderer.mjs';
import { highlightMap } from './unit-highlight.mjs';

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

// Retail ships some monster components as a DC6 cel and not a DCC.  The disc list (`d2.lst`) holds
// `Data\Global\Monsters\TY\TR\TYTRLITNUHTH.DC6` where TY's own LA and RA are `.dcc` -- and TY, TX,
// DI, MP, MQ and 42 account for 17 TR, 10 RA, 10 LA and a dozen S/HD/LG cel components between them.
// A `.DCC`-only lookup drops them silently (`catch {}`), which is exactly F45: Tyrael's body layer is
// the TR cel, so he drew as two wings with no body.  Retail's own draw list names the layer it drew
// (`cel dcc:monsters\TY\TR\TYTRlitNUhth`, `retail/drawlists/act4-tyrael`), so the file exists and is
// the one to draw; only its container differs.
//
// A DC6 is laid out as a DCC direction table so composeCofFrame needs no special case.  The grids
// agree (TY TR is 1x60 against a COF that is 1x60).  One translation is NOT free: a DC6 frame's
// yOffset is its BOTTOM row inclusive (play/assets/drawlist.mjs: top = y + offsetY - height + 1),
// while a DCC frame with bottomUp set carries its top, so the offset is converted once here.
export function dc6AsDcc(bytes) {
  const dc6 = decodeDc6(bytes);
  // The frame field names are the DCC ones (`indexed`, not the DC6 decoder's `indices`): this object
  // is fed to composeCofFrame, which composes DCC frames.
  const frames = dc6.frames.map((frame) => ({ width: frame.width, height: frame.height, xOffset: frame.offsetX,
    yOffset: frame.offsetY - frame.height + 1, bottomUp: true, indexed: frame.indices, opaque: frame.opaque }));
  return { container: 'DC6', directions: dc6.directions, framesPerDirection: dc6.framesPerDirection,
    directionRecords: Array.from({ length: dc6.directions }, (_, direction) =>
      ({ frames: frames.slice(direction * dc6.framesPerDirection, (direction + 1) * dc6.framesPerDirection) })) };
}

export function loadMonsterAnimation(read, actor) {
  if (!/^[\x21-\x7e]{2,4}$/.test(actor.code)) throw new Error(`invalid monster code ${actor.code}`);
  const token = monsterModeToken(actor.mode);
  let cof, weaponClass;
  for (const candidate of ['HTH', '1HS', '1HT', '2HS', '2HT']) {
    try { cof = decodeCof(read(`data\\global\\MONSTERS\\${actor.code}\\COF\\${actor.code}${token}${candidate}.COF`)); weaponClass = candidate; break; }
    catch {}
  }
  if (!cof) throw new Error(`no retail COF for monster ${actor.code} ${token}`);
  const direction = dccDirection(actor.direction, cof.directions), components = new Map(), containers = new Map();
  for (const layer of cof.layerRecords) {
    const component = COMPONENT[layer.component], variant = actor.components[layer.component];
    if (!component || !variant) continue;
    const path = `data\\global\\MONSTERS\\${actor.code}\\${component}\\${actor.code}${component}${variant}${token}${layer.weaponClass.toUpperCase()}.DCC`;
    let cel = null, container = null;
    try { cel = decodeDcc(read(path), { directions: [direction] }); container = 'DCC'; } catch {}
    if (!cel) { try { cel = dc6AsDcc(read(path.replace(/\.DCC$/, '.DC6'))); container = 'DC6'; } catch {} }
    if (cel) { components.set(layer.component, cel); containers.set(layer.component, container); }
  }
  if (!components.size) throw new Error(`no retail DCC components for monster ${actor.code} ${token}/${weaponClass}`);
  return { cof, components, containers, direction, token };
}

/** `shadow: true` draws the unit's shadow under its sprite first, as retail's draw list orders it. */
export function createMonsterRenderer(read, documentObject = globalThis.document, { shadow = false } = {}) {
  const animations = new Map(), failures = new Set(), palettes = new Map(), rasters = new Map(), composedFrames = new Map(), blenders = new Map(), highlights = new Map();
  const shadows = shadow ? createUnitShadowRenderer(read) : null;
  const blenderFor = (act0) => { if (!blenders.has(act0)) { let b = null; try { b = createBlender(read(`data\\global\\palette\\${PALETTE[act0]}\\pal.pl2`), palettes.get(act0)); } catch {} blenders.set(act0, b); } return blenders.get(act0); };
  let lastEvidence = null;
  const palette = (act) => {
    const act0 = Math.min(4, Math.max(0, Math.trunc(Number(act) || 1) - 1));
    if (!palettes.has(act0)) palettes.set(act0, decodeDatPalette(read(`data\\global\\palette\\${PALETTE[act0]}\\pal.dat`)));
    return [act0, palettes.get(act0)];
  };
  return {
    // `highlight`: retail's DrawMode 7, the hovered unit (V5, unit-highlight.mjs) -- the cel's colours through the measured rule.
    draw(ctx, x, y, actor, act, highlight = false) {
      const baseKey = `${actor.code}:${actor.mode}:${actor.direction}:${actor.components.join(',')}`;
      if (failures.has(baseKey)) return null;
      let animation = animations.get(baseKey);
      if (!animation) { try { animation = loadMonsterAnimation(read, actor); animations.set(baseKey, animation); } catch { failures.add(baseKey); return null; } }
      // The engine wraps a looping mode's frame counter; one past the end means a one-shot sequence has
      // finished (Izual's ghost reports S1 at 40,860 frames of a 20-frame rise, whose frame 0 is empty --
      // the modulo drew nothing), so it holds its last frame.
      const whole = actor.framePrecise >>> 8, count = animation.cof.framesPerDirection;
      const frame = (whole >= count && actor.frameCountPrecise && actor.framePrecise >= actor.frameCountPrecise ? count - 1 : whole % count) >>> 0;
      let composed;
      // PERF (F198): a composed frame is a pure function of the (cached) animation and the frame; composed once, its identity hash and opaque count with it.
      const composeKey = `${baseKey}:${frame}`;
      try { composed = composedFrames.get(composeKey); if (!composed) { composed = composeCofFrame(animation.cof, animation.direction, frame, animation.components); composedFrames.set(composeKey, composed); } }
      catch (e) { failures.add(baseKey); console.warn(`monster ${actor.code} mode ${actor.mode} dir ${actor.direction} frame ${frame}: ${e.message}`); return null; }   // a COF/DCC frame-count mismatch must not stop the frame loop
      const [act0, pal] = palette(act), key = `${baseKey}:${frame}:${act0}${highlight ? ':hl' : ''}`;
      if (highlight && !highlights.has(act0)) highlights.set(act0, highlightMap(read(`data\\global\\palette\\${PALETTE[act0]}\\pal.dat`)));
      const hmap = highlight ? highlights.get(act0) : null;
      let canvas = rasters.get(key);
      if (!canvas) {
        canvas = documentObject.createElement('canvas'); canvas.width = composed.width; canvas.height = composed.height;
        if (canvas.width && canvas.height) canvas.getContext('2d').putImageData(
          new ImageData(indexedToRgba(hmap ? composed.indexed.map((v) => hmap[v]) : composed.indexed, composed.opaque, pal), canvas.width, canvas.height), 0, 0);
        rasters.set(key, canvas);
      }
      // D2Client sets the unit's no-shadow bit (0x20) for a dead monster; retail's records: 41 DT/DD instances, 0 shadows.
      if (animation.token !== 'DT' && animation.token !== 'DD') shadows?.draw(ctx, Math.round(x), Math.round(y),
        { cof: animation.cof, components: animation.components, direction: animation.direction, frame },
        { kind: 'monster', id: actor.id, classId: actor.classId, code: actor.code, token: animation.token, direction: animation.direction, frame, act });
      // A COF layer marked transparent draws through its draw effect (Izual's ghost: both layers, effect 3
      // = PL2 AdditiveBlend, the draw mode retail's own frame buffers pin for mode 3). Such a unit is
      // drawn layer by layer in the COF's priority order; effects other than 3/4 stay opaque (declared).
      const blended = animation.cof.layerRecords.some((l) => l.transparent && (l.drawEffect === 3 || l.drawEffect === 4));
      if (blended) {
        const blend = blenderFor(act0);
        for (const component of animation.cof.priority[animation.direction][frame]) {
          const dcc = animation.components.get(component); if (!dcc) continue;
          const layer = animation.cof.layerRecords.find((l) => l.component === component);
          const one = composeCofFrame(animation.cof, animation.direction, frame, new Map([[component, dcc]]));
          if (!one.width || !one.height) continue;
          const left = Math.round(x + one.xOffset), top = Math.round(y + one.yOffset);
          if (layer && layer.transparent && (layer.drawEffect === 3 || layer.drawEffect === 4) && blend)
            blend(ctx, { width: one.width, height: one.height, indices: one.indexed, opaque: one.opaque }, left, top, layer.drawEffect);
          else { const cv = documentObject.createElement('canvas'); cv.width = one.width; cv.height = one.height;
            cv.getContext('2d').putImageData(new ImageData(indexedToRgba(one.indexed, one.opaque, pal), one.width, one.height), 0, 0); ctx.drawImage(cv, left, top); }
        }
      } else if (canvas.width && canvas.height) ctx.drawImage(canvas, Math.round(x + composed.xOffset), Math.round(y + composed.yOffset));
      // Draw-list records (drawlist.mjs conventions): one per COF layer drawn, named as retail's cel
      // records name a DCC (monsters\\<code>\\<comp>\\<code><comp><variant><mode><class>), at the unit's anchor.
      // The record carries the whole identity of the blit, so a row can ask whether the thing DRAWN is
      // the thing the engine asked for: `token` is the sequence that came out of the engine's mode,
      // `frame` the frame of that sequence this blit composed, `dir` the unit's own 0..63 facing --
      // the SAME conventions (and the same units) retail's captured draw list records in `dir=`, so the
      // two compare field for field instead of through a derived index. `frames` is the sequence's
      // length, which is what makes an animation's advance per tick measurable (retail's own capture
      // reports the frame, not the length, and a wrap is only readable against it).
      if (Array.isArray(globalThis.__d2DrawList)) for (const layer of animation.cof.layerRecords) {
        const comp = COMPONENT[layer.component], variant = actor.components[layer.component];
        if (!comp || !variant || !animation.components.has(layer.component)) continue;
        globalThis.__d2DrawList.push({ op: 'unit', path: `monsters\\${actor.code}\\${comp}\\${actor.code}${comp}${variant}${animation.token}${layer.weaponClass}`.toLowerCase(),
          frame, frames: animation.cof.framesPerDirection, dir: actor.direction, token: animation.token,
          x: Math.round(x), y: Math.round(y), mode: highlight ? 7 : 5 });
      }
      if (composed.pixelHash === undefined) { let h = 0x811c9dc5; for (const value of composed.indexed) { h ^= value; h = Math.imul(h, 0x01000193); } composed.pixelHash = h; composed.opaqueCount = composed.opaque.reduce((a, v) => a + v, 0); }
      const pixelHash = composed.pixelHash;
      // `directions` and `frames` are the COF's own lengths, so a caller comparing the drawn identity
      // against the engine's requested one (s278) can check the direction and frame against the
      // sequence they belong to without re-decoding the COF.
      lastEvidence = { id: actor.id, classId: actor.classId, code: actor.code, token: animation.token,
        direction: animation.direction, directions: animation.cof.directions, frame, frames: count,
        components: animation.components.size, width: composed.width,
        // F14: the blit's own placement beside its size -- the rectangle this draw actually painted,
        // in anchor-relative px (`ctx.drawImage(canvas, Math.round(x + xOffset), Math.round(y + yOffset))`
        // above). A row measuring a unit's hover box needs the rect that was DRAWN, not a re-derivation
        // of it from the DCC beside the renderer.
        xOffset: composed.xOffset, yOffset: composed.yOffset,
        // F14: the ANIMATION's own box for this direction -- the union over the drawn layers' DCC
        // direction boxes (unit-formats' `direction.box`, the union across that direction's frames),
        // anchor-relative, the same rectangle Riiablo's HoveredManager hit-tests (`animation.getBox()`).
        // A single frame's cel is smaller than this whenever the animation moves, so the two must be
        // told apart by measurement rather than assumed equal.
        animBox: (() => { let a = null; for (const dcc of animation.components.values()) { const d = dcc.directionRecords[animation.direction]; if (!d || !d.box) continue;
          const b = d.box; a = a ? { minX: Math.min(a.minX, b.minX), minY: Math.min(a.minY, b.minY), maxX: Math.max(a.maxX, b.maxX), maxY: Math.max(a.maxY, b.maxY) } : { minX: b.minX, minY: b.minY, maxX: b.maxX, maxY: b.maxY }; } return a; })(),
        height: composed.height, opaque: composed.opaqueCount, pixelHash: pixelHash >>> 0, mode: highlight ? 7 : 5 };
      return lastEvidence;
    },
    get lastEvidence() { return lastEvidence && { ...lastEvidence }; },
    get decodedAnimations() { return animations.size; }, get failures() { return failures.size; },
  };
}
