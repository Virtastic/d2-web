// The online town NPC's clicks, routed to the state machine (realm-npc-state.mjs). The drawing stays with the
// retail renderers (npc-menu.mjs, speech-box.mjs, vendor-panel.mjs); this file only decides what a click means
// and turns the server's stock into the vendor panel's item records. It holds no game state of its own.
import { decodeItemHeader } from './realm-world.mjs';
import { SELL_MODE } from './realm-npc-packets.mjs';

/**
 * The server's stock (realm-npc-state `view().stock`: { guid, bits, tab, x, y }) as the vendor panel's item
 * records. The grid size and the inventory icon come from the item table by the item's code; a stock entry
 * whose code is unknown is left out rather than drawn without an icon.
 */
export function vendorItemsFrom(stock, itemTable) {
  const items = [];
  for (const s of stock || []) {
    const header = decodeItemHeader(s.bits);
    const entry = header ? itemTable?.byCode(header.code) : null;
    if (!entry) continue;
    items.push({ id: s.guid, guid: s.guid, code: header.code, quality: header.quality, invFile: entry.invFile,
      invPage: s.tab, gridX: s.x, gridY: s.y, invWidth: entry.invWidth, invHeight: entry.invHeight });
  }
  return items;
}

/**
 * The shop's buttons, by the retail vendorButtons() names. Buy acts on the offer's press (the shell calls buyAt),
 * the rest act on release, as the retail vendor window does.
 */
export function createRealmNpcUi({ npc }) {
  return {
    /** A menu row was chosen (on the matching release). Returns whatever the state machine returned. */
    choose: (entry) => npc.choose(entry),
    /** A speech line was dismissed (either button, after the 100 ms gate the screen applies). */
    dismissSpeech: () => npc.dismissSpeech(),
    /** Esc, walking away, or the menu's cancel outside its box. */
    end: () => npc.end(),
    /** A stock offer was bought (a right press, or Yes on the Buy confirm). */
    buyAt: (guid) => npc.buy(guid),
    /** The displayed price of a stock offer, or null while no seam supplies it (drawn as '?'). */
    priceAt: (guid) => npc.priceOf(guid),
    /** A hire offer was chosen in the hire window: the 0x36 names the merc by its name id. */
    hire: (mercName) => npc.hire(mercName),
    /** The armed buy was dropped (the offer rule's disarm): the screen closes its confirm box. */
    disarm: () => true,
    /** The Repair box was declined: Repair mode ends. */
    endRepair: () => { npc.setRepairMode(false); return true; },
    /** Continue on the refusal box (0x4AAF0): the box closes, the cursor modes end. */
    dismissRefusal: () => npc.dismissRefusal(),
    /**
     * A carried item was clicked: sold when sell mode is armed, repaired when repair mode is armed. `info` is read
     * from the item: { mode, durability (stat 0x48), cost (the displayed price) }.
     */
    carriedClick(guid, view, info = {}) {
      if (view?.sellMode) return npc.sell(guid, info.mode ?? SELL_MODE.STORED, info.cost ?? 0);
      if (view?.repairMode) return npc.repair(guid, info.mode ?? 0, info.durability ?? 0);
      return false;
    },
    /** A vendor button was released. Sell and repair are toggles, exit closes the shop, repairAll repairs everything. */
    buttonRelease(button, view) {
      switch (button) {
        case 'sell': npc.setSellMode(!view?.sellMode); return true;
        case 'repair': npc.setRepairMode(!view?.repairMode); return true;
        case 'repairAll': return npc.repairAll();
        case 'exit': return npc.end();
        default: return false;
      }
    },
    /** The NPC left the scene (walked out of range or was removed): the conversation ends with it. */
    walkedAway(guid, sceneMonsters) {
      const view = npc.view();
      if (!view || view.guid !== guid) return false;
      if (sceneMonsters?.some((m) => m.guid === guid)) return false;
      return npc.end();
    },
  };
}
