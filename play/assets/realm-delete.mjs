// DELETE CHARACTER on the realm character select: MCP CHARDELETE, then the roster the picker shows.
// Retail answers a refusal on the screen and leaves the character list as it was; the confirmation box closes either way.
import { mcpDeleteCharacter } from './realm-client.mjs';

/**
 * `session` is the realm MCP session (its `list.characters` is the roster); `toCard` maps a roster entry to a picker card.
 * Resolves { status: 'deleted', characters } on success or on a refusal (`refused` carries the result for the screen to show);
 * a dead transport rejects so the caller can drop back to the login screen.
 */
export async function deleteRealmCharacter(session, name, toCard, { send = mcpDeleteCharacter } = {}) {
  try {
    await send(session, name);
  } catch (error) {
    if (error?.result === undefined) throw error;
    return { status: 'deleted', refused: error.result, characters: session.list.characters.map(toCard) };
  }
  return { status: 'deleted', refused: 0, characters: session.list.characters.map(toCard) };
}
