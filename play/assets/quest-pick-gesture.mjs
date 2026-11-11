// Quest log rows and the description button act on release, not on press: the press latches the
// control under the pointer (a locked row latches nothing), and the release commits it only when it
// lands on the same control. Tabs still switch on press (the shell handles those directly).

/** The latch key for a quest-log hit: a quest number, 'descr', or null for anything else. */
export function questPickTarget(hit, statusOf) {
  if (hit?.descr) return 'descr';
  if (hit?.quest && statusOf(hit.quest) !== 'locked') return hit.quest;
  return null;
}

export function createQuestPickGesture() {
  let pointerId = null, target = null;
  return {
    press(id, t) {
      if (pointerId !== null || t == null) return false;
      pointerId = id; target = t;
      return true;
    },
    /** Returns the committed target (quest number or 'descr') or null. */
    release(id, t) {
      if (pointerId !== id) return null;
      const was = target;
      pointerId = null; target = null;
      return t === was ? was : null;
    },
    cancel(id) {
      if (pointerId !== id) return false;
      pointerId = null; target = null;
      return true;
    },
    get pointerId() { return pointerId; },
    get pressed() { return target; },
  };
}

/** Applies a committed target to the log state: a row selects it, the button toggles the description
 * when a quest is selected. Returns the new { selected, descrOn }. */
export function applyQuestPick(t, selected, descrOn) {
  if (t === 'descr') return { selected, descrOn: selected ? !descrOn : descrOn };
  if (t) return { selected: t, descrOn };
  return { selected, descrOn };
}
