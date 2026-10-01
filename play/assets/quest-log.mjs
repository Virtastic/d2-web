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
export const FRAME = Object.freeze({ done: 24, progress: 0, locked: 26 });     // a?q?.dc6 static frames (retail's own: 24 done, 26 locked, 0 in progress -- the retail captures)
// Quests per act, and where each sits on the 3x2 board as [column, row] -- retail's own placement
// (Act I and III are not in number order: the retail captures, questlog-tab3.txt).
export const QUESTS_IN_ACT = [6, 6, 6, 3, 6];
const SEQ = [[0, 0], [1, 0], [2, 0], [0, 1], [1, 1], [2, 1]];
export const SLOTS = [[[0, 0], [1, 0], [1, 1], [2, 0], [0, 1], [2, 1]], SEQ, [[0, 1], [2, 0], [1, 0], [0, 0], [1, 1], [2, 1]], SEQ, SEQ];
export const QL = Object.freeze({ left: 80, top: 61, tabX: 85, tabPitch: 62, tabY: 93, iconX: 106, iconPitch: 97, iconY: 181, rowPitch: 97,
  titleY: 308, descX: 96, descY: 330, descPitch: 20, titleCentre: 240, wrap: 250, closeX: 358, closeY: 482, descBtnX: 306, descBtnY: 482 });
