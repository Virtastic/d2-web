const STORAGE_KEY = 'd2web.audio.npcSpeech';
export const NPC_SPEECH_DEFAULT = 2;

// Retail cycles 2 (Audio + Text), 1 (Text Only), 0 (Audio Only); s2134 record: first click gives Text Only.
export const NPC_SPEECH_CYCLE = Object.freeze([2, 1, 0]);

export function npcSpeechHasAudio(mode) {
  return mode === 0 || mode === 2;
}

export function npcSpeechHasText(mode) {
  return mode === 1 || mode === 2;
}

const validMode = (mode) => Number.isInteger(mode) && mode >= 0 && mode <= 2;

/** Browser preference for the native NPC Speech option (retail stores this per install). */
export function createNpcSpeechSetting({ storage = globalThis.localStorage } = {}) {
  let mode = NPC_SPEECH_DEFAULT;
  try {
    const raw = storage?.getItem(STORAGE_KEY);
    if (typeof raw === 'string' && /^[012]$/.test(raw)) mode = Number(raw);
  } catch { /* blocked or unavailable storage keeps retail's default */ }
  return {
    get mode() { return mode; },
    set(next) {
      if (!validMode(next)) throw new RangeError('NPC Speech mode must be 0, 1 or 2');
      mode = next;
      try { storage?.setItem(STORAGE_KEY, String(mode)); } catch { /* storage failure does not disable the option */ }
      return mode;
    },
  };
}
