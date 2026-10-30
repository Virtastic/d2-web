// The waypoint menu: opened by clicking a waypoint, it lists the act's waypoints and takes the
// player to any that are active.
//
// What decides what: the menu is open because the ENGINE sent retail's 0x63 (the waypoint
// operate function, OBJECTS_OperateFunction23_Waypoint). Which waypoints exist, which act each
// belongs to and which are active come from d2_session_waypoints -- retail's waypoint table read
// against the player's own waypoint data. Every name is Levels.txt's LevelName resolved through
// the retail string tables. Travel and close are retail's 0x49 TakeOrCloseWp.
//
// Layout: retail's own draw list for this menu: every
// cel's file, frame and anchor below is what retail 1.13c draws for the Rogue Encampment menu.
//
// The tab strip was the one thing P6 declared unverified, and two captures settle it (s492, which
// compares the whole menu against retail's Act II frame buffer, and retail/drawlists/act4-wpmenu):
//   - retail draws a tab for EVERY act the player can travel in, not only the shown one -- one in
//     the Rogue Encampment, two in Lut Gholein (acts I, II), four in the Pandemonium Fortress (I-IV);
//   - the shown act's tab is expwaygatetabs frame 2*(act-1), every other act's is 2*(act-1)+1;
//   - the tabs stand 62 px apart, not 63: measured anchors 85, 147, 209, 271 for acts I..IV, i.e.
//     tabX + tabPitch*(act-1) (a 63 px pitch would put act II at 148);
//   - they are drawn in ACT ORDER, which is not cosmetic: consecutive cels overlap by the one column
//     (the cel is 63 wide on a 62 px pitch), so the later act's cel owns it.
// The click rectangles are retail's (FUN_6FB5A4E0 and friends), recovered and verified

import { decodeDatPalette, decodeDc6, dc6FrameToRgba } from './d2-formats.mjs';
import { createFont, decodeFontTbl, textColourPalette } from './frontend.mjs';
import { recordDc6 } from './drawlist.mjs';

export const WAYGATE_ART = {
  background: 'data\\global\\ui\\MENU\\waygatebackground.dc6',
  tabs: 'data\\global\\ui\\MENU\\expwaygatetabs.dc6',
  icons: 'data\\global\\ui\\MENU\\waygateicons.dc6',
  close: 'data\\global\\ui\\PANEL\\buysellbtn.dc6',
  border: 'data\\global\\ui\\PANEL\\800borderframe.dc6',
};
// The left panel's ornate border, drawn after the menu: 800borderframe frames 0..4 at these anchors.
// Retail's row hit-y table (D2Client .data 0x6FB8E6D8, field 5 of each 6-dword entry), panel coordinates.
const ROW_HIT_Y = [60, 96, 132, 168, 205, 241, 277, 313, 349];
const BORDER = [[0, 253], [256, 63], [0, 484], [0, 553], [256, 553]];
// Retail's draw list (anchors are cel anchors: left edge, BOTTOM row). The panel's four background
// cels have their top-left at (80, 61); the act tab frame 2*(act-1) at (85 + 62*(act-1), 94); the
// close button (buysellbtn frame 10) at (353, 477); per row an icon at (97, 149 + 36i) -- frame 0 for
// the waypoint the player stands at, 3 for another active one, none for an inactive one -- and its
// name in font16 anchored at (160, 144 + 35i) (blue here, white active, grey inactive); last the
// title at (145, 108), then the left panel border (BORDER). `tabWidth` is the CEL's width; the
// PITCH between tabs is `tabPitch` (retail's measured 62), so a band is 62 px and the cels overlap.
export const WAYPOINT_LAYOUT = Object.freeze({ left: 80, top: 61, tabX: 85, tabY: 94, tabWidth: 63, tabPitch: 62, closeX: 353, closeY: 477,
  iconX: 97, iconY: 149, iconPitch: 36, textX: 160, textY: 144, rowHeight: 35, titleX: 145, titleY: 108 });
// The acts whose tab retail draws: the ones the engine reports at least one ACTIVE waypoint in (its
// captures draw one, two and four). Read by the caller from d2_session_waypoints and handed to draw()
// as `tabActs`, so the panel keeps no second source of truth about which waypoints exist.
export const tabActsOf = (rowsByAct) => [...rowsByAct.keys()].filter((act) => (rowsByAct.get(act) || []).some((row) => row.active)).sort((a, b) => a - b);
// The title is retail's string 3990 (waypointsheader, "Choose your destination").
export const TITLE_STRING_INDEX = 3990;
// ...and 3991 (nowaypoints, "No Other Waypoints Activated") when no waypoint but the one the player
// stands at is active (retail's draw list, scene `wpmenu2`: drawn at x 117). Both are centred on the
// same x: 'Choose your destination' starts at 145.