// The quest words of one act (D2MOO Quests.h D2QuestStateFlagIds). Acts I-IV start at 8*(act-1) and
// carry a trailing COMPLETED word at offset 7 (Act IV, four words, at 4); word 0 is the intro, 1..n the
// quests. **Act V does not**: its six quests are words 35..40 (QUESTSTATEFLAG_A5Q1..A5Q6), 33 is A4Q4,
// 34 the enum's unused slot, and there is NO A5COMPLETED -- MAX_QUEST_STATUS = 41 is A5INTRO, one past
// the last word (F411). So the base is per-act, not a formula, and Act V's "done"
// term is empty (a quest there is done only on its own REWARDGRANTED|COMPLETEDBEFORE bits). Retail's log
// treats a finished act as all its quests completed (every icon 24).
export const ACT_BASE = [0, 8, 16, 24, 34];
export const ACT_DONE = [7, 7, 7, 4, 0];   // the act's COMPLETED word offset from its base; 0 = Act V has none
export const questStatus = (words, act, q) => {
  const base = ACT_BASE[act - 1], off = ACT_DONE[act - 1], w = words[base + q] | 0;
  const done = (off ? words[base + off] & 1 : 0) || (w & 0x8001);
  return done ? 'done' : w ? 'progress' : 'locked';
};
const actPalette = (act) => `data\\global\\palette\\ACT${Math.min(5, Math.max(1, act | 0))}\\pal.dat`;
const POSTED_PAD = 4;   // retail's posted box right edge is 102 for 'Oracle: hello'
// The translucent black the message log, the chat bar and the posted line sit on. Retail draws it as a
// DrawBox colour 0 mode 1 (`box 5 30 785 500 color=0 mode=1`, `box 127 497 672 544 color=0 mode=1`,
// `box 11 6 102 22 color=0 mode=1`); the draw list carries the mode, not the alpha, so it is measured
// from retail's own pixels (F191): the log frame `msglog.png` beside the same scene undimmed
// (`cov-a4-town.png` -- the two agree to 0.26/255 outside the box) fits a blend toward black of 0.50
// (least squares over the box interior, k = 0.5019; the flat-region and bright-pixel medians, 0.53 and
// 0.525, are the same value seen through D2's index-space blend table). The previously assumed 0.6 is
// refuted: at 0.6, 16.9% of the box's pixels miss retail by more than 8/255; at 0.5 only 1.8% do.
export const DIM_ALPHA = 0.5;
// The quest-log completion animation (F191, questlog-open-act1-caps.anim): on open an icon whose quest is
// COMPLETING plays frames 0..24 of its a?q?.dc6 -- frame 0 held 4 game frames, then each frame 3 (~8.3 fps).
// "Completing" is retail's own QFLAG_COMPLETEDNOW (bit 14, D2C_OriginalQuestFlags), a state distinct from a
// finished quest: OpenDiablo2's quest_log.go plays the icon only while `status == QuestStatusCompleting` and
// moves it to Completed when the log closes (stopPlayedAnimations). A quest that is merely already done
// (COMPLETEDBEFORE) draws its static frame 24 -- which is why the landed rows s836/s2172, whose fixtures set
// only the done bits, still draw static frames.
export const QUEST_ANIM = (tick) => (tick < 4 ? 0 : Math.min(24, Math.floor((tick - 4) / 3) + 1));
export const COMPLETING = 0x4000;   // QFLAG_COMPLETEDNOW
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
    /** What a canvas point lands on: {tab: act} | {quest: number} | {descr: true} | {close: true} | null. */
    hit(px, py, width, height, shownActs, act) {
      const o = origin(width, height), x = (px - o.x) / o.scale, y = (py - o.y) / o.scale;
      for (const a of shownActs) { const tx = QL.tabX + QL.tabPitch * (a - 1); if (x >= tx && x < tx + QL.tabPitch && y >= 62 && y < QL.tabY) return { tab: a }; }
      for (let i = 0; i < QUESTS_IN_ACT[act - 1]; i++) { const r = rect(act, i); if (x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h) return { quest: i + 1 }; }
      // The description toggle button (questlast.dc6): a 30x30 cel anchored at (306,482), so it occupies
      // y 453..483 above its anchor -- the same convention as the close button beside it.
      if (x >= QL.descBtnX && x < QL.descBtnX + 30 && y >= QL.descBtnY - 30 && y <= QL.descBtnY) return { descr: true };
      if (x >= QL.closeX && x < QL.closeX + 30 && y >= QL.closeY - 30 && y <= QL.closeY) return { close: true };
      return null;
    },
    /** `s`: { act (the shown tab), tabs (acts drawn), words (the 48 record words), selected (1..n | 0), descrOn, string(key) }. */
    draw(ctx, width, height, s, open, paletteAct = s?.act ?? 1) {
      if (!open) return null;
      const o = origin(width, height), A = paletteAct, blits = [], cel = (path, f, x, y) => { const r = chrome.cel(ctx, A, path, f, x, y); if (r) blits.push(r); return r; };
      ctx.save(); ctx.translate(o.x, o.y); ctx.scale(o.scale, o.scale);
      cel(QUEST_ART.bg, 0, 80, 316); cel(QUEST_ART.bg, 1, 336, 316); cel(QUEST_ART.bg, 2, 80, 492); cel(QUEST_ART.bg, 3, 336, 492);
      for (const a of s.tabs) cel(QUEST_ART.tabs, 2 * (a - 1) + (a === s.act ? 0 : 1), QL.tabX + QL.tabPitch * (a - 1), QL.tabY);
      const statuses = [], iconFrames = [];
      for (let q = 1; q <= QUESTS_IN_ACT[s.act - 1]; q++) {
        const status = questStatus(s.words, s.act, q), [c, r] = SLOTS[s.act - 1][q - 1], on = s.selected === q;
        // A quest the engine reports COMPLETING plays frames 0..24 (s.anim is the open's own tick and the
        // set of quests retail would animate -- s836/s2172, which set only the done bits, animate nothing).
        const frame = s.anim && s.anim.quests.has(`${s.act}q${q}`) ? QUEST_ANIM(s.anim.tick) : FRAME[status];
        cel(QUEST_ART.icon(s.act, q), frame, QL.iconX + c * QL.iconPitch, QL.iconY + r * QL.rowPitch);
        cel(QUEST_ART.sockets, on ? 1 : 0, QL.iconX + c * QL.iconPitch - 4, QL.iconY + r * QL.rowPitch + 5 - (on ? 1 : 0));
        statuses.push(status); iconFrames.push(frame);
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
      cel(QUEST_ART.close, 10, QL.closeX, QL.closeY);
      // The description toggle (questlast.dc6, 1 direction x 2 frames per its own DC6 header = a toggle
      // button, OpenDiablo2's ButtonTypeQuestDescr with Toggleable:true). Retail's own captures all show
      // frame 0, so the OFF state is measured and frame 1 is the button's toggled art. What retail
      // changes besides the button's own frame is not evidenced (OpenDiablo2's onDescrClicked is a stub)
      // -- documented in V8d as needing a capture; this draws the toggle's state.
      const descrFrame = s.descrOn ? 1 : 0;
      cel(QUEST_ART.desc, descrFrame, QL.descBtnX, QL.descBtnY);
      BORDER_LEFT.forEach(([i, x, y]) => cel('data\\global\\ui\\PANEL\\800BorderFrame.dc6', i, x, y));
      ctx.restore();
      return { act: s.act, tabs: [...s.tabs], statuses, iconFrames, selected: s.selected, text, blits,
        descrOn: Boolean(s.descrOn), descrBtn: { x: QL.descBtnX, y: QL.descBtnY, frame: descrFrame } };
    },
  };
}

