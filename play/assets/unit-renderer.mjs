import { decodeDatPalette } from './d2-formats.mjs';
import { composeCofFrame, decodeCof, decodeDcc } from './unit-formats.mjs';
import { createPlayerCompositeResolver } from './player-composite.mjs';
import { spriteMetrics } from './sprite-metrics.mjs';
import { createUnitShadowRenderer } from './unit-shadow-renderer.mjs';
import { createRenderCache } from './render-cache.mjs';

const COMPONENT = ['HD', 'TR', 'LG', 'RA', 'LA', 'RH', 'LH', 'SH', 'S1', 'S2', 'S3', 'S4', 'S5', 'S6', 'S7', 'S8'];
// CharStats.txt's fixed seven-row order.  The starting HTH appearance uses the
// same unarmoured component codes for each player token; equipment composition
// will replace these bases as the inventory surface becomes authoritative.
const PLAYER_CLASS = Object.freeze([
  { token: 'AM', name: 'Amazon' }, { token: 'SO', name: 'Sorceress' },
  { token: 'NE', name: 'Necromancer' }, { token: 'PA', name: 'Paladin' },
  { token: 'BA', name: 'Barbarian' }, { token: 'DZ', name: 'Druid' },
  { token: 'AI', name: 'Assassin' },
]);
const START_BASE = Object.freeze({ 0: 'LIT', 1: 'LIT', 2: 'LIT', 3: 'LIT', 4: 'LIT', 8: 'LIT', 9: 'LIT' });
const MODE = Object.freeze(['DT', 'NU', 'WL', 'RN', 'GH', 'TN', 'TW', 'A1', 'A2', 'BL', 'SC', 'TH', 'KK', 'S1', 'S2', 'S3', 'S4', 'DD']);
const PALETTE = ['ACT1', 'ACT2', 'ACT3', 'ACT4', 'ACT5'];
const PLAYER_DECODE_CACHE_ENTRIES = 128;
const PLAYER_DECODE_CACHE_BYTES = 64 * 1024 * 1024;
const PLAYER_RASTER_CACHE_ENTRIES = 256;
const PLAYER_RASTER_CACHE_BYTES = 16 * 1024 * 1024;
const DIR8 = [4,4,4,4,0,0,0,0,0,0,0,0,5,5,5,5,5,5,5,5,1,1,1,1,1,1,1,1,6,6,6,6,6,6,6,6,2,2,2,2,2,2,2,2,7,7,7,7,7,7,7,7,3,3,3,3,3,3,3,3,4,4,4,4];
const DIR16 = [4,4,8,8,8,8,0,0,0,0,9,9,9,9,5,5,5,5,10,10,10,10,1,1,1,1,11,11,11,11,6,6,6,6,12,12,12,12,2,2,2,2,13,13,13,13,7,7,7,7,14,14,14,14,3,3,3,3,15,15,15,15,4,4];
const DIR32 = [4,16,16,8,8,17,17,0,0,18,18,9,9,19,19,5,5,20,20,10,10,21,21,1,1,22,22,11,11,23,23,6,6,24,24,12,12,25,25,2,2,26,26,13,13,27,27,7,7,28,28,14,14,29,29,3,3,30,30,15,15,31,31,4];
const DIR64 = [4,32,16,33,8,34,17,35,0,36,18,37,9,38,19,39,5,40,20,41,10,42,21,43,1,44,22,45,11,46,23,47,6,48,24,49,12,50,25,51,2,52,26,53,13,54,27,55,7,56,28,57,14,58,29,59,3,60,30,61,15,62,31,63];

// D2Common+0x43C30: PLRMODE_SEQUENCE (18) and PLRMODE_KNOCKBACK (19) resolve with the GH token.
export function playerModeToken(mode) { return Number(mode) === 18 || Number(mode) === 19 ? 'GH' : MODE[Number(mode)] || 'NU'; }

// A PLRMODE_SEQUENCE (18) unit draws the mode its current sequence frame names (the record's nMode) at that
// frame's index (nFrame), not the GH stand-in. Without a published seqMode the unit keeps the stand-in.
export function drawnPlayerState(state = {}) {
  if (Number(state.mode) !== 18 || !Number.isInteger(state.seqMode)) return state;
  return { ...state, mode: state.seqMode, framePrecise: state.seqFrame * 256 };
}

