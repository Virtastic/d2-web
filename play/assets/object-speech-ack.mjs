// D2Client 1.13c acknowledges these object-scroll string IDs with GUID -1
// when the scroll lifecycle completes (FUN_6FB2D200 -> FUN_6FB2CEF0).
const NATIVE_SCROLL_ACK_IDS = new Set([127, 396, 20002, 20131, 20169]);

export function nativeObjectSpeechAcknowledgement(speech) {
  if (!speech || !NATIVE_SCROLL_ACK_IDS.has(speech.stringId)) return null;
  return { guid: 0xffffffff, stringId: speech.stringId };
}
