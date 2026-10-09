// Object labels come from the engine's loaded retail Objects.txt rows.  The D2MOO table bridge
// exposes that exact table, whose `wszName` field has already passed through retail's string
// reference resolver; this keeps JS from maintaining a second object-name table.
import { decodeDatPalette } from './d2-formats.mjs';
import { createMpqAssetReader } from './mpq-assets.mjs';
import { PL2_ALPHA_75 } from './frontend.mjs';

const OBJECT_RECORD_SIZE = 448;
const OBJECT_NAME_OFFSET = 0x40;
const OBJECT_NAME_CHARS = 64;
const OBJECT_NAME_POSITION_OFFSET = 0x168;
const names = new WeakMap();
const plateBlenders = new WeakMap();

function objectPlateResources(Module, act) {
  if (!Module || (typeof Module !== 'object' && typeof Module !== 'function')) return null;
  let byAct = plateBlenders.get(Module);
  if (!byAct) { byAct = new Map(); plateBlenders.set(Module, byAct); }
  const key = Math.min(5, Math.max(1, Number(act) | 0));
  if (byAct.has(key)) return byAct.get(key);
  let result = null;
  try {
    const read = createMpqAssetReader(Module), prefix = `data\\global\\palette\\ACT${key}\\`;
    const palette = decodeDatPalette(read(`${prefix}pal.dat`)), pl2 = read(`${prefix}pal.pl2`);
    if (pl2.length >= PL2_ALPHA_75 + 65536) {
      const indices = new Map(); let unique = true;
      for (let i = 0; i < 256; i++) {
        const at = i * 4, rgb = (palette[at] << 16) | (palette[at + 1] << 8) | palette[at + 2];
        if (indices.has(rgb)) { unique = false; break; }
        indices.set(rgb, i);
      }
      if (unique) result = { palette, pl2, indices };
    }
  } catch { /* use the existing fallback when palette resources are unavailable */ }
  byAct.set(key, result);
  return result;
}

function blendObjectHoverPlate(ctx, box, resources) {
  const { palette, pl2, indices } = resources;
  if (!ctx?.getImageData || !ctx?.putImageData) return false;
  const x = Math.trunc(box.x), y = Math.trunc(box.y), width = Math.trunc(box.width), height = Math.trunc(box.height);
  if (width <= 0 || height <= 0 || x < 0 || y < 0 || x + width > ctx.canvas.width || y + height > ctx.canvas.height) return false;
  const image = ctx.getImageData(x, y, width, height), data = image.data, under = new Uint8Array(width * height);
  for (let pixel = 0; pixel < under.length; pixel++) {
    const at = pixel * 4, rgb = (data[at] << 16) | (data[at + 1] << 8) | data[at + 2], index = indices.get(rgb);
    if (data[at + 3] !== 255 || index === undefined) return false;
    under[pixel] = index;
  }
  for (let pixel = 0; pixel < under.length; pixel++) {
    const at = pixel * 4, index = pl2[PL2_ALPHA_75 + under[pixel] * 256];
    data[at] = palette[index * 4]; data[at + 1] = palette[index * 4 + 1];
    data[at + 2] = palette[index * 4 + 2]; data[at + 3] = 255;
  }
  ctx.putImageData(image, x, y);
  return true;
}

function objectTableRow(Module, classId) {
  if (!Number.isInteger(classId) || classId < 0 || !Module?._d2_datatbl) return -1;
  const table = Module.ccall('d2_datatbl', 'string', ['string'], ['objects']);
  const match = /^(\d+):(\d+)$/.exec(table || '');
  if (!match) return -1;
  const base = Number(match[1]), count = Number(match[2]);
  if (!Number.isSafeInteger(base) || classId >= count) return -1;
  const row = base + classId * OBJECT_RECORD_SIZE;
  return Number.isSafeInteger(row) && row >= 0 && row + OBJECT_RECORD_SIZE <= Module.HEAPU8.length ? row : -1;
}

