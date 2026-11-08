// Retail 1.13c D2Client mini-panel button hover text (the D2Client function the HUD pass calls per
// frame after it draws the row). Pure: state in, {text, x, y, color} out; the shell draws it through
// the same centred Font16 plate as the other D2Win #10085 labels.
//
// Rule, in retail's order:
//  - pointer strictly inside the row's box (x in (cx-84, cx+89), y in (H-69, H-47)) and strictly
//    inside one button's 20-wide column (left < x < left+20);
//  - text = string.tbl 4169..4175 for the Character, Inventory, Skill Tree, Automap, Message Log and
//    Quest Log buttons, 4176 for the game menu; 4172 (Party) is the eighth button of the non-expansion row;
//  - every button but the menu then appends " (%s)" (string 4178) with the key bound in the
//    SECOND Configure Controls column first, then the first column (retail's slot 1 then slot 0),
//    skipping a column with no key. Key names come from the language tables (3791.., 3809.., 3825.., 3856)
//    and are the character itself for letters and digits;
//  - drawn centred at x = buttonLeft - 3, y = buttonBottom - 26 (the button's bottom is H-50), colour 0.
// `lang(id)` reads the engine's string table; `bindings` is the Configure Controls table ([key1, key2]
// per slot); `slotOf(name)` finds the slot of a function name.
export const MINI_TOOLTIP_STRINGS = Object.freeze([4169, 4170, 4171, 4173, 4174, 4175, 4176]);
export const MINI_TOOLTIP_ACTIONS = Object.freeze(['Character Screen', 'Inventory Screen', 'Skill Tree', 'Automap', 'Message Log', 'Quest Log', null]);

const KEY_STRINGS = { Tab: 3791, Enter: 3793, Shift: 3794, Ctrl: 3795, Alt: 3796, Space: 3799, 'Print Screen': 3805, '~': 3856 };
export function keyLabel(name, lang) {
  let id = KEY_STRINGS[name];
  let m;
  if (id === undefined && (m = /^Num Pad ([0-9])$/.exec(name))) id = 3809 + Number(m[1]);
  if (id === undefined && (m = /^Mouse ([1-5])$/.exec(name))) id = 3763 + Number(m[1]);
  if (id === undefined && (m = /^F([1-9]|1[0-2])$/.exec(name))) id = 3824 + Number(m[1]);
  return id === undefined ? name : lang(id);
}

// screenLeft: the hovered button's left edge in screen pixels (800x600: W/2-74+21*i, plus the side shift);
// index: 0..6 into the seven drawn buttons. Returns null off the row.
export function miniPanelTooltip(index, screenLeft, w, h, mx, my, { lang, bindings, slotOf }) {
  if (index < 0 || index > 6) return null;
  if (!(mx > screenLeft && mx < screenLeft + 20 && my > h - 69 && my < h - 47)) return null;
  let text = lang(MINI_TOOLTIP_STRINGS[index]);
  const action = MINI_TOOLTIP_ACTIONS[index];
  if (action) {
    const keys = bindings?.[slotOf(action)] ?? [];
    for (const key of [keys[1], keys[0]]) if (key) text += lang(4178).replace('%s', keyLabel(key, lang));
  }
  return { text, x: screenLeft - 3, y: h - 76, color: 0 };
}
