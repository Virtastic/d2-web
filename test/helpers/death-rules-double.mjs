// A test double for the engine's two death-rule exports (realm-death.mjs bindDeathRules), for tests that run without
// the engine build. realm-death.test.mjs checks it against the engine whenever the build is present.
const CLOSES = new Set([1, 2, 3, 4, 5, 9, 10, 0x0b, 0x0c, 0x0d, 0x0f, 0x10, 0x12, 0x14, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d,
  0x1e, 0x1f, 0x20, 0x21, 0x24, 0x25]);
export const DEATH_RULES = Object.freeze({
  _d2_hero_is_down: (type, mode, flags) => +(((flags & 0x10000) !== 0) || (type === 0 && (mode === 0 || mode === 0x11))
    || (type === 1 && (mode === 0 || mode === 0x0c))),
  _d2_death_closes_panel: (panel) => +CLOSES.has(panel),
});
