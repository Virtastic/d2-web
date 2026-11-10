import { test } from 'node:test';
import assert from 'node:assert/strict';
import { readMcpFrame, MCP_RECV_BUFFER } from './realm-client.mjs';

const reader = (bytes) => {
  let buf = Uint8Array.from(bytes);
  return {
    want: async (n) => { if (buf.length < n) throw new Error('short'); return buf.subarray(0, n); },
    skip: (n) => { buf = buf.subarray(n); },
    left: () => buf.length,
  };
};
const frame = (op, bodyLen) => { const t = 3 + bodyLen, b = new Uint8Array(t); b[0] = t & 255; b[1] = t >> 8; b[2] = op; return b; };

test('a frame whose opcode and body fill the 0x400 buffer is delivered', async () => {
  const f = frame(0x30, MCP_RECV_BUFFER - 1);
  const r = reader(f);
  const got = await readMcpFrame(r, 10);
  assert.equal(got.op, 0x30);
  assert.equal(got.body.length, MCP_RECV_BUFFER - 1);
});

test('a frame one byte over the buffer is dropped and the next frame is read', async () => {
  const big = frame(0x30, MCP_RECV_BUFFER);
  const next = frame(0x31, 2);
  const r = reader([...big, ...next]);
  const got = await readMcpFrame(r, 10);
  assert.equal(got.op, 0x31);
  assert.equal(r.left(), 0);
});
