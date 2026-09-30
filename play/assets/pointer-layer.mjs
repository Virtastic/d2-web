// THE POINTER LAYER -- the one owner of "what is the pointer right now", drawn LAST: over the Esc
// menu, every panel and every sprite, exactly as D2Client draws its own pointer cel, and the owner of
// the browser arrow that must not duplicate it.
//
// It exists because the pointer was drawn inside the world pass while the Esc menu was drawn after
// it, so an open menu painted over the pointer (`render()` drew gameMenu after drawFrame) while the
// arrow stayed hidden -- the player lost the pointer. `s383-cursor-over-menu` measures both halves on
// the pixels: the pointer painted 385 px with the menu open overwriting 107 of them, and the held
// item's own art 1025 px with 103 overwritten.
//
// Retail's rule, in the order it applies:
//   * AN ITEM ON THE CURSOR IS RETAIL'S POINTER: the item's own inventory art follows the pointer and
//     the world cel is not drawn over it;
//   * otherwise the world cel the resolvers the click paths already use name (hand, hostile attack,
//     NPC talk) -- this module maps that answer to an asset and draws it, and recomputes no state;
//   * and an item whose art could not be resolved falls back to that cel rather than leaving the
//     player with no pointer at all. Never neither: when nothing at all can be drawn, the browser
//     arrow is restored.
//
// It returns what it drew (`item`/`game`, the same evidence shapes the shell has always exported, and
// the `cursor` style it set) so a row can carry the drawn identity through to the blit.
export function createPointerLayer({ cursorItem, gameCursor }) {
  return {
    /** Draws the pointer and takes the browser arrow with it. `items`/`cursorId`/`context` are the
     *  engine's answers, passed in per frame; nothing here derives game state of its own. */
    draw(ctx, canvas, { session, items, cursorId, pointerX, pointerY, context, tick }) {
      const held = cursorId ? items.find((i) => i.id === cursorId && i.mode === 4) || items.find((i) => i.id === cursorId) : null;
      const item = held && cursorItem ? cursorItem(ctx, canvas.width, canvas.height, held, pointerX, pointerY) : null;
      const game = session && !item && gameCursor ? gameCursor.draw(ctx, canvas, pointerX, pointerY, context, tick) : null;
      // A pointer the client drew is one the OS arrow must not duplicate; neither drawn is the arrow.
      const cursor = item || game ? 'none' : '';
      if (canvas.style.cursor !== cursor) canvas.style.cursor = cursor;
      return { item: held ? { engineId: cursorId, drawn: item || null } : null, game, cursor };
    },
  };
}
