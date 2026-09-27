// The unload hand-off is deliberately tiny: IndexedDB cannot be awaited from beforeunload.
// Keep its encoding and ordering here so save-store remains the owner of character records.

export const UNLOAD_JOURNAL_KEY = 'd2-web-unload-save-v2';
const REVISION_CLOCK_KEY = 'd2-web-save-revision-v1';
const JOURNAL_PREFIX = `${UNLOAD_JOURNAL_KEY}:`;

// A tab can only synchronously leave one .d2s behind, but several characters can close before a
// later document has a chance to import them. Keep those hand-offs disjoint by the name retail put
// in the save header. encodeURIComponent makes this a localStorage key, rather than an implicit
// restriction on character-name punctuation.
export function unloadJournalKey(name) {
  return `${JOURNAL_PREFIX}${encodeURIComponent(name)}`;
}

function base64(bytes) {
  let binary = '';
  // btoa takes a string, not a typed array. Small chunks avoid a huge argument list for saves
  // containing a full inventory.
  for (let at = 0; at < bytes.length; at += 0x4000)
    binary += String.fromCharCode(...bytes.subarray(at, at + 0x4000));
  return btoa(binary);
}

function fromBase64(text) {
  if (typeof text !== 'string') return null;
  try {
    const binary = atob(text), bytes = new Uint8Array(binary.length);
    for (let i = 0; i < binary.length; i++) bytes[i] = binary.charCodeAt(i);
    return bytes;
  } catch { return null; }
}

export function createUnloadSaveJournal(localStorageObject = globalThis.localStorage) {
  let fallback = 0;
  // A clock in localStorage is synchronous across same-origin tabs. Timestamp-sized revisions
  // also make a failed clock write safely later than records made before it.
  function nextRevision() {
    const floor = Date.now() * 1000 + fallback++;
    try {
      const prior = Number(localStorageObject?.getItem(REVISION_CLOCK_KEY)) || 0;
      const revision = Math.max(floor, prior + 1);
      localStorageObject?.setItem(REVISION_CLOCK_KEY, String(revision));
      return revision;
    } catch { return floor; }
  }
  return {
    nextRevision,
    readAll() {
      const keys = new Set();
      try {
        // The un-suffixed key is the v2 hand-off written before journals became per-character.
        // Import it once if valid, so updating the page does not strand a player's last save.
        if (localStorageObject?.getItem(UNLOAD_JOURNAL_KEY)) keys.add(UNLOAD_JOURNAL_KEY);
        // localStorage.key(i) is an index over a live collection. A different same-origin tab can
        // remove the key we just read, shifting the next journal into its index; one index pass
        // would then skip that remaining hand-off. Keep scanning until a complete pass finds no
        // unfamiliar key. Reads stay synchronous for beforeunload, and the Set also leaves a
        // concurrent writer's journal available for the next restore when it appears too late.
        let foundNew;
        do {
          foundNew = false;
          for (let i = 0; i < (localStorageObject?.length || 0); i++) {
            const key = localStorageObject.key(i);
            if (key?.startsWith(JOURNAL_PREFIX) && !keys.has(key)) {
              keys.add(key);
              foundNew = true;
            }
          }
        } while (foundNew);
      } catch { return []; }
      return [...keys].flatMap((key) => {
        try {
          const raw = localStorageObject.getItem(key);
          if (!raw) return [];
          const record = JSON.parse(raw), data = fromBase64(record.data);
          if (!data || !record.name || !Number.isSafeInteger(record.revision) || record.revision < 1) return [];
          // A malformed or manually-created suffix must not be allowed to impersonate another
          // character's journal. The old unsuffixed v2 entry predates this invariant.
          if (key !== UNLOAD_JOURNAL_KEY && key !== unloadJournalKey(record.name)) return [];
          return [{ key, raw, record: { ...record, data } }];
        } catch { return []; }
      });
    },
    write(record) {
      // Base64 is compact and avoids JSON's several-times-larger array-of-numbers representation.
      if (!record?.name || typeof localStorageObject?.setItem !== 'function')
        throw new Error('localStorage unavailable for unload journal');
      localStorageObject.setItem(unloadJournalKey(record.name), JSON.stringify({ ...record, data: base64(record.data) }));
    },
    discard(found) {
      try {
        // Do not erase a newer journal a second tab wrote after we read this one.
        if (found?.key && localStorageObject?.getItem(found.key) === found.raw) localStorageObject.removeItem(found.key);
      } catch {}
    },
    invalidate(name) {
      if (!name) {
        for (const found of this.readAll()) this.discard(found);
        return;
      }
      // A delete for ALPHA must never clear BETA's hand-off. This also reads the legacy entry so
      // an update cannot resurrect a character the player has just deleted.
      for (const found of this.readAll()) if (found.record.name === name) this.discard(found);
    },
  };
}