/** Attach the current frame's provenance and DCC metadata to each loaded draw layer. */
export function playerDrawnEvidence(layers, source, directions, components, frame) {
  return layers.map((layer) => {
    const componentId = layer.componentId ?? COMPONENT.indexOf(layer.component), dcc = components.get(componentId);
    return { ...layer, componentId, source: source[componentId] || null,
      direction: directions.get(componentId), directions: dcc?.directions,
      frame: Math.min(frame, (dcc?.framesPerDirection ?? 1) - 1),
      framesPerDirection: dcc?.framesPerDirection };
  });
}

export function playerClassInfo(playerClass) {
  return PLAYER_CLASS[Math.min(PLAYER_CLASS.length - 1, Math.max(0, Math.trunc(Number(playerClass) || 0)))];
}

export function dccDirection(direction64, directions) {
  const direction = ((Math.trunc(Number(direction64) || 0) % 64) + 64) % 64;
  if (directions === 16) return DIR16[direction];
  if (directions === 8) return DIR8[direction];
  if (directions === 4) return Math.trunc(((direction + 8) & 63) / 16);
  if (directions === 1 || directions === 2) return 0;
  if (directions === 32) return DIR32[direction];
  if (directions === 64) return DIR64[direction];
  return Math.min(directions - 1, Math.trunc(direction * directions / 64));
}

// COF priority uses D2CMP's explicit 64-facing lookup. Its sectors are centered
// on each COF direction and wrap the final sector back to zero. DCC blocks use
// a separate direction order, rotated relative to COF.
export function cofDirection(direction64, directions) {
  const direction = ((Math.trunc(Number(direction64) || 0) % 64) + 64) % 64;
  if (![1, 2, 4, 8, 16, 32, 64].includes(directions)) return 0;
  return Math.trunc((direction * directions + 32) / 64) % directions;
}

export function playerRasterCacheKey({ classToken, token, weaponClass, variants, dccDirection, componentDirections, cofDirection, frame, act0 }) {
  const directionKey = componentDirections || dccDirection;
  return `${classToken}:${token}:${weaponClass}:${variants}:${cofDirection}:${directionKey}:${frame}:${act0}`;
}

// DCC pixels dominate a decoded player animation's retained data. Include their indexed and
// opacity planes, plus fixed allowances for retained JS frame/direction records and the COF tables.
// This is a stable estimate for cache policy, not a measurement of the JS engine's heap overhead.
function playerAnimationRetainedBytes(animation) {
  let bytes = 1024 + (animation.cof.events?.byteLength || 0) + animation.cof.layerRecords.length * 128;
  for (const direction of animation.cof.priority) {
    bytes += 64;
    for (const frame of direction) bytes += 64 + frame.length * 4;
  }
  for (const dcc of animation.components.values()) {
    bytes += 128 + dcc.framesPerDirection * 96;
    for (const direction of dcc.directionRecords) {
      if (!direction) continue;
      // The decoder retains optionalData as a byte array on each frame and a JS
      // number array for the palette bitmap. Both are live for as long as this
      // direction record is cached, even though raster composition only reads
      // the indexed and opacity planes.
      bytes += 128 + direction.paletteEntries.length * 8;
      for (const frame of direction.frames) bytes += frame.indexed.byteLength + frame.opaque.byteLength + (frame.optionalData?.byteLength || 0) + 96;
    }
  }
  return bytes;
}

export function animationFrame(cof, state = {}) {
  if (!cof?.framesPerDirection) return 0;
  const precise = Number(state.framePrecise);
  if (Number.isFinite(precise) && precise >= 0) return (Math.trunc(precise) >>> 8) % cof.framesPerDirection;
  const tick = Math.max(0, Math.trunc(Number(state.tick) || 0));
  return Math.trunc(tick * cof.speed / 256) % cof.framesPerDirection;
}

const LITTLE_ENDIAN = new Uint8Array(new Uint32Array([0x01020304]).buffer)[0] === 4;
const packedPaletteCache = new WeakMap();

