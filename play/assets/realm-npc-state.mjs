// The client side of a town NPC in a Battle.net game: the conversation the server drives (the NPC's message
// list, the shop stock, the transaction results, the hire offers) and the packets the player's choices send. It
// owns no drawing: the shell reads `view()` for the menu, the speech, the shop and the hire window, and calls the
// methods below on clicks. The menu rows are the retail table (npc-menu.mjs); the flow is
//   talk click, 0x13, S2C 0x27 (the lines), 0x2F (open), menu or speech, then an entry's request
//   (0x38 trade, gamble or hire list; 0x34 identify; 0x62 resurrect), answered by S2C 0x9C stock and 0x2A result.
// Hire: retail's menu build sends 0x38 {hire} (13 bytes: 0x38, action u32, NPC guid, the hero's unit id) when the
// conversation opens, ahead of any speech, for the four mercenary hirers (not when the player picks Hire); the server answers 0x4F then one 0x4E per offer, which the hire window lists.
import { npcMenuEntries, talkEntries, entriesAfterSpeech, withTravelRow, withHireRow } from './npc-menu.mjs';
import { npcPackets, NPC_ACTION, SELL_MODE, parseNpcInfo, parseTransaction, parseShopItem, parseHireOffer, HIRE_LIST_HEAD } from './realm-npc-packets.mjs';
import { decodeItemHeader } from './realm-world.mjs';
import { packets, UNIT } from './realm-input.mjs';
import { refusalString } from './vendor-repair-confirm.mjs';

const MENU_SPEECH = 0;   // messages with this menu number are spoken at once; menu 2 are the Talk box's quest topics
/** The hirers whose menu build sends the hire list request (0x48B20: classes 0x96, 0xC6, 0xFC, 0x203). */
export const HIRE_LIST_CLASSES = Object.freeze([150, 198, 252, 515]);

/** The 0x2F that opens the conversation with an NPC and the 0x30 that ends it. */
export const chatStart = (guid) => packets.npcChatStart(UNIT.MONSTER, guid);
export const chatEnd = (guid) => packets.npcChatEnd(UNIT.MONSTER, guid);

/**
 * deps: send(bytes) puts a packet on the wire; classOf(guid) is the NPC's class id; questDone(act, quest) and
 * questName(act, n) answer from the hero's quests; deadMerc() says whether the hireling is dead;
 * transactionCost(item, kind) is the displayed price, kind 'buy' | 'sell' | 'repair', or null while no engine
 * seam supplies it (the server charges its own price); hireStats(mercName, seed) is the offer's
 * { name, level, life, def, cost, skill } or null (the engine seam is MONSTERS_HirelingInit); playerGuid() is
 * the hero's unit id for the hire form of the 0x38 request (trade and gamble carry 0).
 */
