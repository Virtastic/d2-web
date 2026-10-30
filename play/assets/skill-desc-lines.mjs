// SkillDesc line formatter: the retail client's switch on the line type and the leaf formatters it calls.
// A line is (type, texta, textb, calca, calcb); the engine evaluates the calcs. The strings are D2Lang indexes;
// index 0x1506 (the "an evil force" placeholder) means "no string". A leaf that gets a zero value draws nothing.
const NONE = 0x1506;
const sign = (n, on) => (on && n >= 0 ? '+' : '') + n;
// 0x2E060 / 0x2C790: frames at 25 per second, one decimal when the fraction is non-zero.
const secs = (f, on) => {
  const w = Math.trunc(f / 25), t = Math.trunc(((f % 25) * 10) / 25);
  if (!w && !t) return null;
  return t ? `${w}.${t}` : sign(w, on);
};
const range = (n, m, on) => (n === m ? String(n) : `${on ? '+' : ''}${n}-${m}`);
// 0x2F430: the radius leaf, calc * 20 / 3 in tenths of a yard.
const yardsTenths = (v) => {
  let w = Math.trunc(v / 10); const t = v % 10;
  if (w < 1) w = 1;
  return t ? `${w}.${t}` : String(w);
};
const yardsText = (v, key) => yardsTenths(v) + key(yardsTenths(v) === "1" ? DESC_KEY.yard : DESC_KEY.yards);
export const DESC_KEY = { seconds: 0x10ac, percent: 0x10b3, yards: 0x10b6, yard: 0x10c0, perSecond: 0x10be, bolts: 0x10b7, teeth: 0x10b8 };

/** One line's text, or null when the leaf draws nothing. `ctx.line(i)` is the lang table, `ctx.level` the skill level. */
export function formatDescLine(type, a, b, n, m, ctx) {
  const L = (i) => (i && i !== NONE ? String(ctx.line(i) || '') : '');
  const A = L(a), B = L(b), key = (i) => String(ctx.line(i) || '');
  switch (type) {
    case 2: return n ? A + sign(n, 1) + B : null;
    case 3: return n ? A + n + B : null;
    case 4: return n ? A + sign(n, 1) : null;
    case 5: return n ? A + n : null;
    case 6: return n ? sign(n, 1) + A : null;
    case 7: return n ? n + A : null;
    case 12: { const s = secs(n, 0); return s === null ? null : A + s + key(DESC_KEY.seconds); }
    case 16: {
      const x = secs(n, 0), y = secs(m, 0);
      if (x === null && y === null) return null;
      const f = (v) => { const w = Math.trunc(v / 25), t = Math.trunc(((v % 25) * 10) / 25); return t ? `${w}.${t}` : String(w); };
      return A + f(n) + '-' + f(m) + key(DESC_KEY.seconds);
    }
    case 17: return A + range(n, m, 0) + B;
    case 19: return n ? A + yardsText(Math.trunc((n * 20) / 3), key) : null;
    case 31: { const s = secs(n, 0); return s === null ? null : A + s + key(DESC_KEY.seconds); }
    case 32: return n ? A + B + sign(n, 1) + key(DESC_KEY.percent) : null;
    case 33: return ctx.level < 2 ? '' : A + B;
    case 35: return A + n + '-' + m;
    case 36: return n ? (n === 1 ? n + A : n + B) : null;
    case 37: return n ? A + yardsText(Math.trunc((n * 10) / 3), key) : null;
    case 38: return A + range(n, m, 0) + B;
    case 47: return A + range(n, m, 0);
    case 51: return A.replace('%d', String(n)).replace(/%%/g, '%');
    case 52: return n === m ? (n ? A + n + B : null) : A + range(n, m, 1) + B;
    case 57: { const s = secs(n, 1); return s === null ? null : A + s + key(DESC_KEY.seconds); }
    case 59: return A + range(n, m, 0);
    case 61: return A + (n % 256 ? `${Math.floor(n / 256)}.${Math.trunc((n % 256) * 10 / 256)}` : String(Math.floor(n / 256))) + B;
    case 66: return A.replace('%d', String(n)).replace(/%%/g, '%');
    case 70: return n ? A + B + sign(n, 1) : null;
    case 72: return A.length || true ? `${sign(n, 1)}/${m} ` + A : null;
    case 73: return `${n}/${m} ` + A;
    default: return undefined;
  }
}
