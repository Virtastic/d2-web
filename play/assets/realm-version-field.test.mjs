import test from 'node:test';
import assert from 'node:assert/strict';
import { bindEngineProtocol, mcpCreateAndJoin } from './realm-client.mjs';

// The REAL bindEngineProtocol object over a fake engine module whose exports write fixed bytes.
function engine() {
  const heap = new Uint8Array(1 << 20); let top = 16;
  const Module = {
    HEAPU8: heap, _malloc: (n) => { const p = top; top += n + 16; return p; },
    ccall: (name, _rt, _types, args) => {
      const out = Number(args[args.length - 1]);
      if (name === 'd2_realm_client_bncs_logon') { for (let i = 0; i < 40; i++) heap[out + i] = i + 1; return 40; }
      if (name === 'd2_realm_d2gs_gamelogon') { heap.fill(0, out, out + 37); heap[out] = 0x68; heap.set([0x50, 0xCC, 0x5D, 0xED], out + 12); return 37; }
      throw new Error(`unexpected export ${name}`);
    },
  };
  return bindEngineProtocol(Module);
}

test('bncsLogon returns the engine body untouched for classic and expansion alike (no version field in 0x3A)', () => {
  const p = engine();
  const want = Array.from({ length: 40 }, (_, i) => i + 1);
  assert.deepEqual([...p.bncsLogon({ clientToken: 1, serverToken: 2, account: 'a', password: 'b' })], want);
  assert.deepEqual([...p.bncsLogon({ clientToken: 1, serverToken: 2, account: 'a', password: 'b', expansion: false })], want);
});

test('d2gsGamelogon writes 0x2185EDD6 for a classic game and the LoD sum 0xED5DCC50 for an expansion game', () => {
  const p = engine();
  const a = { character: 'Hero', charClass: 1, gameToken: 1, gameHash: 2 };
  assert.deepEqual([...p.d2gsGamelogon({ ...a, expansion: false }).slice(12, 16)], [0xD6, 0xED, 0x85, 0x21]);
  assert.deepEqual([...p.d2gsGamelogon({ ...a, expansion: true }).slice(12, 16)], [0x50, 0xCC, 0x5D, 0xED]);
  assert.deepEqual([...p.d2gsGamelogon(a).slice(12, 16)], [0x50, 0xCC, 0x5D, 0xED]);
});

test('a CREATEGAME the realm never answers fails with the create stage instead of hanging', async () => {
  const transport = { onData: () => {}, onClose: () => {}, send: () => {} };
  const protocol = { mcpFrame: (op) => Uint8Array.from([0, 0, op]), mcpCreateGame: () => [0] };
  const session = { transport, protocol, joined: false, list: { characters: [{ name: 'Hero' }] }, characterLoggedOn: 'Hero', received: [], nextRequestId: 2 };
  const error = await mcpCreateAndJoin(transport, protocol, { realmReply: [1], character: 'Hero', gameName: 'x', session, gameReplyMs: 20 }).catch((e) => e);
  assert.equal(error.mcpStage, 'create');
  assert.equal(error.timedOut, true);
});
