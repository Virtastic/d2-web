// Object quest scrolls use the native speech callback's mouse-down gate. They
// own left/right pointer presses after the 100 ms speech-open interval.
export function dispatchObjectSpeechPointer({ screenIsGame, button, speech, dismiss, clickGate, now }) {
  if (!screenIsGame || !speech) return false;
  const disposition = clickGate.pointerDown(button, now);
  if (disposition === 'inactive') return false;
  if (disposition === 'dismiss') dismiss(speech);
  return true;
}
