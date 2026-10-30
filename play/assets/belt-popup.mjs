// The belt popup and the belt's mouse buttons, as 1.13c D2Client does them.
// Retail HUD shows only the bottom belt row. The rows above are drawn only while the popup is open:
//   - hover: the mouse enters x in [w/2+23, w/2+145], y in [h-39, h-10] (D2Client+0x26FB0 mouse move) -> 0x11C46C = 0x11C470 = 1
//   - the Show Belt key toggles 0xFADFC (D2Client+0x26300 sets 0x11C46C)
//   - drawBelt (D2Client+0x28DC0): popup stays open only while (0xFADFC || 0x11C470) and not 0xFADA4
//   - open + pointer outside the popup box (D2Client+0x26320): hover ends, and so does the popup unless the key holds it.
// Rows above the bottom, by Belts.txt record (the jump table in drawBelt): 2 none, 1 and 4 one, 0 and 5 two, 3 three.
export const POPUP_ROWS = Object.freeze({ 0: 2, 1: 1, 2: 0, 3: 3, 4: 1, 5: 2 });
export const POPUP_BG = 'data\\global\\ui\\Panel\\ctrlpnl_popbelt';
export const popupRows = (record, boxCount = 16) => Math.min(POPUP_ROWS[record] ?? 0, Math.max(0, Math.floor(boxCount / 4) - 1));
// Background cel i (0 = nearest the HUD row): bottom-left at (w/2+21, h-i*32-41), 800x600 HUD coordinates -> (421, 559-32i).
export const popupBgPosition = (i, w = 800, h = 600) => ({ x: (w >> 1) + 21, y: h - i * 32 - 41 });

export function createBeltPopup() {
  const s = { expanded: false, hover: false, key: false, blocked: false };
  const inClosed = (x, y, w, h) => y > h - 0x30 && x >= (w >> 1) + 0x17 && x <= (w >> 1) + 0x91 && y >= h - 0x27 && y <= h - 10;
  // boxes: the worn belt's full Belts.txt list, rows: popupRows(...). The open hit box is the top extra row's first box to the bottom row's last.
  const inOpen = (x, y, boxes, rows) => {
    if (!boxes?.length) return false;
    const used = boxes.slice(0, 4 * (rows + 1));
    const left = Math.min(...used.map((b) => b.left)), right = Math.max(...used.map((b) => b.right));
    const top = Math.min(...used.map((b) => b.top)), bottom = Math.max(...used.map((b) => b.bottom));
    return x >= left && x <= right && y >= top && y <= bottom;
  };
  return {
    get open() { return s.expanded; },
    get state() { return { ...s }; },
    block(on) { s.blocked = Boolean(on); if (on) s.expanded = false; },
    toggleKey() { s.key = !s.key; s.expanded = s.key && !s.blocked; return s.expanded; },
    mouseMove(x, y, w, h, boxes, rows) {
      if (s.expanded) {
        if (!inOpen(x, y, boxes, rows)) { if (!s.key) s.expanded = false; s.hover = false; }
      } else if (inClosed(x, y, w, h) && !s.blocked) { s.expanded = true; s.hover = true; }
      return s.expanded;
    },
    // drawBelt's per-frame recompute.
    frame() { if ((!s.key && !s.hover) || s.blocked) s.expanded = false; return s.expanded; },
  };
}

// What a mouse button does on a belt box (D2Client+0x28050 left down, 0x29B50 right down -> 0x29990).
//   left : empty cursor + potion in the box -> lift it (packet 0x24); item on cursor + empty box -> place it (0x23 {item, slot});
//          item on cursor + potion in the box -> swap (0x25 {cursor, belt}); empty cursor + empty box -> nothing.
//   right: potion in the box -> use it (0x26), on the mercenary with shift held; empty box -> nothing.
// cursorItem is the cursor item's id (0 when empty); beltItem the box's item or null.
export function beltClickAction({ button, shift = false, slot, beltItem, cursorItem }) {
  if (button === 2) return beltItem ? { op: 'use', slot, merc: Boolean(shift) } : null;
  if (button !== 0) return null;
  if (!cursorItem) return beltItem ? { op: 'lift', a: beltItem.id } : null;
  return beltItem ? { op: 'swap', a: cursorItem, b: beltItem.id } : { op: 'place', a: cursorItem, b: slot };
}