function packedPaletteColors(palette) {
  let entry = packedPaletteCache.get(palette);
  if (entry) {
    // Palette inputs are mutable typed arrays. Keep the public helper's behavior
    // correct if a caller edits one after its packed lookup has been cached.
    let same = true;
    // Word-compare (256 loads, not 1024) when the view is 4-byte aligned; same answer.
    if (palette.byteOffset % 4 === 0) {
      const now = new Uint32Array(palette.buffer, palette.byteOffset, 256), was = entry.words;
      for (let i = 0; i < 256; i++) if (now[i] !== was[i]) { same = false; break; }
    } else for (let i = 0; i < palette.length; i++) if (palette[i] !== entry.snapshot[i]) { same = false; break; }
    if (same) return entry.colors;
  }
  const colors = new Uint32Array(512);
  for (let index = 0; index < 256; index++) {
    const source = index * 4;
    const rgb = palette[source] | (palette[source + 1] << 8) | (palette[source + 2] << 16);
    colors[index] = rgb;
    colors[index + 256] = rgb | 0xff000000;
  }
  const snapshot = palette.slice();
  packedPaletteCache.set(palette, { snapshot, words: new Uint32Array(snapshot.buffer), colors });
  return colors;
}

export function indexedToRgba(indexed, opaque, palette, indexMap = null, mapTransparent = true) {
  if (!(indexed instanceof Uint8Array) || !(opaque instanceof Uint8Array) || indexed.length !== opaque.length) throw new TypeError('indexed sprite planes must match');
  if (!(palette instanceof Uint8Array) || palette.length !== 1024) throw new TypeError('sprite palette must contain 256 RGBA entries');
  if (indexMap !== null && (!(indexMap instanceof Uint8Array) || indexMap.length !== 256)) throw new TypeError('sprite index map must contain 256 entries');
  const rgba = new Uint8ClampedArray(indexed.length * 4);
  // The retail palettes and indexed planes are byte values. On little-endian hosts
  // a packed word stores the same RGBA bytes while avoiding four clamped stores for
  // every pixel. Keep the byte path for other architectures.
  // The offline benchmark covers a 38,400-pixel in-memory sprite. The 512-pixel
  // cutoff is a conservative small-cel guard; its crossover is not measured.
  if (LITTLE_ENDIAN && indexed.length > 512) {
    const words = new Uint32Array(rgba.buffer, rgba.byteOffset, indexed.length);
    const colors = packedPaletteColors(palette);
    if (indexMap && !mapTransparent) {
      for (let i = 0; i < indexed.length; i++) {
        const alpha = opaque[i] ? 256 : 0, index = indexed[i];
        words[i] = colors[(alpha ? indexMap[index] : index) + alpha];
      }
    } else if (indexMap) {
      for (let i = 0; i < indexed.length; i++) words[i] = colors[indexMap[indexed[i]] + (opaque[i] ? 256 : 0)];
    } else {
      for (let i = 0; i < indexed.length; i++) words[i] = colors[indexed[i] + (opaque[i] ? 256 : 0)];
    }
    return rgba;
  }
  for (let i = 0; i < indexed.length; i++) {
    const index = indexMap && (mapTransparent || opaque[i]) ? indexMap[indexed[i]] : indexed[i];
    const source = index * 4, target = i * 4;
    rgba[target] = palette[source]; rgba[target + 1] = palette[source + 1]; rgba[target + 2] = palette[source + 2];
    rgba[target + 3] = opaque[i] ? 255 : 0;
  }
  return rgba;
}

export function loadPlayerAnimation(read, playerClass, mode, { headOnly = false, direction64 = null, composition = null } = {}) {
  const player = playerClassInfo(playerClass);
  const token = playerModeToken(mode);
  // Death COFs are the unarmed poses: 1.13c ships BADTHTH/BADDHTH, not
  // weapon-class variants such as BADT1HS or BADD1HS.  A worn weapon still
  // selects the live-mode COF, but it must not leak into DT/DD's file name.
  const weaponClass = (token === 'DT' || token === 'DD' ? 'hth' : (composition?.weaponClass || 'hth')).toUpperCase();
  const cof = decodeCof(read(`data\\global\\CHARS\\${player.token}\\COF\\${player.token}${token}${weaponClass}.COF`));
  const components = new Map();
  const loadedDirections = new Map();
  const drawn = [];
  for (const layer of cof.layerRecords) {
    if (headOnly && layer.component !== 0) continue;
    const component = COMPONENT[layer.component], base = composition?.components?.[layer.component] || START_BASE[layer.component];
    if (!component || !base) continue;
    const path = `data\\global\\CHARS\\${player.token}\\${component}\\${player.token}${component}${base}${token}${layer.weaponClass.toUpperCase()}.DCC`;
    const bytes = read(path);
    // D2CMP's direction lookup is keyed by this DCC's header count. Most player
    // layers have 16 directions, but a small set of Paladin RH axe layers have 8.
    const layerDirection = direction64 == null ? null : dccDirection(direction64, bytes[2]);
    components.set(layer.component, decodeDcc(bytes, layerDirection == null ? {} : { directions: [layerDirection] }));
    if (layerDirection != null) loadedDirections.set(layer.component, layerDirection);
    drawn.push({ component, componentId: layer.component, variant: base.toLowerCase(), source: composition?.source?.[layer.component] || null, path: path.toLowerCase() });
  }
  // Every component the COF names must load; which ones those are is the COF's business -- the
  // dead pose (DD) is a single torso layer, the whole body one sprite, with no head at all.
  if (!components.size || (headOnly && !components.has(0))) throw new Error(`${player.name} ${token}/${weaponClass} animation loaded none of its retail components`);
  const componentIds = [...components.keys()].sort((a, b) => a - b);
  return { playerClass: PLAYER_CLASS.indexOf(player), classToken: player.token, token, weaponClass: weaponClass.toLowerCase(), cof, components, componentIds, drawn,
    direction64, loadedDirections };
}

