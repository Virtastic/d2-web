// Retail 1.13c D2Client+0x26970 (experience bar) and +0x278B0 (stamina bar): hover tooltips.
// Pure: state in, {text, x, y, color} out; the shell draws it
// through the same centred Font16 plate as the other D2Win #10085 labels.
// `fmt` are string.tbl 4163 "panelexp" and 4164 "panelstamina" (read from the engine's table).
const format = (fmt, a, b) => { let n = 0; return fmt.replace(/%[ud]/g, () => (n++ ? b : a)); };

// w/h: screen size (retail DAT_6fb8bc48/4c). mx/my: pointer. Returns null when the pointer is off both bars.
// `engine(w, h, mx, my, stamina, staminaMax, over)` is the retail bar boxes (d2_client_hud_bar_tip, D2Client+0x26970/+0x278B0):
// null for off both bars, else {bar: 1|2, x, y, color, cur}. The rectangles below stand in before the engine loads.
export function hudBarTooltip(w, h, mx, my, { experience, nextExperience, stamina, staminaMax, staminaOverMax = false, fmtExp, fmtStamina, engine = null, formatEngine = null }) {
  if (engine) {
    const r = engine(w, h, mx, my, stamina, staminaMax, staminaOverMax ? 1 : 0);
    if (!r) return null;
    // `formatEngine(id, a, b)` is the retail Unicode::sprintf over string.tbl entry `id` (d2_client_format_string); the JS replace is the pre-load fallback.
    const fmt = (id, f, a, b) => (formatEngine && formatEngine(id, a, b)) || format(f, a, b);
    return r.bar === 1
      ? { bar: 'experience', text: fmt(4163, fmtExp, experience >>> 0, nextExperience >>> 0), x: r.x, y: r.y, color: r.color }
      : { bar: 'stamina', text: fmt(4164, fmtStamina, r.cur, staminaMax), x: r.x, y: r.y, color: r.color };
  }
  const cx = Math.trunc(w / 2);
  if (mx >= cx - 146 && mx <= cx - 23 && my >= h - 43 && my <= h - 34)
    return { bar: 'experience', text: format(fmtExp, experience >>> 0, nextExperience >>> 0), x: cx - 146, y: h - 51, color: 0 };
  if (mx >= cx - 127 && mx <= cx - 25 && my >= h - 27 && my <= h - 9) {
    // cur > max (or the 0x88 state) clamps cur to max and paints it colour 3.
    const over = stamina > staminaMax || staminaOverMax;
    return { bar: 'stamina', text: format(fmtStamina, over ? staminaMax : stamina, staminaMax), x: cx - 76, y: h - 52, color: over ? 3 : 0 };
  }
  return null;
}

// Lit add-point button hover text: string.tbl 3986 "strlvlup" (New Stats, left button) and 3987 "strnewskl"
// (New Skill, right). Retail D2Client+0x50430 / +0x501F0 (800x600 mode, which the widescreen client keeps): SetPopUpUnicodeText
// at x = W/2 -179 / +178, y = H-50, centred; W/H are the screen size, so the plate follows the screen centre, never the 800 panel.
// `engine(w, h, mx, my)` is the retail hit test and plate position (d2_client_level_btn_tip, via 0x4FF10 / 0x4FE70): null off
// both, else {side: 0|1, x, y}; `lit[side]` is the caller's evidence that the button is drawn lit. Without an engine the
// caller's own hit index and the offsets above stand in.
export function levelButtonTooltip(index, w, h, texts, { engine = null, mx = 0, my = 0, lit = [] } = {}) {
  if (engine) {
    const r = engine(w, h, mx, my);
    return r && lit[r.side] ? { bar: r.side ? 'skill' : 'stats', text: texts[r.side], x: r.x, y: r.y, color: 0 } : null;
  }
  return index < 0 ? null : { bar: index ? 'skill' : 'stats', text: texts[index], x: Math.trunc(w / 2) + (index ? 178 : -179), y: h - 50, color: 0 };
}
