// The closed realm's game list, on the realm (MCP) socket. A character of a closed realm never asks the chat
// server for games: the join panel sends the realm a list request (0x05), a row picked on it a game-info request
// (0x06), and Cancel on a pending create a cancel (0x13). Bodies only; the caller frames them.
// Pure of the page: bytes in, rows and text lines out.
const enc = new TextEncoder(), dec = new TextDecoder();
const u16 = (b, o) => b[o] | (b[o + 1] << 8);
const u32 = (b, o) => (b[o] | (b[o + 1] << 8) | (b[o + 2] << 16) | (b[o + 3] << 24)) >>> 0;
const cstr = (b, o) => { const e = b.indexOf(0, o); return e < 0 ? null : { text: dec.decode(b.subarray(o, e)), next: e + 1 }; };

export const MCP_GAMELIST = 0x05, MCP_GAMEINFO = 0x06, MCP_CANCEL = 0x13, MCP_QUEUE = 0x14;
// The list request's flag word: 0x800 for a hardcore character, 0 otherwise.
export const GAMELIST_HARDCORE = 0x800;
// A picked row's info request goes out on the panel's one-second tick, the last pick only.
export const GAME_INFO_TICK_MS = 1000;

/** MCP 0x05 request body: `[request id u16][flags u32][search text\0]`. The join panel always searches for the empty text. */
export function mcpGameListBody(requestId, { hardcore = false, search = '' } = {}) {
  const text = enc.encode(search), flags = hardcore ? GAMELIST_HARDCORE : 0;
  return Uint8Array.from([requestId & 255, (requestId >> 8) & 255, flags & 255, flags >> 8, 0, 0, ...text, 0]);
}

/** MCP 0x06 request body: `[request id u16][game name\0]`. */
export function mcpGameInfoBody(requestId, name) {
  return Uint8Array.from([requestId & 255, (requestId >> 8) & 255, ...enc.encode(String(name)), 0]);
}

/** MCP 0x13: no body at all. */
export const mcpCancelBody = () => new Uint8Array(0);

/**
 * MCP 0x05 reply body: `[request id u16][index u32][players u8][status u32][name\0][description\0]`, one game per
 * frame. `end` is set when anything follows the description's NUL: that frame closes the list and is not a game.
 * Status 0xFFFFFFFE also closes it, 0xFFFFFFFF empties it. Null for a body too short to hold the strings.
 */
export function parseMcpGameListReply(body) {
  if (body.length < 11) return null;
  const name = cstr(body, 11), description = name && cstr(body, name.next);
  if (!description) return null;
  return { requestId: u16(body, 0), index: u32(body, 2), players: body[6], status: u32(body, 7),
    name: name.text, description: description.text, end: description.next < body.length };
}

/**
 * MCP 0x06 reply body: `[request id u16][status u32][uptime u32][level u8][level difference u8][max players u8]
 * [count u8][class u8 x16][level u8 x16][description\0]` then `count` character names.
 */
export function parseMcpGameInfoReply(body) {
  if (body.length < 47) return null;
  const description = cstr(body, 46);
  if (!description) return null;
  const count = Math.min(body[13], 16), characters = [];
  let o = description.next;
  for (let i = 0; i < count; i++) {
    const name = cstr(body, o);
    if (!name) break;
    characters.push({ name: name.text, charClass: body[14 + i], level: body[30 + i] }); o = name.next;
  }
  return { requestId: u16(body, 0), status: u32(body, 2), uptime: u32(body, 6), level: body[10], levelDifference: body[11],
    maxPlayers: body[12], count: body[13], description: description.text, characters };
}

const CLASS_NAMES = ['Amazon', 'Sorceress', 'Necromancer', 'Paladin', 'Barbarian', 'Druid', 'Assassin'];
/**
 * The text the join panel shows for a picked game, a line per entry. The words are the client's own (they are not
 * in the string tables): the description if any, the elapsed time, the level range when the game has one, the
 * player limit when it is under eight, then each character's name and `Level N Class`.
 */
