// PlugY 11.x shared-stash (.sss) and per-character (.d2x) page files, layout re-derived from reference/PlugY
// SharedSaveFile.cpp + ExtendedSaveFile.cpp + InfinityStash.cpp (GPL-3: layout only, no code copied). All fields LE.
//   .sss: "SSS\0" | "01" | [v"02": u32 sharedGold] | u32 nPages | page*
//   .d2x: "CSTM" | "01" | u32 (low byte = players-1, the rest ignored by PlugY) | u32 nPages | page*
//   page: "ST" | u32 flags | name (<= 20 chars) \0 | "JM" | u16 nItems | item bitstreams (D2 item format, inventory page 4)
// PlugY 10.00 pages carry no flags word; a name followed directly by "JM" tells the two apart. Item bitstreams are
// byte-aligned and self-delimiting only through the item decoder, so the reader and the writer take itemSize(bytes, off)
// -> length (d2-item-size.mjs). Pages are kept as opaque "JM" sections (the engine's stash export), with per-page
// { flags, name } riding beside them so a PlugY file survives a read and a write.
// Fallback form (earlier builds, and any write whose items the decoder cannot confirm): the page name is "L<byteLen>", which
// splits a section without a decoder. The reader accepts both; the writer emits the PlugY form whenever it can verify it.
const u16 = (b, o) => b[o] | (b[o + 1] << 8);
const u32 = (b, o) => (b[o] | (b[o + 1] << 8) | (b[o + 2] << 16) | (b[o + 3] << 24)) >>> 0;
const u32le = (v) => [v & 255, (v >>> 8) & 255, (v >>> 16) & 255, (v >>> 24) & 255];
const asc = (s) => [...s].map((c) => c.charCodeAt(0));
const EMPTY = new Uint8Array([0x4a, 0x4d, 0, 0]);   // a "JM" section with no items
export const MAX_PAGES = 1000;
export const MAX_NAME = 20;
const checkCount = (n) => { if (n < 1 || n > MAX_PAGES) throw new Error('bad page count'); };

// True when itemSize walks the whole section exactly: count items, ending on the last byte.
function sectionVerifies(sec, itemSize) {
  if (!itemSize) return false;
  try { let o = 4; for (let k = u16(sec, 2); k > 0; k--) o += itemSize(sec, o); return o === sec.length; } catch { return false; }
}
function stList(sections, meta, real) {
  return sections.flatMap((s, i) => {
    const sec = s && s.length >= 4 ? s : EMPTY, m = (meta && meta[i]) || {};
    if (!real) return [...asc('ST'), ...asc('L' + sec.length), 0, ...sec];   // legacy page: no flags word
    return [...asc('ST'), ...u32le(m.flags >>> 0), ...asc(String(m.name || '').slice(0, MAX_NAME)), 0, ...sec];
  });
}
// Parse n "ST" pages from o. Returns { pages: Uint8Array[] (the JM sections), meta: [{flags, name}] }.
function stPages(b, o, n, itemSize) {
  const pages = [], meta = [];
  const nul = (from) => { let e = from; while (e < b.length && b[e]) e++; if (e >= b.length) throw new Error('unterminated page name'); return e; };
  for (let i = 0; i < n; i++) {
    if (u16(b, o) !== 0x5453) throw new Error('bad ST tag'); o += 2;
    let flags = 0;
    if (u16(b, nul(o) + 1) !== 0x4d4a) { flags = u32(b, o); o += 4; }   // 11.x record: a flags word first
    const e = nul(o), name = String.fromCharCode(...b.subarray(o, e)); o = e + 1;
    if (u16(b, o) !== 0x4d4a) throw new Error('bad JM tag');
    const m = /^L(\d+)$/.exec(name), len = m ? +m[1] : 0;
    if (m && len >= 4 && o + len <= b.length && (o + len === b.length || u16(b, o + len) === 0x5453)) {   // fallback form
      pages.push(b.slice(o, o + len)); meta.push({ flags, name: '' }); o += len; continue;
    }
    if (!itemSize) throw new Error('item decoder required for a PlugY page');
    const start = o; o += 4;
    for (let k = u16(b, start + 2); k > 0; k--) { const l = itemSize(b, o); if (!(l > 0) || o + l > b.length) throw new Error('bad item'); o += l; }
    pages.push(b.slice(start, o)); meta.push({ flags, name });
  }
  return { pages, meta, end: o };
}
const realOk = (sections, itemSize) => sections.every((s) => !s || s.length < 4 || sectionVerifies(s, itemSize));

// opts: { gold, meta: [{flags, name}], itemSize }. Gold is stored only in the .sss (v"02"), as PlugY does.
export function pagesToSss(sections, { gold = 0, meta = null, itemSize = null } = {}) {
  const head = gold ? [...asc('02'), ...u32le(gold)] : asc('01');
  return Uint8Array.from([...asc('SSS\0'), ...head, ...u32le(sections.length), ...stList(sections, meta, realOk(sections, itemSize))]);
}
export function sssToPages(b, itemSize = null) {
  if (u32(b, 0) !== 0x00535353) throw new Error('bad SSS header');
  const ver = String.fromCharCode(b[4], b[5]);
  let o = 6, gold = 0;
  if (ver === '02') { gold = u32(b, o); o += 4; } else if (ver !== '01') throw new Error('bad SSS version ' + ver);
  const n = u32(b, o); checkCount(n);
  const { pages, meta } = stPages(b, o + 4, n, itemSize);
  return { pages, meta, gold, cur: 0 };
}
// The live page rides in the 3 bytes PlugY ignores. live = { page, shared, personal? }. A shared page showing records both
// the shared page and the personal page the .d2s holds: personal | page << 10 | 0x800000 (bit 23 marks the form). The older
// form (shared flag in byte 9 = 1, page in bytes 7-8) is still read: its .d2s held the shared page's items.
export function pagesToD2x(sections, live = { page: 0, shared: false }, { meta = null, itemSize = null } = {}) {
  const spare = live.shared && live.personal != null
    ? [live.personal & 255, ((live.personal >> 8) & 3) | ((live.page & 63) << 2), (live.page >> 6) | 0x80]
    : [live.page & 255, (live.page >> 8) & 255, live.shared ? 1 : 0];
  return Uint8Array.from([...asc('CSTM'), ...asc('01'), 0, ...spare, ...u32le(sections.length), ...stList(sections, meta, realOk(sections, itemSize))]);
}
export function d2xToPages(b, itemSize = null) {
  if (u32(b, 0) !== 0x4d545343 || String.fromCharCode(b[4], b[5]) !== '01') throw new Error('bad D2X header');
  const n = u32(b, 10); checkCount(n);
  const { pages, meta } = stPages(b, 14, n, itemSize);
  if (b[9] & 0x80) {
    const v = b[7] | (b[8] << 8) | ((b[9] & 0x7f) << 16);
    return { pages, meta, live: { page: (v >> 10) & 1023, shared: true, personal: Math.min(v & 1023, n - 1) } };
  }
  return { pages, meta, live: { page: Math.min(u16(b, 7), n - 1), shared: (b[9] & 1) === 1 } };
}
