import { decodeDatPalette, decodeDc6 } from './d2-formats.mjs';
import { cofLayerRecord, composeCofFrame, decodeCof, decodeDcc } from './unit-formats.mjs';
import { cofDirection, dccDirection, indexedToRgba } from './unit-renderer.mjs';
import { createBlender } from './frontend.mjs';
import { createUnitShadowRenderer } from './unit-shadow-renderer.mjs';
import { highlightMap } from './unit-highlight.mjs';
import { advanceIzualGhostS1Clock, resetIzualGhostS1Clock } from './izual-ghost-clock.mjs';
import { createRenderCache } from './render-cache.mjs';

const COMPONENT = ['HD','TR','LG','RA','LA','RH','LH','SH','S1','S2','S3','S4','S5','S6','S7','S8'];
const MODE = ['DT','NU','WL','GH','A1','A2','BL','SC','S1','S2','S3','S4','DD','KB','SQ','RN'];
const PALETTE = ['ACT1','ACT2','ACT3','ACT4','ACT5'];
const MONSTER_LAYER_RASTER_ENTRIES = 256;
const MONSTER_LAYER_RASTER_BYTES = 16 * 1024 * 1024;
const MONSTER_RASTER_ENTRIES = 1024, MONSTER_RASTER_BYTES = 64 * 1024 * 1024;
const MONSTER_FILE_ENTRIES = 512, MONSTER_FILE_BYTES = 48 * 1024 * 1024, MONSTER_ANIMATION_ENTRIES = 1024;
const cofLayerBlendCache = new WeakMap();

// PERF: a monster's 64 facings land on the 8-32 directions its DCCs carry, and every facing of a mode
// shares one COF. Decoded once per file bytes -- the renderer's file cache hands back the same bytes --
// so a monster turning re-reads and re-decodes nothing, and composeCofFrame's memo hits across facings.
const decodedByBytes = new WeakMap();
function decodeOnce(bytes, key, decode) {
  let byKey = decodedByBytes.get(bytes);
  if (!byKey) decodedByBytes.set(bytes, byKey = new Map());
  let value = byKey.get(key);
  if (value === undefined) byKey.set(key, value = decode(bytes));
  return value;
}

// A monster reuses its decoded COF across every frame. Whether that COF needs the per-layer blend
// path is immutable animation metadata, so classify it once instead of scanning layerRecords for
// every visible actor on every frame.
export function monsterCofHasLayerBlend(cof) {
  let blended = cofLayerBlendCache.get(cof);
  if (blended === undefined) {
    blended = cof.layerRecords.some((layer) => layer.transparent && (layer.drawEffect === 3 || layer.drawEffect === 4));
    cofLayerBlendCache.set(cof, blended);
  }
  return blended;
}

// The animation's direction boxes are immutable decoded DCC metadata. Their union is reused in
// per-frame evidence, so compute it with the loaded animation rather than scanning every actor draw.
export function monsterAnimationBox(components, componentDirections) {
  let box = null;
  for (const [slot, dcc] of components) {
    const direction = dcc.directionRecords[componentDirections.get(slot)];
    const bounds = direction?.box;
    if (!bounds) continue;
    box = box ? { minX: Math.min(box.minX, bounds.minX), minY: Math.min(box.minY, bounds.minY),
      maxX: Math.max(box.maxX, bounds.maxX), maxY: Math.max(box.maxY, bounds.maxY) }
      : { minX: bounds.minX, minY: bounds.minY, maxX: bounds.maxX, maxY: bounds.maxY };
  }
  return box;
}

export function fourcc(value) {
  let text = '';
  for (let i = 0; i < 4; i++) { const c = (value >>> (i * 8)) & 0xff; if (c) text += String.fromCharCode(c); }
  return text.trim();
}

export function monsterComponentDirectionKey(componentDirections) {
  return [...componentDirections].sort(([a], [b]) => a - b).map(([component, direction]) => `${component}.${direction}`).join(',');
}

