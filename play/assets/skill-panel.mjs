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
  screenX: 401, screenY: 64, width: 320, height: 432,
  gridX: -55, gridY: -5, columnWidth: 69, rowHeight: 68, iconSize: 48,
  tabY: 8, tabWidth: 100, tabHeight: 28,
});

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
  const palette = decodeDatPalette(read('data\\global\\palette\\SKY\\pal.dat'));
  const font = createFont({
    dc6: decodeDc6(read('data\\local\\font\\latin\\font16.dc6')),
    tbl: decodeFontTbl(read('data\\local\\font\\latin\\font16.tbl')),
    palette: decodeDatPalette(read('data\\global\\palette\\units\\pal.dat')), documentObject,
  });
  const iconSheets = new Map();
  const iconsFor = (playerClass) => {
    let sheet = iconSheets.get(playerClass);
    if (!sheet) {
      sheet = decodeDc6(read(skillIconAsset(playerClass))).frames.map((f) => frameCanvas(f, palette, documentObject));
      iconSheets.set(playerClass, sheet);
    }
    return sheet;
  };
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
    y: SKILL_LAYOUT.gridY + skill.row * SKILL_LAYOUT.rowHeight,
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
      if (y >= SKILL_LAYOUT.tabY && y < SKILL_LAYOUT.tabY + SKILL_LAYOUT.tabHeight) {
        const t = Math.floor(x / SKILL_LAYOUT.tabWidth) + 1;
        if (t >= 1 && t <= 3) return { tab: t };
      }
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
      const icons = iconsFor(state.playerClass ?? 0);
      ctx.save(); ctx.translate(o.x, o.y); ctx.scale(o.scale, o.scale);
      // This tab's four frames, laid out 2x2. Drawing them in a column -- the first guess -- put
      // vertical strips across the world instead of a panel.
      // Four frame-sets for three tabs: set 0 is a blank backdrop, so tab N is set N.
      const base = tab * TAB_FRAMES;
      const tl = art[base], tr = art[base + 1], bl = art[base + 2], br = art[base + 3];
      if (tl) ctx.drawImage(tl, 0, 0);
      if (tr) ctx.drawImage(tr, tl ? tl.width : 0, 0);
      if (bl) ctx.drawImage(bl, 0, tl ? tl.height : 0);
      if (br) ctx.drawImage(br, tl ? tl.width : 0, tl ? tl.height : 0);

      const drawn = [], missingIcons = [];
      for (const skill of state.skills || []) {
        if (skill.page !== tab) continue;
        const r = cellRect(skill);
        const icon = icons[skill.iconCel];
        // Retail dims a skill the character cannot take yet rather than hiding it, so the tree
        // reads as a plan rather than a list of what is available right now.
        const available = (state.level ?? 1) >= skill.requiredLevel;
        ctx.globalAlpha = available ? 1 : 0.45;
        if (icon) ctx.drawImage(icon, r.x, r.y);
        ctx.globalAlpha = 1;
        if (!icon) missingIcons.push({ id: skill.id, iconCel: skill.iconCel });
        if (skill.baseLevel > 0) {
          const text = String(skill.baseLevel);
          font.draw(ctx, text, r.x + r.w - font.measure(text), r.y + r.h - 14);
        }
        drawn.push({ id: skill.id, x: r.x, y: r.y, baseLevel: skill.baseLevel, available });
      }

      for (let t = 1; t <= 3; t++) {
        const label = `TAB ${t}`;
        ctx.globalAlpha = t === tab ? 1 : 0.5;
        font.draw(ctx, label, (t - 1) * SKILL_LAYOUT.tabWidth + 12, SKILL_LAYOUT.tabY + 6);
        ctx.globalAlpha = 1;
      }
      const points = state.skillPoints > 0 ? `SKILL POINTS ${state.skillPoints}` : '';
      if (points) font.draw(ctx, points, 12, SKILL_LAYOUT.height - 24);

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
