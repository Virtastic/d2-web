// The Battle.net LADDER window's data: MCP LADDERDATA (0x11). The request is `[mode u8][request id u16 LE]`; the mode is the
// ladder's base plus a class tab (0 = every class). The reply is `[flag u8][total u16][chunk u16][offset u16][data]`, the data
// `[rankBase u32][count u32][nameWidth u32]` and per entry `[expLo u32][expHi u32][stats u32][name, nameWidth bytes]`. An empty
// ladder is 14 zero bytes. The realm may not filter by the mode, so rows carry their flags and the screen filters again.
import { mcpLadderRequest } from './realm-client.mjs';

// string.tbl 5317 / 5318 / 10090 / 10091 (the LADDER panel's buttons), each with the base mode D2Multi's button handler stores.
export const LADDER_BASE = [0x09, 0x00, 0x1b, 0x13];   // by button kind: standard, hardcore, expansion, expansion hardcore
export const ladderMode = (kind, classTab = 0) => LADDER_BASE[kind] + classTab;
export const ladderRequestBody = (mode, id = 1) => new Uint8Array([mode & 0xff, id & 0xff, (id >> 8) & 0xff]);

const u32 = (b, o) => (b[o] | (b[o + 1] << 8) | (b[o + 2] << 16) | (b[o + 3] << 24)) >>> 0;
const u16 = (b, o) => b[o] | (b[o + 1] << 8);

/** One LADDERDATA reply body to `{ flag, entries }`; `entries` is [] for the empty form. Throws on a malformed body. */
export function parseLadderReply(body) {
  if (body.length < 7) throw new Error(`MCP LADDERDATA body is short (${body.length} bytes)`);
  const flag = body[0], total = u16(body, 1), chunk = u16(body, 3), offset = u16(body, 5);
  if (total === 0) return { flag, entries: [] };
  if (offset + chunk > total || 7 + chunk > body.length) throw new Error('MCP LADDERDATA chunk runs past its buffer');
  const data = new Uint8Array(total); data.set(body.subarray(7, 7 + chunk), offset);
  return { flag, entries: parseLadderData(data), partial: chunk < total };
}

export function parseLadderData(data) {
  if (data.length < 12) throw new Error('MCP LADDERDATA data is short');
  const count = u32(data, 4), width = u32(data, 8), entries = [];
  if (width > 64 || 12 + count * (12 + width) > data.length) throw new Error('MCP LADDERDATA entries run past the data');
  for (let i = 0, at = 12; i < count; i++, at += 12 + width) {
    const stats = u32(data, at + 8);
    let end = at + 12; while (end < at + 12 + width && data[end]) end++;
    entries.push({ rank: i + 1, name: new TextDecoder().decode(data.subarray(at + 12, end)), experience: u32(data, at),
      charClass: stats & 0xf, died: !!(stats & 0x10), expansion: !!(stats & 0x20), hardcore: !!(stats & 0x40),
      progression: (stats >> 8) & 0xff, level: (stats >>> 16) & 0xff });
  }
  return entries;
}

/** The rows one ladder shows: the realm sends every character by experience, so the kind (0 standard, 1 hardcore, 2 expansion, 3 expansion hardcore) and the class tab filter here. */
export function ladderRows(entries, kind, classTab = 0) {
  const expansion = kind >= 2, hardcore = kind === 1 || kind === 3;
  return entries.filter((e) => e.expansion === expansion && e.hardcore === hardcore && (!classTab || e.charClass === classTab - 1))
    .map((e, i) => ({ ...e, rank: i + 1 }));
}

/** Ask the realm for a ladder over the MCP session and wait for the answer. */
export async function fetchLadder(session, kind, classTab = 0, { request = mcpLadderRequest } = {}) {
  return ladderRows((await request(session, ladderMode(kind, classTab))).entries, kind, classTab);
}
