// Retail does not update the scrolling text object while its voice channel is active.
// Make that ordering explicit so the box's own first draw establishes its native clock origin.
export function createNpcSpeechScrollGate() {
  let token = 0;
  let ready = true;
  return {
    begin() {
      ready = false;
      return ++token;
    },
    cancel() {
      token++;
      ready = true;
    },
    get ready() { return ready; },
    async waitForVoice(current, playback, player) {
      try {
        const result = await playback;
        if (result?.started) await player?.whenDialogueDone?.();
      } finally {
        if (current === token) ready = true;
      }
    },
  };
}
