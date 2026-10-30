// PlugY shared-stash page control over the retail stash panel (PY1, PY3). OPT-IN: ?plugy=1 (default page is strict retail; nothing here draws). Page 0 is the engine's own stash; pages 1.. are
// .sss pages (plugy-stash.mjs). Art: PlugY's own StashBtns.dc6 / SharedGoldBtns.dc6, laid out by reference/PlugY Interface_Stash.cpp
// (defaults for the 10x10 BigStash at 800x600: x = WindowStartX + v, y = the sprite's BOTTOM edge, RY(v) = ResolutionY + NegWindowStartY - v = 600 - 59 - v = 541 - v; WindowStartX = 80 measured
// against retail/frames/plugy-stash-r29.png, s869). The panel behind them is PlugY's TradeStash.dc6 (stash-panel.mjs setArt); until the art loads
// the plain canvas control stays. The shell swaps a page in/out of the engine stash via d2_session_stash_export/clear/insert (JM bytes).
// Prev/next, the index jumps and the personal/shared toggle are live; the shared-gold buttons call onGold("put" | "take") (PlugY: all the gold that fits).
export const PLUGY_GRID = { columns: 10, rows: 10, gridLeft: 96, gridTop: 143 };   // BigStash.cpp gridLeft 16 / gridTop 82 on the panel at (80,61)
const POPUP_TEXT_UP = 3;   // the text baseline sits 3 px above the box bottom (calibrated on plugy-rb-hover-toggle)
const POPUP_DY = 1;   // popup baseline = button top - 32 + POPUP_DY (calibrated on the capture)
const WSX = 80, BOTTOM = 541;   // RX(v) = WSX + v; RY(v) = BOTTOM - v
const BTN = {
  prev: { f: 0, x: WSX + 0x80, y: BOTTOM - 0x40 - 32 }, next: { f: 2, x: WSX + 0xA0, y: BOTTOM - 0x40 - 32 },
  shared: { f: 4, x: WSX + 0x10, y: BOTTOM - 0x40 - 32 }, prevIndex: { f: 8, x: WSX + 0x60, y: BOTTOM - 0x40 - 32 }, nextIndex: { f: 10, x: WSX + 0xC0, y: BOTTOM - 0x40 - 32 },
  putGold: { f: 0, gold: true, x: WSX + 0x1C, y: BOTTOM - 0x1A8 - 32 }, takeGold: { f: 2, gold: true, x: WSX + 0x105, y: BOTTOM - 0x1A8 - 32 },
};
export const PLUGY_NAME_AT = { x: WSX + 0x4A + 4, y: BOTTOM - 0x19A - 6 };
/** True when bytes are a DC6 (version 6, termination 1 header): a dev server that answers a missing art file with a page is not art. */
export const isDc6 = (b) => Boolean(b) && b.length > 24 && b[0] === 6 && b[1] === 0 && b[2] === 0 && b[3] === 0 && b[4] === 1;
const LIVE = ['prev', 'next', 'shared', 'prevIndex', 'nextIndex', 'putGold', 'takeGold'];
export const MAX_PAGE = 999;   // last page index of a set (the sidecar caps a set at 1000 pages)
// Page a button leads to, from `page`. No wrap (PlugY): prev stops at 0, next grows the set on demand. Index buttons land on the
// previous / next page whose number is a multiple of 10 (100 with shift); shift+prev is the first page, shift+next the last non-empty.
export function pageTarget(name, page, shift = false, lastUsed = 0) {
  const step = shift ? 100 : 10;
  if (name === 'prev') return shift ? 0 : Math.max(0, page - 1);
  if (name === 'next') return shift ? Math.max(page, lastUsed) : Math.min(MAX_PAGE, page + 1);
  let q = page + (name === 'nextIndex' ? 1 : -1);
  while (q > 0 && q < MAX_PAGE && (q + 1) % step) q += name === 'nextIndex' ? 1 : -1;
  return Math.max(0, Math.min(MAX_PAGE, q));
}
// The bottom-right button is the panel's Close (TradeStash art, frame from StashBtns per the capture) -- a hit rect only, drawn by the panel art.
BTN.close = { hit: true, popDy: -3, x: WSX + 0x110, y: BOTTOM - 0x40 - 32 };
// Hover popups (PlugY D2PrintPopup: Font16 text centred on the button's top-centre, retail frames plugy-rb-hover-*.png, F176). Strings are the captured ones.
// prevIndex is measured starting at x=196 (not centred, unexplained) so it carries a fixed left edge.
const POPUP = {
  prev: () => 'Previous Page (+shift: First Page)', next: () => 'Next Page (+shift: Last not empty Page)',
  shared: (s) => (s.showShared ? 'Toggle to Personal Stash' : 'Toggle to Shared Stash'),
  prevIndex: () => 'Previous Index : by 10 Pages (+Shift: 100)', nextIndex: () => 'Next Index : by 10 Pages (+shift: 100)',
  putGold: () => 'Put Gold', takeGold: () => 'Take Gold', close: () => 'Close',
};
const POPUP_LEFT = { prevIndex: 196 };
// onPage(fromPage, toPage, fromShared, toShared) === false refuses the turn. lastUsed(shared) = index of the last non-empty page.
export function createPlugyStash({ enabled = false, pages = 1, onPage = null, onGold = null, lastUsed = () => 0 } = {}) {
  const s = { enabled, pages, page: 0, showShared: false, hover: null, text: null, mem: { false: 0, true: 0 } };
  s.art = null;   // { stash: canvas[], gold: canvas[] } once StashBtns.dc6 / SharedGoldBtns.dc6 are decoded (setArt)
  s.down = null;
  const scaleOf = (w, h) => { const k = Math.min(1, w / 800, h / 600); return { k, x: (w - 800 * k) / 2, y: (h - 600 * k) / 2 }; };
  const ready = () => s.art?.stash?.length >= 12 && s.art?.gold?.length >= 4;
  return Object.assign(s, {
    BTN,
    /** art: { stash: canvases of StashBtns.dc6, gold: canvases of SharedGoldBtns.dc6 }, in frame order. */
    setArt(art) { s.art = art; },
    /** Track the pointer (canvas px) for the hover popup. */
    pointer(px, py, w, h) { const o = scaleOf(w, h); s.hover = px == null ? null : { x: (px - o.x) / o.k, y: (py - o.y) / o.k }; },
    draw(ctx, w, h, g, open, act = 1) {
      if (!s.enabled || !open || !g) return null;
      const o = scaleOf(w, h);
      ctx.save(); ctx.translate(o.x, o.y); ctx.scale(o.k, o.k);
      if (ready()) {
        const drawn = [];
        for (const [name, b] of Object.entries(BTN)) {
          if (b.hit) continue;
          const frame = b.f + (s.down === name ? 1 : 0) + (name === 'shared' && s.showShared ? 2 : 0);
          ctx.drawImage((b.gold ? s.art.gold : s.art.stash)[frame], b.x, b.y);
          drawn.push({ name, art: b.gold ? 'SharedGoldBtns.dc6' : 'StashBtns.dc6', frame, x: b.x, y: b.y, w: 32, h: 32, live: LIVE.includes(name) });
        }
        const popup = s.text && s.hover ? Object.entries(BTN).find(([, b]) => s.hover.x >= b.x && s.hover.x < b.x + 32 && s.hover.y >= b.y && s.hover.y < b.y + 32) : null;
        let shown = null;
        if (popup) {
          const [name, b] = popup, str = POPUP[name](s), tw = s.text.measure(act, str), x = POPUP_LEFT[name] ?? b.x + 16 - Math.trunc(tw / 2), y = b.y + POPUP_DY + (b.popDy || 0);
          ctx.fillStyle = 'rgba(0,0,0,0.75)'; ctx.fillRect(x - 4, y - 16, tw + 8, 16); s.text.draw(ctx, act, str, x, y - POPUP_TEXT_UP);
          shown = { name, text: str, box: [x - 4, y - 16, x + tw + 4, y], at: [x, y] };
        }
        ctx.restore();
        return { popup: shown, showShared: s.showShared, art: 'StashBtns.dc6', buttons: drawn, x: BTN.shared.x, y: BTN.prev.y, w: 32 * 8, h: 32, page: s.page, pages: s.pages };
      }
      ctx.restore();
      return null;   // no PlugY art, no control: nothing here is drawn from invented art (the shell turns the whole feature off, see isDc6)
    },
    /** Move to (page, shared); false when onPage refused (the state stays). Each set remembers its own page. */
    go(page, shared = s.showShared) {
      const was = { page: s.page, shared: s.showShared };
      if (page === was.page && shared === was.shared) return true;
      const memWas = s.mem[was.shared];
      s.page = page; s.showShared = shared; s.pages = Math.max(s.pages, page + 1); s.mem[was.shared] = was.page;   // before onPage: its save reads the personal page last shown
      if (onPage && onPage(was.page, page, was.shared, shared) === false) { s.page = was.page; s.showShared = was.shared; s.mem[was.shared] = memWas; return false; }
      return true;
    },
    /** Returns true when the click hit the control (and pages it). shift = PlugY's first / last / by-100 variants. */
    click(px, py, w, h, g, open, shift = false) {
      if (!s.enabled || !open || !g) return false;
      const o = scaleOf(w, h), x = (px - o.x) / o.k, y = (py - o.y) / o.k;
      if (ready()) {
        const hit = LIVE.find((n) => x >= BTN[n].x && x < BTN[n].x + 32 && y >= BTN[n].y && y < BTN[n].y + 32);
        if (!hit) return false;
        if (hit === 'putGold' || hit === 'takeGold') { if (onGold) onGold(hit === 'putGold' ? 'put' : 'take'); }
        else if (hit === 'shared') s.go(s.mem[!s.showShared], !s.showShared);
        else s.go(pageTarget(hit, s.page, shift, lastUsed(s.showShared)));
        return true;
      }
      return false;
    },
  });
}