export function loadAmazonAnimation(read, mode, options = {}) { return loadPlayerAnimation(read, 0, mode, options); }

export function selectAmazonFrame(animation, state = {}) {
  // Keep the historic scalar as the ordinary 16-direction result for compatibility
  // and diagnostics. Raster composition uses a separate direction per DCC layer.
  const direction = dccDirection(state.direction, animation.cof.directions);
  const componentDirections = new Map();
  for (const [component, dcc] of animation.components)
    componentDirections.set(component, dccDirection(state.direction, dcc.directions));
  const componentIds = animation.componentIds || [...componentDirections.keys()].sort((a, b) => a - b);
  const componentDirectionKey = componentIds.map((component) => `${component}.${componentDirections.get(component)}`).join(',');
  const cofFacing = cofDirection(state.direction, animation.cof.directions);
  const frame = animationFrame(animation.cof, state);
  const cofPriority = animation.cof.priority[cofFacing][frame];
  const layerDrawOrder = cofPriority.filter((component) => animation.components.get(component));
  return { direction, cofDirection: cofFacing, frame,
    cofPriority: [...cofPriority], layerDrawOrder,
    componentDirections, componentDirectionKey,
    composed: composeCofFrame(animation.cof, cofFacing, frame, animation.components, componentDirections, componentDirectionKey) };
}

export const selectPlayerFrame = selectAmazonFrame;

/**
 * Synchronous retail player sprite provider; decoded modes and rasterized frames are cached.
 * `shadow: true` draws the unit's shadow under the sprite first, the way retail's draw list does
 * (`shadow dcc:` immediately before the matching `cel dcc:`). It is off by default: the character
 * selection screen draws the same sprites, and retail casts no shadow there.
 */
