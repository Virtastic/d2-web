// The skill tree: the other half of levelling up.
//
// Cycle 108 gave stat points somewhere to go; skill points still had nowhere (
// P4). `d2_session_command_add_skill` is retail's own 0x3B handler and was already proved by
// scenario; what was missing was any way to see the tree.
//
// Nothing here decides which skills exist or where they sit. `d2_session_class_skills` reads
// Skills.txt and SkillDesc.txt for the player's own class and returns, per skill, its tab, row,
// column, required level, current level, retail icon index and the .tbl index of its name. A tree
// drawn from anything else would be a picture of a skill tree rather than this character's.

import { decodeDatPalette, decodeDc6, dc6FrameToRgba } from './d2-formats.mjs';
import { textColourPalette } from './frontend.mjs';
import { BORDER_RIGHT, CHROME_ART, createPanelChrome } from './panel-chrome.mjs';
import { createFont, decodeFontTbl } from './frontend.mjs';
import { TOOLTIP_FRAME, drawTooltipFrame } from './vendor-panel.mjs';
import { PANEL_CLOSE_ART, panelCloseFrame, panelCloseLayout } from './panel-close.mjs';

export const SESSION_SKILL_BYTES = 26;   // D2SessionSkill: thirteen uint16 (manaLine/manaCost are V19's)

// Retail names the tree background per class: skltree_<letter>_back.dc6. The letters follow
// CharStats.txt's class order, which is also the engine's class index. All seven are distinct
// files that happen to share a byte length -- checked, because a lookup that quietly returns the
// same asset for every name is exactly how every floor tile once drew the wrong art.
const CLASS_LETTER = ['a', 's', 'n', 'p', 'b', 'd', 'i'];
// D2's class tokens, the same ones the .cof and .dcc paths use.
const CLASS_TOKEN = ['am', 'so', 'ne', 'pa', 'ba', 'dr', 'as'];
export const skillTreeAsset = (playerClass) =>
  `data\\global\\ui\\SPELLS\\skltree_${CLASS_LETTER[playerClass] ?? 'a'}_back.dc6`;
export const skillIconAsset = (playerClass) =>
  `data\\global\\ui\\SPELLS\\${CLASS_TOKEN[playerClass] ?? 'am'}skillicon.dc6`;