// ---- the message log ----------------------------------------------------------------------------
// Retail: a translucent black box (5,30)-(785,500), two nested boxpieces frames, a scroll bar
// (textslid) on the right, a gold Font16 title and a Close label. The record counts are 400 boxpieces
// and 33 textslid cels -- generated below from the measured cycles, not listed.
const BOX = MENU + 'boxpieces.dc6', SLID = MENU + 'textslid.dc6';
// The message log's own measured geometry (F191 captures msglog-multi-caps-{top,bottom,up1}): the
// newest line at the TOP, 23 rows visible at a 16 px pitch from (37,90); the scroll thumb at x=766
// (frame 16 top, 14 body, 15 bottom), its top cel at y=94 + 12*floor(scroll*5/maxScroll) -- the three
// captures give thumb tops 94/130/154 for scroll 0/3/4, so it travels 5 track cels over the 4-line
// range. The two arrow buttons are textslid frames 9 (up, y=81) and 8 (down, y=453).
export const MSG = Object.freeze({ x: 37, y: 90, pitch: 16, rows: 23, trackTop: 93, trackPitch: 12, trackCount: 31,
  thumbX: 766, thumbCels: 24, thumbTravel: 5, up: { x: 766, y: 81, w: 12, h: 13 }, down: { x: 766, y: 453, w: 12, h: 13 } });