export function createRealmNpc({ send, classOf = () => -1, questDone = () => false, questName = () => '',
  deadMerc = () => false, transactionCost = () => null, hireStats = () => null, playerGuid = () => 0xffffffff } = {}) {
  let npc = null;            // { guid, classId, messages, entries, speech, spoken, mode, offers }
  let stock = new Map();     // item guid -> { guid, bits, tab, x, y }
  let result = null;         // the last transaction outcome
  let sellMode = false, repairMode = false;

  function close(notify) {
    if (npc && notify) send(chatEnd(npc.guid));
    npc = null; stock = new Map(); sellMode = false; repairMode = false;
  }

  function menuFor(classId) {
    const entries = npcMenuEntries(classId);
    return entries ? withHireRow(withTravelRow(entries, classId, questDone), classId, questDone) : null;
  }

  /** The menu is shown; its hire request went out when the conversation opened. */
  function openMenu() { npc.mode = 'menu'; }

  function onInfo(info) {
    const classId = classOf(info.guid);
    const messages = [...info.messages].sort((a, b) => a.stringId - b.stringId);
    const speech = messages.filter((m) => m.menu === MENU_SPEECH);
    const entries = menuFor(classId);
    if (!entries && !speech.length) return;   // neither a menu nor a line: nothing to open
    npc = { guid: info.guid, classId, messages, entries, speech, spoken: 0, mode: 'speech', offers: [] };
    stock = new Map();
    send(chatStart(info.guid));
    // Retail's conversation open builds the menu at once (before any line is played), and the build asks the
    // four hirers for their offers, so the request leaves here, ahead of the speech.
    if (entries && HIRE_LIST_CLASSES.includes(classId)) send(npcPackets.request(NPC_ACTION.HIRE, info.guid, playerGuid()));
    if (!speech.length) openMenu(); else npc.mode = 'speech';
  }

  function onShop(item) {
    if (!npc) return;
    if (!item.add) { stock.delete(item.guid); return; }
    const header = decodeItemHeader(item.bits);
    if (!header) return;
    stock.set(item.guid, { guid: item.guid, bits: item.bits, tab: header.page, mode: header.mode, x: header.x, y: header.y });
  }

  /** One hire offer: the seam's stats, or nulls where the seam knows none (shown as unknown, never a made-up number). */
  function onOffer(offer) {
    const s = hireStats(offer.mercName, offer.seed) || {};
    npc.offers.push({ mercName: offer.mercName, seed: offer.seed, name: s.name ?? '', level: s.level ?? null, life: s.life ?? null,
      def: s.def ?? null, cost: s.cost ?? null, skill: s.skill ?? '' });
  }

  const api = {
    /** The player clicked another town NPC: an open conversation with a different one ends. */
    talkSent(guid) { if (npc && npc.guid !== guid) close(true); },
    /** Apply one complete server packet. */
    handle(p) {
      switch (p[0]) {
        case 0x27: { const info = parseNpcInfo(p); if (info) onInfo(info); break; }
        case 0x2a: { const t = parseTransaction(p); if (t) { result = { ...t, refusal: refusalString(t.result) }; if (npc?.mode === 'hire' && t.result <= 6) close(true); } break; }
        case 0x9c: { const item = parseShopItem(p); if (item) onShop(item); break; }
        case HIRE_LIST_HEAD: if (npc) npc.offers = []; break;
        case 0x4e: { const offer = parseHireOffer(p); if (offer && npc) onOffer(offer); break; }
        default: break;
      }
    },
    feed(bytes, sizeOf) {
      for (let at = 0; at < bytes.length;) {
        const n = sizeOf(bytes.subarray(at));
        if (!(n > 0) || at + n > bytes.length) break;
        api.handle(bytes.subarray(at, at + n)); at += n;
      }
    },
    /** What the shell draws: null when no NPC is open. */
    view() {
      if (!npc) return null;
      return { guid: npc.guid, classId: npc.classId, mode: npc.mode, entries: npc.entries, speech: npc.speech[npc.spoken] ?? null,
        stock: [...stock.values()], offers: npc.offers, result, sellMode, repairMode };
    },
    isOpen: () => npc !== null,
    shopOpen: () => npc !== null && (npc.mode === 'trade' || npc.mode === 'gamble'),
    /** The player dismissed the current speech: the server is told which line it was (0x31), then the menu returns. */
    dismissSpeech() {
      if (!npc || npc.mode !== 'speech') return false;
      const line = npc.speech[npc.spoken];
      if (line) send(npcPackets.messageDone(npc.guid, line.stringId));
      npc.spoken++;
      if (npc.spoken >= npc.speech.length) {
        if (npc.entries) openMenu(); else close(true);
      }
      return true;
    },
    /** Pick a menu row (an entry of `view().entries`). */
    choose(entry) {
      if (!npc || (npc.mode !== 'menu' && npc.mode !== 'talk')) return false;
      switch (entry.action) {
        case 'talk': npc.entries = talkEntries(npc.messages, questName); npc.mode = 'talk'; return true;
        case 'topic': send(npcPackets.messageDone(npc.guid, entry.stringId));
          npc.entries = entriesAfterSpeech(npc.entries, entry.stringId); return true;
        case 'intro': case 'gossip': return true;   // the line is played by the shell; nothing goes to the server
        case 'trade': npc.mode = 'trade'; send(npcPackets.request(NPC_ACTION.TRADE, npc.guid, 0)); return true;
        case 'gamble': npc.mode = 'gamble'; send(npcPackets.request(NPC_ACTION.GAMBLE, npc.guid, 0)); return true;
        // The hire window lists the offers received at menu build (0x38 sent then): choosing Hire sends nothing.
        case 'hire':
          if (deadMerc()) { send(npcPackets.resurrectMerc(npc.guid)); close(true); return true; }
          npc.mode = 'hire'; return true;
        case 'identify': send(npcPackets.identifyAll(npc.guid)); close(true); return true;
        case 'cancel': close(true); return true;
        default: return false;   // travel and the other rows are the shell's own
      }
    },
    /** Esc, walking away, or the exit button. */
    end() { if (!npc) return false; close(true); return true; },
    /** The displayed price of a stocked item, or null while no seam supplies it. */
    priceOf(itemGuid) {
      const item = stock.get(itemGuid);
      if (!item) return null;
      const cost = transactionCost(item, 'buy');
      return Number.isFinite(cost) ? cost : null;
    },
    buy(itemGuid, multi = false) {
      const item = stock.get(itemGuid);
      if (!item || !api.shopOpen()) return false;
      // With no price seam (unwired test or no player) the packet's cost field is 0.
      const cost = api.priceOf(itemGuid) ?? 0;
      send(npcPackets.buy(npc.guid, itemGuid, item.mode ?? 0, cost, { gamble: npc.mode === 'gamble', multi })); return true;
    },
    /** Sell a carried item: `info` is { mode, cost } read from the item (its own mode, the displayed price). */
    sell(itemGuid, mode = SELL_MODE.STORED, cost = 0) {
      if (!api.shopOpen()) return false;
      send(npcPackets.sell(npc.guid, itemGuid, mode, cost)); return true;
    },
    /** Repair one carried item: its mode and durability stat go in the packet. */
    repair(itemGuid, mode = 0, durability = 0) {
      if (!api.shopOpen()) return false; send(npcPackets.repair(npc.guid, itemGuid, mode, durability)); return true;
    },
    repairAll() { if (!api.shopOpen()) return false; send(npcPackets.repairAll(npc.guid)); return true; },
    hire(hirelingId) {
      if (!npc || npc.mode !== 'hire') return false;
      send(npcPackets.hire(npc.guid, hirelingId)); return true;   // the conversation ends on the 0x2A result
    },
    /** A gambled item is bought unidentified; this identifies it. */
    identifyGambled(itemGuid) { send(npcPackets.identifyGambled(itemGuid)); return true; },
    setSellMode(on) { sellMode = Boolean(on); if (on) repairMode = false; },
    setRepairMode(on) { repairMode = Boolean(on); if (on) sellMode = false; },
    takeResult() { const r = result; result = null; return r; },
  };
  return api;
}
