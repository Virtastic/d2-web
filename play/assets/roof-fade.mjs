// F66b: the per-tile alpha of a wall the client hides, recovered from D2Client.dll 1.13c
// (Wall2.cpp 6fb2a340 starts a fade, the draw pass 6fb4ee90 steps it;.
// A wall record carries alpha (starts 0xff), a hidden bit, a fading bit, from/to and an end tick.
//   hide asked, not hidden yet : from 0xff, to 0x80; end = now + 500 (alpha 0xff) else now + 500*(alpha-128)/127
//   hide gone,  hidden         : from 0x80, to 0xff; end = now + 500 (alpha 0x80) else now + 500*(255-alpha)/127
// The draw pass, per frame: fading and now >= end -> alpha = to, fading off; else
//   alpha = from + trunc((to - from) * (now - end + 500) / 500).   (Only in the driver modes the client calls
// "gfx >= 4"; the software modes snap to `to` -- not modelled, the fade is the default.)
// alpha 0 -> not drawn; 0xff -> retail's wall entry (39); else the transparent entry (40) at that alpha.
export function newFade() { return { alpha: 0xff, hidden: false, fading: false, from: 0xff, to: 0xff, end: 0 }; }

export function stepFade(s, hide, now) {
  if (hide && !s.hidden) {
    s.from = 0xff; s.to = 0x80;
    s.end = s.alpha === 0xff ? now + 500 : now + 500 + Math.trunc((s.alpha - 0xff) * 500 / 127);
    s.hidden = s.fading = true;
  } else if (!hide && s.hidden) {
    s.from = 0x80; s.to = 0xff;
    s.end = s.alpha === 0x80 ? now + 500 : now + 500 + Math.trunc((0x80 - s.alpha) * 500 / 127);
    s.hidden = false; s.fading = true;
  }
  if (s.fading) {
    if (now >= s.end) { s.alpha = s.to; s.fading = false; }
    else s.alpha = s.from + Math.trunc((s.to - s.from) * (now - s.end + 500) / 500);
  }
  return s.alpha;
}

/** One fade per wall tile, kept for the life of the session; returns 0xff for a wall never hidden. */
export function createRoofFades() {
  const fades = new Map();
  return (key, hide, now) => {
    let s = fades.get(key);
    if (!s) { if (!hide) return 0xff; s = newFade(); fades.set(key, s); }
    return stepFade(s, hide, now);
  };
}
