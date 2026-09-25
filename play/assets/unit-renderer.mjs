import { decodeDatPalette } from './d2-formats.mjs';
import { composeCofFrame, decodeCof, decodeDcc } from './unit-formats.mjs';

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
const START_BASE = Object.freeze({ 0: 'BHM', 1: 'LIT', 2: 'LIT', 3: 'LIT', 4: 'LIT', 8: 'LIT', 9: 'LIT' });
const MODE = Object.freeze(['DT', 'NU', 'WL', 'RN', 'GH', 'TN', 'TW', 'A1', 'A2', 'BL', 'SC', 'TH', 'KK', 'S1', 'S2', 'S3', 'S4', 'DD']);
const PALETTE = ['ACT1', 'ACT2', 'ACT3', 'ACT4', 'ACT5'];
const DIR8 = [4,4,4,4,0,0,0,0,0,0,0,0,5,5,5,5,5,5,5,5,1,1,1,1,1,1,1,1,6,6,6,6,6,6,6,6,2,2,2,2,2,2,2,2,7,7,7,7,7,7,7,7,3,3,3,3,3,3,3,3,4,4,4,4];
const DIR16 = [4,4,8,8,8,8,0,0,0,0,9,9,9,9,5,5,5,5,10,10,10,10,1,1,1,1,11,11,11,11,6,6,6,6,12,12,12,12,2,2,2,2,13,13,13,13,7,7,7,7,14,14,14,14,3,3,3,3,15,15,15,15,4,4];

export function playerModeToken(mode) { return MODE[Number(mode)] || 'NU'; }

export function playerClassInfo(playerClass) {
  return PLAYER_CLASS[Math.min(PLAYER_CLASS.length - 1, Math.max(0, Math.trunc(Number(playerClass) || 0)))];
}

export function dccDirection(direction64, directions) {
  const direction = ((Math.trunc(Number(direction64) || 0) % 64) + 64) % 64;
  if (directions === 16) return DIR16[direction];
  if (directions === 8) return DIR8[direction];
  if (directions === 4) return Math.trunc(((direction + 8) & 63) / 16);
  if (directions === 1) return 0;
  return Math.min(directions - 1, Math.trunc(direction * directions / 64));
}

export function animationFrame(cof, state = {}) {
  if (!cof?.framesPerDirection) return 0;
  const precise = Number(state.framePrecise);
  if (Number.isFinite(precise) && precise >= 0) return (Math.trunc(precise) >>> 8) % cof.framesPerDirection;
  const tick = Math.max(0, Math.trunc(Number(state.tick) || 0));
  return Math.trunc(tick * cof.speed / 256) % cof.framesPerDirection;
}

export function indexedToRgba(indexed, opaque, palette) {
  if (!(indexed instanceof Uint8Array) || !(opaque instanceof Uint8Array) || indexed.length !== opaque.length) throw new TypeError('indexed sprite planes must match');
  if (!(palette instanceof Uint8Array) || palette.length !== 1024) throw new TypeError('sprite palette must contain 256 RGBA entries');
  const rgba = new Uint8ClampedArray(indexed.length * 4);
  for (let i = 0; i < indexed.length; i++) {
    const source = indexed[i] * 4, target = i * 4;
    rgba[target] = palette[source]; rgba[target + 1] = palette[source + 1]; rgba[target + 2] = palette[source + 2];
    rgba[target + 3] = opaque[i] ? 255 : 0;
  }
  return rgba;
}

export function loadPlayerAnimation(read, playerClass, mode, { headOnly = false, direction = null } = {}) {
  const player = playerClassInfo(playerClass);
  const token = playerModeToken(mode);
  const cof = decodeCof(read(`data\\global\\CHARS\\${player.token}\\COF\\${player.token}${token}HTH.COF`));
  const components = new Map();
  for (const layer of cof.layerRecords) {
    if (headOnly && layer.component !== 0) continue;
    const component = COMPONENT[layer.component], base = START_BASE[layer.component];
    if (!component || !base) continue;
    const path = `data\\global\\CHARS\\${player.token}\\${component}\\${player.token}${component}${base}${token}${layer.weaponClass.toUpperCase()}.DCC`;
    components.set(layer.component, decodeDcc(read(path), direction == null ? {} : { directions: [direction] }));
  }
  // Every component the COF names must load; which ones those are is the COF's business -- the
  // dead pose (DD) is a single torso layer, the whole body one sprite, with no head at all.
  if (!components.size || (headOnly && !components.has(0))) throw new Error(`${player.name} ${token}/HTH animation loaded none of its retail components`);
  return { playerClass: PLAYER_CLASS.indexOf(player), classToken: player.token, token, cof, components };
}

export function loadAmazonAnimation(read, mode, options = {}) { return loadPlayerAnimation(read, 0, mode, options); }

export function selectAmazonFrame(animation, state = {}) {
  const direction = dccDirection(state.direction, animation.cof.directions);
  const frame = animationFrame(animation.cof, state);
  return { direction, frame, composed: composeCofFrame(animation.cof, direction, frame, animation.components) };
}

export const selectPlayerFrame = selectAmazonFrame;

/** Synchronous retail player sprite provider; decoded modes and rasterized frames are cached. */
export function createPlayerRenderer(read, documentObject = globalThis.document) {
  const animations = new Map(), palettes = new Map(), rasters = new Map();
  let lastEvidence = null;
  function animation(playerClass, mode, direction64) {
    const player = playerClassInfo(playerClass), token = playerModeToken(mode), direction = dccDirection(direction64, 16), key = `${player.token}:${token}:${direction}`;
    let value = animations.get(key); if (!value) { value = loadPlayerAnimation(read, playerClass, mode, { direction }); animations.set(key, value); }
    return value;
  }
  function palette(act) {
    const act0 = Math.min(4, Math.max(0, Math.trunc(Number(act) || 1) - 1));
    let value = palettes.get(act0);
    if (!value) { value = decodeDatPalette(read(`data\\global\\palette\\${PALETTE[act0]}\\pal.dat`)); palettes.set(act0, value); }
    return { act0, value };
  }
  return {
    draw(ctx, x, y, state = {}) {
      const activeAnimation = animation(state.playerClass, state.mode, state.direction);
      const selected = selectAmazonFrame(activeAnimation, state), pal = palette(state.act);
      const key = `${activeAnimation.classToken}:${playerModeToken(state.mode)}:${selected.direction}:${selected.frame}:${pal.act0}`;
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
      if (canvas.width && canvas.height) ctx.drawImage(canvas, Math.round(x + composed.xOffset), Math.round(y + composed.yOffset));
      lastEvidence = { playerClass: activeAnimation.playerClass, classToken: activeAnimation.classToken, token: playerModeToken(state.mode), direction: selected.direction, frame: selected.frame,
        width: composed.width, height: composed.height, opaque: composed.opaque.reduce((a, value) => a + value, 0),
        components: activeAnimation.components.size };
      return lastEvidence;
    },
    get decodedModes() { return animations.size; },
    get rasterizedFrames() { return rasters.size; },
    get lastEvidence() { return lastEvidence && { ...lastEvidence }; },
  };
}

/** Compatibility entry point for existing Amazon-only callers and evidence. */
export function createAmazonPlayerRenderer(read, documentObject = globalThis.document) { return createPlayerRenderer(read, documentObject); }
