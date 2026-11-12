// The Battle.net chat leg on the BNCS socket the login leaves open (retail keeps it for the whole
// session): ENTERCHAT (0x0A) names the character, JOINCHANNEL (0x0C) enters the channel, CHATCOMMAND
// (0x0E) carries a typed line, and CHATEVENT (0x0F) brings every line, join and leave back.
// Frames are `FF id len16LE body`. Pure of the engine, so the bytes are testable in node.
import { getGamesPacket, GAME_LIST_ID } from './bnet-gamelist.mjs';
import { newsRequestPacket } from './bnet-news.mjs';
const enc = new TextEncoder(), dec = new TextDecoder();
const cstr = (s) => [...enc.encode(String(s)), 0];
export const bncsPacket = (id, body = []) => Uint8Array.from([0xff, id, (body.length + 4) & 0xff, (body.length + 4) >> 8, ...body]);
// BNClient's ENTERCHAT sender (+0xC520) copies each string with a 0x100-byte limit: at most 255 bytes, then the NUL.
const capped = (s) => [...enc.encode(String(s)).subarray(0, 255), 0];
export const enterChatPacket = (name, realm = '') => bncsPacket(0x0a, [...capped(name), ...capped(realm)]);
// The body is the product dword: 'VD2D' for classic, 'PX2D' for Lord of Destruction.
export const getChannelListPacket = (expansion = true) => bncsPacket(0x0b, expansion ? [0x50, 0x58, 0x32, 0x44] : [0x56, 0x44, 0x32, 0x44]);
// flags 5: the first join of a Diablo II client, which lands in the product's own channel.
export const joinChannelPacket = (channel, flags = 5) => bncsPacket(0x0c, [flags & 255, (flags >>> 8) & 255, (flags >>> 16) & 255, (flags >>> 24) & 255, ...cstr(channel)]);
// SID_GETCHANNELLIST's reply: channel names, each NUL-terminated, ended by an empty name.
export function parseChannelList(body) {
  const out = []; let at = 0;
  while (at < body.length) { const end = body.indexOf(0, at); if (end < 0 || end === at) break; out.push(dec.decode(body.subarray(at, end))); at = end + 1; }
  return out;
}
// CHATCOMMAND (+0xC400) sends the text as given, raw: a typed `/f l`, `/f a name` or `/f r name` is no different from
// any other line (retail's slash dispatcher keeps only /r, /reply and /d2notify). The text is a C string, so it ends at
// its first NUL. The 255-byte cap is the chat box's own limit, not a C400 check.
export const chatCommandPacket = (text) => bncsPacket(0x0e, cstr(String(text).split('\0')[0].slice(0, 255)));

const u32 = (b, o) => (b[o] | (b[o + 1] << 8) | (b[o + 2] << 16) | (b[o + 3] << 24)) >>> 0;
// BNClient+0xFEC0 queues at most 0x120 bytes of the event body (a longer one is cut there, not refused): fields past the cut are
// not there for the front end.
export const CHAT_EVENT_CAP = 0x120;
export function parseChatEvent(whole) {
  const body = whole.subarray(0, CHAT_EVENT_CAP);
  if (body.length < 26) return null;
  const nameEnd = body.indexOf(0, 24); if (nameEnd < 0) return null;
  const textEnd = body.indexOf(0, nameEnd + 1);
  return { event: u32(body, 0), flags: u32(body, 4), ping: u32(body, 8), username: dec.decode(body.subarray(24, nameEnd)),
    text: dec.decode(body.subarray(nameEnd + 1, textEnd < 0 ? body.length : textEnd)),
    raw: body.subarray(nameEnd + 1) };
}

// A chat event as a log line (D2Win colour: 0 white, 1 red, 2 green, 3 blue, 4 gold, 5 grey), or null.
export function chatLine({ event, username, text }) {
  switch (event) {
    case 0x05: return { text: `<${username}> ${text}`, colour: 0 };
    case 0x04: return { text: `${username} whispers: ${text}`, colour: 2 };
    case 0x0a: return { text: `You whisper to ${username}: ${text}`, colour: 2 };
    case 0x17: return { text: `*${username} ${text}*`, colour: 5 };
    case 0x06: case 0x12: case 0x15: case 0x16: return { text, colour: 3 };
    case 0x07: return { text: `You have joined channel: ${text}`, colour: 5 };
    case 0x02: return { text: `${username} has joined the channel.`, colour: 5 };
    case 0x03: return { text: `${username} has left the channel.`, colour: 5 };
    case 0x13: return { text, colour: 1 };
    case 0x0d: return { text: `The channel "${text}" is full. If you are having problems using Battle.net commands, please click the Help button.`, colour: 1 };
    case 0x0f: return { text: `The channel "${text}" is restricted. If you are having problems using Battle.net commands, please click the Help button.`, colour: 1 };
    default: return null;
  }
}

