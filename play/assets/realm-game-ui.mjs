// The in-game UI a joined game's start packet leaves. The engine's port of the 1.13c client's handler
// (src/core/d2client/d2client_afa90.cpp) clears every panel flag and reopens the three a running game
// always has; this module only carries that to the page's panels when another start packet arrives.
// It decides nothing: a panel closes because its flag in the engine's table is 0.

/** The client's panel numbers the page has a panel for. NPC menu (8), shop (0x0C) and trade (0x17) close through their own server packets. */
export const REALM_PANELS = Object.freeze({ 0x01: 'inventory', 0x02: 'character', 0x04: 'skillTree', 0x05: 'chat', 0x09: 'gameMenu', 0x0a: 'automap',
  0x0f: 'quests', 0x14: 'waypoint', 0x16: 'party', 0x18: 'messageLog', 0x19: 'stash', 0x1a: 'cube', 0x21: 'help', 0x24: 'hireling' });

export function createRealmGameUi() {
  let seen = 0;
  return {
    /** `ui` is { starts, panels } (d2gsClientGameUi) or null; `close(names)` shuts the named page panels. Returns the names closed. */
    observe(ui, close) {
      if (!ui || ui.starts === seen) return [];
      seen = ui.starts;
      const names = Object.entries(REALM_PANELS).filter(([slot]) => !ui.panels[Number(slot)]).map(([, name]) => name);
      close(names);
      return names;
    },
  };
}
