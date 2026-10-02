// Object labels come from the engine's loaded retail Objects.txt rows.  The D2MOO table bridge
// exposes that exact table, whose `wszName` field has already passed through retail's string
// reference resolver; this keeps JS from maintaining a second object-name table.
const OBJECT_RECORD_SIZE = 448;
const OBJECT_NAME_OFFSET = 0x40;
const OBJECT_NAME_CHARS = 64;
const names = new WeakMap();

export function objectRetailName(Module, classId) {
  if (!Number.isInteger(classId) || classId < 0 || !Module?._d2_datatbl) return '';
  let cache = names.get(Module);
  if (!cache) { cache = new Map(); names.set(Module, cache); }
  if (cache.has(classId)) return cache.get(classId);
  const table = Module.ccall('d2_datatbl', 'string', ['string'], ['objects']);
  const match = /^(\d+):(\d+)$/.exec(table || '');
  if (!match) return '';
  const base = Number(match[1]), count = Number(match[2]);
  if (!Number.isSafeInteger(base) || classId >= count) return '';
  const at = base + classId * OBJECT_RECORD_SIZE + OBJECT_NAME_OFFSET;
  if (!Number.isSafeInteger(at) || at < 0 || at + OBJECT_NAME_CHARS * 2 > Module.HEAPU8.length) return '';
  const bytes = Module.HEAPU8.slice(at, at + OBJECT_NAME_CHARS * 2);
  const name = new TextDecoder('utf-16le').decode(bytes).split('\0', 1)[0];
  const result = name.trim(); cache.set(classId, result); return result;
}

// Retail's captured Rogue Encampment stash hover (issue 6, frame 0, (268,226)) draws a colour-0,
// mode-2 16px plate at x-centre and 54px above the object's anchor; Font16 glyphs use mode 5,
// baseline box-bottom - 3. The scene-specific frame identity is kept in s2201.
export function drawObjectHoverLabel(ctx, target, font, projectStatic, recordPrim, paintGlyphs = true) {
  if (!target || !font) return null;
  // Retail 1.13c's ordinary cy chest uses a class-specific title rather than Objects.txt's
  // mixed-case row name, and puts the plate 20px higher than the issue 6 stash plate. Measured from
  // paired frame-0 hover/away captures; keep this keyed to the exact retail class.
  const chest = target.classId === 240 && String(target.token).toLowerCase() === 'cy';
  const label = chest ? 'CHEST' : target.label;
  const width = font.measure(label), [sx, sy] = projectStatic(target.x, target.y);
  const x = Math.round(sx), box = {
    x: Math.round(x - width / 2 - 4), y: Math.round(sy) - (chest ? 74 : 54),
    width: width + 8, height: 16,
  };
  ctx.save(); ctx.fillStyle = 'rgba(0,0,0,.5)';
  ctx.fillRect(box.x, box.y, box.width, box.height); ctx.restore();
  recordPrim('box', box.x, box.y, box.x + box.width, box.y + box.height, 0, 2);
  const glyphs = [];
  if (paintGlyphs) font.drawAnchored(ctx, label, box.x + 4, box.y + 13,
    (glyph) => glyphs.push(glyph));
  return { ...target, requested: label, font: 'data\\local\\font\\latin\\font16.dc6', glyphs,
    nameSource: { table: 'objects', row: target.classId, field: 'wszName' },
    x, y: box.y + 13, box, placement: chest ? 'chest-title' : 'above-object' };
}
