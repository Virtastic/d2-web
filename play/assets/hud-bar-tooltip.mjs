// Retail 1.13c D2Client+0x26970 (experience bar) and +0x278B0 (stamina bar): hover tooltips.
// Pure: state in, {text, x, y, color} out; the shell draws it
// through the same centred Font16 plate as the other D2Win #10085 labels.
// `fmt` are string.tbl 4163 "panelexp" and 4164 "panelstamina" (read from the engine's table).
const format = (fmt, a, b) => { let n = 0; return fmt.replace(/%[ud]/g, () => (n++ ? b : a)); };

// w/h: screen size (retail DAT_6fb8bc48/4c). mx/my: pointer. Returns null when the pointer is off both bars.
export function hudBarTooltip(w, h, mx, my, { experience, nextExperience, stamina, staminaMax, staminaOverMax = false, fmtExp, fmtStamina }) {
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
export function levelButtonTooltip(index, w, h, texts) {
  return index < 0 ? null : { bar: index ? 'skill' : 'stats', text: texts[index], x: Math.trunc(w / 2) + (index ? 178 : -179), y: h - 50, color: 0 };
}