export function parseMonsterAnimations(bytes, count) {
  const view = bytes instanceof DataView ? bytes : new DataView(bytes.buffer, bytes.byteOffset || 0, bytes.byteLength);
  if (!Number.isInteger(count) || count < 0 || view.byteLength < count * 96) throw new RangeError('truncated monster animation records');
  return Array.from({ length: count }, (_, row) => {
    const at = row * 96;
    return { id: view.getUint32(at, true), classId: view.getUint32(at + 4, true), code: fourcc(view.getUint32(at + 8, true)),
      mode: view.getUint32(at + 12, true), framePrecise: view.getUint32(at + 16, true), frameCountPrecise: view.getUint32(at + 20, true),
      components: Array.from({ length: 16 }, (_, i) => fourcc(view.getUint32(at + 24 + i * 4, true))),
      animSpeed: view.getUint16(at + 88, true), direction: view.getUint8(at + 90),
      hasAnimSequence: (view.getUint8(at + 91) & 0x01) !== 0, supportsDeadMode: (view.getUint8(at + 91) & 0x02) !== 0, inClientRoom: (view.getUint8(at + 91) & 0x04) !== 0, seqMode: view.getUint8(at + 91) >> 4,
      baseWeaponClass: fourcc(view.getUint32(at + 92, true)).toUpperCase() || null };
  });
}

// The native selector chooses policy 1 for MonStats2-supported DD and policy 2
// for the fallback row. Both test #10426 before advancing; policy 2 clamps to
// the final precise frame, while policy 1 leaves the current frame unchanged.
export function advanceMonsterDeadClock(actor, previous = null) {
  if (Math.trunc(Number(actor.mode)) !== 12 || actor.hasAnimSequence !== false || typeof actor.supportsDeadMode !== 'boolean') return null;
  const gameFrame = Number(actor.gameFrame);
  if (!Number.isSafeInteger(gameFrame)) return null;
  const priorEventSequence = previous?.modeEventSequence ?? 0;
  const modeEvents = Array.isArray(actor.modeEvents) ? actor.modeEvents : [];
  const modeEventSequence = modeEvents.reduce((latest, event) => Math.max(latest, Number(event.sequence) || 0), priorEventSequence);
  const terminalReentry = modeEvents.some((event) => event.toMode === 12 && event.sequence > priorEventSequence);
  const frameCountPrecise = Math.max(0, Math.trunc(Number(actor.frameCountPrecise) || 0));
  const rawSpeed = Math.trunc(Number(actor.animSpeed) || 0);
  const speed = (rawSpeed << 16) >> 16;
  if (!previous || previous.mode !== 12 || gameFrame < previous.gameFrame || terminalReentry) {
    return { mode: 12, gameFrame, framePrecise: 0, modeEventSequence };
  }
  let framePrecise = previous.framePrecise;
  for (let tick = previous.gameFrame; tick < gameFrame; tick++) {
    if (frameCountPrecise > 0 && frameCountPrecise <= framePrecise + speed) {
      if (!actor.supportsDeadMode) framePrecise = Math.max(0, frameCountPrecise - 256);
      break;
    }
    framePrecise += speed;
  }
  return { mode: 12, gameFrame, framePrecise, modeEventSequence };
}

// Preserve the native CUnit animation fields at the realm draw boundary. `gameFrame` is the
// shell's count of 25 Hz client-motion ticks, each of which advances this CUnit once.
export function realmMonsterAnimationActor(monster, gameFrame) {
  return { id: monster.guid, classId: monster.classId, code: monster.code, mode: monster.mode,
    direction: monster.direction, components: monster.components, framePrecise: monster.framePrecise,
    frameCountPrecise: monster.frameCountPrecise, animSpeed: monster.animSpeed, gameFrame,
    hasAnimSequence: (monster.animationFlags & 0x01) !== 0, seqMode: monster.animationFlags >> 4,
    supportsDeadMode: (monster.animationFlags & 0x02) !== 0,
    modeEvents: monster.modeEvents ?? [] };
}