export function objectRetailName(Module, classId) {
  if (!Number.isInteger(classId) || classId < 0 || !Module?._d2_datatbl) return '';
  let cache = names.get(Module);
  if (!cache) { cache = new Map(); names.set(Module, cache); }
  if (cache.has(classId)) return cache.get(classId);
  const row = objectTableRow(Module, classId);
  if (row < 0) return '';
  const at = row + OBJECT_NAME_OFFSET;
  const bytes = Module.HEAPU8.slice(at, at + OBJECT_NAME_CHARS * 2);
  const name = new TextDecoder('utf-16le').decode(bytes).split('\0', 1)[0];
  const result = name.trim(); cache.set(classId, result); return result;
}

// D2Common 1.13c ordinal 11059 returns Objects.txt::NameOffset for an object unit. Read the same
// loaded row here so the browser applies that native per-object vertical offset without a class map.
export function objectRetailNameOffset(Module, classId) {
  const row = objectTableRow(Module, classId), at = row + OBJECT_NAME_POSITION_OFFSET;
  if (row < 0 || at + 4 > Module.HEAPU8.length) return null;
  const bytes = Module.HEAPU8.slice(at, at + 4);
  return new DataView(bytes.buffer, bytes.byteOffset, 4).getInt32(0, true);
}

// D2Client adds Objects.txt::NameOffset to the projected object anchor before queuing its label
// through D2Win 10085. D2Win's deferred 10110 pass applies the same Font16 box layout as NPC labels.
export function drawObjectHoverLabel(ctx, target, font, projectStatic, recordPrim, paintGlyphs = true, Module = null, act = 1) {
  if (!target || !font) return null;
  const glyphs = [], fontPath = 'data\\local\\font\\latin\\font16.dc6';
  if (!Number.isInteger(target.nameOffset)) return { ...target, requested: target.label, font: fontPath,
    glyphs, box: null, placement: 'missing-native-name-offset',
    geometryError: `Objects.txt NameOffset missing for class ${target.classId}` };
  // The ordinary cy chest's localized row name is replaced by retail's uppercase title. The
  // native offset and all plate geometry still come from its Objects.txt row and D2Win path.
  const chest = target.classId === 240 && String(target.token).toLowerCase() === 'cy';
  const label = chest ? 'CHEST' : target.label;
  const width = font.measure(label), [sx, sy] = projectStatic(target.x, target.y);
  const x = Math.round(sx), anchorY = Math.round(sy), boxWidth = width + 8;
  const viewportWidth = ctx.canvas?.width ?? 800, viewportHeight = ctx.canvas?.height ?? 600;
  const left = Math.min(Math.max(x - Math.trunc(boxWidth / 2), 0), Math.max(0, viewportWidth - boxWidth));
  const storedY = anchorY + target.nameOffset;
  const bottom = Math.min(Math.max(11, storedY + 2), viewportHeight - 5);
  const top = bottom - 16, glyphY = bottom - 3;
  const box = { x: left, y: top, width: boxWidth, height: 16 };
  const resources = objectPlateResources(Module, act);
  let plateCompositing = 'canvas-half-alpha-fallback';
  try { if (resources && blendObjectHoverPlate(ctx, box, resources)) plateCompositing = 'PL2 AlphaBlend[2][under][0]'; }
  catch { /* the canvas may be unavailable or clipped; retain the fallback */ }
  if (plateCompositing === 'canvas-half-alpha-fallback') {
    ctx.save(); ctx.fillStyle = 'rgba(0,0,0,.5)';
    ctx.fillRect(box.x, box.y, box.width, box.height); ctx.restore();
  }
  recordPrim('box', box.x, box.y, box.x + box.width, box.y + box.height, 0, 2);
  if (paintGlyphs) font.drawAnchored(ctx, label, box.x + 4, glyphY,
    (glyph) => glyphs.push(glyph));
  return { ...target, requested: label, font: fontPath, glyphs, storedY, nativeTextHeight: 16,
    nameSource: { table: 'objects', row: target.classId, field: 'wszName' },
    x, y: glyphY, box, placement: chest ? 'chest-title' : 'above-object', plateCompositing };
}
