// Wire the synchronous journal and best-effort IndexedDB fallback into the page lifecycle.
export function installUnloadAutosave({ shouldSave, saveBytes, store, onStored, onError, teardown }) {
  addEventListener('beforeunload', () => {
    if (shouldSave()) {
      const bytes = saveBytes();
      if (bytes) {
        try { store.putDuringUnload(bytes); }
        catch (error) { onError(error); }
        // A full/quota-failed journal must not pretend to be durable: start the normal write too.
        store.put(bytes).then(onStored).catch(onError);
      }
    }
    teardown();
  });
}