// The background holds the panel frame and the tree's connector arrows; the skill ICONS are a
// separate per-class sheet, <token>skillicon.dc6, indexed by SkillDesc's iconCel. The shared
// SPELLS\SkillIcon.dc6 is NOT that sheet -- it holds 24 frames against a class's 30 skills, and
// indexing it by iconCel drew unrelated art (a green block where Magic Arrow should be).
//
// 16 frames = 4 tabs x 4, each tab a 2x2 grid of 256x256, 64x256, 256x176, 64x176 -- a 320x432
// panel, the same size as the other retail panels.
const TAB_FRAMES = 4;
// Prior art, not derived: OpenDiablo2 d2game/d2player/skillicon.go places a skill at
//   x = 346 + SkillColumn*69,  y = 59 + SkillRow*68
// in 800x600 screen coordinates, with skilltree.go putting the panel at (401, 64). Relative to the
// panel that is x = -55 + column*69 and y = -5 + row*68.
//
// This grid was invented at 90x58 for a cycle, which is why the icons never sat on the
// background's connector arrows -- and it was written off as "needs RE" while the constants sat in
// reference/OpenDiablo2. See , "UI / HUD panel layouts".
// Where the panel's own art is anchored. OpenDiablo2 puts the skill tree at (401, 64) and retail's
// draw list anchors its backdrop frames at (400|656, 316|492) -- the same pair the inventory panel's
// InvChar6 frames use, in the same sheet geometry -- so the panel is 320 wide at x 400, and its top is
// the character screen's own PANEL_ORIGIN.y, which s251 pinned against retail's draw list.
export const SKILL_PANEL_ORIGIN = Object.freeze({ x: 400, y: 61 });
export const SKILL_LAYOUT = Object.freeze({
  screenX: 0, screenY: 0, width: 800, height: 600,
  // Retail's draw list: icons at x 346 + 69 * SkillColumn, their
  // top rows per SkillRow below; the three tabs down the panel's right side, page 3 at the top.
  gridX: 346, columnWidth: 69, rowTops: [75, 143, 211, 279, 348, 416], iconSize: 48,
  tabX: 630, tabWidth: 88, tabTops: { 3: 172, 2: 280, 1: 388 }, tabHeight: 100,
});
// The tab names per class, bottom (page 1) to top (page 3): OpenDiablo2's skilltree.go keys; each tab's
// lines centred on x 677 around its middle row (retail: Warcries 222; Combat/Masteries 324-336;
// Combat/Skills 430-442).
const TAB_KEYS = {
  4: [['StrSklTree21', 'StrSklTree4'], ['StrSklTree21', 'StrSklTree22'], ['StrSklTree20']],
  2: [['StrSklTree19'], ['StrSklTree17', 'StrSklTree18', 'StrSklTree5'], ['StrSklTree16', 'StrSklTree5']],
  3: [['StrSklTree15', 'StrSklTree4'], ['StrSklTree14', 'StrSklTree13'], ['StrSklTree12', 'StrSklTree13']],
  6: [['StrSklTree30'], ['StrSklTree31', 'StrSklTree32'], ['StrSklTree33', 'StrSklTree34']],
  1: [['StrSklTree25', 'StrSklTree5'], ['StrSklTree24', 'StrSklTree5'], ['StrSklTree23', 'StrSklTree5']],
  0: [['StrSklTree10', 'StrSklTree11', 'StrSklTree4'], ['StrSklTree8', 'StrSklTree9', 'StrSklTree4'], ['StrSklTree6', 'StrSklTree7', 'StrSklTree4']],
  5: [['StrSklTree26'], ['StrSklTree27', 'StrSklTree28'], ['StrSklTree29']],
};
const TAB_MID = { 1: 436, 2: 330, 3: 222 };
// The hover plate is the item tooltip's retail frame (vendor-panel.mjs TOOLTIP_FRAME / drawTooltipFrame).
const PLATE_PAD = TOOLTIP_FRAME.pad, PLATE_LINE = TOOLTIP_FRAME.line;

/** The plate's box (see the comment where it is drawn); `cell` is the hovered icon's rect. */
export function drawPlate(ctx, font, cell, lines) {
  const box = plateBox(cell, Math.max(...lines.map((l) => font.measure(l.text))), lines.length);
  drawTooltipFrame(ctx, box.x, box.y, box.w, box.h, 'rgba(0,0,0,0.75)'); // retail-measured skill plate fill
  lines.forEach((l, i) => { l.x = Math.floor(box.x + box.w / 2 - font.measure(l.text) / 2); l.y = box.y + 13 + i * PLATE_LINE;
    if (l.text) font.drawAnchored(ctx, l.text, l.x, l.y); });
  return box;
}
export function plateBox(cell, textWidth, lines) {
  const w = textWidth + PLATE_PAD * 2, h = lines * PLATE_LINE;
  return { x: Math.max(0, Math.min(SKILL_LAYOUT.width - w, cell.x)), y: Math.min(cell.y + cell.h + 4, SKILL_LAYOUT.height - 5 - h), w, h };
}

/**
 * Retail's own mana line text for a skill's cost, in the engine's 1/256 units (V19).
 *
 * Measured, not derived: retail 1.13c's frame of an unlearned Bash's tooltip reads "Mana Cost: 2"
 * (engine 512) and of Whirlwind's "Mana Cost: 12.5" (engine 3200), so a cost with a fraction is
 * printed with its TRUNCATED tenth. Retail's screen carries no more precision than that: its frame
 * of Whirlwind's line shows "12.5" for 3200/256, and the community's transcription of the same
 * display -- the Arreat Summit's per-level Prayer table, 1.1/1.3/1.5/1.7 at levels 2..5 -- is the
 * truncation of 1.1875/1.375/1.5625/1.75. Whole costs print without a decimal, as retail's "Mana
 * Cost: 2" does.
 */