// 1.13c D2Common+0x43C30 (D2Common_10884_COMPOSIT_unk): a monster in MONMODE_KNOCKBACK (13) resolves its COF with the GH token -- no KB COF exists.
export function monsterModeToken(mode) { const m = Math.trunc(Number(mode)); return m === 13 ? 'GH' : MODE[m] || 'NU'; }

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
  // Mode 14 (SQ) is a MonSeq sequence: retail draws each step with the sequence's own mode (dwSeqMode), no SQ COF exists.
  const mode = actor.mode === 14 && actor.hasAnimSequence ? actor.seqMode : actor.mode;
  const token = monsterModeToken(mode);
  let cof, weaponClass;
  const usesBaseWeaponClass = mode === 4 || mode === 5;
  // Retail's monster asset list contains only DTHTH/DDHTH death COFs. Keep
  // death selection exact even when the live unit snapshot carries another
  // BaseW, and do not fall through to a non-death class if an asset is absent.
  const deathMode = actor.mode === 0 || actor.mode === 12;
  const exactWeaponClass = usesBaseWeaponClass && typeof actor.baseWeaponClass === 'string' && /^[A-Z0-9]{1,4}$/.test(actor.baseWeaponClass)
    ? actor.baseWeaponClass : null;
  const candidates = deathMode ? ['HTH']
    : exactWeaponClass ? [exactWeaponClass] : ['HTH', '1HS', '1HT', '2HS', '2HT'];
  for (const candidate of candidates) {
    try { cof = decodeOnce(read(`data\\global\\MONSTERS\\${actor.code}\\COF\\${actor.code}${token}${candidate}.COF`), 'cof', decodeCof); weaponClass = candidate; break; }
    catch {}
  }
  if (!cof) throw new Error(`no retail COF for monster ${actor.code} ${token}`);
  // COF priority and DCC block selection are separate native lookups. In particular,
  // some monster layers have a direction count different from the COF and sibling layers.
  const direction = dccDirection(actor.direction, cof.directions), cofFacing = cofDirection(actor.direction, cof.directions);
  const components = new Map(), containers = new Map(), componentDirections = new Map();
  for (const layer of cof.layerRecords) {
    const component = COMPONENT[layer.component], variant = actor.components[layer.component];
    if (!component || !variant) continue;
    const path = `data\\global\\MONSTERS\\${actor.code}\\${component}\\${actor.code}${component}${variant}${token}${layer.weaponClass.toUpperCase()}.DCC`;
    let cel = null, container = null, layerDirection = null;
    try {
      const bytes = read(path);
      layerDirection = dccDirection(actor.direction, bytes[2]);
      cel = decodeOnce(bytes, layerDirection, (b) => decodeDcc(b, { directions: [layerDirection] })); container = 'DCC';
    } catch {}
    if (!cel) { try {
      cel = decodeOnce(read(path.replace(/\.DCC$/, '.DC6')), 'dc6', dc6AsDcc);
      layerDirection = dccDirection(actor.direction, cel.directions);
      container = 'DC6';
    } catch {} }
    if (cel) { components.set(layer.component, cel); containers.set(layer.component, container); componentDirections.set(layer.component, layerDirection); }
  }
  if (!components.size) throw new Error(`no retail DCC components for monster ${actor.code} ${token}/${weaponClass}`);
  // Keep `direction` as the historic COF-count DCC index for API compatibility;
  // the compositor and COF priority table use the explicit `cofDirection` field.
  // The shadow draw evidence needs a plain object, but component directions are fixed for this
  // cached animation. Materialize it here instead of allocating it again for every rendered frame.
  const componentDirectionEvidence = Object.fromEntries(componentDirections);
  const animBox = monsterAnimationBox(components, componentDirections);
  return { cof, components, containers, componentDirections, componentDirectionEvidence,
    componentDirectionKey: monsterComponentDirectionKey(componentDirections), animBox, direction, cofDirection: cofFacing, token };
}

