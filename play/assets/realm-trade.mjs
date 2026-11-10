// Player-to-player trade in a joined game: the request boxes and the two-grid trade screen around the state
// machine in bnet-game-trade.mjs. The screen is the left panel (art ui\panel\trade, the partner's offer in the
// upper grid = inventory page 1, the player's own in the lower = page 2) beside the open inventory. The grids
// are painted by the stash panel's renderer from the items the server placed; this file adds the accept and
// cancel marks, the names and gold figures, and routes presses. Every packet leaves through `send`.
import { newTrade, applyTradePacket, tradeAnswer, tradeMouseDown, tradeMouseUp, tradeSetGold, tradeItemAction, tradeTick,
  tradeMarks, tradeButtonAt, tradeTip, tradeGoldOpen, tradeGoldClose, takeTradeEvents, TRADE_STR, TRADE_STATE } from './bnet-game-trade.mjs';
import { resolveUnitSound } from './unit-sound-resolver.mjs';
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
// The request box is the client's generic menu box (the one a town NPC's menu uses), anchored at screen point
// (320, 100) whatever the screen size: as wide as its widest line plus 20 and centred on the anchor, 15 px a line
// plus 15, its top 21 above the anchor for the one line that is not a button. A half-black box sits behind it.
// GUESS: the frame pieces round the box (the NPC menu's boxpieces) are not painted here, and the line under the
// pointer is not turned blue; sharing the NPC menu's painter settles both.
export const TRADE_BOX = Object.freeze({ centre: 320, anchor: 100, title: 21, pitch: 15, padX: 20, padY: 15 });
/** The box for lines of the given Font16 widths: its rectangle and each line's text left edge and bottom row. */
export function tradeBoxLayout(widths) {
  const B = TRADE_BOX, w = Math.max(0, ...widths) + B.padX, x = B.centre - Math.trunc(w / 2), y = B.anchor - B.title;
  return { x, y, w, h: widths.length * B.pitch + B.padY,
    lines: widths.map((lw, i) => ({ x: x + (lw < w ? Math.trunc((w - lw + 1) / 2) + 1 : 0), y: y + B.pitch * (i + 1), w: lw })) };
}

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
 * `onLeave()` when an unanswered cancel ends the game, `onSound(row, event)` a Sounds.txt row to play, and
 * `classOf(who, guid)` the hero class (0..6) of 'own' or of the 'partner' unit, null when that unit is not in view.
 */
export function createRealmTrade({ send, now = () => 0, string = () => '', carried = () => 0, measure = (s) => 8 * String(s).length,
  onOpen = null, onClose = null, onLeave = null, onSound = null, classOf = () => null }) {
  let t = newTrade();
  const flush = (out) => { for (const b of out) send(b); return out.length; };
  // What the step asked to be heard. A hero's line is that hero's own voice row; a hero not in view says nothing.
  const hear = () => {
    for (const e of takeTradeEvents(t)) {
      if (!onSound) continue;
      if (e.sound != null) { onSound(e.sound, e); continue; }
      const classId = classOf(e.who, e.guid);
      const row = classId == null ? null : resolveUnitSound({ unitType: 0, classId, soundEvent: e.speech, isLocalPlayer: e.who === 'own' });
      if (row != null) onSound(row, e);
    }
  };
  // The screen's edges, noticed after every step so the shell opens and closes the inventory with it.
  const step = (fn) => {
    const ui = t.ui, panel = t.panel, result = fn();
    if (!panel && t.panel && onOpen) onOpen();
    if (ui && !t.ui && onClose) onClose();
    hear();
    return result;
  };
  const boxLines = () => {
    if (!t.box) return [];
    const lines = t.box === 'asked'
      ? [{ text: string(TRADE_STR.ASKED) }, { text: string(TRADE_STR.ACCEPT), answer: true }, { text: string(TRADE_STR.CANCEL), answer: false }]
      : [{ text: string(TRADE_STR.ASKING) }, { text: string(TRADE_STR.CANCEL), answer: false }];
    const box = tradeBoxLayout(lines.map((l) => measure(l.text)));
    return lines.map((l, i) => ({ ...l, ...box.lines[i] }));
  };
  const boxAt = (px, py, width, height) => {
    const o = origin(width, height), x = (px - o.x) / o.scale, y = (py - o.y) / o.scale;
    return boxLines().find((l) => 'answer' in l && x >= l.x && x < l.x + l.w && y > l.y - TRADE_BOX.pitch && y <= l.y) || null;
  };
  const inOwnGrid = (x, y) => { const g = t.ownGrid; return Boolean(g) && x >= g.x0 && x < g.x1 && y >= g.y0 && y < g.y1; };

  return {
    get state() { return t; },
    get open() { return t.panel; },
    get box() { return t.box; },
    reset() { step(() => { t = Object.assign(t, newTrade()); }); },
    /** A server trade packet (0x77, 0x78, 0x79). */
    feed(bytes, env = {}) { return step(() => flush(applyTradePacket(t, bytes, { now: now(), ...env }))); },
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
      step(() => flush(tradeMouseDown(t, tradeButtonAt(p.x, p.y), { cursorItem })));
      return true;
    },
    /** The inventory's coin going down: it drops a held accept without telling the server (the gold box follows). */
    goldPress() { if (t.panel) step(() => tradeMouseDown(t, 'gold')); },
    /** The coin released: true when the gold box may open (opening it withdraws an accept). */
    goldOpen(env = {}) { const r = tradeGoldOpen(t, env); flush(r.send); return r.open; },
    /** The gold box closed without its OK. */
    goldClosed() { tradeGoldClose(t); },
    /** The hover text at a canvas point: { text, x, y, left: true } (the plate's left edge and anchor row on the 800x600 screen), or null. */
    tip(px, py, width, height) {
      const p = tradePoint(px, py, width, height), r = tradeTip(t, p.x, p.y);
      return r ? { text: string(r.string), x: r.x + TRADE_ORIGIN.x, y: r.y + TRADE_ORIGIN.y, left: true } : null;
    },
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
    setGold(amount) { return t.panel ? step(() => flush(tradeSetGold(t, amount, carried()))) : 0; },
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
        const o = origin(width, height), lines = boxLines(), box = tradeBoxLayout(lines.map((l) => chrome.measure(act, l.text)));
        ctx.save(); ctx.translate(o.x, o.y); ctx.scale(o.scale, o.scale);
        ctx.fillStyle = 'rgba(0,0,0,0.5)'; ctx.fillRect(box.x, box.y, box.w, box.h);
        lines.forEach((l, i) => chrome.text(ctx, act, l.text, box.lines[i].x, box.lines[i].y));
        ctx.restore();
        evidence.box = [box.x, box.y, box.w, box.h];
        evidence.lines = lines.map((l) => l.text);
      }
      return evidence;
    },
  };
}
