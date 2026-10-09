/** Queue the latest world aim once for each simulation advance while left-click is held. */
export function queueHeldPointerRepeat(input, held, advanced, x, y, canRetarget) {
  if (!held || !advanced || !canRetarget(x, y)) return false;
  input.pointer(x, y, 0, true);
  return true;
}