export function gameInfoLines(info) {
  const lines = [], two = (n) => String(n).padStart(2, '0');
  // GUESS: the blank line after the description and the one before the characters (two text-box appends whose
  // arguments were not traced); a retail frame of the info box with a described, populated game would settle it.
  if (info.description) lines.push(info.description, '');
  lines.push(`Elapsed Time: ${Math.trunc(info.uptime / 3600)}:${two(Math.trunc((info.uptime % 3600) / 60))}:${two(info.uptime % 60)}`);
  if (info.levelDifference < 100) {
    lines.push(info.levelDifference === 0 ? `Level ${info.level}`
      : `Level ${Math.max(1, info.level - info.levelDifference)} to ${Math.min(99, info.level + info.levelDifference)}`);
  }
  if (info.maxPlayers !== 0 && info.maxPlayers < 8) lines.push(`Up to ${info.maxPlayers} Players`);
  lines.push('');
  for (const c of info.characters) {
    if (!c.name) continue;
    lines.push(c.name, CLASS_NAMES[c.charClass] ? `Level ${c.level} ${CLASS_NAMES[c.charClass]}` : `Level ${c.level}`);
  }
  return lines;
}

/**
 * The join panel's realm side. `send(op, body)` puts a request on the realm socket, `nextId()` is the session's
 * request counter (shared with create and join). `frame({ op, body })` takes every realm frame and returns true
 * for the ones it used. `onGames(rows)` gets the list each time it is shown; `onInfo(lines)` a picked game's text.
 */
export function createMcpGameList({ send, nextId, hardcore = false, onGames = () => {}, onInfo = () => {}, now = () => Date.now() }) {
  let listId = -1, rows = [], open = false;
  let pending = null, infoId = -1, infoTick = -Infinity;
  const show = () => { open = false; onGames(rows.map((r) => ({ ...r }))); };
  return {
    /** Ask for the list (the panel opening, and every 15 s while it is open). */
    request() {
      // The realm this project is tested against answers an empty list with nothing at all, so a request that got
      // no closing frame is closed here, by the next one. GUESS: retail keeps its rows until a reply says otherwise;
      // a capture of a retail client against a realm that goes empty would settle which.
      if (open) show();
      listId = nextId(); rows = []; open = true;
      send(MCP_GAMELIST, mcpGameListBody(listId, { hardcore }));
      return listId;
    },
    /** A row was picked: its info is asked on the next one-second tick, and only for the last row picked. */
    select(name) { pending = { name: String(name), id: nextId() }; onInfo([]); },
    tick() {
      if (now() - infoTick < GAME_INFO_TICK_MS) return;
      infoTick = now();
      if (!pending) return;
      infoId = pending.id; send(MCP_GAMEINFO, mcpGameInfoBody(pending.id, pending.name)); pending = null;
    },
    frame({ op, body }) {
      if (op === MCP_GAMELIST) {
        const r = parseMcpGameListReply(body);
        if (!r || r.requestId !== listId) return true;   // an answer to an older request is dropped
        if (r.status === 0xffffffff) { rows = []; show(); }
        else if (r.status === 0xfffffffe || r.end) show();
        // One row per game index (its low word): the name is cut to 15 bytes and the description to 31.
        else if (!rows.some((g) => (g.index & 0xffff) === (r.index & 0xffff)))
          rows.push({ name: r.name.slice(0, 15), description: r.description.slice(0, 31), players: r.players, index: r.index & 0xffff });
        return true;
      }
      if (op === MCP_GAMEINFO) {
        const r = parseMcpGameInfoReply(body);
        if (!r || r.requestId !== infoId) return true;
        // GUESS: a refused info request (status -1 or -2) leaves the box empty; the client's branch for those two
        // results was not read.
        if (r.status < 0xfffffffe) onInfo(gameInfoLines(r));
        return true;
      }
      return false;
    },
    get rows() { return rows.map((r) => ({ ...r })); },
  };
}
