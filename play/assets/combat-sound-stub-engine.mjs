// Test double for combatSoundEngine(Module): the client sound list as a pass-through (every submit starts at the next
// pump with its own row), and the engine's mode/impact rows supplied by the test. Test-only.
export function stubCombatEngine(over = {}) {
  let next = 1; const live = new Map();
  const queue = {
    submit: (row) => { const h = next++; live.set(h, row); return h; },
    pump: () => { const out = [...live].map(([handle, row]) => ({ handle, asked: row, row, start: 0 })); live.clear(); return out; },
    ended() {}, state: () => 0, hero() {}, stamp() {}, busy: () => false, reset() { live.clear(); },
  };
  return { modeSounds: () => [], impactSounds: () => [], queue, ...over };
}