export function formatManaCost(cost) {
  const whole = Math.floor(cost / 256), fraction = cost % 256;
  return fraction ? `${whole}.${Math.trunc(fraction * 10 / 256)}` : String(whole);
}

export function parseClassSkills(bytes, count) {
  const b = bytes instanceof Uint8Array ? bytes : new Uint8Array(bytes);
  if (!Number.isInteger(count) || count < 0 || b.length < count * SESSION_SKILL_BYTES)
    throw new RangeError('invalid class skill extent');
  const v = new DataView(b.buffer, b.byteOffset, b.byteLength), out = [];
  for (let i = 0; i < count; i++) {
    const o = i * SESSION_SKILL_BYTES;
    out.push({ id: v.getUint16(o, true), page: v.getUint16(o + 2, true), row: v.getUint16(o + 4, true),
      column: v.getUint16(o + 6, true), requiredLevel: v.getUint16(o + 8, true),
      baseLevel: v.getUint16(o + 10, true), iconCel: v.getUint16(o + 12, true),
      nameStr: v.getUint16(o + 14, true), descStr: v.getUint16(o + 16, true),
      manaStr: v.getUint16(o + 18, true), canLearn: v.getUint16(o + 20, true) !== 0,
      manaLine: v.getUint16(o + 22, true) !== 0, manaCost: v.getUint16(o + 24, true) });
  }
  return out;
}

function frameCanvas(frame, palette, documentObject) {
  const canvas = documentObject.createElement('canvas');
  canvas.width = Math.max(1, frame.width); canvas.height = Math.max(1, frame.height);
  if (!frame.width || !frame.height) return canvas;
  canvas.getContext('2d').putImageData(new ImageData(dc6FrameToRgba(frame, palette), frame.width, frame.height), 0, 0);
  return canvas;
}

