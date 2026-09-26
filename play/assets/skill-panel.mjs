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

export const SESSION_SKILL_BYTES = 16;   // D2SessionSkill: eight uint16

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
      nameStr: v.getUint16(o + 14, true) });
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

export function createSkillPanelRenderer(read, documentObject = globalThis.document) {
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
  // The skill under the pointer, so the panel can name it. Retail shows the name, description and
  // mana cost on hover; the NAME is exact -- SkillDesc's wStrName is a .tbl index the engine
  // already hands us -- so it lands now and the rest stays open (V19).
  let hovered = null;

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
      for (const set of [0, tab]) for (const [k, x, y] of [[0, 400, 316], [1, 656, 316], [2, 400, 492], [3, 656, 492]])
        if (bg[set * TAB_FRAMES + k]) chrome.cel(ctx, act, path, set * TAB_FRAMES + k, x, y);
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

      const drawn = [], missingIcons = [];
      for (const skill of state.skills || []) {
        if (skill.page !== tab) continue;
        const r = cellRect(skill);
        const sheet = skill.baseLevel > 0 ? icons : greyFor(state.playerClass ?? 0, act);
        const icon = sheet[skill.iconCel];
        const available = (state.level ?? 1) >= skill.requiredLevel;
        if (icon) ctx.drawImage(icon, r.x, r.y);
        else missingIcons.push({ id: skill.id, iconCel: skill.iconCel });
        // The skill's level in Font16 anchored at the icon's right edge, 12 rows below its bottom
        // (retail's act-V tree, skilltree-act5: '2' at 532,134 for the icon at 484..531 x 75..122).
        if (skill.baseLevel > 0) font.drawAnchored(ctx, String(skill.baseLevel), r.x + r.w, r.y + r.h - 1 + 12);
        drawn.push({ id: skill.id, x: r.x, y: r.y, baseLevel: skill.baseLevel, available });
      }
      chrome.cel(ctx, act, CHROME_ART.close, 10, 571, 477);
      chrome.border(ctx, act, BORDER_RIGHT);

      // The hovered skill's real name, from the retail string table. Drawn last so it sits over
      // the icons, and clamped to the panel so a long name cannot run off the edge.
      let namePlate = null;
      if (hovered && state.name) {
        const label = state.name(hovered) || '';
        if (label) {
          const r = cellRect(hovered);
          const w = font.measure(label) + 10;
          const nx = Math.max(2, Math.min(SKILL_LAYOUT.width - w - 2, r.x + r.w / 2 - w / 2));
          const ny = Math.max(2, r.y - font.lineHeight - 6);
          ctx.save(); ctx.fillStyle = 'rgba(0,0,0,.78)';
          ctx.fillRect(nx, ny, w, font.lineHeight + 4); ctx.restore();
          font.draw(ctx, label, nx + 5, ny + 2);
          namePlate = { id: hovered.id, label, x: nx, y: ny };
        }
      }
      ctx.restore();

      // missingIcons is reported, not swallowed: an iconCel with no frame behind it is the exact
      // shape of the defect that drew a green block, and a count of drawn icons cannot see it.
      return { x: o.x, y: o.y, scale: o.scale, tab, drawn, missingIcons, namePlate,
        icons: icons.length, skillPoints: state.skillPoints ?? 0, total: (state.skills || []).length };
    },
  };
}
