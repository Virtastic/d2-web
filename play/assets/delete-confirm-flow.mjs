// The delete confirmation is a small state machine, not a fire-and-forget callback.  A character
// is only safe to select again after the IndexedDB delete AND the replacement listing have settled.
// Retail refuses a named character's delete while the realm connection is gone: instead of deleting, it
// opens the OK-only popup with string 5211 and the character stays.  `connected` is asked on YES; an empty
// name deletes without the check.
export const DELETE_REFUSED_STRING = 5211;
export function createDeleteConfirmFlow(onDelete, connected = null) {
  let armed = -1, state = 'idle', error = '', notice = 0;
  return {
    get armed() { return armed; },
    get state() { return state; },
    get error() { return error; },
    get notice() { return notice; },
    get blocked() { return armed >= 0 || state === 'notice' || state === 'pending' || state === 'refresh-failed'; },
    arm(index) { if (this.blocked) return false; armed = index; state = 'armed'; error = ''; return true; },
    // A failed refresh has no trustworthy replacement listing.  Dismissing it into the old list
    // would make a character whose delete already committed selectable again.
    cancel() { if (state === 'armed') { armed = -1; state = 'idle'; error = ''; } },
    // The popup's OK: it only closes; nothing was deleted.
    dismissNotice() { if (state !== 'notice') return false; state = 'idle'; notice = 0; return true; },
    async confirm(name) {
      if (armed < 0 || state === 'pending') return false;
      if (name && connected && !connected()) { armed = -1; state = 'notice'; notice = DELETE_REFUSED_STRING; return false; }
      state = 'pending'; error = '';
      try {
        const result = await onDelete?.(name);
        if (result?.status === 'deleted') { armed = -1; state = 'idle'; return true; }
        state = 'refresh-failed'; error = result?.error || 'character deletion could not refresh the saved-character list';
      } catch (cause) {
        state = 'refresh-failed'; error = String(cause?.message || cause);
      }
      return false;
    },
  };
}