export function createPlayerRenderer(read, documentObject = globalThis.document, { shadow = false, decodeCacheByteLimit = PLAYER_DECODE_CACHE_BYTES } = {}) {
  const animations = createRenderCache(PLAYER_DECODE_CACHE_ENTRIES, decodeCacheByteLimit, playerAnimationRetainedBytes);
  const palettes = new Map();
  const rasters = createRenderCache(PLAYER_RASTER_CACHE_ENTRIES, PLAYER_RASTER_CACHE_BYTES,
    (canvas) => canvas.width * canvas.height * 4);
  const composite = createPlayerCompositeResolver(read);
  const shadows = shadow ? createUnitShadowRenderer(read) : null;
  let lastEvidence = null;
  function animation(playerClass, mode, direction64, composition) {
    const player = playerClassInfo(playerClass), token = playerModeToken(mode);
    const variants = COMPONENT.map((_, index) => composition.components[index] || '').join(',');
    const key = `${player.token}:${token}:${composition.weaponClass}:${variants}:${direction64}`;
    let value = animations.get(key); if (!value) { value = loadPlayerAnimation(read, playerClass, mode, { direction64, composition }); animations.set(key, value); }
    return value;
  }
  function palette(act) {
    const act0 = Math.min(4, Math.max(0, Math.trunc(Number(act) || 1) - 1));
    let value = palettes.get(act0);
    if (!value) { value = decodeDatPalette(read(`data\\global\\palette\\${PALETTE[act0]}\\pal.dat`)); palettes.set(act0, value); }
    return { act0, value };
  }
  return {
    draw(ctx, x, y, input = {}) {
      const state = drawnPlayerState(input);
      // DT/DD use the native unarmed COF even when a corpse record carries no live weapon class.
      // Resolve that mode before composition validation, as loadPlayerAnimation does below.
      const token = playerModeToken(state.mode);
      const composition = composite.resolve(state.items, START_BASE,
        token === 'DT' || token === 'DD' ? 'hth' : state.weaponClass, state.primaryWeaponId);
      const activeAnimation = animation(state.playerClass, state.mode, state.direction, composition);
      const selected = selectAmazonFrame(activeAnimation, state), pal = palette(state.act);
      // Decoded DCC pixels can be cached by variant, but their provenance cannot:
      // an Armour.txt-equivalent swap retains the same raster and changes the
      // source item the engine currently reports as worn.
      const drawn = playerDrawnEvidence(activeAnimation.drawn, composition.source,
        selected.componentDirections, activeAnimation.components, selected.frame);
      const key = playerRasterCacheKey({ classToken: activeAnimation.classToken, token: playerModeToken(state.mode),
        weaponClass: activeAnimation.weaponClass, variants: activeAnimation.drawn.map((layer) => layer.variant).join(','),
        cofDirection: selected.cofDirection, dccDirection: selected.direction, componentDirections: selected.componentDirectionKey,
        frame: selected.frame, act0: pal.act0 });
      let canvas = rasters.get(key);
      if (!canvas) {
        const { composed } = selected;
        canvas = documentObject.createElement('canvas'); canvas.width = composed.width; canvas.height = composed.height;
        if (canvas.width && canvas.height) {
          const image = new ImageData(indexedToRgba(composed.indexed, composed.opaque, pal.value), canvas.width, canvas.height);
          canvas.getContext('2d').putImageData(image, 0, 0);
        }
        rasters.set(key, canvas);
      }
      const { composed } = selected;
      shadows?.draw(ctx, Math.round(x), Math.round(y),
        { cof: activeAnimation.cof, components: activeAnimation.components, direction: selected.cofDirection,
          dccDirection: selected.componentDirections, frame: selected.frame },
        { kind: 'player', classToken: activeAnimation.classToken, token: playerModeToken(state.mode), direction: selected.direction,
          componentDirections: Object.fromEntries(selected.componentDirections), frame: selected.frame, act: state.act });
      if (canvas.width && canvas.height) ctx.drawImage(canvas, Math.round(x + composed.xOffset), Math.round(y + composed.yOffset));
      // `direction` and `directions` retain the ordinary COF-count scalar for compatibility. Actual
      // per-blit identities are in `drawn` and `componentDirections`, since native D2CMP maps every
      // DCC against its own header count. `engineDirection` is the facing from this same snapshot.
      lastEvidence = { playerClass: activeAnimation.playerClass, classToken: activeAnimation.classToken, token: playerModeToken(state.mode), direction: selected.direction,
        loadDirections: Object.fromEntries(activeAnimation.loadedDirections), componentDirections: Object.fromEntries(selected.componentDirections),
        cofDirection: selected.cofDirection, cofPriority: [...selected.cofPriority],
        layerDrawOrder: [...selected.layerDrawOrder], loadedComponents: [...activeAnimation.components.keys()], frame: selected.frame,
        engineDirection: state.direction, directions: activeAnimation.cof.directions,
        width: composed.width, height: composed.height, opaque: spriteMetrics(composed, null, composed.opaque).opaque,
        components: activeAnimation.components.size, weaponClass: activeAnimation.weaponClass,
        primaryWeaponId: composition.primaryWeaponId,
        handAssignments: composition.handAssignments.map((assignment) => ({ ...assignment })),
        drawn };
      if (Array.isArray(globalThis.__d2DrawList)) for (const layer of [...drawn].sort((a, b) => selected.layerDrawOrder.indexOf(a.componentId) - selected.layerDrawOrder.indexOf(b.componentId))) globalThis.__d2DrawList.push({ op: 'unit', ...layer, dir: state.direction, frame: selected.frame, x: Math.round(x), y: Math.round(y), mode: 5 });
      return lastEvidence;
    },
    get decodedModes() { return animations.size; },
    get decodedBytes() { return animations.retainedBytes; },
    get rasterizedFrames() { return rasters.size; },
    get lastEvidence() { return lastEvidence && { ...lastEvidence }; },
  };
}

/** Compatibility entry point for existing Amazon-only callers and evidence. */
export function createAmazonPlayerRenderer(read, documentObject = globalThis.document) { return createPlayerRenderer(read, documentObject); }
