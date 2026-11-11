// A channel user's lobby record, kept between updates the way retail keeps it (the token routine in D2Multi, called again on the same
// record by every flag or statstring update of a user already in the list). Fields the routine does not reach keep their old value:
//   - a non-operator user whose event flags hold 0x08, 0x01, 0x04 or 0x101000, or whose statstring is shorter than four bytes, returns
//     before the product tag is stored, so the tag (and the level, realm) of the previous update stays;
//   - the hardcore word and the character bytes are cleared on every call, so a stale record never shows hardcore.
// A Diablo II statstring with nothing after the tag, or (for a non-operator) with no character, rewrites the tag to UNKN.
// Product tags are the wire order: 'PX2D' expansion, 'VD2D' classic, 'TS2D' shared stash.
import { parseStatstring } from './bnet-statstring.mjs';

export const D2_TAGS = new Set(['PX2D', 'VD2D', 'TS2D']);
// Icon token returned for a non-operator user by product tag (everything else: 7).
const PRODUCT_TOKEN = {
  RTSJ: 0x0d, RATS: 0x0d, RHSS: 0x0d, TAHC: 0x16, LTRD: 0x0a, RHSD: 0x0a, TSTD: 0x0a, NB2W: 0x0f, PXES: 0x0e, PX3W: 0x10, '3RAW': 0x10,
};
// The product line of the hover tip (D2Win's text, English literals: no string id).
const PRODUCT_NAME = {
  RHSS: 'Starcraft Shareware', RTSJ: 'Starcraft', RATS: 'Starcraft', LTRD: 'Diablo', TSTD: 'Diablo', RHSD: 'Diablo Shareware',
  PXES: 'Brood War', PX3W: 'Frozen Throne', NB2W: 'Warcraft II', '3RAW': 'Warcraft III',
};
const TOKEN_NAME = { 0x12: 'Blizzard Rep', 0x14: 'Sysop', 0x13: 'Moderator', 0x17: 'Speaker', 0x15: 'Referee', 0x16: 'Chat', 0x18: 'Open Character' };

const bytesOf = (stat) => (typeof stat === 'string' ? Uint8Array.from(stat, (c) => c.charCodeAt(0) & 255) : stat ?? new Uint8Array(0));

// prev: the record before this update (null for a new user). Returns the record after it.
export function advanceBandRecord(prev, stat, flags) {
  const b = bytesOf(stat);
  let len = b.indexOf(0); if (len < 0) len = b.length;
  const rec = { tag: prev?.tag ?? '', level: prev?.level ?? 0, realm: prev?.realm ?? '', hardcore: false, cls: -1, word: 0, token: 7 };
  const op = Boolean(flags & 2), plain = !op;
  if (op) rec.token = 0x13;
  else if (flags & 8) { rec.token = 0x14; return rec; }
  else if (flags & 1) { rec.token = 0x12; return rec; }
  else if (flags & 4) { rec.token = 0x17; return rec; }
  else if (flags & 0x101000) { rec.token = 0x15; return rec; }
  if (len < 4) return rec;
  const tag = String.fromCharCode(...b.subarray(0, 4));
  rec.tag = tag;
  if (D2_TAGS.has(tag)) {
    if (!b[4]) { rec.tag = 'UNKN'; if (plain) rec.token = 0x18; return rec; }
    const p = parseStatstring(b);
    if (p.equipment.length) { rec.level = p.level; rec.realm = p.realm; rec.hardcore = p.hardcore; rec.cls = p.classId; rec.word = p.flags; return rec; }
    if (plain) { rec.tag = 'UNKN'; rec.token = 7; }
    return rec;
  }
  if (plain) { if (PRODUCT_TOKEN[tag]) rec.token = PRODUCT_TOKEN[tag]; else rec.tag = 'UNKN'; }
  return rec;
}

// The hover tip's lines (D2Win's tip routine). `levelText(n)` / `hardcoreText` are the D2Lang strings; account is the "*account" text.
export function bandTipLines(rec, account, levelText, hardcoreText) {
  const lines = [];
  if (rec.tag === 'PX2D') lines.push('Expansion');
  const levelPart = () => {
    const level = D2_TAGS.has(rec.tag) ? rec.level : 0;
    if (level < 1) return;
    lines.push(levelText(level));
    if (rec.hardcore) lines.push(hardcoreText);
    lines.push(`Realm: ${rec.realm.slice(0, 31)}`);
  };
  if (rec.token === 0x13) { lines.push('Moderator'); levelPart(); }
  else if (TOKEN_NAME[rec.token]) lines.push(TOKEN_NAME[rec.token]);
  else if (D2_TAGS.has(rec.tag)) levelPart();
  else lines.push(PRODUCT_NAME[rec.tag] ?? 'Unknown');
  lines.push(`Account Name: ${account}`);
  return lines;
}
