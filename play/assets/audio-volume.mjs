// F235: the SOUND OPTIONS screen's Sound and Music sliders, and the browser audio graph they drive.
//
// The ENGINE has no volume to bind to, and neither does our prior art: `reference/D2MOO/source/
// D2Sound/src/D2Sound.cpp` is a stub with no `volume`/`gain`/`SNDVOL` symbol, and `grep -rn volume
// src/` is empty -- there is no settings surface to export. So the volume is the CLIENT's: the
// engine decides the game, the client plays it, and each player that produces sound gets one gain.
//
// What is retail's here: the slider VALUE at 0..1, by knob geometry (`knobX(v) = 339 + round(v*266)`
// fits every capture in `options-pages.mjs`; retail's own idle capture has Sound at full, knob 605,
// and Music at 472 = 0.5). What is OURS and NOT retail's -- declared in the decompile, and the
// capture ask F235 carries -- is (a) the mapping from that value to a Web Audio gain (here linear,
// gain = v) and (b) that the value persists in localStorage rather than wherever retail keeps it.
// Neither is a claim about retail; both are the browser's own playback, and neither is asserted as
// parity.

const STORAGE_KEY = 'd2web.audio.volume';
// Retail's own defaults, from its idle capture: Sound at full (knob 605), Music at 472 (0.5).
const DEFAULTS = Object.freeze({ sound: 1, music: 0.5 });

const clamp01 = (v) => { const n = Number(v); return Math.max(0, Math.min(1, Number.isFinite(n) ? n : 0)); };

/**
 * The Sound/Music slider values and the players they drive. A player joins with `attach(key, player)`
 * and must expose `setVolume(v)` (0..1) and a `volume` getter that reads its OWN live node, so
 * `evidence.applied` is what the audio graph actually holds rather than what this store asked for.
 */
export function createAudioVolume({ storage = globalThis.localStorage } = {}) {
  const values = { ...DEFAULTS };
  try {
    const raw = storage?.getItem(STORAGE_KEY);
    const saved = raw ? JSON.parse(raw) : null;
    for (const key of Object.keys(values)) if (Number.isFinite(Number(saved?.[key]))) values[key] = clamp01(saved[key]);
  } catch { /* absent, blocked or corrupt storage: retail's defaults stand and the slider still works */ }
  const attached = { sound: [], music: [] };
  const attach = (key, player) => {
    if (!player || typeof player.setVolume !== 'function' || attached[key].includes(player)) return player;
    attached[key].push(player); player.setVolume(values[key]); return player;
  };
  return {
    attach,
    get values() { return { ...values }; },
    /** The slider's own callback: record the value, drive the graph, persist. */
    set(key, value) {
      if (!(key in values)) throw new Error(`unknown audio channel ${key}`);
      values[key] = clamp01(value);
      for (const p of attached[key]) p.setVolume(values[key]);
      try { storage?.setItem(STORAGE_KEY, JSON.stringify(values)); } catch { /* a full/blocked store is not a crash */ }
      return values[key];
    },
    /** `values` is what the sliders hold (what the knobs are drawn from); `applied` is each player's
     *  own live node, read from the node, so a comparison of the two is an identity check. */
    get evidence() {
      const applied = {};
      for (const [key, players] of Object.entries(attached)) applied[key] = players.map((p) => p.volume);
      return { values: { ...values }, applied };
    },
  };
}
