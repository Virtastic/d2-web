// Battle.net Diablo II statstring parser for the lobby user list.
//
// Layout, as the retail 1.13c lobby reads it (all offsets after the second comma):
//   "PX2D"                 4-byte product tag, first byte first on the wire ("PX2D" expansion, "VD2D" classic)
//   realm "," name ","     comma-terminated text
//   +0  +1                 two header bytes (skipped)
//   +2  .. +12             eleven equipment bytes; a first byte of 0 means "no character data"
//   +13                    class + 1
//   +14 .. +24             eleven colour bytes
//   +25                    level
//   +26 +27                flags, little-endian u16: 0x04 hardcore, 0x08 dead (the lobby treats a character as dead only with both 0x04 and 0x08), 0x20 expansion, 0x40 ladder
//   +28 +29                a little-endian word (act/progress)
//   +30 +31 +32            three bytes (0xFF reads as 0)
// A short or malformed string never throws; it returns the defaults with `valid: false`.

export const CLASS_NAMES = ['Amazon', 'Sorceress', 'Necromancer', 'Paladin', 'Barbarian', 'Druid', 'Assassin'];

const D2_PRODUCTS = new Set(['PX2D', 'VD2D', 'TS2D']);   // expansion, classic, and the shared-stash product tag
const FLAG_HARDCORE = 0x04, FLAG_DEAD = 0x08, FLAG_EXPANSION = 0x20, FLAG_LADDER = 0x40;

function defaults() {
  return {
    valid: false, product: '', realm: '', name: '',
    classId: -1, className: '', level: 0,
    hardcore: false, dead: false, expansion: false, ladder: false,
    flags: 0, equipment: [], colors: [], progress: 0, extra: [0, 0, 0],
  };
}

function toBytes(input) {
  if (typeof input === 'string') {
    const b = new Uint8Array(input.length);
    for (let i = 0; i < input.length; i++) b[i] = input.charCodeAt(i) & 0xFF;
    return b;
  }
  if (input instanceof Uint8Array) return input;
  if (input instanceof ArrayBuffer) return new Uint8Array(input);
  return null;
}

const text = (b, from, to) => { let s = ''; for (let i = from; i < to; i++) s += String.fromCharCode(b[i]); return s; };

export function parseStatstring(input) {
  const out = defaults();
  const b = toBytes(input);
  if (!b || b.length < 4) return out;
  out.product = text(b, 0, 4);
  if (!D2_PRODUCTS.has(out.product)) return out;
  const c1 = b.indexOf(0x2C, 4);
  if (c1 < 0) return out;
  const c2 = b.indexOf(0x2C, c1 + 1);
  if (c2 < 0) return out;
  out.realm = text(b, 4, c1);
  out.name = text(b, c1 + 1, c2);
  const o = c2 + 1;
  if (b.length < o + 14 || b[o + 2] === 0) return out;   // no gear byte: the client treats the character as absent
  out.equipment = Array.from(b.subarray(o + 2, o + 13));
  const classId = b[o + 13] - 1;
  if (classId >= 0 && classId < CLASS_NAMES.length) { out.classId = classId; out.className = CLASS_NAMES[classId]; } else out.classId = classId < 0 ? -1 : classId;
  if (b.length < o + 26) return out;
  out.colors = Array.from(b.subarray(o + 14, o + 25));
  out.level = b[o + 25];
  if (b.length >= o + 28) {
    out.flags = b[o + 26] | (b[o + 27] << 8);
    out.hardcore = (out.flags & FLAG_HARDCORE) !== 0;
    out.dead = (out.flags & FLAG_DEAD) !== 0;
    out.expansion = (out.flags & FLAG_EXPANSION) !== 0;
    out.ladder = (out.flags & FLAG_LADDER) !== 0;
  }
  if (b.length >= o + 30) out.progress = b[o + 28] | (b[o + 29] << 8);
  for (let i = 0; i < 3; i++) { const v = b[o + 30 + i]; out.extra[i] = v === undefined || v === 0xFF ? 0 : v; }
  out.valid = true;
  return out;
}
