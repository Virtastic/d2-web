// Player-to-player trade in a joined game: the request boxes and the two-grid trade screen around the state
// machine in bnet-game-trade.mjs. The screen is the left panel (art ui\panel\trade, the partner's offer in the
// upper grid = inventory page 1, the player's own in the lower = page 2) beside the open inventory. The grids
// are painted by the stash panel's renderer from the items the server placed; this file adds the accept and
// cancel marks, the names and gold figures, and routes presses. Every packet leaves through `send`.
import { newTrade, applyTradePacket, tradeAnswer, tradeMouseDown, tradeMouseUp, tradeSetGold, tradeItemAction, tradeTick,
  tradeMarks, tradeButtonAt, TRADE_STR, TRADE_STATE } from './bnet-game-trade.mjs';
import { createStashPanelRenderer } from './stash-panel.mjs';
import { createPanelChrome, CHROME_ART } from './panel-chrome.mjs';

export const TRADE_ART = 'data\\global\\ui\\panel\\trade.dc6';
export const TRADE_PAGE = Object.freeze({ THEIRS: 1, OWN: 2 });
// Where the 640x480 layout sits on the 800x600 screen.
export const TRADE_ORIGIN = Object.freeze({ x: 80, y: 60 });
// The panel's cels and text in the 640x480 layout: a cel by its left edge and bottom row, text by its bottom row.
export const TRADE_LAYOUT = Object.freeze({
  theirMark: [275, 202], accept: [223, 416], cancel: [275, 416],
  theirName: [25, 27], ownName: [25, 242], nameMax: 112, goldLabelCentre: 187, goldRight: 292, theirRow: 27, ownRow: 242,
});
// GUESS: the request box's line positions. The retail box is built at (320, 100) of the 640 layout by the client's
// generic message-box code, whose line metrics are not ported; reading that builder settles the numbers.
export const TRADE_BOX = Object.freeze({ centre: 400, first: 160, pitch: 24, height: 16 });

/** The renderers the screen paints with: both grids and the shared chrome (buttons, Font16). */
export function createTradeRenderers(read, documentObject = globalThis.document) {
  return {
    own: createStashPanelRenderer(read, documentObject, { page: TRADE_PAGE.OWN, art: TRADE_ART, close: false }),
    theirs: createStashPanelRenderer(read, documentObject, { page: TRADE_PAGE.THEIRS, art: TRADE_ART, bare: true }),
    chrome: createPanelChrome(read, documentObject),
  };
}

const origin = (width, height) => {
  const scale = Math.min(1, width / 800, height / 600);
  return { scale, x: (width - 800 * scale) / 2, y: (height - 600 * scale) / 2 };
};
/** A canvas point in the 640x480 layout the client's hit tests use. */
export function tradePoint(px, py, width, height) {
  const o = origin(width, height);
  return { x: (px - o.x) / o.scale - TRADE_ORIGIN.x, y: (py - o.y) / o.scale - TRADE_ORIGIN.y };
}

/**
 * `send(bytes)`, `now()`, `string(index)` the string table, `carried()` the gold the player carries,
 * `measure(text)` Font16 width (for the box's hit test), `onOpen()`/`onClose()` when the screen appears and goes,
 * `onLeave()` when an unanswered cancel ends the game.
 */
