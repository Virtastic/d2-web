// A single-player game joined through the realm client path inside node: the local game as the server end of
// the in-process connection, the realm client state as the receiving end, stepped one game tick at a time.
// One engine per process, so each test file that uses this runs its own game.
import { bindEngineProtocol, d2gsJoin } from '../../play/assets/realm-client.mjs';
import { localGameTransport } from '../../play/assets/realm-local.mjs';
import { haveArchives, haveEngine, loadEngine } from './node-engine.mjs';

export const localGameSkip = () => (haveEngine() && haveArchives() ? false : 'needs the staged engine and the game archives');
export const hex2 = (v) => v.toString(16).padStart(2, '0');
export const le = (...fields) => fields.flatMap(([value, size]) => Array.from({ length: size }, (_, i) => (value >>> (8 * i)) & 0xff));
export const packet = (opcode, ...fields) => Uint8Array.from([opcode, ...le(...fields)]);

/**
 * Create a local game (seed, class, area) and join it as the realm client does.
 * `packets` holds every server packet the client was fed, in order, as { tick, op, bytes }.
 */
export async function openLocalGame({ seed = 1, charClass = 1, level = 2, setup = null } = {}) {
  const Module = await loadEngine({ archives: true });
  const call = (name, types = [], values = [], ret = 'number') => Module.ccall(name, ret, types, values);
  call('d2_session_client_feed', ['number'], [1]);
  if (!call('d2_session_create_class_at', Array(7).fill('number'), [seed, 0, 0, level, charClass, 0, 0])) throw new Error('no local game');
  setup?.(call, Module);
  const protocol = bindEngineProtocol(Module);
  const transport = localGameTransport(Module);
  await transport.ready();
  const game = await d2gsJoin(transport, protocol, { character: 'PLAYER', charClass, expansion: true, gameToken: 0, gameHash: 0, difficulty: 0, timeoutMs: 3000 });
  const g = { Module, call, protocol, transport, game, packets: [], rejected: [], tick: 0 };
  // The page reads the stream without waiting, as the in-process game hands its client every packet of a frame first.
  const takeFrames = () => { const out = []; for (let f = game.readFrameNow(); f; f = game.readFrameNow()) out.push(f); return out; };
  const settle = async () => {};
  g.splitFrame = (frame, tick) => {
    const out = [];
    for (let at = 0; at < frame.length;) {
      const size = protocol.d2gsPacketSize(frame.subarray(at));
      if (size <= 0) { out.push({ tick, op: -1, bytes: frame.slice(at) }); break; }
      out.push({ tick, op: frame[at], bytes: frame.slice(at, at + size) });
      at += size;
    }
    return out;
  };
  /** One game tick as the page runs it: the server frame and its dispatch, then the client's frame. Returns this tick's packets. */
  g.step = async (n = 1) => {
    const mine = [];
    for (let i = 0; i < n; i++) {
      g.tick++;
      transport.tick(1);
      protocol.d2gsClientMotionTick();
      await settle();
      for (const frame of takeFrames()) {
        for (const p of g.splitFrame(frame, g.tick)) { g.packets.push(p); mine.push(p); }
        const applied = protocol.d2gsClientPlayerFeed(frame);
        if (applied < 0) g.rejected.push({ tick: g.tick, applied, head: Array.from(frame.subarray(0, 12)).map(hex2).join('') });
      }
    }
    return mine;
  };
  g.send = (bytes) => transport.send(bytes);
  g.server = () => ({ id: call('d2_session_player_id'), level: call('d2_session_player_level'), act: call('d2_session_player_act'), x: call('d2_session_player_x'), y: call('d2_session_player_y') });
  g.client = () => protocol.d2gsClientPlayerState?.();
  g.ops = (from = 0) => g.packets.slice(from).map((p) => hex2(p.op));
  g.refused = () => transport.refused().map((r) => `${hex2(r.opcode)}:${r.result}`);
  await g.step(8);
  return g;
}
