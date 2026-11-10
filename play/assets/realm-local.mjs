// The in-process game connection: the same transport interface a game-server socket gives the
// realm client (send / onData / onClose / close / ready), with the local game as the server.
//
// The retail client is always a client of the game server; in single player the packets cross a
// local queue instead of a socket, uncompressed. This is that queue. Client packets go to the
// engine's local server end as the player's input; what the local game sends its client comes
// back in emission order, starting with the greeting and the join sequence. No game rule lives
// here: every packet is composed and consumed by the engine.
import { loopbackTransport } from './realm-client.mjs';

const PACKET_SCRATCH = 1024;

/**
 * A transport whose server is the engine's live local session.
 *
 * The owner calls `pump()` after each game tick it runs; `tick(frames)` runs the game and pumps.
 */
export function localGameTransport(Module) {
  const call = (name, ret, types, values) => Module.ccall(name, ret, types, values);
  const scratch = Module._malloc(PACKET_SCRATCH), countAt = Module._malloc(4);
  let open = false, freed = false;
  const take = () => {
    const at = Number(call('d2_local_d2gs_out', 'number', [], []));
    const length = call('d2_local_d2gs_take', 'number', [], []);
    return length > 0 ? Module.HEAPU8.slice(at, at + length) : null;
  };
  const stats = { sent: 0, taken: 0, refusedSends: 0, serverBytes: 0 };
  const pipe = loopbackTransport((bytes) => {
    if (!open || !bytes.length || bytes.length > PACKET_SCRATCH) return null;
    Module.HEAPU8.set(bytes, Number(scratch));
    stats.sent++;
    if (call('d2_local_d2gs_client', 'number', ['bigint', 'number'], [BigInt(scratch), bytes.length]) === 1) stats.taken++;
    else stats.refusedSends++;
    const reply = take();
    if (reply) stats.serverBytes += reply.length;
    return reply;
  });
  const say = (bytes) => { if (bytes) { stats.serverBytes += bytes.length; pipe.serverSay(bytes); } return bytes ? bytes.length : 0; };
  const release = () => { if (!freed) { freed = true; Module._free(scratch); Module._free(countAt); } };
  return {
    send: (bytes) => { if (!open) return false; pipe.send(bytes); return true; },
    onData: pipe.onData,
    onClose: pipe.onClose,
    get isOpen() { return open; },
    ready: async () => {
      if (open) return;
      if (!call('d2_local_d2gs_open', 'number', [], []))
        throw new Error('no local game to connect to');
      open = true;
      say(take());   // the greeting
    },
    /** Deliver what the game sent since the last call. Returns the byte count. */
    pump: () => {
      if (!open) return 0;
      call('d2_local_d2gs_pump', 'number', [], []);
      return say(take());
    },
    /** Advance the local game and deliver what it sent. */
    tick(frames = 1) { if (!open) return 0; call('d2_session_tick', 'number', ['number'], [frames]); return this.pump(); },
    /** Client packets the server had no entry for, or the game refused: [{ opcode, result }]. */
    refused: () => {
      if (freed) return [];
      const at = Number(call('d2_local_d2gs_refused', 'number', ['bigint'], [BigInt(countAt)]));
      const n = new Int32Array(Module.HEAPU8.buffer, Number(countAt), 1)[0];
      const rows = new Int32Array(Module.HEAPU8.buffer, at, n * 2);
      return Array.from({ length: n }, (_, i) => ({ opcode: rows[i * 2], result: rows[i * 2 + 1] }));
    },
    dropped: () => call('d2_local_d2gs_dropped', 'number', [], []),
    stats: () => ({ ...stats }),
    close: () => {
      if (!open) return;
      open = false;
      call('d2_local_d2gs_close', null, [], []);
      pipe.close();
      release();
    },
  };
}

/**
 * The realm flow of a local game: what the shell's join routine reads in place of a logged-in
 * realm session. The character is the live session's player; `connect()` opens the in-process
 * connection and `pump()` delivers what the game sent after a tick.
 */
export function openLocalRealm(Module, protocol, { name = 'Local' } = {}) {
  const flags = Module.ccall('d2_session_game_flags', 'number', [], []);
  const character = { name, charClass: (flags >> 16) & 0xff, flags: flags & 0x100 ? 0x20 : 0 };
  let transport = null;
  return { server: { region: 'local', host: 'local' }, realmReply: null, bncs: null, protocol, mcp: null, mcpSession: null,
    local: {
      joined: { character, game: { difficulty: flags & 0xff }, ticket: { token: 0, hash: 0 } },
      connect: () => (transport = localGameTransport(Module)),
      pump: () => transport?.pump() ?? 0,
      /** What crossed the connection and what the server did not take (the join's own evidence). */
      evidence: () => (transport ? { ...transport.stats(), refused: transport.refused(), dropped: transport.dropped() } : null),
    } };
}

/**
 * Leaving a local game: the in-process connection is released first (no lobby, no game server to wait on),
 * then the single-player exit runs, which writes the character and returns to the character list.
 */
export async function exitLocalGame({ release, saveAndExit }) {
  try { await release(); } finally { saveAndExit(); }
}
