// D2Launch's SINGLE PLAYER release callback branches on whether its *.d2s search found a file.
// Keep storage failures separate from an empty profile so they remain visible to the player.
export function singlePlayerDestination({ hasSaveFiles, error = '' }) {
  if (error) return 'error';
  return hasSaveFiles ? 'select' : 'create';
}

// Native save enumeration and destination setup finish inside one release callback. IndexedDB
// yields to later input; only the current request may publish a profile or change its screen.
export function createCharacterEntryReader() {
  let generation = 0;
  return {
    cancel() { generation++; },
    async read({ load, isCurrent, apply, describeError }) {
      const request = ++generation;
      let profile, error = '';
      try { profile = await load(); }
      catch (cause) { profile = { characters: [], hasSaveFiles: false }; error = describeError(cause); }
      if (request !== generation || !isCurrent()) return false;
      apply(profile, error);
      return true;
    },
  };
}
