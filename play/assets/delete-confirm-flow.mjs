// The delete confirmation is a small state machine, not a fire-and-forget callback.  A character
// is only safe to select again after the IndexedDB delete AND the replacement listing have settled.
export function createDeleteConfirmFlow(onDelete) {
  let armed = -1, state = 'idle', error = '';
  return {
    get armed() { return armed; },
    get state() { return state; },
    get error() { return error; },
    get blocked() { return armed >= 0 || state === 'pending' || state === 'refresh-failed'; },
    arm(index) { if (this.blocked) return false; armed = index; state = 'armed'; error = ''; return true; },
    // A failed refresh has no trustworthy replacement listing.  Dismissing it into the old list
    // would make a character whose delete already committed selectable again.
    cancel() { if (state === 'armed') { armed = -1; state = 'idle'; error = ''; } },
    async confirm(name) {
      if (armed < 0 || state === 'pending') return false;
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
