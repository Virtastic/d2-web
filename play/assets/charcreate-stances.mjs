// Character-create hero stance machine.
//
// Prior art used: OpenDiablo2
// d2game/d2gamescreen/select_hero_class.go. Its `handleCursorButtonPress` starts the clicked
// hero's forward walk and changes every already-Selected hero to Retreating; `Advance` returns a
// completed forward walk to Selected and a completed back walk to Idle. Keeping that state here
// prevents the chosen form field from being mistaken for a sprite stance.

export const HERO_STANCE = Object.freeze({
  IDLE: 'Idle',
  IDLE_SELECTED: 'IdleSelected',
  APPROACHING: 'Approaching',
  SELECTED: 'Selected',
  RETREATING: 'Retreating',
});

/** Retail front-end durations are expressed in 25 Hz ticks, while the shell drives this at 60 Hz. */
export function createHeroStances(heroes, { retailHz = 25, updateHz = 60 } = {}) {
  const states = heroes.map(() => ({ stance: HERO_STANCE.IDLE, since: 0 }));
  const elapsedRetailTicks = (now, since) => Math.floor((now - since) * retailHz / updateHz);
  const durationTicks = (milliseconds) => Math.max(1, Math.round(milliseconds * retailHz / 1000));

  // A walk lasts its DC6's frame count in 25 Hz ticks, one frame per tick (F84, retail captures).
  const forwardTicks = (hero) => hero.forward?.count || durationTicks(hero.forwardMs);
  const settle = (now) => {
    for (let index = 0; index < states.length; index++) {
      const state = states[index], hero = heroes[index];
      if (state.stance === HERO_STANCE.APPROACHING
        && elapsedRetailTicks(now, state.since) >= forwardTicks(hero)) {
        state.stance = HERO_STANCE.SELECTED;
      } else if (state.stance === HERO_STANCE.RETREATING
        && elapsedRetailTicks(now, state.since) >= durationTicks(hero.backMs)) {
        state.stance = HERO_STANCE.IDLE;
      }
    }
  };

  return {
    settle,
    stance(index, now) { settle(now); return states[index]?.stance ?? HERO_STANCE.IDLE; },
    elapsed(index, now) {
      settle(now);
      return elapsedRetailTicks(now, states[index]?.since ?? now);
    },
    /** Hover only lands while no hero walks (OD2); clicks are not gated. */
    canSelect(now) {
      settle(now);
      return states.every(({ stance }) => stance !== HERO_STANCE.APPROACHING && stance !== HERO_STANCE.RETREATING);
    },
    choose(index, now) {
      settle(now);
      if (index < 0 || index >= states.length) return false;
      // Retail ignores every hero click while a forward walk runs (F99b, the retail captures).
      if (states.some(({ stance }) => stance === HERO_STANCE.APPROACHING)) return false;
      if (states[index].stance === HERO_STANCE.SELECTED) return false;
      for (const state of states) {
        if (state.stance === HERO_STANCE.SELECTED || state.stance === HERO_STANCE.APPROACHING) {
          state.stance = HERO_STANCE.RETREATING;
          state.since = now;
        }
      }
      states[index].stance = HERO_STANCE.APPROACHING;
      states[index].since = now;
      return true;
    },
    hover(index, now) {
      settle(now);
      if (!this.canSelect(now) || index < 0) return;
      const state = states[index];
      if (state.stance === HERO_STANCE.IDLE) state.stance = HERO_STANCE.IDLE_SELECTED;
    },
    unhover(index, now) {
      settle(now);
      if (index < 0) return;
      const state = states[index];
      if (state.stance === HERO_STANCE.IDLE_SELECTED) state.stance = HERO_STANCE.IDLE;
    },
    evidence(now) { settle(now); return states.map(({ stance }) => stance); },
  };
}
