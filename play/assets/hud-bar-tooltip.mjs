// Retail 1.13c D2Client+0x26970 (experience bar) and +0x278B0 (stamina bar): hover tooltips.
// Pure: state in, {text, x, y, color} out; the shell draws it
// through the same centred Font16 plate as the other D2Win #10085 labels.
// Text is string.tbl 4163 "panelexp" and 4164 "panelstamina", formatted by the engine.

// w/h: screen size (retail DAT_6fb8bc48/4c). mx/my: pointer. Returns null when the pointer is off both bars.
// `engine(w, h, mx, my, stamina, staminaMax, over)` is the retail bar boxes (d2_client_hud_bar_tip, D2Client+0x26970/+0x278B0):
// null for off both bars, else {bar: 1|2, x, y, color, cur}. `formatEngine(id, a, b)` is the retail Unicode::sprintf over
// string.tbl entry `id` (d2_client_format_string). Without the engine nothing is drawn.
export function hudBarTooltip(w, h, mx, my, { experience, nextExperience, stamina, staminaMax, staminaOverMax = false, engine = null, formatEngine = null }) {
  if (!engine || !formatEngine) return null;
  const r = engine(w, h, mx, my, stamina, staminaMax, staminaOverMax ? 1 : 0);
  if (!r) return null;
  return r.bar === 1
    ? { bar: 'experience', text: formatEngine(4163, experience >>> 0, nextExperience >>> 0) ?? '', x: r.x, y: r.y, color: r.color }
    : { bar: 'stamina', text: formatEngine(4164, r.cur, staminaMax) ?? '', x: r.x, y: r.y, color: r.color };
}

// Lit add-point button hover text: string.tbl 3986 "strlvlup" (New Stats, left button) and 3987 "strnewskl"
// (New Skill, right). Retail D2Client+0x50430 / +0x501F0 (800x600 mode, which the widescreen client keeps): SetPopUpUnicodeText
// at x = W/2 -179 / +178, y = H-50, centred; W/H are the screen size, so the plate follows the screen centre, never the 800 panel.
// `engine(w, h, mx, my)` is the retail hit test and plate position (d2_client_level_btn_tip, via 0x4FF10 / 0x4FE70): null off
// both, else {side: 0|1, x, y}; `lit[side]` is the caller's evidence that the button is drawn lit. No engine, no plate.
export function levelButtonTooltip(w, h, texts, { engine = null, mx = 0, my = 0, lit = [] } = {}) {
  const r = engine ? engine(w, h, mx, my) : null;
  return r && lit[r.side] ? { bar: r.side ? 'skill' : 'stats', text: texts[r.side], x: r.x, y: r.y, color: 0 } : null;
}
