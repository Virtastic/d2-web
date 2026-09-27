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
import { PANEL_CLOSE_ART, panelCloseLayout } from './panel-close.mjs';

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
// The hover plate: its BOX is ours (padding, fill, placement) -- retail's tooltip frame exists since
// 2026-09-27 (`retail/drawlists/skilltree-hover.bin`, a captured pointer parked on Bash's icon) but
// our plate is not drawn from it. Its padding is this client's and is named here so the row can say
// which part is retail's text and which part is our box around it.
const PLATE_PAD = 5;

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
      const S = (k) => (state.string?.(k) || '');
      const centred = (t, cx, y) => font.drawAnchored(ctx, t, Math.floor(cx - font.measure(t) / 2), y);
      const cx = 659 + font.measure(S('StrSklTree1') || 'Skill') / 2;
      [['StrSklTree1', 85], ['StrSklTree2', 97], ['StrSklTree3', 109]].forEach(([k, y]) => centred(S(k), cx, y));
      centred(String(state.skillPoints ?? 0), cx, 140);
      const keys = TAB_KEYS[state.playerClass ?? 0] || TAB_KEYS[0];
      for (const t of [1, 2, 3]) { const lines = keys[t - 1].map(S), top = TAB_MID[t] - 6 * (lines.length - 1);
        lines.forEach((l, i) => centred(l, cx, top + 12 * i)); }

      const drawn = [], missingIcons = [], plus = [];
      const iconPath = skillIconAsset(state.playerClass ?? 0);
      for (const skill of state.skills || []) {
        if (skill.page !== tab) continue;
        const r = cellRect(skill);
        const sheet = skill.baseLevel > 0 ? icons : greyFor(state.playerClass ?? 0, act);
        const icon = sheet[skill.iconCel];
        const available = skillPointOverlay.isCandidate(skill);
        if (icon) ctx.drawImage(icon, r.x, r.y);
        else missingIcons.push({ id: skill.id, iconCel: skill.iconCel });
        if (available) {
          const blit = skillPointOverlay.draw(chrome, ctx, act, r);
          if (blit) plus.push({ id: skill.id, icon: { x: r.x, y: r.y, w: r.w, h: r.h }, ...blit });
        }
        // The skill's level in Font16 anchored at the icon's right edge, 12 rows below its bottom
        // (retail's act-V tree, skilltree-act5: '2' at 532,134 for the icon at 484..531 x 75..122).
        if (skill.baseLevel > 0) font.drawAnchored(ctx, String(skill.baseLevel), r.x + r.w, r.y + r.h - 1 + 12);
        // Identity, not coverage: the cel DRAWN is named (its sheet and its SkillDesc iconCel), so a
        // row can compare the blit against retail's own draw list rather than counting icons.
        drawn.push({ id: skill.id, x: r.x, y: r.y, baseLevel: skill.baseLevel, available,
          iconCel: skill.iconCel, sheet: iconPath, frame: skill.iconCel, w: r.w, h: r.h, grey: !(skill.baseLevel > 0) });
      }
      // skilltree.go assigns a close x per class and open tab; do not flatten it
      // to the right-hand button merely because that happens to fit one page.
      const close = panelCloseLayout('skills', { playerClass: state.playerClass, tab });
      const closeBlit = chrome.cel(ctx, act, PANEL_CLOSE_ART.path, PANEL_CLOSE_ART.frame, close.x - 1, close.y + 28);
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
      const plateLines = [];
      const line = state.line || (() => '');
      let namePlate = null;
      if (hovered) {
        const name = hovered.nameStr ? line(hovered.nameStr) : '';
        if (name) plateLines.push({ kind: 'name', index: hovered.nameStr, text: name });
        const desc = hovered.descStr ? String(line(hovered.descStr) || '') : '';
        for (const l of desc.split('\n').filter((t) => t.length).reverse())
          plateLines.push({ kind: 'desc', index: hovered.descStr, text: l });
        // The mana line: SkillDesc's `str mana` template (the SAME index for passives and attacks --
        // the table decides who gets a line, not this file) and the cost the engine hands over. A
        // template that resolves to nothing leaves the line out, as every other unresolvable index is.
        if (hovered.manaLine) {
          const template = hovered.manaStr ? line(hovered.manaStr) : '';
          if (template) plateLines.push({ kind: 'mana', index: hovered.manaStr, text: template + formatManaCost(hovered.manaCost), cost: hovered.manaCost });
        }
        const shown = plateLines;
        if (shown.length) {
          const r = cellRect(hovered);
          const w = Math.max(...shown.map((l) => font.measure(l.text))) + PLATE_PAD * 2;
          const h = shown.length * font.lineHeight + PLATE_PAD * 2;
          const nx = Math.max(2, Math.min(SKILL_LAYOUT.width - w - 2, r.x + r.w / 2 - w / 2));
          const ny = Math.max(2, Math.min(SKILL_LAYOUT.height - h - 2, r.y - h - 6));
          ctx.save(); ctx.fillStyle = 'rgba(0,0,0,.78)';
          ctx.fillRect(nx, ny, w, h); ctx.restore();
          shown.forEach((l, i) => font.draw(ctx, l.text, nx + PLATE_PAD, ny + PLATE_PAD + i * font.lineHeight));
          namePlate = { id: hovered.id, label: shown[0].text, x: nx, y: ny };
          plate = { id: hovered.id, x: nx, y: ny, w, h, lines: shown,
            manaStr: hovered.manaStr, manaTemplate: hovered.manaStr ? line(hovered.manaStr) : '',
            manaLine: Boolean(hovered.manaLine), manaCost: hovered.manaCost };
        } else plate = null;
      } else plate = null;
      ctx.restore();

      // missingIcons is reported, not swallowed: an iconCel with no frame behind it is the exact
      // shape of the defect that drew a green block, and a count of drawn icons cannot see it.
      return { x: o.x, y: o.y, scale: o.scale, tab, drawn, plus, missingIcons, namePlate, plate, cels,
        icons: icons.length, skillPoints: state.skillPoints ?? 0, total: (state.skills || []).length,
        close: { ...close, path: PANEL_CLOSE_ART.path, frame: PANEL_CLOSE_ART.frame, blit: closeBlit } };
    },
  };
}
