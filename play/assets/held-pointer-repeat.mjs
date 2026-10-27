/** Queue the latest world aim once for each simulation advance while a button is held (left 0, right 2). */
export function queueHeldPointerRepeat(input, held, advanced, x, y, canRetarget, button = 0) {
  if (!held || !advanced || !canRetarget(x, y)) return false;
  input.pointer(x, y, button, true);
  return true;
}
