// The Battle.net chat leg on the BNCS socket the login leaves open (retail keeps it for the whole
// session): ENTERCHAT (0x0A) names the character, JOINCHANNEL (0x0C) enters the channel, CHATCOMMAND
// (0x0E) carries a typed line, and CHATEVENT (0x0F) brings every line, join and leave back.
// Frames are `FF id len16LE body`. Pure of the engine, so the bytes are testable in node.
const enc = new TextEncoder(), dec = new TextDecoder();
const cstr = (s) => [...enc.encode(String(s)), 0];
export const bncsPacket = (id, body = []) => Uint8Array.from([0xff, id, (body.length + 4) & 0xff, (body.length + 4) >> 8, ...body]);
export const enterChatPacket = (name, realm = '') => bncsPacket(0x0a, [...cstr(name), ...cstr(realm)]);
export const getChannelListPacket = () => bncsPacket(0x0b, [0, 0, 0, 0]);
// flags 5: the first join of a Diablo II client, which lands in the product's own channel.
export const joinChannelPacket = (channel, flags = 5) => bncsPacket(0x0c, [flags & 255, 0, 0, 0, ...cstr(channel)]);
export const chatCommandPacket = (text) => bncsPacket(0x0e, cstr(String(text).replace(/[\0\r\n]/g, ' ').slice(0, 255)));

const u32 = (b, o) => (b[o] | (b[o + 1] << 8) | (b[o + 2] << 16) | (b[o + 3] << 24)) >>> 0;
export function parseChatEvent(body) {
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
    case 0x04: return { text: `<${username}> ${text}`, colour: 2 };
    case 0x0a: return { text: `<to ${username}> ${text}`, colour: 2 };
    case 0x17: return { text: `<${username} ${text}>`, colour: 0 };
    case 0x06: return { text: `Broadcast: ${text}`, colour: 4 };
    case 0x07: return { text: `Joined channel ${text}`, colour: 4 };
    case 0x01: case 0x02: return event === 0x02 ? { text: `${username} has joined the channel`, colour: 5 } : null;
    case 0x03: return { text: `${username} has left the channel`, colour: 5 };
    case 0x12: return { text: text, colour: 4 };
    case 0x13: case 0x0d: case 0x0e: case 0x0f: return { text: text, colour: 1 };
    default: return null;
  }
}

// Owns the open BNCS transport and its reader. `onEvent` sees every parsed event first (true: handled);
// `onLine` gets each displayable line it left.
export function createBnetChat({ transport, reader, character, realm = '', channel = 'Diablo II', onLine = () => {}, onEvent = () => false, onClose = () => {} }) {
  let entered = false, wantJoin = false, joined = false, closed = false;
  const send = (bytes) => transport.send(bytes);
  const handle = ({ id, body }) => {
    if (id === 0x25) send(bncsPacket(0x25, [...body.subarray(0, 4)]));
    else if (id === 0x0a) { entered = true; if (wantJoin) { wantJoin = false; join(); } }
    else if (id === 0x0f) {
      const e = parseChatEvent(body); if (!e) return;
      if (e.event === 0x07) joined = true;
      if (onEvent(e)) return;
      const line = chatLine(e); if (line) onLine(line);
    }
  };
  const join = () => { send(getChannelListPacket()); send(joinChannelPacket(channel)); };
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
    get entered() { return entered; }, get joined() { return joined; }, get open() { return !closed && transport.isOpen; },
    enter() { send(enterChatPacket(character, realm)); },
    join() { if (entered) join(); else wantJoin = true; },
    say(text) { return send(chatCommandPacket(text)); },
    close() { closed = true; try { reader.cancel(); } catch { /* done */ } try { transport.close(); } catch { /* done */ } },
  };
}
