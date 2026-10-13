export const NPC_SPEECH_CLICK_COOLDOWN_MS = 100;

/** Reproduce D2Client's per-speech mouse-down gate (GetTickCount DWORD arithmetic). */
export function createNpcSpeechClickGate() {
  let openedAt = null;

  return {
    begin(now) { openedAt = Math.trunc(now) >>> 0; },
    cancel() { openedAt = null; },
    pointerDown(button, now) {
      if (openedAt === null || (button !== 0 && button !== 2)) return 'inactive';
      const elapsed = ((Math.trunc(now) >>> 0) - openedAt) >>> 0;
      return elapsed < NPC_SPEECH_CLICK_COOLDOWN_MS ? 'consumed' : 'dismiss';
    },
  };
}
