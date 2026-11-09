// char-guard -- a character is in one game at a time, and the bridge says so the way the client expects.
//
// The realm refuses a second game for a character that is still in one, but it does so late: the game
// server's character load fails after the client already holds a game ticket, the connection drops,
// and the client shows its generic "disconnected" message. The 1.13c client has a first-class answer
// for a refused CREATEGAME/JOINGAME (a failure result in the reply, then back to the lobby with the
// realm connection kept), so the bridge gives it exactly that when the character it logged on is held
// by a live game connection of another session. Everything else is forwarded byte for byte.
//
// What the bridge reads (never changes): the CHARLOGON request and answer on /mcp (to learn which
// character a session logged on) and the GAMELOGON packet on /d2gs (to learn which character a game
// connection holds). Anything it cannot parse switches the guard off for that connection (fail open).

const MCP_CREATEGAME = 0x03, MCP_JOINGAME = 0x04, MCP_CHARLOGON = 0x07;
export const REFUSED = 0x01;            // the realm's generic "failed" result; the client maps it to its failure text
const GAMELOGON = 0x68, GAMELOGON_LEN = 37, GAMELOGON_NAME_AT = 21, NAME_LEN = 16;

const key = (name) => String(name).toLowerCase();

/** Split an accumulating MCP stream (`[len:u16 LE incl. itself][op][body]`) into whole frames. */
export function makeMcpFramer({ selector = false } = {}) {
  let buf = Buffer.alloc(0), skip = selector, bad = false;
  return {
    get bad() { return bad; },
    get atBoundary() { return !skip && buf.length === 0; },
    push(chunk) {
      buf = Buffer.concat([buf, chunk]);
      if (skip && buf.length) { buf = buf.subarray(1); skip = false; }
      const frames = [];
      while (!bad && buf.length >= 2) {
        const len = buf.readUInt16LE(0);
        if (len < 3) { bad = true; break; }
        if (buf.length < len) break;
        frames.push(Buffer.from(buf.subarray(0, len)));
        buf = buf.subarray(len);
      }
      return frames;
    },
  };
}

const frame = (op, body) => { const f = Buffer.alloc(3 + body.length); f.writeUInt16LE(f.length, 0); f[2] = op; body.copy(f, 3); return f; };

/** The failure reply a refused request gets: same op, the request id echoed, a failure result. */
export function refusalReply(req) {
  const id = req.readUInt16LE(3);
  if (req[2] === MCP_CREATEGAME) {      // request_id(u16) token(u16) unknown(u16) result(u32)
    const b = Buffer.alloc(10); b.writeUInt16LE(id, 0); b.writeUInt32LE(REFUSED, 6);
    return frame(MCP_CREATEGAME, b);
  }
  const b = Buffer.alloc(18);           // request_id token unknown gsIp(u32=0) hash(u32) result(u32)
  b.writeUInt16LE(id, 0); b.writeUInt32LE(REFUSED, 14);
  return frame(MCP_JOINGAME, b);
}

export function createCharGuard({ graceMs = 1500, onLog = () => {} } = {}) {
  const held = new Map();               // lowercase name -> { n: live game connections holding it }

  const hold = (name) => {
    const k = key(name);
    const rec = held.get(k) || { n: 0 };
    rec.n++; held.set(k, rec);
    let done = false;
    return () => {                      // the game connection closed; the realm's own unlock trails it by about a second
      if (done) return; done = true;
      const t = setTimeout(() => { if (--rec.n <= 0) held.delete(k); }, graceMs);
      t.unref?.();
    };
  };
  const isHeld = (name) => (held.get(key(name))?.n || 0) > 0;

  /** Watch one /d2gs connection's client bytes; the first GAMELOGON names the character it holds. */
  function d2gsTap() {
    let buf = Buffer.alloc(0), release = null, off = false;
    return {
      fromClient(chunk) {
        if (off || release) return;
        buf = Buffer.concat([buf, chunk]);
        if (buf[0] !== GAMELOGON) { off = true; return; }
        if (buf.length < GAMELOGON_LEN) return;
        const raw = buf.subarray(GAMELOGON_NAME_AT, GAMELOGON_NAME_AT + NAME_LEN);
        const z = raw.indexOf(0), name = raw.subarray(0, z < 0 ? NAME_LEN : z).toString('latin1');
        buf = Buffer.alloc(0);
        if (name) release = hold(name);
      },
      close() { if (release) { release(); release = null; } },
    };
  }

  /** Watch one /mcp connection. `fromClient` returns the bytes to forward and an optional reply for the client. */
  function mcpTap() {
    const toRealm = makeMcpFramer({ selector: true }), fromRealm = makeMcpFramer();
    let pending = null, character = null, off = false, first = true;
    return {
      get character() { return character; },
      fromClient(data) {
        if (off) return { forward: data, reply: null };
        const head = first ? data.subarray(0, 1) : Buffer.alloc(0);
        first = false;
        const frames = toRealm.push(data);
        if (toRealm.bad) { off = true; return { forward: data, reply: null }; }
        let reply = null, refused = false;
        const keep = [];
        for (const f of frames) {
          const op = f[2];
          if (op === MCP_CHARLOGON) {
            const z = f.indexOf(0, 3);
            pending = z > 3 ? f.subarray(3, z).toString('latin1') : null;
          } else if ((op === MCP_CREATEGAME || op === MCP_JOINGAME) && f.length >= 5
                     && character && fromRealm.atBoundary && isHeld(character)) {
            onLog(`char guard: refused op ${op}, character already in a game`);
            reply = refusalReply(f); refused = true; continue;
          }
          keep.push(f);
        }
        // Only a chunk that held a refused request is rewritten; every other chunk is forwarded as received.
        // A chunk that ends inside a frame is never rewritten (the tail would be lost): the realm handles it.
        if (refused && !toRealm.atBoundary) return { forward: data, reply: null };
        return { forward: refused ? Buffer.concat([head, ...keep]) : data, reply };
      },
      fromRealm(chunk) {
        if (off) return;
        for (const f of fromRealm.push(chunk)) {
          if (f[2] === MCP_CHARLOGON && f.length >= 7 && f.readUInt32LE(3) === 0) character = pending;
        }
        if (fromRealm.bad) off = true;
      },
    };
  }

  return { d2gsTap, mcpTap, isHeld };
}