/** The hover plate's lines for a skill record (V19; the tree and the skill-select popup share it). `state`: line, string, skillDesc, skillMana. */
export function buildSkillPlate(hovered, state, compact = false) {
  const plateLines = [], unmodelled = [];
  const line = state.line || (() => ''), key = state.string || (() => '');
  const blank = () => plateLines.push({ kind: 'blank', text: '' });
  // Retail's layout (skilltree-hover*.txt, 16 px slots): [unlearned + blank], name, description,
  // Required Level, blank, [SkillDesc's dsc2 lines + blank], the level block, [blank + synergy].
  if (!hovered.baseLevel) { plateLines.push({ kind: 'unlearned', text: key('StrSkill119') }); blank(); }
  const name = hovered.nameStr ? line(hovered.nameStr) : '';
  if (name) plateLines.push({ kind: 'name', index: hovered.nameStr, text: name });
  // The popup's plate (compact) shows SkillDesc's SHORT text, not the long description (v9b-popup-iconhover-r29: "stuns your target").
  const shortIdx = compact ? state.skillStrings?.(hovered.id)?.short : 0;
  const desc = compact ? (shortIdx ? String(line(shortIdx) || '') : '') : (hovered.descStr ? String(line(hovered.descStr) || '') : '');
  for (const l of (compact ? desc.split('\n').filter((t) => t.length) : desc.split('\n').filter((t) => t.length).reverse()))
    plateLines.push({ kind: 'desc', index: hovered.descStr, text: l });
  // Retail's learned-skill plate (skilltree-hover-bash5-r20) has NO Required Level line, only the spacer: drawn for an unlearned skill.
  if (!hovered.baseLevel) plateLines.push({ kind: 'required', text: key('skilldesc3') + hovered.requiredLevel });
  blank();
  const level = hovered.baseLevel || 1;
  let d = state.skillDesc ? state.skillDesc(hovered.id, level) : [];
  // SkillDesc's line types this client formats, each measured on a retail frame: 2 = "A +calc B",
  // 4 = "A +calc", 12 = "A calc seconds", 52 = "A calc B"; a zero value is not drawn (Frenzy's dsc2line2).
  // Other types are reported in plate.unmodelled, never drawn as a guess.
  const sign = (n) => (n >= 0 ? '+' : '') + n;
  const stat = (e) => {
    const a = e.a ? line(e.a) : '', b = e.b ? line(e.b) : '';
    if (e.type === 2) return a + sign(e.calcA) + b;
    if (e.type === 4) return a + sign(e.calcA);
    if (e.type === 12) return a + e.calcA / 25 + key('StrSkill16');   // frames at 25 per second (Frenzy 150 -> 6)
    if (e.type === 52 || e.type === 3) return a + e.calcA + b;   // 3: Stun's `Attack: 40 percent` (v9b-popup-iconhover-r29)
    return null;
  };
  const stats = (from, to) => {
    const out = [];
    for (let i = to - 1; i >= from; i--) {
      const e = d[i];
      if (!e || !e.type || e.type === 1 || e.type === 5) continue;
      const text = stat(e);
      if (text === null) unmodelled.push({ index: i, type: e.type }); else if (e.calcA !== 0) out.push({ kind: 'stat', text });
    }
    return out;
  };
  const dsc2 = stats(6, 10);
  // The popup plate carries the dsc2 lines inside the level block, straight after its header, with no spacer around them.
  if (dsc2.length && !compact) { plateLines.push(...dsc2); blank(); }
  // One level block (header, SkillDesc's stat lines bottom-up, the mana line); a learned skill draws
  // it twice, "Current Skill Level: N" then retail's "Next Level" at N+1 (skilltree-hover-bash5-r20).
  const levelBlock = (header, lvl, cost, omitZeroMana) => {
    const dl = lvl === level ? d : (state.skillDesc ? state.skillDesc(hovered.id, lvl) : []);
    plateLines.push({ kind: 'header', text: header });
    if (compact) plateLines.push(...dsc2);
    const saved = d; d = dl; plateLines.push(...stats(1, 6)); d = saved;
    if (hovered.manaLine) {
      // The mana line: SkillDesc's `str mana` template (the SAME index for passives and attacks --
      // the table decides who gets a line, not this file) and the cost the engine hands over. A
      // template that resolves to nothing leaves the line out, as every other unresolvable index is.
      const template = hovered.manaStr ? line(hovered.manaStr) : '';
      // Type 5 (Frenzy: `usmc/128`) prints its own evaluated calc (3 where SKILLS_GetManaCosts says 1.5); type 1 the engine's cost.
      const five = dl.slice(0, 6).find((e) => e.type === 5);
      // Retail's Next Level block draws no mana line when the printed cost is 0 (Double Swing 5 -> 6, skilltree-hover-dswing5-r29).
      const shown = five ? five.calcA : formatManaCost(cost);
      if (template && !(omitZeroMana && Number(shown) === 0)) plateLines.push({ kind: 'mana', index: hovered.manaStr, text: template + shown, cost });
    }
  };
  levelBlock(hovered.baseLevel ? key('StrSkill2') + hovered.baseLevel : key('StrSkill17'), level, hovered.manaCost);
  // The skill-select popup's plate is the Current block only: no Next Level, no synergy (v9b-popup-iconhover-r29, Stun 1).
  if (hovered.baseLevel && state.skillMana && !compact) { blank(); levelBlock(key('StrSkill1'), level + 1, state.skillMana(hovered.id, level + 1), true); }
  // The synergy block: type 40 header ("%s Receives Bonuses From:" with the skill's name) and
  // type 63 lines "Name: +calc% text", in table order.
  const syn = d.slice(10, 17).filter((e) => e.type === 63).map((e) => ({ kind: 'syn', text: `${line(e.a)}: +${e.calcA}% ${line(e.b)}` }));
  const head = d.slice(10, 17).find((e) => e.type === 40);
  if (head && syn.length && !compact) { blank(); plateLines.push({ kind: 'synheader', text: line(head.a).replace('%s', line(head.b)) }, ...syn); }
  return { lines: plateLines, unmodelled };
}

