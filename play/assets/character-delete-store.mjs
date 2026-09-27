// Store transaction for the character-select confirmation.  `remove` succeeding is not enough:
// the fresh listing is the state the player may select from, so a failed refresh is explicit.
export async function deleteAndRefreshCharacter(store, name) {
  try { await store.remove(name); }
  catch (cause) { return { status: 'delete-failed', error: `delete ${name}: ${cause}` }; }
  try { return { status: 'deleted', characters: await store.list() }; }
  catch (cause) { return { status: 'refresh-failed', error: `delete ${name} succeeded, but refreshing characters failed: ${cause}` }; }
}
