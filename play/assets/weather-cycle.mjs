// Retail's weather cycle, one tick per rendered game frame. This lives in the client because retail's own
// cycle does: it is D2Client state, not D2Game's -- nothing in the engine (D2Game/D2Common) has one, so
// putting it in src/ would invent a server-side rule retail lacks.
//   phase 0 dry 7500+r%7500 | 1 ramp up 250+r%250 | 2 full 3000+r%3000 | 3 ramp down 125+r%50
// intensity: phase 1 floor(target*elapsed/total), 2 target, 3 floor(target*remaining/total), 0 zero;
// the streak count on screen IS the intensity. Target: 255 for the game's first full phase, then 32+r%224
// rolled once per cycle (native range 32..255). A fresh game starts in phase 2 (2 of 2 capture-rain starts; f11b: 61 of 61 samples
// raining) -- f11b2's 308 dry samples are the one unexplained contradiction.
const BASE = [7500, 250, 3000, 125], SPAN = [7500, 250, 3000, 50];

export function createWeatherCycle(seed = 0x1f2e3d4c) {
  let s = seed >>> 0;
  const rand = (n) => { s = (Math.imul(s, 1664525) + 1013904223) >>> 0; return (s >>> 8) % n; };
  const st = { phase: 2, total: 0, remaining: 0, target: 255 };
  const roll = () => { st.total = st.remaining = BASE[st.phase] + rand(SPAN[st.phase]); };
  roll();
  const intensity = () => {
    const { phase: p, total, remaining, target } = st;
    return p === 2 ? target : p === 1 ? Math.floor(target * (total - remaining) / total) : p === 3 ? Math.floor(target * remaining / total) : 0;
  };
  return {
    state: st,
    get intensity() { return intensity(); },
    /** Advance n frames; a phase advances when its countdown reaches 0. */
    tick(n = 1) {
      for (; n > 0; n--) {
        if (--st.remaining > 0) continue;
        st.phase = (st.phase + 1) & 3;
        if (st.phase === 1) st.target = 32 + rand(224);
        roll();
      }
    },
    /** Force a state (rows compare against retail's peeked one). */
    set(phase, total, remaining, target) { Object.assign(st, { phase, total, remaining, target }); },
  };
}
