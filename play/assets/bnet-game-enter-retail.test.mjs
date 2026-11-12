// The BNCS frames a stock client sends when it creates a realm game: 0x1C, 0x22, 0x10 after the MCP join request.
// Replays the recorded runs (bnet-proof/live1, live2; private vectors, the test skips without them): the browser's create
// path drives a fake MCP and BNCS transport, and every BNCS frame it sends after the MCP 0x03/0x04 is compared with the recording.
import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import { mcpCreateAndJoin } from './realm-client.mjs';
import { createBnetChat } from './bnet-chat.mjs';
import { createChatLink } from './bnet-chat-link.mjs';
import { createMcpGameList } from './realm-gamelist.mjs';

const hex = (b) => Buffer.from(b).toString('hex');
const dir = new URL('../../vectors/bnet-proof/', import.meta.url).pathname;
const runs = ['live1', 'live2'].filter((r) => fs.existsSync(`${dir}${r}/rec.jsonl`));

// Retail's C>S BNCS frames after its MCP 0x04 (join) request, up to the 100 ms window that held them.
function recorded(run) {
  const rows = fs.readFileSync(`${dir}${run}/rec.jsonl`, 'utf8').trim().split('\n').map((l) => JSON.parse(l)).filter((r) => r.ev === 'chunk' && r.dir === 'c2s');
  const mcp = rows.find((r) => r.port === '6113' && r.hex.slice(4, 6) === '04' && r.hex.length > 10 && r.hex.slice(2, 4) === '00');
  const out = [];
  for (const r of rows) if (r.port === '6112' && r.t >= mcp.t && r.t < mcp.t + 0.1 && /^ff(1c|22|10)/.test(r.hex)) out.push(r.hex);
  return out;
}

const frame = (op, body) => Uint8Array.from([(body.length + 3) & 255, (body.length + 3) >> 8, op, ...body]);

for (const run of runs) {
  test(`${run}: create sends BNCS 0x1C, 0x22, 0x10 as the stock client does (0 differing bytes)`, async () => {
    const want = recorded(run);
    assert.deepEqual(want.map((h) => h.slice(2, 4)), ['1c', '22', '10']);
    const bncsOut = [], order = [];
    const bncs = { isOpen: true, onData: () => {}, onClose: () => {}, send: (b) => { bncsOut.push(hex(b)); order.push('bncs'); return true; }, close() {} };
    const reader = { want: () => new Promise(() => {}), skip() {}, cancel() {} };
    const chat = createBnetChat({ transport: bncs, reader, character: 'Sbnpc' });
    const link = createChatLink({ dial: async () => { throw new Error('no'); }, createChat: () => chat });
    link.attach({}, {});
    bncsOut.length = 0; order.length = 0;
    let onData = () => {};
    const transport = { onData: (f) => { onData = f; }, onClose: () => {},
      send: (bytes) => {
        const op = bytes[2], id = bytes[3] | (bytes[4] << 8); order.push(`mcp${op}`);
        if (op === 0x03) onData(frame(0x03, [id & 255, id >> 8, 6, 0, 0, 0, 0, 0, 0, 0]));
        if (op === 0x04) onData(frame(0x04, [id & 255, id >> 8, 7, 0, 10, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0]));
      } };
    const protocol = { mcpFrame: (op, body) => Uint8Array.from([0, 0, op, (body[0] ?? 0) & 255, (body[1] ?? 0) & 255]),
      mcpCreateGame: ({ requestId }) => [requestId & 255, requestId >> 8], mcpJoinGame: ({ requestId }) => [requestId & 255, requestId >> 8] };
    const session = { transport, protocol, joined: false, list: { characters: [{ name: 'Sbnpc' }] }, characterLoggedOn: 'Sbnpc', received: [], nextRequestId: 2 };
    await mcpCreateAndJoin(transport, protocol, { realmReply: [1], character: 'Sbnpc', gameName: 'stockgame', session, keepAliveMs: 0,
      onJoinSent: (name, opts) => link.enterGame(name, opts) });
    assert.deepEqual(order, ['mcp3', 'mcp4', 'bncs', 'bncs', 'bncs']);
    let diffs = 0;
    for (let i = 0; i < Math.max(want.length, bncsOut.length); i++) {
      const a = Buffer.from(want[i] ?? '', 'hex'), b = Buffer.from(bncsOut[i] ?? '', 'hex');
      for (let k = 0; k < Math.max(a.length, b.length); k++) if (a[k] !== b[k]) diffs++;
    }
    assert.equal(diffs, 0, `${bncsOut.join(' ')} vs ${want.join(' ')}`);
  });
}

test('the pinned frames: 0x1C 36 bytes, 0x22 23 bytes, 0x10 4 bytes', async () => {
  const { startAdvPacket, notifyJoinPacket, bncsPacket } = await import('./bnet-chat.mjs');
  assert.equal(hex(startAdvPacket('Stockgame')), 'ff1c2400000000000000000000000000000000000000000053746f636b67616d65000000');
  assert.equal(hex(notifyJoinPacket('Stockgame')), 'ff221700505832440d00000053746f636b67616d650000');
  assert.equal(hex(bncsPacket(0x10)), 'ff100400');
});

test('a join of another game announces and leaves, and sends no 0x1C', () => {
  const out = [];
  const chat = createBnetChat({ transport: { isOpen: true, send: (b) => { out.push(hex(b).slice(2, 4)); return true; }, close() {} }, reader: { want: () => new Promise(() => {}), skip() {}, cancel() {} }, character: 'x' });
  chat.enterGame('Other', { created: false });
  assert.deepEqual(out, ['22', '10']);
});

test('the Join panel polls the realm with MCP 0x05 every 15 s, never BNCS 0x09', async () => {
  const { GAME_REFRESH_MS } = await import('./bnet-gamelist.mjs');
  assert.equal(GAME_REFRESH_MS, 15000);
  const sent = []; let id = 1;
  const list = createMcpGameList({ send: (op, body) => sent.push({ op, body }), nextId: () => ++id });
  list.request();
  assert.equal(sent.length, 1);
  assert.equal(sent[0].op, 0x05);
});
