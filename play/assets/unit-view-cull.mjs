// Which monsters the frame draws at all. Retail D2Client draws the units of the rooms around the view, so
// its draw list has no record for a unit far off screen; this client used to hand EVERY monster the engine
// holds to the sprite pass. On a level that holds hundreds at once (Worldstone Keep Level 3, ~250) that
// decoded and drew ~234 off-screen units a frame, which spent the per-frame sprite-load budget before the
// monsters in view were reached: those stayed deferred (invisible) for frames on end.
//
// The rectangle is the anchor's, widened by a cel's extent past it: art rises above the anchor (a large
// monster's cel is ~300 px tall), so an anchor below the bottom edge can still paint; little lies below
// or beside it. Retail's own per-room cull is not recovered; this margin keeps every unit whose art can
// reach the frame and drops only those whose art cannot, so what it removes is invisible by construction.
export const UNIT_VIEW_MARGIN = Object.freeze({ left: 256, right: 256, top: 128, bottom: 400 });

export function unitInView(sx, sy, width, height, m = UNIT_VIEW_MARGIN) {
  return sx >= -m.left && sx <= width + m.right && sy >= -m.top && sy <= height + m.bottom;
}
