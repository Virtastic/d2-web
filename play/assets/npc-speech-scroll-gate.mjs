// FUN_6FB2E970 skips the speech renderer only while the voice sound (DAT_6FBC3B0B) is NOT yet
// active (FUN_6FB384D0 == 0); once the channel is active the box draws WHILE the voice plays.
// The clock origin is the first draw after the voice has started (or immediately when none plays).
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
        await playback;
      } finally {
        if (current === token) ready = true;
      }
    },
  };
}
