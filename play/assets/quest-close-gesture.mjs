export const questCloseFrame = (pressed) => pressed ? 11 : 10;

export function createQuestCloseGesture() {
  let pointerId = null;
  return {
    press(id) {
      if (pointerId !== null) return false;
      pointerId = id;
      return true;
    },
    release(id, overClose) {
      if (pointerId !== id) return false;
      pointerId = null;
      return Boolean(overClose);
    },
    cancel(id) {
      if (pointerId !== id) return false;
      pointerId = null;
      return true;
    },
    get pointerId() { return pointerId; },
    get pressed() { return pointerId !== null; },
  };
}