/** `shadow: true` draws the unit's shadow under its sprite first, as retail's draw list orders it. */
export function createMonsterRenderer(read, documentObject = globalThis.document, { shadow = false,
  layerRasterLimit = MONSTER_LAYER_RASTER_ENTRIES, layerRasterByteLimit = MONSTER_LAYER_RASTER_BYTES } = {}) {
  const failures = new Set(), palettes = new Map(), blenders = new Map(), highlights = new Map();
  // PERF: bounded, and a raster is keyed by its composed plane, which facings of one DCC direction
  // share -- not by the 0..63 facing, which kept a canvas per facing per frame for the whole session.
  const animations = createRenderCache(MONSTER_ANIMATION_ENTRIES);
  const files = createRenderCache(MONSTER_FILE_ENTRIES, MONSTER_FILE_BYTES, (bytes) => bytes?.byteLength ?? 0);
  const animationRead = (path) => files.get(path) ?? files.set(path, read(path));
  const rasters = createRenderCache(MONSTER_RASTER_ENTRIES, MONSTER_RASTER_BYTES, (canvas) => canvas.width * canvas.height * 4);
  const composedIds = new WeakMap();
  let nextComposedId = 1;
  // Per-layer fallback rasters are immutable for a decoded animation frame and palette. Reuse
  // them across actors and redraws, with a finite budget for long sessions.
  const layerRasters = createRenderCache(layerRasterLimit, layerRasterByteLimit,
    (canvas) => canvas.width * canvas.height * 4);
  const izualGhostClocks = new Map();
  const deadMonsterClocks = new Map();
  const shadows = shadow ? createUnitShadowRenderer(read) : null;
  const blenderFor = (act0) => { if (!blenders.has(act0)) { let b = null; try { b = createBlender(read(`data\\global\\palette\\${PALETTE[act0]}\\pal.pl2`), palettes.get(act0)); } catch {} blenders.set(act0, b); } return blenders.get(act0); };
  let lastEvidence = null;
  const palette = (act) => {
    const act0 = Math.min(4, Math.max(0, Math.trunc(Number(act) || 1) - 1));
    if (!palettes.has(act0)) palettes.set(act0, decodeDatPalette(read(`data\\global\\palette\\${PALETTE[act0]}\\pal.dat`)));
    return [act0, palettes.get(act0)];
  };
  return {
    // Session teardown can reuse actor IDs; their client-local clocks belong to the old session.
    resetSession() { resetIzualGhostS1Clock(izualGhostClocks); deadMonsterClocks.clear(); },
    // `highlight`: retail's DrawMode 7, the hovered unit (V5, unit-highlight.mjs) -- the cel's colours through the measured rule.
    draw(ctx, x, y, actor, act, highlight = false) {
      // Izual's S1 uses its own CUnit clock; the server frame epoch is not a cel index.
      const clock = advanceIzualGhostS1Clock(actor, izualGhostClocks.get(actor.id));
      if (clock) izualGhostClocks.set(actor.id, clock);
      else izualGhostClocks.delete(actor.id);
      // Attack COFs depend on BaseW. Keep decoded assets, rasters and failed loads
      // separate when the same token/components arrive with another class.
      const attackClass = actor.mode === 4 || actor.mode === 5 || actor.mode === 14 ? actor.baseWeaponClass ?? '' : '';
      const baseKey = `${actor.code}:${actor.mode}:${actor.seqMode ?? ''}:${actor.direction}:${actor.components.join(',')}:${attackClass}`;
      if (failures.has(baseKey)) return null;
      let animation = animations.get(baseKey);
      if (!animation) { try { animation = loadMonsterAnimation(animationRead, actor); animations.set(baseKey, animation); } catch { failures.add(baseKey); return null; } }
      const count = animation.cof.framesPerDirection;
      const deadClock = advanceMonsterDeadClock(actor, deadMonsterClocks.get(actor.id));
      if (deadClock) deadMonsterClocks.set(actor.id, deadClock);
      else deadMonsterClocks.delete(actor.id);
      const framePrecise = deadClock?.framePrecise ?? clock?.framePrecise ?? actor.framePrecise;
      const whole = framePrecise >>> 8;
      const frame = (whole >= count && actor.frameCountPrecise && framePrecise >= actor.frameCountPrecise ? count - 1 : whole % count) >>> 0;
      let composed;
      // PERF (F198): a composed frame is a pure function of the (cached) animation and the frame; composed once, its identity hash and opaque count with it.
      const componentDirectionKey = animation.componentDirectionKey ?? monsterComponentDirectionKey(animation.componentDirections);
      try { composed = composeCofFrame(animation.cof, animation.cofDirection, frame, animation.components, animation.componentDirections, componentDirectionKey); }
      catch (e) { failures.add(baseKey); console.warn(`monster ${actor.code} mode ${actor.mode} dir ${actor.direction} frame ${frame}: ${e.message}`); return null; }   // a COF/DCC frame-count mismatch must not stop the frame loop
      let composedId = composedIds.get(composed);
      if (!composedId) composedIds.set(composed, composedId = nextComposedId++);
      const [act0, pal] = palette(act), key = `${composedId}:${act0}${highlight ? ':hl' : ''}`;
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
        { cof: animation.cof, components: animation.components, direction: animation.cofDirection, dccDirection: animation.componentDirections, frame },
        { kind: 'monster', id: actor.id, classId: actor.classId, code: actor.code, token: animation.token, direction: animation.cofDirection,
          componentDirections: animation.componentDirectionEvidence, frame, act });
      // A COF layer marked transparent draws through its draw effect (Izual's ghost: both layers, effect 3
      // = PL2 AdditiveBlend, the draw mode retail's own frame buffers pin for mode 3). Such a unit is
      // drawn layer by layer in the COF's priority order; effects other than 3/4 stay opaque (declared).
      const blended = monsterCofHasLayerBlend(animation.cof);
      if (blended) {
        const blend = blenderFor(act0);
        for (const component of animation.cof.priority[animation.cofDirection][frame]) {
          const dcc = animation.components.get(component); if (!dcc) continue;
          const layer = cofLayerRecord(animation.cof, component);
          const one = composeCofFrame(animation.cof, animation.cofDirection, frame, new Map([[component, dcc]]), animation.componentDirections, componentDirectionKey);
          if (!one.width || !one.height) continue;
          const left = Math.round(x + one.xOffset), top = Math.round(y + one.yOffset);
          if (layer && layer.transparent && (layer.drawEffect === 3 || layer.drawEffect === 4) && blend)
            blend(ctx, { width: one.width, height: one.height, indices: one.indexed, opaque: one.opaque }, left, top, layer.drawEffect);
          else {
            const layerKey = `${key}:layer:${component}`;
            let cv = layerRasters.get(layerKey);
            if (!cv) {
              cv = documentObject.createElement('canvas'); cv.width = one.width; cv.height = one.height;
              cv.getContext('2d').putImageData(new ImageData(indexedToRgba(one.indexed, one.opaque, pal), one.width, one.height), 0, 0);
              layerRasters.set(layerKey, cv);
            }
            ctx.drawImage(cv, left, top);
          }
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
          frame, frames: animation.cof.framesPerDirection, uid: `1,${actor.id}`, dir: actor.direction, token: animation.token,
          x: Math.round(x), y: Math.round(y), mode: highlight ? 7 : 5 });
      }
      if (composed.pixelHash === undefined) { let h = 0x811c9dc5; for (const value of composed.indexed) { h ^= value; h = Math.imul(h, 0x01000193); } composed.pixelHash = h; composed.opaqueCount = composed.opaque.reduce((a, v) => a + v, 0); }
      const pixelHash = composed.pixelHash;
      // `directions` and `frames` are the COF's own lengths, so a caller comparing the drawn identity
      // against the engine's requested one (s278) can check the direction and frame against the
      // sequence they belong to without re-decoding the COF.
      lastEvidence = { id: actor.id, classId: actor.classId, code: actor.code, token: animation.token,
        // `direction` remains the historical COF-count DCC index for callers that need compatibility;
        // the actual per-layer DCC slots are `drawnComponents[].direction`.
        direction: animation.direction, engineDirection: actor.direction, cofDirection: animation.cofDirection, directions: animation.cof.directions, frame, frames: count,
        components: animation.components.size, width: composed.width,
        componentDirections: Object.fromEntries(animation.componentDirections),
        drawnComponents: Array.from(animation.components.keys(), (slot) => ({ slot,
          requestedVariant: actor.components[slot], container: animation.containers.get(slot),
          direction: animation.componentDirections.get(slot), directions: animation.components.get(slot)?.directions })),
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
        animBox: animation.animBox,
        height: composed.height, opaque: composed.opaqueCount, pixelHash: pixelHash >>> 0, mode: highlight ? 7 : 5 };
      return lastEvidence;
    },
    get lastEvidence() { return lastEvidence && { ...lastEvidence }; },
    get decodedAnimations() { return animations.size; }, get failures() { return failures.size; },
    get rasterizedLayers() { return layerRasters.size; }, get rasterizedLayerBytes() { return layerRasters.retainedBytes; },
  };
}
