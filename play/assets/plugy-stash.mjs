// PlugY 11.x shared-stash (.sss) and page ("ST") container, re-derived from reference/PlugY
// SharedSaveFile.cpp + InfinityStash.cpp (GPL-3: layout only, no code copied).
// Layout: "SSS\0" | "01" | [v"02": u32 sharedGold] | u32 nPages | page*   (all LE)
// page:   "ST" | u32 flags | name\0 | "JM" | u16 nItems | item bitstreams (D2 item format, page 4)
// Item bitstreams are byte-aligned and self-delimiting only through the item decoder, so the
// caller supplies itemSize(bytes, off) -> length; items stay opaque bytes here.
const u16 = (b, o) => b[o] | (b[o + 1] << 8);
const u32 = (b, o) => (b[o] | (b[o + 1] << 8) | (b[o + 2] << 16) | (b[o + 3] << 24)) >>> 0;
const asc = (s) => [...s].map((c) => c.charCodeAt(0));

export function readSss(b, itemSize) {
  if (u32(b, 0) !== 0x00535353) throw new Error('bad SSS header');
  let o = 4, gold = 0;
  const ver = String.fromCharCode(b[o], b[o + 1]); o += 2;
  if (ver === '02') { gold = u32(b, o); o += 4; } else if (ver !== '01') throw new Error('bad SSS version ' + ver);
  const n = u32(b, o); o += 4;
  const pages = [];
  for (let i = 0; i < n; i++) {
    if (u16(b, o) !== 0x5453) throw new Error('bad ST tag'); o += 2;   // "ST"
    // PlugY 10.00 (the build D2SE ships, a real one measured 2026-09-29) writes NO flags field; the 11.x source's
    // reader tells the two apart the same way: a NUL-terminated name followed by "JM" means flags are absent.
    let flags = 0, l = o; while (b[l]) l++;
    if (u16(b, l + 1) !== 0x4d4a) { flags = u32(b, o); o += 4; }
    let e = o; while (b[e]) e++;
    const name = String.fromCharCode(...b.subarray(o, e)); o = e + 1;
    if (u16(b, o) !== 0x4d4a) throw new Error('bad JM tag'); o += 2;   // "JM"
    const cnt = u16(b, o); o += 2;
    const items = [];
    for (let k = 0; k < cnt; k++) { const len = itemSize(b, o); items.push(b.slice(o, o + len)); o += len; }
    pages.push({ flags, name, items });
  }
  return { gold, pages };
}

export function writeSss({ gold = 0, pages, legacy = false }) {   // legacy: PlugY 10.00 layout, flags-0 pages carry no flags field
  const out = [...asc('SSS\0')];
  const w32 = (v) => out.push(v & 255, (v >>> 8) & 255, (v >>> 16) & 255, (v >>> 24) & 255);
  if (gold) { out.push(...asc('02')); w32(gold); } else out.push(...asc('01'));   // PlugY omits gold => v01
  w32(pages.length);
  for (const p of pages) {
    out.push(...asc('ST')); if (!(legacy && !p.flags)) w32(p.flags);
    out.push(...asc(p.name.slice(0, 20)), 0);
    // p.section: the engine's exported "JM"+count+items bytes, already in page layout (PY1 swap).
    if (p.section) out.push(...p.section);
    else { out.push(...asc('JM'), p.items.length & 255, p.items.length >> 8); for (const it of p.items) out.push(...it); }
  }
  return Uint8Array.from(out);
}

// Client persistence (PY1): the shell's stash pages as one .sss blob, pages kept as the engine's opaque
// "JM" sections. Page name = "L<byteLen>" so a section splits back without the item decoder; flags bit 0
// marks the page the engine's stash currently is. Throws on any malformed input (caller falls back).
export const pagesToSss = (sections, cur) => writeSss({ pages: sections.map((s, i) => ({ flags: i === cur ? 1 : 0, name: 'L' + (s ? s.length : 0), section: s || new Uint8Array([0x4a, 0x4d, 0, 0]) })) });
export function sssToPages(b) {
  if (u32(b, 0) !== 0x00535353 || String.fromCharCode(b[4], b[5]) !== '01') throw new Error('bad SSS header');
  const n = u32(b, 6), pages = []; let o = 10, cur = 0;
  if (n < 1 || n > 64) throw new Error('bad page count');
  for (let i = 0; i < n; i++) {
    if (u16(b, o) !== 0x5453) throw new Error('bad ST tag'); o += 2;
    if (u32(b, o) & 1) cur = i;
    o += 4; let e = o; while (b[e]) e++;
    const m = /^L(\d+)$/.exec(String.fromCharCode(...b.subarray(o, e))); if (!m) throw new Error('bad page name');
    o = e + 1; const len = +m[1];
    if (len < 4 || o + len > b.length || u16(b, o) !== 0x4d4a) throw new Error('bad JM section');
    pages.push(b.slice(o, o + len)); o += len;
  }
  return { pages, cur };
}
