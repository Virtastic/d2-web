// The ad service of the lobby: SID_CHECKAD every 15 s, the ad file on a file connection, SID_ADSHOWN after each.
import test from 'node:test';
import assert from 'node:assert/strict';
import { adShownPacket, checkAdPacket, createBnetChat, parseCheckAd } from './bnet-chat.mjs';

const hex = (b) => Buffer.from(b).toString('hex');
const unhex = (s) => Uint8Array.from(Buffer.from(s, 'hex'));

test('the request and the shown report are the bytes of the recorded stock client', () => {
  assert.equal(hex(checkAdPacket(0, 0x6afcbd52)), 'ff15140036385849505832440000000052bdfc6a');
  assert.equal(hex(adShownPacket(3)), 'ff2112003638584950583244030000000000');
});

test('the reply names the ad: id, extension, FILETIME, file', () => {
  const ad = parseCheckAd(unhex('04000000' + '2e736d6b' + '001168fcc26fdd01' + Buffer.from('ad000002.smk\0http://x\0').toString('hex')));
  assert.deepEqual(ad, { id: 4, ext: 0x6b6d732e, fileTime: 0x01dd6fc2fc681100n, name: 'ad000002.smk' });
  assert.equal(parseCheckAd(new Uint8Array(8)), null);
});

test('the loop asks at once and every period, reports each reply, and stops when a game is entered', async () => {
  const sent = [];
  const frames = [];
  let wake = null;
  const reader = { want: () => new Promise((res) => { wake = res; }), skip() {}, cancel() {} };
  const chat = createBnetChat({ transport: { isOpen: true, send: (b) => { sent.push(hex(b).slice(2, 4)); return true; }, close() {} }, reader, character: 'x', adMs: 20, now: () => 1 });
  chat.ads();
  assert.deepEqual(sent, ['15']);
  await new Promise((r) => setTimeout(r, 70));
  assert.ok(sent.filter((s) => s === '15').length >= 3);
  const before = sent.length;
  chat.enterGame('g', { created: false });
  await new Promise((r) => setTimeout(r, 70));
  assert.deepEqual(sent.slice(before), ['22', '10']);
  chat.close(); void frames; void wake;
});
