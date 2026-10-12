// D2Launch's SINGLE PLAYER release callback branches on whether its *.d2s search found a file.
// Keep storage failures separate from an empty profile so they remain visible to the player.
export function singlePlayerDestination({ hasSaveFiles, error = '' }) {
  if (error) return 'error';
  return hasSaveFiles ? 'select' : 'create';
}