export const messageScrollMax = (total) => Math.max(0, total - MSG.rows);
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
    const f = { white: mk(pal), gold: mk(pl2 ? textColourPalette(pl2, pal, 4) : pal, 4) };
    // V8c: retail draws a message line in FontInGameChat (msglog-with-message-r22: x=37, y=90, name in colour 4, the rest colour 0).
    const chatDc6 = decodeDc6(read('data\\local\\font\\latin\\fontingamechat.dc6')), chatTbl = decodeFontTbl(read('data\\local\\font\\latin\\fontingamechat.tbl'));
    const ck = (p, extra = 0) => createFont({ dc6: chatDc6, tbl: chatTbl, palette: p, documentObject, path: 'data\\local\\font\\latin\\fontingamechat.dc6', drawAs: { op: 'celcolor', mode: 5, extra } });
    f.chat = { 0: ck(pal), 4: ck(pl2 ? textColourPalette(pl2, pal, 4) : pal, 4) };
    fonts.set(act, f); return f; })();
  // 'ÿc4Name' colour codes: 4 gold, anything else white (the only colours retail's chat names use).
  const drawChat = (ctx, f, line, x, y) => { for (const part of String(line).split(/(?=ÿc\d)/)) { const m = /^ÿc(\d)/.exec(part), font = f.chat[m && m[1] === '4' ? 4 : 0], t = m ? part.slice(3) : part; font.drawAnchored(ctx, t, x, y); x += font.measure(t); } };
  return {
    /** The open Enter chat bar (F175 capture chatbar-open-rb): alpha box 127,497-672,544 mode 1, Font13 text at 131,512, caret `_` after it. */
    drawChatBar(ctx, width, height, text, act = 1) { const o = origin(width, height); ctx.save(); ctx.translate(o.x, o.y); ctx.scale(o.scale, o.scale);
      ctx.fillStyle = `rgba(0,0,0,${DIM_ALPHA})`; ctx.fillRect(127, 497, 545, 47); drawChat(ctx, fontFor(act), text + '_', 131, 512); ctx.restore();
      return { box: [127, 497, 672, 544], text: [131, 512], dim: DIM_ALPHA }; },
    /** A freshly posted line drawn over the game (posted-rb): alpha box 11,6-(text end+POSTED_PAD),22, line at 15,20. */
    drawPosted(ctx, width, height, line, act = 1) { const o = origin(width, height), f = fontFor(act); ctx.save(); ctx.translate(o.x, o.y); ctx.scale(o.scale, o.scale);
      const w = String(line).split(/(?=ÿc\d)/).reduce((n, part) => n + f.chat[0].measure(/^ÿc\d/.test(part) ? part.slice(3) : part), 0), box = [11, 6, 15 + w + POSTED_PAD, 22];
      ctx.fillStyle = `rgba(0,0,0,${DIM_ALPHA})`; ctx.fillRect(box[0], box[1], box[2] - box[0], box[3] - box[1]); drawChat(ctx, f, line, 15, 20); ctx.restore();
      return { box, text: [15, 20], width: w, dim: DIM_ALPHA }; },
    /** The Close label's rectangle (the button retail draws under its cursor is a 32x29 orotate cel). */
    closeRect: { x: 355, y: 462, w: 80, h: 30 },
    hitClose(px, py, width, height) { const o = origin(width, height), x = (px - o.x) / o.scale, y = (py - o.y) / o.scale, r = this.closeRect; return x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h; },
    /** Which scroll arrow a canvas point lands on: 'up' | 'down' | null (the two textslid buttons). */
    hitScroll(px, py, width, height) {
      const o = origin(width, height), x = (px - o.x) / o.scale, y = (py - o.y) / o.scale;
      const inBtn = (b) => x >= b.x && x < b.x + b.w && y >= b.y && y < b.y + b.h;
      return inBtn(MSG.up) ? 'up' : inBtn(MSG.down) ? 'down' : null;
    },
    draw(ctx, width, height, s, open, act = 1) {
      if (!open) return null;
      const o = origin(width, height), blits = [], f = fontFor(act), lines = s.lines || [];
      const maxScroll = messageScrollMax(lines.length), scroll = Math.max(0, Math.min(maxScroll, s.scroll | 0));
      ctx.save(); ctx.translate(o.x, o.y); ctx.scale(o.scale, o.scale);
      ctx.fillStyle = `rgba(0,0,0,${DIM_ALPHA})`; ctx.fillRect(5, 30, 780, 470);          // retail: box 5 30 785 500, mode 1 (alpha)
      for (const [fr, x, y] of messageLogPieces()) { const r = chrome.cel(ctx, act, BOX, fr, x, y); if (r) blits.push(r); }
      const slid = (fr, x, y) => { const r = chrome.cel(ctx, act, SLID, fr, x, y); if (r) blits.push(r); };
      slid(9, MSG.thumbX + 1, MSG.up.y); for (let y = MSG.trackTop; y < MSG.trackTop + MSG.trackPitch * MSG.trackCount; y += MSG.trackPitch) slid(13, 767, y); slid(8, 767, MSG.down.y);
      // The scroll thumb, only when the feed overflows (retail draws none for an empty log: msglog.txt).
      const thumb = maxScroll ? MSG.trackTop + 1 + MSG.trackPitch * Math.floor(scroll * MSG.thumbTravel / maxScroll) : 0;
      if (maxScroll) { slid(16, MSG.thumbX, thumb); for (let i = 1; i < MSG.thumbCels - 1; i++) slid(14, MSG.thumbX, thumb + MSG.trackPitch * i); slid(15, MSG.thumbX, thumb + MSG.trackPitch * (MSG.thumbCels - 1)); }
      f.gold.drawAnchored(ctx, s.title, 349, 51); f.white.drawAnchored(ctx, s.close, 376, 486);
      // Newest at the TOP: row i shows line (len-1) - (scroll+i) (msglog-multi-caps-top: 'ee' is a
      // later message than 'ab' but sits BELOW it, so the list runs newest-first downward).
      const rows = [];
      for (let i = 0; i < MSG.rows; i++) { const idx = lines.length - 1 - (scroll + i); if (idx < 0) break;
        drawChat(ctx, f, lines[idx], MSG.x, MSG.y + MSG.pitch * i); rows.push({ index: idx, y: MSG.y + MSG.pitch * i, text: lines[idx] }); }
      ctx.restore();
      return { title: s.title, lines: lines.length, scroll, maxScroll, rows, thumb: maxScroll ? { y: thumb, cels: MSG.thumbCels } : null, blits, dim: DIM_ALPHA };
    },
  };
}
