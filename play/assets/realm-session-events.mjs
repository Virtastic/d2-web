// What a joined game's server packets ask the page to present, taken from the engine once per delivery and
// handed to the same presenters a local game uses: the message log, the unit sound player, the overhead text
// and the unit overlays. Nothing here reads a packet; the engine has already applied each one to its state.
//   0x2c      a unit makes a sound                        unitSound({ type, id, sound })
//   0x5a      a message line, text already composed       line(text)
//   0x76      a unit's overhead text is freed             clearOverhead(type, id)
//   0x8a      a townsperson has something to say          the unit carries an overlay row until it has played
//   0x88a70   an interface sound a message line asked for uiSound(id)
//   0x01      the game's flags arrived                    gameFlags({ difficulty, flags, expansion, ladder })
//   0x3f      the cursor took up an item to use           openInventory() when the item asks for it
//   0x47/0x48 a unit's worn items are to be read again    recheckItems(type, id)
import { eventColourCode } from './bnet-roster.mjs';

export function createRealmSessionPresenter(sinks = {}) {
  const overlays = new Map();   // `${type}:${id}` to { type, id, overlayId }
  const counts = new Map();
  function present(events) {
    for (const e of events ?? []) {
      counts.set(e.op, (counts.get(e.op) ?? 0) + 1);
      switch (e.op) {
        case 0x2c: sinks.unitSound?.({ type: e.a, id: e.b, sound: e.c }); break;
        case 0x5a: if (e.text) sinks.line?.(eventColourCode(e.b) + e.text, e); break;
        case 0x76: sinks.clearOverhead?.(e.a, e.b); break;
        case 0x8a: overlays.set(`${e.a}:${e.b}`, { type: e.a, id: e.b, overlayId: e.c }); break;
        case 0x88a70: sinks.uiSound?.(e.a); break;
        case 0x01: sinks.gameFlags?.({ difficulty: e.a, flags: e.b, expansion: e.c !== 0, ladder: e.d !== 0 }); break;
        case 0x3f: if (e.d) sinks.openInventory?.(); break;
        case 0x47: case 0x48: sinks.recheckItems?.(e.a, e.b); break;
        default: break;
      }
    }
    return events?.length ?? 0;
  }
  // The overlays the units carry this frame, through the overlay player a local game uses: each plays once and
  // is then let go. `fx` is that player (update(tick, hits) gives the live effects); the entities gain `.overlay`.
  function applyOverlays(fx, tick, entities) {
    if (!fx || !overlays.size) return 0;
    const live = new Map(fx.update(tick, [...overlays.values()]).map((f) => [`${f.type}:${f.id}`, f]));
    for (const key of [...overlays.keys()]) if (!live.has(key)) overlays.delete(key);
    for (const entity of entities ?? []) { const f = live.get(`${entity.type}:${entity.id}`); if (f) entity.overlay = f; }
    return live.size;
  }
  return { present, applyOverlays, get overlayCount() { return overlays.size; }, count: (op) => counts.get(op) ?? 0,
    reset() { overlays.clear(); counts.clear(); } };
}
