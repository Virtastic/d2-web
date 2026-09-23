// The waypoint menu: opened by clicking a waypoint, it lists the act's waypoints and takes the
// player to any that are active.
//
// What decides what: the menu is open because the ENGINE sent retail's 0x63 (the waypoint
// operate function, OBJECTS_OperateFunction23_Waypoint). Which waypoints exist, which act each
// belongs to and which are active come from d2_session_waypoints -- retail's waypoint table read
// against the player's own waypoint data. Every name is Levels.txt's LevelName resolved through
// the retail string tables. Travel and close are retail's 0x49 TakeOrCloseWp.
//
// What is NOT retail data, declared: the art is retail's (data\global\ui\MENU\waygate*.dc6) but
// no layout prior art covers this screen (OpenDiablo2 stopped before it), so the ROW positions
// below are chosen by looking at the art and are listed in until a retail
// draw list gives the real numbers.

import { decodeDatPalette, decodeDc6, dc6FrameToRgba } from './d2-formats.mjs';
import { createFont, decodeFontTbl } from './frontend.mjs';

export const WAYGATE_ART = {
  background: 'data\\global\\ui\\MENU\\waygatebackground.dc6',
  tabs: 'data\\global\\ui\\MENU\\expwaygatetabs.dc6',
  icons: 'data\\global\\ui\\MENU\\waygateicons.dc6',
};
// Measured from the art, not retail layout data (see the header). The background's dark list band
// runs from panel y 60 to 374 (314 px) and Act I has 9 waypoints: 9 x 35 = 315, so a 35 px pitch
// from y 60 fills the band exactly. (A first guess of 43 px ran two rows off the band's bottom --
// caught by looking at the frame, not by any count.) Icons sit in the bright left column, x 16-46.
// expwaygatetabs is 10 frames of 63x31 -- five acts x two states -- and 5 x 63 = 315 spans the
// panel, so the tabs sit in the empty top band at y 0.
export const WAYPOINT_LAYOUT = Object.freeze({ left: 80, top: 60, rowX: 58, rowY: 60, rowHeight: 35, iconX: 16, iconY: 2, tabY: 0, tabWidth: 63 });

function frameCanvas(frame, palette, documentObject) {
  const canvas = documentObject.createElement('canvas');
  canvas.width = Math.max(1, frame.width); canvas.height = Math.max(1, frame.height);
  if (frame.width && frame.height)
    canvas.getContext('2d').putImageData(new ImageData(dc6FrameToRgba(frame, palette), frame.width, frame.height), 0, 0);
  return canvas;
}

const actPalette = (act) => `data\\global\\palette\\ACT${Math.min(5, Math.max(1, act | 0))}\\pal.dat`;

export function createWaypointPanelRenderer(read, documentObject = globalThis.document) {
  const dc6 = { background: decodeDc6(read(WAYGATE_ART.background)), tabs: decodeDc6(read(WAYGATE_ART.tabs)),
    icons: decodeDc6(read(WAYGATE_ART.icons)) };
  const font = createFont({
    dc6: decodeDc6(read('data\\local\\font\\latin\\font16.dc6')),
    tbl: decodeFontTbl(read('data\\local\\font\\latin\\font16.tbl')),
    palette: decodeDatPalette(read('data\\global\\palette\\units\\pal.dat')), documentObject,
  });
  const perAct = new Map();
  const art = (act) => {
    let a = perAct.get(act);
    if (!a) {
      const palette = decodeDatPalette(read(actPalette(act)));
      a = Object.fromEntries(Object.entries(dc6).map(([k, d]) => [k, d.frames.map((f) => frameCanvas(f, palette, documentObject))]));
      perAct.set(act, a);
    }
    return a;
  };
  const origin = (width, height) => {
    const scale = Math.min(1, width / 800, height / 600);
    return { scale, x: (width - 800 * scale) / 2, y: (height - 600 * scale) / 2 };
  };
  const L = WAYPOINT_LAYOUT;
  const rowRect = (i) => ({ x: L.left + L.iconX, y: L.top + L.rowY + i * L.rowHeight, w: 280, h: L.rowHeight });

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
    /** `rows` are the engine's waypoints for the shown act: [{ levelId, name, active }]. */
    draw(ctx, width, height, rows, act, open) {
      if (!open) return null;
      const a = art(act), o = origin(width, height);
      ctx.save(); ctx.translate(o.x, o.y); ctx.scale(o.scale, o.scale);
      let composed = { width: 0, height: 0 };
      const bg = a.background;
      if (bg.length === 4) {
        const [tl, tr, bl, br] = bg;
        ctx.drawImage(tl, L.left, L.top); ctx.drawImage(tr, L.left + tl.width, L.top);
        ctx.drawImage(bl, L.left, L.top + tl.height); ctx.drawImage(br, L.left + tl.width, L.top + tl.height);
        composed = { width: tl.width + tr.width, height: tl.height + bl.height };
      } else if (bg.length) { ctx.drawImage(bg[0], L.left, L.top); composed = { width: bg[0].width, height: bg[0].height }; }
      // Act tabs. UNVERIFIED frame assignment: frames 2i / 2i+1 read as act i unselected / selected.
      const tabs = [];
      for (let i = 0; i < 5 && a.tabs.length >= 10; i++) {
        const frame = a.tabs[2 * i + (i === act - 1 ? 1 : 0)];
        ctx.drawImage(frame, L.left + i * L.tabWidth, L.top + L.tabY);
        tabs.push({ act: i + 1, selected: i === act - 1 });
      }
      const drawn = [];
      (rows || []).forEach((row, i) => {
        const r = rowRect(i);
        // waygateicons has five frames, read as one per act (UNVERIFIED); an inactive waypoint is
        // shown by dimming, and cannot be taken.
        const icon = a.icons[Math.min(a.icons.length - 1, Math.max(0, act - 1))];
        ctx.save(); if (!row.active) ctx.globalAlpha = 0.35;
        if (icon) ctx.drawImage(icon, r.x, r.y + L.iconY);
        ctx.restore();
        ctx.save(); if (!row.active) ctx.globalAlpha = 0.5;
        font.draw(ctx, row.name, L.left + L.rowX, r.y + Math.round((r.h - font.lineHeight) / 2));
        ctx.restore();
        drawn.push({ levelId: row.levelId, name: row.name, active: row.active, at: [r.x, r.y] });
      });
      ctx.restore();
      return { open: true, act, composed, tabs, drawn, layout: { rowY: L.rowY, rowHeight: L.rowHeight } };
    },
  };
}
