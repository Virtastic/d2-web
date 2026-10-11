// Object labels come from the engine's loaded retail Objects.txt rows.  The D2MOO table bridge
// exposes that exact table, whose `wszName` field has already passed through retail's string
// reference resolver; this keeps JS from maintaining a second object-name table.
const OBJECT_RECORD_SIZE = 448;
const OBJECT_NAME_OFFSET = 0x40;
const OBJECT_NAME_CHARS = 64;
const OBJECT_NAME_POSITION_OFFSET = 0x168;
const names = new WeakMap();

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
export function drawObjectHoverLabel(ctx, target, font, projectStatic, recordPrim, paintGlyphs = true) {
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
  ctx.save(); ctx.fillStyle = 'rgba(0,0,0,.5)';
  ctx.fillRect(box.x, box.y, box.width, box.height); ctx.restore();
  recordPrim('box', box.x, box.y, box.x + box.width, box.y + box.height, 0, 2);
  if (paintGlyphs) font.drawAnchored(ctx, label, box.x + 4, glyphY,
    (glyph) => glyphs.push(glyph));
  return { ...target, requested: label, font: fontPath, glyphs, storedY, nativeTextHeight: 16,
    nameSource: { table: 'objects', row: target.classId, field: 'wszName' },
    x, y: glyphY, box, placement: chest ? 'chest-title' : 'above-object' };
}
