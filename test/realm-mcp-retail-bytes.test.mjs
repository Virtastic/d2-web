// The lobby requests our client builds in JavaScript, compared byte for byte with what the original client sends.
// The expected bytes come from recordings kept in the repository: captures of the original request builders (the `expect`
// lines of their capture files) and the frames the original client wrote to a loopback realm. No hand-written expectations.
import test from 'node:test';
import assert from 'node:assert/strict';
import { existsSync, readFileSync } from 'node:fs';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';
import { mcpCharDeleteBody, mcpCreateCharacter, bindEngineProtocol } from '../play/assets/realm-client.mjs';
import { ladderRequestBody } from '../play/assets/realm-ladder.mjs';
import { haveEngine, loadEngine } from './helpers/node-engine.mjs';

const ROOT = join(dirname(fileURLToPath(import.meta.url)), '..');
const hex = (b) => [...b].map((x) => x.toString(16).padStart(2, '0')).join(' ');

// Descriptor `expect` lines: `01 00 00 00 <len:u32> <len bytes of {op, body}> <pad>`; returns the body after the op byte.
function retailBodies(rva) {
  const file = join(ROOT, 'tools', 'capture', 'fn', `d2mcpclient-${rva}.txt`);
  if (!existsSync(file)) return null;
  return readFileSync(file, 'utf8').split('\n').filter((l) => l.startsWith('expect ')).map((l) => {
    const b = l.split(/\s+/).slice(2).map((h) => parseInt(h, 16));
    const len = b[4] | (b[5] << 8);
    return { op: b[8], body: Uint8Array.from(b.slice(9, 8 + len)) };
  });
}
const have = (rva) => !!retailBodies(rva)?.length;

test('LADDERDATA request body equals retail composer 0x6D50', { skip: !have('6d50') && 'needs descriptor' }, () => {
  const [r] = retailBodies('6d50');
  assert.equal(r.op, 0x11);
  assert.equal(hex(ladderRequestBody(r.body[0], r.body[1] | (r.body[2] << 8))), hex(r.body));
});

test('CHARDELETE request body equals retail composer 0x6E60', { skip: !have('6e60') && 'needs descriptor' }, () => {
  const [r] = retailBodies('6e60');
  assert.equal(r.op, 0x0a);
  const cookie = r.body[0] | (r.body[1] << 8);
  assert.equal(hex(mcpCharDeleteBody('abc', cookie)), hex(r.body));
});

test('CHARCREATE request body equals retail composer 0x7480 (class 0, no flags, empty name)', { skip: !have('7480') && 'needs descriptor' }, async () => {
  const r = retailBodies('7480').find((x) => x.body.length === 7 && x.body.every((v) => v === 0));
  assert.ok(r, 'the all-zero retail case');
  const sent = [];
  const protocol = { mcpFrame: (op, body) => [(body.length + 3) & 0xff, 0, op, ...body] };
  const reader = { want: async (n) => Uint8Array.from([7, 0, 2, 0, 0, 0, 0]).subarray(0, n), skip() {} };
  await mcpCreateCharacter({ transport: { send: (f) => sent.push(Uint8Array.from(f)) }, protocol, reader, list: { total: 0, returned: 0, characters: [] } },
    { name: '', charClass: 0, expansion: false });
  assert.equal(sent[0][2], r.op);
  assert.equal(hex(sent[0].subarray(3)), hex(r.body));
});

const LEAVE = join(ROOT, 'vectors', 'd2multi', 'mcp-leave-ok.txt');
test('engine selector and CHARLOGON frames equal the bytes retail wrote to the loopback realm', { skip: (!haveEngine() || !existsSync(LEAVE)) && 'needs engine and vector' }, async () => {
  const sent = readFileSync(LEAVE, 'utf8').split('\n').filter((l) => /^MCP conn=1 C len=/.test(l))
    .map((l) => l.replace(/^MCP conn=1 C len=\d+ /, '').trim());
  const protocol = bindEngineProtocol(await loadEngine({}));
  assert.equal(hex(protocol.mcpSelector()), sent[0]);
  assert.equal(hex(protocol.mcpFrame(0x07, protocol.mcpCharlogon('Aelwyn'))), sent[2]);
});
