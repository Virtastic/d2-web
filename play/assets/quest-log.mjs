// The quest log (Q) and the message log (M): the two mini-panel icons that used to press and open nothing
//. Layout is retail's own draw list (the retail captures,
// msglog.txt) -- every cel file, frame and anchor below is what retail 1.13c drew; OpenDiablo2's
// quest_log.go (frames 24/25/26, description labels) is the cross-check. The engine decides: the quest
// state is the player's quest record (d2_session_quest_record, packet 0x28's words); this draws it.
import { decodeDatPalette, decodeDc6 } from './d2-formats.mjs';
import { createFont, decodeFontTbl, textColourPalette } from './frontend.mjs';
import { createPanelChrome, BORDER_LEFT } from './panel-chrome.mjs';

const MENU = 'data\\global\\ui\\MENU\\';
export const QUEST_ART = { bg: MENU + 'questbackground.dc6', tabs: MENU + 'expquesttabs.dc6', sockets: MENU + 'questsockets.dc6',
  desc: MENU + 'questlast.dc6', close: 'data\\global\\ui\\PANEL\\buysellbtn.dc6', icon: (act, q) => `${MENU}a${act}q${q}.dc6` };
export const FRAME = Object.freeze({ done: 24, progress: 25, locked: 26 });     // a?q?.dc6 static frames (OpenDiablo2; 24 and 26 seen in retail)
// Quests per act, and where each sits on the 3x2 board as [column, row] -- retail's own placement
// (Act I and III are not in number order: the retail captures, questlog-tab3.txt).
export const QUESTS_IN_ACT = [6, 6, 6, 3, 6];
const SEQ = [[0, 0], [1, 0], [2, 0], [0, 1], [1, 1], [2, 1]];
export const SLOTS = [[[0, 0], [1, 0], [1, 1], [2, 0], [0, 1], [2, 1]], SEQ, [[0, 1], [2, 0], [1, 0], [0, 0], [1, 1], [2, 1]], SEQ, SEQ];
export const QL = Object.freeze({ left: 80, top: 61, tabX: 85, tabPitch: 62, tabY: 93, iconX: 106, iconPitch: 97, iconY: 181, rowPitch: 97,
  titleY: 308, descX: 96, descY: 330, descPitch: 20, titleCentre: 240, wrap: 250, closeX: 358, closeY: 482, descBtnX: 306, descBtnY: 482 });
// The quest words of one act start at 8*(act-1); word 0 is the intro, 1..n the quests. Act 4's "act
// done" word is 4, the others' 7. Retail's log treats a finished act as all quests completed (its
// frames for a save whose acts I-III were done: every icon 24).
export const questStatus = (words, act, q) => {
  const base = 8 * (act - 1), done = words[base + (act === 4 ? 4 : 7)] & 1, w = words[base + q] | 0;
  return done || (w & 0x8001) ? 'done' : w ? 'progress' : 'locked';
};
const actPalette = (act) => `data\\global\\palette\\ACT${Math.min(5, Math.max(1, act | 0))}\\pal.dat`;
const origin = (width, height) => { const scale = Math.min(1, width / 800, height / 600); return { scale, x: (width - 800 * scale) / 2, y: (height - 600 * scale) / 2 }; };