// ENTERCHAT's reply (BNClient+0x10CC0): the first string is the unique name the server gave the character; it is kept (0x100-byte
// copy, so at most 255 bytes) and read back through slot 0x18 (+0x15770).
export const parseEnterChat = (body) => { let e = body.indexOf(0); if (e < 0) e = body.length; return dec.decode(body.subarray(0, Math.min(e, 255))); };

// Owns the open BNCS transport and its reader. `onEvent` sees every parsed event first (true: handled);
// `onLine` gets each displayable line it left. `onNews` gets each SID_NEWS_INFO body, `onEntered` the ENTERCHAT unique name.
export function createBnetChat({ transport, reader, character, realm = '', expansion = true, channel = 'Diablo II', onLine = () => {}, onEvent = () => false, onGames = () => {}, onChannels = () => {}, onClose = () => {},
  onNews = () => {}, onEntered = () => {},
}) {
  let entered = false, wantJoin = false, joined = false, closed = false, current = '', wantFlags, uniqueName = '';
  const send = (bytes) => transport.send(bytes);
  const handle = ({ id, body }) => {
    if (id === 0x25) send(bncsPacket(0x25, [...body.subarray(0, 4)]));
    else if (id === GAME_LIST_ID) onGames(body);
    else if (id === 0x0b) onChannels(parseChannelList(body));
    else if (id === 0x46) onNews(body);
    else if (id === 0x0a) { entered = true; uniqueName = parseEnterChat(body); onEntered(uniqueName); if (wantJoin) { wantJoin = false; join(); } }
    else if (id === 0x0f) {
      const e = parseChatEvent(body); if (!e) return;
      if (e.event === 0x07) { joined = true; current = e.text; }
      if (onEvent(e)) return;
      const line = chatLine(e); if (line) onLine(line);
    }
  };
  const join = (name = channel, flags = wantFlags) => { send(joinChannelPacket(name, flags)); };
  // SID_NULL is not sent here: the connection's keep-alive (realm-client's startBncsNull) owns it, since retail's pump does.
  (async () => {
    try {
      for (;;) {
        const head = await reader.want(4, null);
        const total = head[2] | (head[3] << 8);
        if (head[0] !== 0xff || total < 4) throw new Error('not a BNCS frame');
        const frame = await reader.want(total, null); const body = frame.slice(4); const id = frame[1];
        reader.skip(total); handle({ id, body });
      }
    } catch { closed = true; onClose(); }
  })();
  return {
    get entered() { return entered; }, get joined() { return joined; }, get channel() { return current; }, get open() { return !closed && transport.isOpen; },
    get uniqueName() { return uniqueName; },
    // SID_NEWS_INFO with the newest timestamp held: the reply reaches `onNews`.
    news(newest = 0) { return closed ? false : send(newsRequestPacket(newest)) !== false; },
    // The CHANNEL panel's list: ask again, the reply reaches `onChannels`.
    channels() { return closed ? false : send(getChannelListPacket(expansion)) !== false; },
    enter() { send(getChannelListPacket(expansion)); send(enterChatPacket(character, realm)); },
    join(name, flags) { wantFlags = flags; if (name) channel = name; if (entered) join(); else wantJoin = true; },
    // SID_GETADVLISTEX: the reply reaches `onGames` as the raw body.
    games() { return closed ? false : send(getGamesPacket()) !== false; },
    // C400 returns false for an empty text (NULL or "") before EB00: nothing is sent. Blanks count as text.
    say(text) { if (!text) return false; return closed ? false : send(chatCommandPacket(text)) !== false; },
    // SID_LEAVECHAT (FF 10 04 00), what the lobby's QUIT sends; the socket stays open, chat must be entered again.
    leave() { if (closed) return false; entered = false; joined = false; wantJoin = false; return send(bncsPacket(0x10)) !== false; },
    close() { closed = true; try { reader.cancel(); } catch { /* done */ } try { transport.close(); } catch { /* done */ } },
  };
}