export function createSkillPanelRenderer(read, documentObject = globalThis.document, skillPointOverlay) {
  if (!skillPointOverlay) throw new Error('skill panel needs its skill-point overlay');
  // Everything drawn through the ACT's palette (the panel is shown in whatever act the player is in):
  // per act, its pal.dat, the Font16 face, the colour-5 grey (the act PL2's TextColorShifts[5]) and the
  // icon sheets. The layer order and anchors do not depend on the act.
  const actPath = (act, f) => `data\\global\\palette\\ACT${Math.min(5, Math.max(1, act | 0))}\\${f}`;
  const fontDc6 = decodeDc6(read('data\\local\\font\\latin\\font16.dc6')), fontTbl = decodeFontTbl(read('data\\local\\font\\latin\\font16.tbl'));
  const perAct = new Map();
  const forAct = (act) => {
    let a = perAct.get(act);
    if (!a) {
      const palette = decodeDatPalette(read(actPath(act, 'pal.dat')));
      a = { palette, font: createFont({ dc6: fontDc6, tbl: fontTbl, palette, documentObject }),
        grey: textColourPalette(read(actPath(act, 'pal.pl2')), palette, 5), icons: new Map(), greyIcons: new Map() };
      perAct.set(act, a);
    }
    return a;
  };
  let font = forAct(1).font;
  const chrome = createPanelChrome(read, documentObject);
  const sheetOf = (a, key, pal, playerClass) => { let s = a[key].get(playerClass);
    if (!s) { s = decodeDc6(read(skillIconAsset(playerClass))).frames.map((f) => frameCanvas(f, pal, documentObject)); a[key].set(playerClass, s); } return s; };
  const greyFor = (playerClass, act = 1) => { const a = forAct(act); return sheetOf(a, 'greyIcons', a.grey, playerClass); };
  const iconsFor = (playerClass, act = 1) => { const a = forAct(act); return sheetOf(a, 'icons', a.palette, playerClass); };
  const bgFrames = new Map();
  const bgFor = (playerClass) => { let s = bgFrames.get(playerClass); if (!s) { s = decodeDc6(read(skillTreeAsset(playerClass))).frames; bgFrames.set(playerClass, s); } return s; };
  const palette = forAct(1).palette;
  const backgrounds = new Map();
  const background = (playerClass) => {
    let art = backgrounds.get(playerClass);
    if (!art) {
      const dc6 = decodeDc6(read(skillTreeAsset(playerClass)));
      if (!dc6.frames.length) throw new Error(`empty skill tree for class ${playerClass}`);
      art = dc6.frames.map((f) => frameCanvas(f, palette, documentObject));
      backgrounds.set(playerClass, art);
    }
    return art;
  };

  let tab = 1;
  // The skill under the pointer, so the panel can name it. Retail shows the name, the description
  // and the mana line on hover, each a .tbl index SkillDesc carries, so all three land from the
  // engine's record (V19).
  let hovered = null;
  let plate = null;

  const origin = (width, height) => {
    const scale = Math.min(1, width / 800, height / 600);
    return { scale,
      x: (width - 800 * scale) / 2 + SKILL_LAYOUT.screenX * scale,
      y: (height - 600 * scale) / 2 + SKILL_LAYOUT.screenY * scale };
  };
  // Retail rows count from the top of the tab; column 1 is the leftmost.
  // SkillColumn/SkillRow are used RAW here, not as (n-1): OpenDiablo2 multiplies the table value
  // straight, and SkillDesc numbers its columns and rows from 1.
  const cellRect = (skill) => ({
    x: SKILL_LAYOUT.gridX + skill.column * SKILL_LAYOUT.columnWidth,
    y: SKILL_LAYOUT.rowTops[Math.max(0, Math.min(5, skill.row - 1))],
    w: SKILL_LAYOUT.iconSize, h: SKILL_LAYOUT.iconSize,
  });

  return {
    get tab() { return tab; },
    get hovered() { return hovered; },
    /** The panel's own rectangle on the 800x600 screen. Its backdrop cels are anchored where retail
     *  anchors them (skilltree.txt: the frames at (400|656, 316|492) below, like InvChar6's), so the
     *  panel is the same 320x441 as the inventory's, at the same left -- and the tabs (630..718), the
     *  close box and the icons all sit inside it, which s427 asserts. The click routing's panel test
     *  reads this rather than a second guess in the shell. */
    panelRect() { return { left: SKILL_PANEL_ORIGIN.x, top: SKILL_PANEL_ORIGIN.y, width: 320, height: 441 }; },
    /** Track the pointer so draw() can name the skill under it. Returns the skill id, or null. */
    hover(px, py, width, height, state) {
      const hit = this.skillAt(px, py, width, height, state);
      hovered = hit && hit.skill ? hit.skill : null;
      return hovered ? hovered.id : null;
    },
    set tab(v) { tab = Math.min(3, Math.max(1, v | 0)); },

    /**
     * The skill under a canvas point on the current tab, or null. Shares cellRect() with draw(), so
     * a click cannot land on a skill other than the one the player sees.
     */
    skillAt(px, py, width, height, state) {
      const o = origin(width, height);
      const x = (px - o.x) / o.scale, y = (py - o.y) / o.scale;
      // Tabs first: they sit above the grid and would otherwise be swallowed by it.
      if (x >= SKILL_LAYOUT.tabX && x < SKILL_LAYOUT.tabX + SKILL_LAYOUT.tabWidth)
        for (const t of [1, 2, 3]) { const top = SKILL_LAYOUT.tabTops[t]; if (y >= top && y < top + SKILL_LAYOUT.tabHeight) return { tab: t }; }
      for (const skill of state?.skills || []) {
        if (skill.page !== tab) continue;
        const r = cellRect(skill);
        if (x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h) return { skill };
      }
      return null;
    },

    /** `state` is { playerClass, level, skillPoints, skills: parseClassSkills(...), name(skill) }. */
    draw(ctx, width, height, state, visible = true) {
      if (!visible || !state) return null;
      const o = origin(width, height);
      const art = background(state.playerClass ?? 0);
      const act = state.act || 1; font = forAct(act).font;
      const icons = iconsFor(state.playerClass ?? 0, act);
      ctx.save(); ctx.translate(o.x, o.y); ctx.scale(o.scale, o.scale);
      // Retail: the backdrop set (frames 0-3) then the open page's set (4*page .. +3), each at
      // (400|656, 316|492), bottom rows (skilltree.txt).
      const bg = bgFor(state.playerClass ?? 0), path = skillTreeAsset(state.playerClass ?? 0);
      // Every panel cel this frame blitted, with the cel it actually drew -- the record a draw-list
      // comparison reads, so "the backdrop is at retail's anchor" is checked, not asserted.
      const cels = [];
      for (const set of [0, tab]) for (const [k, x, y] of [[0, 400, 316], [1, 656, 316], [2, 400, 492], [3, 656, 492]])
        if (bg[set * TAB_FRAMES + k]) { const f = set * TAB_FRAMES + k;
          cels.push({ path, frame: f, anchor: { x, y }, drawn: chrome.cel(ctx, act, path, f, x, y)?.drawn ?? null }); }
      // "Skill / Choices / Remaining" and the count, each line centred on x 677 (retail: 659/649/639,
      // rows 85/97/109; the count at 669,140).
      // Every text draw this frame made, at the (left-x, y) pixel it actually painted at -- the same
      // convention retail's own draw-list `text` records use -- so a scenario can compare positions
      // without re-deriving them (client-parity P4b, "not yet checked record-for-record").
      const labels = [];
      const S = (k) => (state.string?.(k) || '');
      const centred = (t, cx, y) => { const x = Math.floor(cx - font.measure(t) / 2); font.drawAnchored(ctx, t, x, y); labels.push({ text: t, x, y }); };
      const cx = 659 + font.measure(S('StrSklTree1') || 'Skill') / 2;
      [['StrSklTree1', 85], ['StrSklTree2', 97], ['StrSklTree3', 109]].forEach(([k, y]) => centred(S(k), cx, y));
      // Count: left-anchored, not centred -- retail measured x=668 for '1' and '11', 669 for '0' (skilltree-points/-points11/-spent-r20).
      { const t = String(state.skillPoints ?? 0), x = t[0] === '0' ? 669 : 668; font.drawAnchored(ctx, t, x, 140); labels.push({ text: t, x, y: 140 }); }
      const keys = TAB_KEYS[state.playerClass ?? 0] || TAB_KEYS[0];
      for (const t of [1, 2, 3]) { const lines = keys[t - 1].map(S), top = TAB_MID[t] - 6 * (lines.length - 1);
        lines.forEach((l, i) => centred(l, cx, top + 12 * i)); }

      const drawn = [], missingIcons = [], plus = [], digits = [];
      const iconPath = skillIconAsset(state.playerClass ?? 0);
      for (const skill of state.skills || []) {
        if (skill.page !== tab) continue;
        const r = cellRect(skill);
        const available = skillPointOverlay.isCandidate(skill);
        // A skill the player can learn now draws coloured, as a learned one does (retail's
        // skilltree-points capture: only the learnable Bash is extra=0, the rest grey).
        const sheet = skill.baseLevel > 0 || available ? icons : greyFor(state.playerClass ?? 0, act);
        const icon = sheet[skill.iconCel];
        if (icon) ctx.drawImage(icon, r.x, r.y);
        else missingIcons.push({ id: skill.id, iconCel: skill.iconCel });
        // No "+" over a learnable icon: retail's skilltree-points frame (one unspent point, Bash learnable) has no level.dc6 over the tree (V20 struck).
        // The skill's level in Font16 anchored at the icon's right edge, 12 rows below its bottom
        // (retail's act-V tree, skilltree-act5: '2' at 532,134 for the icon at 484..531 x 75..122).
        // retail's base level is drawn 4 px left of the icon's right edge (measured on the two-digit levels of charpanel-skilltree tick 21; one-digit unverified)
        if (skill.baseLevel > 0) { const dx = r.x + r.w - 4, dy = r.y + r.h - 1 + 12; font.drawAnchored(ctx, String(skill.baseLevel), dx, dy); digits.push({ text: String(skill.baseLevel), x: dx, y: dy, iconCel: skill.iconCel }); }
        // Identity, not coverage: the cel DRAWN is named (its sheet and its SkillDesc iconCel), so a
        // row can compare the blit against retail's own draw list rather than counting icons.
        drawn.push({ id: skill.id, x: r.x, y: r.y, baseLevel: skill.baseLevel, available,
          iconCel: skill.iconCel, sheet: iconPath, frame: skill.iconCel, w: r.w, h: r.h, grey: !(skill.baseLevel > 0 || available) });
      }
      // skilltree.go assigns a close x per class and open tab; do not flatten it
      // to the right-hand button merely because that happens to fit one page. panelCloseLayout
      // (panel-close.mjs) is the one place this rect lives -- the drawn cel and the click hit-box
      // both come from it now, so they cannot drift the way close.x-1,close.y+28 vs the raw table
      // entry did.
      const close = panelCloseLayout('skills', { playerClass: state.playerClass, tab });
      // The frame is the widget's held/rest state (FUN_6FB28240 draws `held + 10`), so the cel the
      // player sees while the button is down is the one D2Client itself would blit.
      const closeFrame = panelCloseFrame(state.closePressed);
      const closeBlit = chrome.cel(ctx, act, PANEL_CLOSE_ART.path, closeFrame, close.x, close.y + close.height - 1);
      chrome.border(ctx, act, BORDER_RIGHT);

      // The hovered skill's own retail text: SkillDesc's wStrName and wStrLong are .tbl indices the
      // ENGINE carries (d2_session_class_skills), so the name and the description lines are retail's
      // strings and not a sentence this client wrote (V19). The PLATE is ours -- retail's skill
      // tooltip has no frame capture yet, which is V19's remaining gap -- so its art is not claimed
      // here. A line whose index resolves to nothing is left out and reported (missing), never
      // filled with invented text.
      //
      // The description's lines are drawn in REVERSE order of the string, which is how retail shows
      // them: SkillDesc stores `str long` bottom-up, so Amazon skill 22 reads "enhances your arrows
      // and bolts / to track your target / or seek one of its own / always hits" where the table has
      // those four lines the other way round (checked on all 30 of a class's descriptions: every one
      // is reversed, and the community's own text -- "Creates a magical arrow or bolt that does extra
      // damage" for skill 6, whose table order is the opposite -- is the display order). The lines are
      // drawn verbatim, not re-wrapped: retail's data already wrapped them.
      //
      // The mana line IS drawn, from the engine and from a RETAIL measurement. The
      // engine decides both halves -- manaLine (SkillDesc's own line table carries a 1 somewhere:
      // descline1..6 / dsc2line1..4 / dsc3line1..7, which is where retail draws "Mana Cost: ") and
      // manaCost (SKILLS_GetManaCosts, the number the game spends, at the level the tooltip
      // describes) -- and this file only formats it. The format is measured, not guessed: retail
      // 1.13c's own tooltip frame (`retail/drawlists/skilltree-hover.bin`, decoded from its glyph
      // draws) reads "Mana Cost: 2" for Bash and "Mana Cost: 12.5" for Whirlwind, both unlearned, so
      // the 1/256-unit cost is printed with its truncated tenth. Those two cases are whole-or-half
      // and exact; a LEARNED skill at level 2 or more reaches fractions a tenth cannot express
      // (Prayer level 2 is 1.1875) and there the tenth is retail's display as the community
      // transcribed it -- the Arreat Summit's per-level Prayer table reads 1.1 / 1.3 / 1.5 for
      // 1.1875 / 1.375 / 1.5625 -- which is a cross-check, not a measurement (
      // V19 names the frame that would settle it).
      // The line goes LAST, after the description: retail draws the block in reverse table order and
      // its mana line is descline1 in 162 of the 163 skills that have one (Fire Bolt's is dsc2line1,
      // the one case where retail's order differs -- named in V19).
      const line = state.line || (() => '');
      const key = state.string || (() => '');
      let namePlate = null, unmodelled = [];
      if (hovered) {
        const built = buildSkillPlate(hovered, state); const plateLines = built.lines; unmodelled = built.unmodelled;
        const shown = plateLines;
        if (shown.length) {
          const r = cellRect(hovered);
          // The box is measured from retail's own frames (skilltree-hover, -frenzy, -ww): pad 4 either
          // side of the widest line, 16 px per line, first glyph row 13 below the top; x = the icon's
          // left clamped to the screen's right edge, y = 4 under the icon's bottom row clamped so the
          // bottom stays 5 above the screen's; every line centred on the box.
          const { x: nx, y: ny, w, h } = drawPlate(ctx, font, r, shown);
          namePlate = { id: hovered.id, label: shown.find((l) => l.kind === 'name')?.text ?? shown[0].text, x: nx, y: ny };
          plate = { id: hovered.id, x: nx, y: ny, w, h, lines: shown,
            manaStr: hovered.manaStr, manaTemplate: hovered.manaStr ? line(hovered.manaStr) : '',
            manaLine: Boolean(hovered.manaLine), manaCost: hovered.manaCost, unmodelled };
        } else plate = null;
      } else plate = null;
      ctx.restore();

      // missingIcons is reported, not swallowed: an iconCel with no frame behind it is the exact
      // shape of the defect that drew a green block, and a count of drawn icons cannot see it.
      return { x: o.x, y: o.y, scale: o.scale, tab, drawn, plus, missingIcons, namePlate, plate, cels, labels, digits,
        icons: icons.length, skillPoints: state.skillPoints ?? 0, total: (state.skills || []).length,
        close: { ...close, path: PANEL_CLOSE_ART.path, frame: closeFrame, pressed: Boolean(state.closePressed), blit: closeBlit } };
    },
  };
}
