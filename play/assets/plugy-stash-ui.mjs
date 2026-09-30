// PlugY shared-stash page control over the retail stash panel (PY1, PY3). OPT-IN: ?plugy=1 (default page is strict retail; nothing here draws). Page 0 is the engine's own stash; pages 1.. are
// .sss pages (plugy-stash.mjs). Art: PlugY's own StashBtns.dc6 / SharedGoldBtns.dc6, laid out by reference/PlugY Interface_Stash.cpp
// (defaults for the 10x10 BigStash at 800x600: x = WindowStartX + v, y = the sprite's BOTTOM edge = 600 - 60 - v; WindowStartX = 80 measured
// against retail/frames/plugy-stash-r29.png, s869). The panel behind them is PlugY's TradeStash.dc6 (stash-panel.mjs setArt); until the art loads
// the plain canvas control stays. The shell swaps a page in/out of the engine stash via d2_session_stash_export/clear/insert (JM bytes).
// Only previous/next page are live; the index, shared-toggle and shared-gold buttons are drawn as PlugY draws them but inert (PY3 blockers).
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
const LIVE = ['prev', 'next', 'shared'];
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
export function createPlugyStash({ enabled = false, pages = 2, onPage = null } = {}) {
  const s = { enabled, pages, page: 0, showShared: false, hover: null, text: null };
  const rect = (g) => ({ x: g.left + 96, y: g.top + 8, w: 96, h: 18 });
  s.art = null;   // { stash: canvas[], gold: canvas[] } once StashBtns.dc6 / SharedGoldBtns.dc6 are decoded (setArt)
  s.down = null;
  const scaleOf = (w, h) => { const k = Math.min(1, w / 800, h / 600); return { k, x: (w - 800 * k) / 2, y: (h - 600 * k) / 2 }; };
  const ready = () => s.art?.stash?.length >= 12 && s.art?.gold?.length >= 4;
  return Object.assign(s, {
    rect, BTN,
    /** art: { stash: canvases of StashBtns.dc6, gold: canvases of SharedGoldBtns.dc6 }, in frame order. */
    setArt(art) { s.art = art; },
    /** Track the pointer (canvas px) for the hover popup. */
    pointer(px, py, w, h) { const o = scaleOf(w, h); s.hover = px == null ? null : { x: (px - o.x) / o.k, y: (py - o.y) / o.k }; },
    draw(ctx, w, h, g, open, act = 1) {
      if (!s.enabled || !open || !g) return null;
      const r = rect(g), o = scaleOf(w, h);
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
      ctx.fillStyle = '#000'; ctx.fillRect(r.x, r.y, r.w, r.h);
      ctx.fillStyle = '#c7b377'; ctx.font = '12px sans-serif'; ctx.textAlign = 'center'; ctx.textBaseline = 'middle';
      ctx.fillText('<', r.x + 10, r.y + 9); ctx.fillText(`${s.page + 1}/${s.pages}`, r.x + 48, r.y + 9); ctx.fillText('>', r.x + 86, r.y + 9);
      ctx.restore();
      return { ...r, page: s.page, pages: s.pages };
    },
    /** Returns true when the click hit the control (and pages it). */
    click(px, py, w, h, g, open) {
      if (!s.enabled || !open || !g) return false;
      const r = rect(g), o = scaleOf(w, h), x = (px - o.x) / o.k, y = (py - o.y) / o.k;
      if (ready()) {
        const hit = LIVE.find((n) => x >= BTN[n].x && x < BTN[n].x + 32 && y >= BTN[n].y && y < BTN[n].y + 32);
        if (!hit) return false;
        if (hit === 'shared') { s.showShared = !s.showShared; return true; }   // PY3: the toggle shows the measured shared state (header red, popup); separate shared/personal page sets are PART
        const was = s.page;
        s.page = (s.page + (hit === 'next' ? 1 : s.pages - 1)) % s.pages;
        if (s.page !== was && onPage && onPage(was, s.page) === false) s.page = was;
        return true;
      }
      if (x < r.x || x >= r.x + r.w || y < r.y || y >= r.y + r.h) return false;
      const was = s.page;
      if (x < r.x + 32) s.page = (s.page + s.pages - 1) % s.pages; else if (x >= r.x + 64) s.page = (s.page + 1) % s.pages;
      if (s.page !== was && onPage && onPage(was, s.page) === false) s.page = was;   // a refused swap (engine insert failed, nothing lost) stays on the page
      return true;
    },
  });
}