export function createQuestLogRenderer(read, documentObject = globalThis.document) {
  const chrome = createPanelChrome(read, documentObject);
  const fontDc6 = decodeDc6(read('data\\local\\font\\latin\\fontformal11.dc6')), tbl = decodeFontTbl(read('data\\local\\font\\latin\\fontformal11.tbl'));
  const fonts = new Map();
  const fontFor = (act) => fonts.get(act) ?? (fonts.set(act, createFont({ dc6: fontDc6, tbl, palette: decodeDatPalette(read(actPalette(act))), documentObject,
    path: 'data\\local\\font\\latin\\FontFormal11.dc6', drawAs: { op: 'celcolor', mode: 5 } })), fonts.get(act));
  const rect = (act, i) => { const [c, r] = SLOTS[act - 1][i]; return { x: QL.iconX + c * QL.iconPitch - 4, y: QL.iconY + r * QL.rowPitch - 90, w: 80, h: 95 }; };
  const wrap = (font, text) => {
    const lines = []; let cur = '';
    for (const word of String(text).match(/\S+\s*/g) || []) { if (cur && font.measure((cur + word).trimEnd()) > QL.wrap) { lines.push(cur); cur = ''; } cur += word; }
    return cur ? [...lines, cur] : lines;
  };
  return {
    rect,
    /** What a canvas point lands on: {tab: act} | {quest: number} | {close: true} | null. */
    hit(px, py, width, height, shownActs, act) {
      const o = origin(width, height), x = (px - o.x) / o.scale, y = (py - o.y) / o.scale;
      for (const a of shownActs) { const tx = QL.tabX + QL.tabPitch * (a - 1); if (x >= tx && x < tx + QL.tabPitch && y >= 62 && y < QL.tabY) return { tab: a }; }
      for (let i = 0; i < QUESTS_IN_ACT[act - 1]; i++) { const r = rect(act, i); if (x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h) return { quest: i + 1 }; }
      if (x >= QL.closeX && x < QL.closeX + 30 && y >= QL.closeY - 30 && y <= QL.closeY) return { close: true };
      return null;
    },
    /** `s`: { act (the shown tab), tabs (acts drawn), words (the 48 record words), selected (1..n | 0), string(key) }. */
    draw(ctx, width, height, s, open, paletteAct = s?.act ?? 1) {
      if (!open) return null;
      const o = origin(width, height), A = paletteAct, blits = [], cel = (path, f, x, y) => { const r = chrome.cel(ctx, A, path, f, x, y); if (r) blits.push(r); return r; };
      ctx.save(); ctx.translate(o.x, o.y); ctx.scale(o.scale, o.scale);
      cel(QUEST_ART.bg, 0, 80, 316); cel(QUEST_ART.bg, 1, 336, 316); cel(QUEST_ART.bg, 2, 80, 492); cel(QUEST_ART.bg, 3, 336, 492);
      for (const a of s.tabs) cel(QUEST_ART.tabs, 2 * (a - 1) + (a === s.act ? 0 : 1), QL.tabX + QL.tabPitch * (a - 1), QL.tabY);
      const statuses = [];
      for (let q = 1; q <= QUESTS_IN_ACT[s.act - 1]; q++) {
        const status = questStatus(s.words, s.act, q), [c, r] = SLOTS[s.act - 1][q - 1], on = s.selected === q;
        cel(QUEST_ART.icon(s.act, q), FRAME[status], QL.iconX + c * QL.iconPitch, QL.iconY + r * QL.rowPitch);
        cel(QUEST_ART.sockets, on ? 1 : 0, QL.iconX + c * QL.iconPitch - 4, QL.iconY + r * QL.rowPitch + 5 - (on ? 1 : 0));
        statuses.push(status);
      }
      let text = null;
      if (s.selected && statuses[s.selected - 1] !== 'locked') {
        const font = fontFor(A), key = `qstsa${s.act}q${s.selected}`, name = s.string(key);
        const body = statuses[s.selected - 1] === 'done' ? s.string('qstsprevious') : s.string(key + '1');
        font.drawAnchored(ctx, name, QL.titleCentre - (font.measure(name) >> 1), QL.titleY);
        const lines = body && body !== key + '1' ? wrap(font, body) : [];
        lines.forEach((l, i) => font.drawAnchored(ctx, l, QL.descX, QL.descY + i * QL.descPitch));
        text = { name, lines };
      }
      cel(QUEST_ART.close, 10, QL.closeX, QL.closeY); cel(QUEST_ART.desc, 0, QL.descBtnX, QL.descBtnY);
      BORDER_LEFT.forEach(([i, x, y]) => cel('data\\global\\ui\\PANEL\\800BorderFrame.dc6', i, x, y));
      ctx.restore();
      return { act: s.act, tabs: [...s.tabs], statuses, selected: s.selected, text, blits };
    },
  };
}

