// Retail's NPC alert is Overlay.txt's `npcalert` record.  This module deliberately
// accepts its pending state from the engine: deriving it from a client-side quest
// table would give the browser a second, eventually divergent source of truth.
// The engine's answer is `d2_session_npc_speech_alerts` (monspawn_check.cpp): one byte per unit, read
// from the player's own NPC-intro flags -- the buffer D2Game syncs to the client in packet 0x91 and
// clears on client packet 0x4D, which is retail's "this NPC's speech has been acknowledged".
import { decodeDatPalette } from './d2-formats.mjs';
import { decodeDcc } from './unit-formats.mjs';
import { createBlender } from './frontend.mjs';

export const NPC_SPEECH_ALERT = Object.freeze({
  overlay: 'npcalert',
  path: 'data\\global\\overlays\\NPCSpeechBalloon.dcc',
  palette: 'data\\global\\palette\\ACT1\\pal.dat',
  // Overlay.txt's Trans column is the D2Gfx draw mode. The npcalert row is mode 3
  // (PL2 additive), which is what gives the retail speech marker its bright halo.
  drawMode: 3,
  frames: 16,
  animationRate: 9,
  // Retail's Overlay.txt `npcalert` row: Xoffset = -5, Yoffset = -7 (columns 11/12).  The record's
  // Height1..4 columns (-30, 0, 0, -50 -- columns 13..16) are NOT the draw offsets: retail adds the
  // one the unit's MonStats2 `OverlayHeight` selects to Yoffset, and every talkable town NPC has
  // OverlayHeight = 2 (MonStats2.txt: warriv1/kashya/gheed/charsi/akara/... all 2, so Height2 = 0),
  // so the drawn offset is exactly (-5, -7).  Reading Height1/Height4 as the offsets -- the
  // -30/-50 this module used to carry -- floated the balloon ~43 px too high and ~25 px too far
  // left; measured against retail's own capture `retail/frames/w0.png`, where the balloon's bottom
  // edge sits ~18 px above the NPC's head, not the ~56 px that -50 produced (see `s1370`).
  offsetX: -5,
  offsetY: -7,
});

// `pending` is intentionally a required boolean supplied by an engine export.  A
// missing value is a negative result, not permission to infer a quest state here.
export function createNpcSpeechAlertRenderer(read) {
  let dcc = null;
  const paletteState = new Map();
  const load = (act) => {
    if (!dcc) dcc = decodeDcc(read(NPC_SPEECH_ALERT.path), { directions: [0] });
    const paletteId = Math.min(4, Math.max(0, (Number(act) | 0) - 1));
    let state = paletteState.get(paletteId);
    if (!state) {
      const name = ['ACT1', 'ACT2', 'ACT3', 'ACT4', 'ACT5'][paletteId];
      const palette = decodeDatPalette(read(`data\\global\\palette\\${name}\\pal.dat`));
      const blend = createBlender(read(`data\\global\\palette\\${name}\\pal.pl2`), palette);
      state = { palette, blend }; paletteState.set(paletteId, state);
    }
    return state;
  };
  return {
    draw(ctx, x, y, npc, pending, tick = 0, act = 1) {
      const anchor = { x: Math.round(x + NPC_SPEECH_ALERT.offsetX), y: Math.round(y + NPC_SPEECH_ALERT.offsetY) };
      if (pending !== true) return { id: npc?.id ?? 0, pending: false, drawn: false, anchor };
      const { blend } = load(act);
      const frame = Math.floor(Math.max(0, Number(tick) || 0) / NPC_SPEECH_ALERT.animationRate) % NPC_SPEECH_ALERT.frames;
      const image = dcc.directionRecords[0]?.frames[frame];
      if (!image) throw new Error(`NPCSpeechBalloon has no frame ${frame}`);
      const screen = { x: Math.round(anchor.x + image.xOffset), y: Math.round(anchor.y + (image.bottomUp ? image.yOffset : image.yOffset - image.height + 1)) };
      blend(ctx, { width: image.width, height: image.height, indices: image.indexed, opaque: image.opaque }, screen.x, screen.y, NPC_SPEECH_ALERT.drawMode);
      return { id: npc?.id ?? 0, pending: true, drawn: true, overlay: NPC_SPEECH_ALERT.overlay,
        path: NPC_SPEECH_ALERT.path, palette: `data\\global\\palette\\${['ACT1', 'ACT2', 'ACT3', 'ACT4', 'ACT5'][Math.min(4, Math.max(0, (Number(act) | 0) - 1))]}\\pal.dat`,
        drawMode: NPC_SPEECH_ALERT.drawMode, frame, anchor, screen,
        offset: { x: NPC_SPEECH_ALERT.offsetX, y: NPC_SPEECH_ALERT.offsetY }, width: image.width, height: image.height,
        opaque: image.opaque.reduce((n, value) => n + value, 0) };
    },
  };
}