function frameCanvas(frame, palette, documentObject) {
  const canvas = documentObject.createElement('canvas');
  canvas.width = Math.max(1, frame.width); canvas.height = Math.max(1, frame.height);
  if (frame.width && frame.height)
    canvas.getContext('2d').putImageData(new ImageData(dc6FrameToRgba(frame, palette), frame.width, frame.height), 0, 0);
  return canvas;
}

const actPalette = (act) => `data\\global\\palette\\ACT${Math.min(5, Math.max(1, act | 0))}\\pal.dat`;

export function createWaypointPanelRenderer(read, documentObject = globalThis.document, { string = null } = {}) {
  const dc6 = Object.fromEntries(Object.entries(WAYGATE_ART).map(([k, p]) => [k, decodeDc6(read(p))]));
  const fontDc6 = decodeDc6(read('data\\local\\font\\latin\\font16.dc6')), tbl = decodeFontTbl(read('data\\local\\font\\latin\\font16.tbl'));
  const palette = decodeDatPalette(read('data\\global\\palette\\units\\pal.dat'));
  const tint = (rgb) => palette.map((v, i) => (i % 4 === 3 ? v : Math.round(v * rgb[i % 4] / 0xff)));
  const fontOf = (pal) => createFont({ dc6: fontDc6, tbl, palette: pal, documentObject, path: 'data\\local\\font\\latin\\font16.dc6', drawAs: { op: 'celcolor', mode: 5 } });
  // D2Win text colours 0 white, 3 blue, 5 grey.  These glyph indices go through the
  // player's current act PL2; switching a shown waypoint tab has no captured proof that
  // retail changes this palette too (P6), so it deliberately is a separate draw input.
  const fontsByAct = new Map();
  const fontsFor = (act) => { let f = fontsByAct.get(act); if (!f) {
    const pal = decodeDatPalette(read(actPalette(act)));
    let pl2 = null; try { pl2 = read(actPalette(act).replace(/pal\.dat$/i, 'pal.pl2')); } catch {}
    f = pl2 ? { white: fontOf(pal), blue: fontOf(textColourPalette(pl2, pal, 3)), grey: fontOf(textColourPalette(pl2, pal, 5)) }
      : { white: fontOf(palette), blue: fontOf(tint([0x69, 0x69, 0xff])), grey: fontOf(tint([0x69, 0x69, 0x69])) };
    fontsByAct.set(act, f); } return f; };
  const perAct = new Map();
  const art = (act) => {
    let a = perAct.get(act);
    if (!a) {
      const pal = decodeDatPalette(read(actPalette(act)));
      a = Object.fromEntries(Object.entries(dc6).map(([k, d]) => [k, d.frames.map((f) => frameCanvas(f, pal, documentObject))]));
      perAct.set(act, a);
    }
    return a;
  };
  const origin = (width, height) => {
    const scale = Math.min(1, width / 800, height / 600);
    return { scale, x: (width - 800 * scale) / 2, y: (height - 600 * scale) / 2 };
  };
  const L = WAYPOINT_LAYOUT;
  // A cel drawn as retail anchors it (left, bottom row), and recorded for the draw-list rows. The
  // record is returned so a caller can report what it BLITTED: this used to return nothing, and the
  // one caller that built evidence out of it (the selected tab below) then reported a plan instead.
  const cel = (ctx, key, canvases, i, x, anchorY) => {
    const f = dc6[key].frames[i], c = canvases[i]; if (!f || !c) return null;
    const top = anchorY + f.offsetY - f.height + 1;
    ctx.drawImage(c, x + f.offsetX, top); return recordDc6(WAYGATE_ART[key], i, f, x + f.offsetX, top);
  };
  // The click band of row i is retail's (FUN_6FB5A240 "Hit test"): panel px 18..296
  // (screen x 98..376) and py in (hityRow, hityRow + 30) with retail's own hit-y table, whose pitch is
  // 36/37 rather than the text's 35 (screen y = py + 60 -> hityRow+61 .. hityRow+89).
  const rowRect = (i) => ({ x: L.left + 18, y: ROW_HIT_Y[i] + 61, w: 279, h: 29 });

  return {
    evidence: Object.fromEntries(Object.entries(dc6).map(([k, d]) => [k, { frames: d.frames.length, sizes: d.frames.map((f) => [f.width, f.height]) }])),
    /** The waypoint row under a canvas point, or null. Shares rowRect with draw(). */
    rowAt(px, py, width, height, rows) {
      const o = origin(width, height), x = (px - o.x) / o.scale, y = (py - o.y) / o.scale;
      for (let i = 0; i < (rows || []).length; i++) {
        const r = rowRect(i);
        if (x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h) return rows[i];
      }
      return null;
    },
    /** `rows` are the engine's waypoints for `shownAct`; `paletteAct` is the player's current act;
     *  `tabActs` are the acts retail draws a tab for (the caller reads them from the engine's own
     *  table -- tabActsOf). Absent, only the shown act's tab is drawn, which is a one-act menu. */
    draw(ctx, width, height, rows, shownAct, open, hereLevelId = null, paletteAct = shownAct, tabActs = null, closePressed = false) {
      if (!open) return null;
      const a = art(paletteAct), o = origin(width, height), fonts = fontsFor(paletteAct);
      ctx.save(); ctx.translate(o.x, o.y); ctx.scale(o.scale, o.scale);
      const bg = dc6.background.frames;
      let composed = { width: 0, height: 0 };
      if (bg.length === 4) {
        const bottom0 = L.top + bg[0].height - 1, bottom1 = bottom0 + bg[2].height;
        cel(ctx, 'background', a.background, 0, L.left, bottom0); cel(ctx, 'background', a.background, 1, L.left + bg[0].width, bottom0);
        cel(ctx, 'background', a.background, 2, L.left, bottom1); cel(ctx, 'background', a.background, 3, L.left + bg[0].width, bottom1);
        composed = { width: bg[0].width + bg[1].width, height: bg[0].height + bg[2].height };
      }
      // Every tab is evidence only if its cel reached the canvas, so each is reported from the blit's
      // own record: `frame` is the index the layout asked for and `drawn` says whether expwaygatetabs
      // carried it -- a tab reported from a plan built before the draw would claim a tab nobody drew.
      // Drawn in ACT ORDER because consecutive cels overlap by one column (63-wide cel, 62 px pitch)
      // and retail's later act owns it; reported with the shown act first, which is the order s298
      // reads.
      const acts = (Array.isArray(tabActs) && tabActs.length ? [...tabActs] : [shownAct]).sort((x, y) => x - y);
      if (!acts.includes(shownAct)) acts.push(shownAct), acts.sort((x, y) => x - y);
      const tabs = [];
      for (const act of acts) {
        const frame = 2 * (act - 1) + (act === shownAct ? 0 : 1);
        const cel_ = cel(ctx, 'tabs', a.tabs, frame, L.tabX + L.tabPitch * (act - 1), L.tabY);
        tabs.push({ act, selected: act === shownAct, frame, drawn: Boolean(cel_), cel: cel_ });
      }
      tabs.sort((x, y) => Number(y.selected) - Number(x.selected));
      cel(ctx, 'close', a.close, closePressed ? 11 : 10, L.closeX, L.closeY);
      const drawn = [];
      (rows || []).forEach((row, i) => {
        const here = row.levelId === hereLevelId;
        if (row.active) cel(ctx, 'icons', a.icons, here ? 0 : 3, L.iconX, L.iconY + i * 36);
        (here ? fonts.blue : row.active ? fonts.white : fonts.grey).drawAnchored(ctx, row.name, L.textX, L.textY + i * L.rowHeight);
        const r = rowRect(i);
        drawn.push({ levelId: row.levelId, name: row.name, active: row.active, at: [r.x, r.y] });
      });
      if (string) {
        const others = (rows || []).some((row) => row.active && row.levelId !== hereLevelId);
        const title = string(others ? TITLE_STRING_INDEX : TITLE_STRING_INDEX + 1);
        const centre = L.titleX + fonts.white.measure(string(TITLE_STRING_INDEX)) / 2;
        fonts.white.drawAnchored(ctx, title, others ? L.titleX : Math.floor(centre - fonts.white.measure(title) / 2), L.titleY);
      }
      BORDER.forEach(([x, y], i) => cel(ctx, 'border', a.border, i, x, y));
      ctx.restore();
      return { open: true, act: shownAct, paletteAct, composed, tabs, drawn, layout: { rowY: rowRect(0).y, rowHeight: L.rowHeight } };
    },
  };
}