export function createRealmTrade({ send, now = () => 0, string = () => '', carried = () => 0, measure = (s) => 8 * String(s).length,
  onOpen = null, onClose = null, onLeave = null }) {
  let t = newTrade();
  const flush = (out) => { for (const b of out) send(b); return out.length; };
  // The screen's edges, noticed after every step so the shell opens and closes the inventory with it.
  const step = (fn) => {
    const ui = t.ui, panel = t.panel, result = fn();
    if (!panel && t.panel && onOpen) onOpen();
    if (ui && !t.ui && onClose) onClose();
    return result;
  };
  const boxLines = () => {
    if (!t.box) return [];
    const lines = t.box === 'asked'
      ? [{ text: string(TRADE_STR.ASKED) }, { text: string(TRADE_STR.ACCEPT), answer: true }, { text: string(TRADE_STR.CANCEL), answer: false }]
      : [{ text: string(TRADE_STR.ASKING) }, { text: string(TRADE_STR.CANCEL), answer: false }];
    return lines.map((l, i) => { const w = measure(l.text); return { ...l, x: TRADE_BOX.centre - (w >> 1), y: TRADE_BOX.first + i * TRADE_BOX.pitch, w }; });
  };
  const boxAt = (px, py, width, height) => {
    const o = origin(width, height), x = (px - o.x) / o.scale, y = (py - o.y) / o.scale;
    return boxLines().find((l) => 'answer' in l && x >= l.x && x < l.x + l.w && y > l.y - TRADE_BOX.height && y <= l.y) || null;
  };
  const inOwnGrid = (x, y) => { const g = t.ownGrid; return Boolean(g) && x >= g.x0 && x < g.x1 && y >= g.y0 && y < g.y1; };

  return {
    get state() { return t; },
    get open() { return t.panel; },
    get box() { return t.box; },
    reset() { step(() => { t = Object.assign(t, newTrade()); }); },
    /** A server trade packet (0x77, 0x78, 0x79). */
    feed(bytes, env = {}) { return step(() => flush(applyTradePacket(t, bytes, env))); },
    /** The lower grid's rectangle in canvas pixels of the 800x600 screen, from its Inventory.txt geometry. */
    setOwnGrid(g) { t.ownGrid = g ? { x0: g.gridLeft, y0: g.gridTop, x1: g.gridLeft + g.columns * g.boxW, y1: g.gridTop + g.rows * g.boxH } : null; },
    /** A press while a request box is up: its buttons answer, and the box swallows every press. */
    boxPress(px, py, width, height) {
      if (!t.box) return false;
      const line = boxAt(px, py, width, height);
      if (line) step(() => flush(tradeAnswer(t, line.answer, now())));
      return true;
    },
    /** Answer the box without a pointer (keys): true accepts a request, false cancels. */
    answer(accept) { return step(() => flush(tradeAnswer(t, accept, now()))); },
    /** The left button down on the panel outside its grids. */
    press(px, py, width, height, { cursorItem = false } = {}) {
      if (!t.panel) return false;
      const p = tradePoint(px, py, width, height);
      flush(tradeMouseDown(t, tradeButtonAt(p.x, p.y), { cursorItem }));
      return true;
    },
    /** The inventory's coin going down: it drops a held accept without telling the server (the gold box follows). */
    goldPress() { if (t.panel) tradeMouseDown(t, 'gold'); },
    /** The left button up anywhere while the panel is open. */
    release(px, py, width, height) {
      if (!t.panel) return false;
      const o = origin(width, height), p = tradePoint(px, py, width, height);
      const where = inOwnGrid((px - o.x) / o.scale, (py - o.y) / o.scale) ? 'ownGrid' : tradeButtonAt(p.x, p.y);
      step(() => flush(tradeMouseUp(t, where, now())));
      return true;
    },
    /** The inventory was closed (its close button, Escape): the trade is cancelled. */
    closed() { if (t.panel && t.state !== TRADE_STATE.WAIT) step(() => flush(tradeMouseUp(t, 'close', now()))); },
    /** The gold box's OK. */
    setGold(amount) { return t.panel ? flush(tradeSetGold(t, amount, carried())) : 0; },
    /** The inventory's item gate: false refuses the action (a reply is awaited; the accept is withdrawn). */
    itemAction() {
      const r = step(() => tradeItemAction(t, now()));
      flush(r.send);
      if (r.leave && onLeave) onLeave();
      return r.allowed;
    },
    tick(env) {
      const r = step(() => tradeTick(t, now(), env));
      flush(r.send);
      if (r.leave && onLeave) onLeave();
    },
    boxLines,
    /**
     * Paint the screen and the request box. `view`: { items, act, layout: {theirs, own}, hover, ownName }.
     * Returns what was painted, for the frame's evidence.
     */
    draw(ctx, width, height, renderers, view) {
      const act = view.act || 1, chrome = renderers.chrome, evidence = { state: t.state, panel: t.panel, box: t.box };
      if (t.panel && view.layout) {
        this.setOwnGrid(view.layout.own);
        evidence.own = renderers.own.draw(ctx, width, height, view.layout.own, view.items, true, act, false, view.hover);
        evidence.theirs = renderers.theirs.draw(ctx, width, height, view.layout.theirs, view.items, true, act, false, view.hover ? { ...view.hover, cursorItem: null } : null);
        const o = origin(width, height), L = TRADE_LAYOUT, X = TRADE_ORIGIN.x, Y = TRADE_ORIGIN.y, marks = tradeMarks(t);
        ctx.save(); ctx.translate(o.x, o.y); ctx.scale(o.scale, o.scale);
        const mark = (m, [x, y]) => {
          chrome.cel(ctx, act, CHROME_ART.close, m.frame, x + X, y + Y);
          // GUESS: the client draws these cels through a palette colour shift (1 red, 2 green); here it is a flat
          // multiply over the cel's square. Drawing through the act palette's shift table settles the exact colours.
          if (m.tint) { ctx.save(); ctx.globalCompositeOperation = 'multiply'; ctx.fillStyle = m.tint === 'red' ? '#ff5050' : '#50ff50'; ctx.fillRect(x + X, y + Y - 31, 32, 32); ctx.restore(); }
        };
        mark(marks.theirs, L.theirMark); mark(marks.accept, L.accept); mark(marks.cancel, L.cancel);
        const fit = (s) => { let name = String(s || ''); while (name && chrome.measure(act, name) > L.nameMax) name = name.slice(0, -1); return name; };
        const label = string(TRADE_STR.GOLD), half = chrome.measure(act, label) >> 1;
        const row = (name, gold, at, y) => {
          chrome.text(ctx, act, fit(name), at[0] + X, at[1] + Y);
          chrome.text(ctx, act, label, L.goldLabelCentre - half + X, y + Y);
          const figure = String(gold >>> 0);
          chrome.text(ctx, act, figure, L.goldRight - chrome.measure(act, figure) + X, y + Y);
        };
        row(t.partner, t.theirGold, L.theirName, L.theirRow);
        row(view.ownName, t.ownGold, L.ownName, L.ownRow);
        ctx.restore();
        evidence.marks = marks; evidence.gold = [t.ownGold, t.theirGold]; evidence.partner = t.partner;
      }
      if (t.box) {
        const o = origin(width, height), lines = boxLines();
        ctx.save(); ctx.translate(o.x, o.y); ctx.scale(o.scale, o.scale);
        const w = Math.max(...lines.map((l) => chrome.measure(act, l.text))) + 32, top = TRADE_BOX.first - TRADE_BOX.height - 10;
        ctx.fillStyle = 'rgba(0,0,0,0.8)'; ctx.fillRect(TRADE_BOX.centre - (w >> 1), top, w, lines.length * TRADE_BOX.pitch + 12);
        for (const l of lines) chrome.text(ctx, act, l.text, TRADE_BOX.centre - (chrome.measure(act, l.text) >> 1), l.y);
        ctx.restore();
        evidence.lines = lines.map((l) => l.text);
      }
      return evidence;
    },
  };
}