// ---- the message log ----------------------------------------------------------------------------
// Retail: a translucent black box (5,30)-(785,500), two nested boxpieces frames, a scroll bar
// (textslid) on the right, a gold Font16 title and a Close label. The record counts are 400 boxpieces
// and 33 textslid cels -- generated below from the measured cycles, not listed.
const BOX = MENU + 'boxpieces.dc6', SLID = MENU + 'textslid.dc6';
const TOP = [3, 4, 5, 6, 7, 2], BOT = [20, 21, 16, 17, 18, 19], LEFT = [11, 12, 10], RIGHT = [13, 14, 15];
export function messageLogPieces() {
  const out = [];
  for (const [top, bot, left, right] of [[42, 510, [52, 38], [54, 38]], [77, 475, [87, 32], [89, 32]]]) {
    const corners = top === 42 ? 501 : 466;
    out.push([0, 5, top], [1, 773, top]);
    for (let i = 0; i < 63; i++) out.push([TOP[i % 6], 17 + 12 * i, top]);
    for (let j = 0; j < left[1]; j++) out.push([LEFT[j % 3], 1, left[0] + 12 * j]);
    for (let j = 0; j < right[1]; j++) out.push([RIGHT[j % 3], 778, right[0] + 12 * j]);
    out.push([8, 5, corners], [9, 773, corners]);
    for (let i = 0; i < 63; i++) out.push([BOT[i % 6], 18 + 12 * i, bot]);
  }
  return out;
}
export function createMessageLogRenderer(read, documentObject = globalThis.document) {
  const chrome = createPanelChrome(read, documentObject);
  const fontDc6 = decodeDc6(read('data\\local\\font\\latin\\font16.dc6')), tbl = decodeFontTbl(read('data\\local\\font\\latin\\font16.tbl'));
  const fonts = new Map();
  const fontFor = (act) => fonts.get(act) ?? (() => {
    const pal = decodeDatPalette(read(actPalette(act)));
    let pl2 = null; try { pl2 = read(actPalette(act).replace(/pal\.dat$/i, 'pal.pl2')); } catch { /* the plain palette then */ }
    const mk = (p, extra = 0) => createFont({ dc6: fontDc6, tbl, palette: p, documentObject, path: 'data\\local\\font\\latin\\font16.dc6', drawAs: { op: 'celcolor', mode: 5, extra } });
    const f = { white: mk(pal), gold: mk(pl2 ? textColourPalette(pl2, pal, 4) : pal, 4) }; fonts.set(act, f); return f; })();
  return {
    /** The Close label's rectangle (the button retail draws under its cursor is a 32x29 orotate cel). */
    closeRect: { x: 355, y: 462, w: 80, h: 30 },
    hitClose(px, py, width, height) { const o = origin(width, height), x = (px - o.x) / o.scale, y = (py - o.y) / o.scale, r = this.closeRect; return x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h; },
    draw(ctx, width, height, s, open, act = 1) {
      if (!open) return null;
      const o = origin(width, height), blits = [], f = fontFor(act);
      ctx.save(); ctx.translate(o.x, o.y); ctx.scale(o.scale, o.scale);
      ctx.fillStyle = 'rgba(0,0,0,0.6)'; ctx.fillRect(5, 30, 780, 470);          // retail: box 5 30 785 500, mode 1 (alpha)
      for (const [fr, x, y] of messageLogPieces()) { const r = chrome.cel(ctx, act, BOX, fr, x, y); if (r) blits.push(r); }
      const slid = (fr, y) => { const r = chrome.cel(ctx, act, SLID, fr, 767, y); if (r) blits.push(r); };
      slid(9, 81); for (let y = 93; y <= 453; y += 12) slid(13, y); slid(8, 453);
      f.gold.drawAnchored(ctx, s.title, 349, 51); f.white.drawAnchored(ctx, s.close, 376, 486);
      s.lines.slice(-30).forEach((l, i) => f.white.drawAnchored(ctx, l, 20, 100 + 15 * i));
      ctx.restore();
      return { title: s.title, lines: s.lines.length, blits };
    },
  };
}
