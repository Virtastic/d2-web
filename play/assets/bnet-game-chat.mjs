// In-game chat over the game server connection: what Enter sends and how a chat packet from the
// server reads in the message log. Pure of the engine and the DOM, so the bytes are testable in node.
//
// Typed say: [0x15][type 1][language][text NUL][recipient NUL][0]. The client packet sizer reads two
// NUL-terminated strings and then a signed count byte, so the packet is text + 6 bytes with no recipient.
const MAX_TEXT = 255, MAX_NAME = 15;
const latin1 = (s) => Array.from(String(s), (c) => { const n = c.charCodeAt(0); return n < 256 ? n : 0x3f; });

// opcode 0x15 is chat (say type 1, whisper type 2); opcode 0x14 with type 0 is the overhead line ("!text").
export function gameChatPacket(text, { language = 0, whisperTo = '', overhead = false } = {}) {
  const body = latin1(text).filter((b) => b !== 0).slice(0, MAX_TEXT);
  const name = whisperTo ? latin1(whisperTo).filter((b) => b !== 0).slice(0, MAX_NAME) : [];
  return Uint8Array.from([overhead ? 0x14 : 0x15, overhead ? 0 : name.length ? 2 : 1, language & 0xff, ...body, 0, ...name, 0, 0]);
}

// A typed line that goes to the game as chat. A leading "/" is a command: the game does not carry it
// (the whisper and the realm commands are separate paths, see routeGameLine), so it is not said aloud.
export function isSayable(text) {
  const t = String(text);
  return t.length > 0 && t[0] !== '/';
}

// The string table ids the chat paths use, with the English text as the fallback when the engine has none.
export const CHAT_STR = {
  4048: ': ',
  3654: ' whispers: ',
  3658: 'ÿc0You whispered to ÿc1%sÿc0: %s',
};
const fixCodes = (s) => String(s).replace(/Ã¿/g, 'ÿ');
export function chatString(id, lookup, table = CHAT_STR) {
  const s = lookup?.(id);
  return fixCodes(s || table[id] || '');
}
// printf for the two forms the tables use: %s and %d, left to right.
export function fillFormat(fmt, ...args) {
  let i = 0;
  return fmt.replace(/%[sd]/g, () => String(args[i++] ?? ''));
}

const str = (bytes) => String.fromCharCode(...bytes);
const validName = (n) => n.length >= 1 && n.length <= MAX_NAME;
// The server's chat packet as a message-log line, or null for a kind not shown here.
//   1 = said aloud: the sender in gold, then ": " and the text
//   2 = whisper from a player: "name whispers: text"
//   4 = system line: text only
//   6 = your own whisper echoed: "You whispered to name: text"
// Types 5 and 7 are not lines (5 is the overhead bubble, see gameChatBubble).
// `lookup(id)` is the engine's string by index; without it the English text is used.
export function gameChatLine(chat, lookup) {
  if (!chat) return null;
  const name = str(chat.name), text = str(chat.message);
  const S = (id) => chatString(id, lookup);
  switch (chat.messageType) {
    case 4: return text;
    case 6: return fillFormat(S(3658), name, text);
    case 2: return validName(name) ? name + S(3654) + text : text;
    case 1: {
      // A sender of unit type 0 or 1 queues nothing legible; the server does not send those.
      if (chat.subType === 0 || chat.subType === 1) return null;
      return validName(name) ? `ÿc4${name}ÿc0${S(4048)}${text}` : text;
    }
    default: return null;
  }
}

// Type 5: an overhead line over the speaking unit. { unitType, unitGuid, text } or null.
export function gameChatBubble(chat) {
  if (!chat || chat.messageType !== 5) return null;
  const text = str(chat.message);
  return { unitType: chat.subType, unitGuid: chat.unitGuid, text };
}

// What Enter does with a typed line in a game. `roster.find(name)` answers whether a player of that name
// is in the game. Returns the actions the shell performs, in order:
//   { send: bytes }       a packet for the game server
//   { realm: line }       a line for the realm chat connection (when one is open)
//   { notice: text }      a local system line
//   { client: word }      a local client toggle ('fps', 'nopickup'): nothing to send
// The checks run in the client's order: exact local words first, then the leading character of the line
// ("!" overhead, "@name" whisper, "/" command), then plain say.
const WHISPER_WORDS = ['/msg', '/m', '/whisper', '/w'];
const HELP_WORDS = ['/help', '/?'];
const isWord = (line, w) => line.length >= w.length && line.slice(0, w.length).toLowerCase() === w
  && (line.length === w.length || line[w.length] === ' ');
const usage = (cmd, tail = '') => `<BattleNet>: Correct usage is: ${cmd}${tail}`;
export function routeGameLine(line, { roster, language = 0, multiplayer = true } = {}) {
  const text = String(line);
  const lower = text.toLowerCase();
  if (lower === '/fps' || lower === '/framerate') return [{ client: 'fps' }];
  if (lower === '/nopickup') return [{ client: 'nopickup' }];
  if (text[0] === '!' && text.length > 1) return [{ send: gameChatPacket(text.slice(1), { language, overhead: true }) }];
  if (multiplayer && text[0] === '@') {
    const m = /^@(\S+)\s*(.*)$/.exec(text);
    if (m && m[1].length < 0x32) {
      if (!m[2]) return [];
      return [{ send: gameChatPacket(m[2], { language, whisperTo: m[1] }) }];
    }
  }
  if (text[0] === '/') {
    for (const cmd of WHISPER_WORDS) {
      if (!isWord(text, cmd)) continue;
      const parts = text.split(/ +/).filter(Boolean);
      if (parts.length < 3) return [{ notice: usage(cmd, ' (name) (message)') }];
      const who = parts[1];
      const rest = text.replace(/^ *\S+ +\S+ +/, '');
      if (roster?.find(who)) return [{ send: gameChatPacket(rest, { language, whisperTo: who }) }];
      return [{ realm: `${parts[0]} ${who} ${rest}` }];
    }
    for (const cmd of HELP_WORDS) {
      if (!isWord(text, cmd)) continue;
      const parts = text.split(/ +/).filter(Boolean);
      if (parts.length === 1) return [{ realm: text }];
      return [{ notice: usage(cmd) }];
    }
    return [{ realm: text }];
  }
  if (text.length > 0) return [{ send: gameChatPacket(text, { language }) }];
  return [];
}
