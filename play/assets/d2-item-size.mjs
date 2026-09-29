// Byte length of one 1.13c item bitstream ("JM" tag, flags, quality data, magic property lists) plus
// its socketed children, so plugy-stash.mjs can split a page into items. Layout re-derived from
// reference/d2itemreader (d2item_parse_single); tables come from the retail .txt files, not literals.
const rows = (txt) => txt.split(/\r?\n/).filter(Boolean).map((l) => l.split('\t'));
export function itemTables(statCostTxt, armorTxt, weaponsTxt, miscTxt) {
  const codes = (txt) => { const [h, ...r] = rows(txt); const c = h.indexOf('code'), s = h.indexOf('stackable');
    return { all: new Set(r.map((x) => x[c])), stack: new Set(r.filter((x) => s >= 0 && x[s] === '1').map((x) => x[c])) }; };
  const [h, ...r] = rows(statCostTxt), col = (n) => h.indexOf(n);
  const stats = r.map((x, id) => ({ bits: +x[col('Save Bits')] || 0, add: +x[col('Save Add')] || 0, enc: +x[col('Encode')] || 0,
    pbits: +x[col('Save Param Bits')] || 0, next: [17, 48, 50, 52, 54, 55, 57, 58].includes(id) ? id + 1 : 0 }));
  const a = codes(armorTxt), w = codes(weaponsTxt), m = codes(miscTxt);
  return { stats, armor: a.all, weapon: w.all, stack: new Set([...w.stack, ...m.stack]) };
}

function single(b, start, T) {
  let pos = start * 8;
  const rd = (n) => { let v = 0; for (let i = 0; i < n; i++, pos++) { if ((pos >> 3) >= b.length) throw new Error('item past end'); v += ((b[pos >> 3] >> (pos & 7)) & 1) * 2 ** i; } return v; };
  if (rd(16) !== 0x4d4a) throw new Error('item without JM tag at ' + start);
  rd(4); rd(1); rd(6); const socketed = rd(1); rd(4); const ear = rd(1); rd(1 + 3);
  const simple = rd(1); rd(1 + 1); const personalized = rd(1); rd(1); const runeword = rd(1); rd(5 + 8 + 2 + 3 + 4 + 4 + 4 + 3);
  let code = '', nsock = 0;
  if (!ear) { for (let i = 0; i < 4; i++) { const c = String.fromCharCode(rd(8)); code += c === ' ' ? '' : c; } nsock = rd(3); }
  else { rd(3 + 7); for (let i = 0; i < 16; i++) if (!rd(7)) break; }
  if (!simple) {
    rd(32 + 7); const q = rd(4);
    if (rd(1)) rd(3);
    if (rd(1)) rd(11);
    if (q === 1 || q === 3) rd(3); else if (q === 4) rd(22); else if (q === 5 || q === 7) rd(12);
    else if (q === 6 || q >= 8) { rd(16); for (let i = 0; i < 6; i++) if (rd(1)) rd(11); }
    if (runeword) rd(16);
    if (personalized) for (let i = 0; i < 16; i++) if (!rd(7)) break;
    if (code === 'tbk' || code === 'ibk') rd(5);
    rd(1);
    if (T.armor.has(code)) rd(11);
    if (T.armor.has(code) || T.weapon.has(code)) { if (rd(8)) rd(9); }
    if (T.stack.has(code)) rd(9);
    if (socketed) rd(4);
    const setBits = q === 5 ? rd(5) : 0;
    const list = () => { for (;;) { const id = rd(9); if (id === 511) return; const s = T.stats[id];
      if (!s || !s.bits || s.enc === 4) throw new Error('bad stat ' + id + ' in ' + code + ' q' + q);
      if (s.enc === 2) rd(6 + 10 + s.bits); else if (s.enc === 3) rd(6 + 10 + 8 + 8);
      else rd(s.pbits + s.bits);
      for (let n = s.next, g = 0; n && g < 4; n = T.stats[n].next, g++) rd(T.stats[n].bits); } };
    list();
    for (let i = 0; i < 5; i++) if (setBits >> i & 1) list();
    if (runeword) list();
  }
  return { len: Math.ceil((pos - start * 8) / 8), nsock: simple ? 0 : nsock };
}

/** itemSize(bytes, off) for readSss: the item and its socketed children. */
export const itemSizer = (T) => (b, off) => {
  const it = single(b, off, T); let len = it.len;
  for (let i = 0; i < it.nsock; i++) len += single(b, off + len, T).len;
  return len;
};
