import test from 'node:test';
import assert from 'node:assert/strict';
import crypto from 'node:crypto';
import * as S from './bnet-srp.mjs';

const hex = (b) => Buffer.from(b).toString('hex');
const fromHex = (h) => Uint8Array.from(Buffer.from(h, 'hex'));

// Fixed inputs shared with the vectors below: user "user", pass "pass", a = 0x0123..cdef (32 bytes),
// salt[i] = 5i+1, B[i] = 7i+3 with B[31] = 0x12 (so B < N).
const A_EXP_USER = 'user', A_EXP_PASS = 'pass';
const a = BigInt('0x0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef');
const salt = Uint8Array.from({ length: 32 }, (_, i) => 5 * i + 1);
const B = Uint8Array.from({ length: 32 }, (_, i) => 7 * i + 3);
B[31] = 0x12;

test('sha1 matches the platform SHA-1 over many lengths', () => {
  for (let n = 0; n < 300; n++) {
    const data = Uint8Array.from({ length: n }, (_, i) => (i * 31 + n) & 0xff);
    assert.equal(hex(S.sha1(data)), crypto.createHash('sha1').update(data).digest('hex'), `length ${n}`);
  }
});

test('the fixed NLS constant I is SHA1(N) xor SHA1(g) over the little-endian bytes', () => {
  // bncsutil nls.c bncsutil_NLS_I, as its raw byte array.
  assert.equal(hex(S.SRP_I), '6c0e97ed0af96babb15889eb8bba25a4f08c01f8');
});

test('the modulus and generator are the NLS v1 values', () => {
  assert.equal(S.SRP_N.toString(16).toUpperCase(), 'F8FF1A8B619918032186B68CA092B5557E976C78C73212D91216F6658523C787');
  assert.equal(S.SRP_G, 0x2fn);
});

test('bigToLe and leToBig are inverse for 32-byte values', () => {
  const v = S.SRP_N - 12345n;
  assert.equal(S.leToBig(S.bigToLe(v, 32)), v);
  assert.equal(S.bigToLe(0x0102n, 4).join(','), '2,1,0,0');
});

test('x, u, S, K, M1 and M2 match the bncsutil NLS oracle for fixed inputs', () => {
  const r = S.srpClient({ user: A_EXP_USER, pass: A_EXP_PASS, a, salt, B });
  assert.equal(hex(r.A), '970357954d7714a6cdd7130bc4e95b55e162191d0758f2007e1754a9490ab11f');
  assert.equal(S.srpX(salt, A_EXP_USER, A_EXP_PASS), BigInt('0x5F834727F83871FF77417E750ADEDCA8EA42048A'));
  assert.equal(S.srpU(B), 0x15f12317);
  assert.equal(hex(S.srpS(a, B, S.srpX(salt, A_EXP_USER, A_EXP_PASS), S.srpU(B))),
    '3c0c47febd8cd4e91549cb8ca3be5c408148f7f5a29bfe152c0b6b1dfdcbdba5');
  assert.equal(hex(r.K), 'a3cf68bc17e5b863b26978080f29240117559a1415d665982ea559b5a0f41a1b302f94a911db5b00');
  assert.equal(hex(r.M1), 'e165134ac70c7d4ba602470d60f528189724102f');
  assert.equal(hex(r.M2), '48e046732cfc1fa81b741a0f6cac7fe8dcb5d7bd');
});

test('names and passwords are uppercased before they are hashed', () => {
  const lower = S.srpClient({ user: 'user', pass: 'pass', a, salt, B });
  const upper = S.srpClient({ user: 'USER', pass: 'PASS', a, salt, B });
  assert.equal(hex(lower.M1), hex(upper.M1));
  assert.equal(S.asciiUpper('abéz'), 'ABéZ');
});

test('the account logon packet is FF 53 len body = A then the NUL-terminated name', () => {
  const A = Uint8Array.from({ length: 32 }, (_, i) => i);
  const p = S.accountLogonPacket(A, 'Jon');
  assert.equal(hex(p.subarray(0, 4)), 'ff532800');
  assert.equal(p.length, 40);
  assert.equal(hex(p.subarray(4, 36)), hex(A));
  assert.equal(hex(p.subarray(36)), '4a6f6e00');
});

test('the 0x53 reply parses to status, salt and B, and rejects another length', () => {
  const body = Uint8Array.from([0, 0, 0, 0, ...salt, ...B]);
  const p = Uint8Array.from([0xff, 0x53, 72, 0, ...body]);
  const r = S.parseAccountLogonReply(p);
  assert.equal(r.status, 0);
  assert.equal(hex(r.salt), hex(salt));
  assert.equal(hex(r.B), hex(B));
  assert.throws(() => S.parseAccountLogonReply(p.subarray(0, 40)));
  assert.throws(() => S.parseAccountLogonReply(Uint8Array.from([0xff, 0x54, 72, 0, ...body])));
});

test('the logon proof packet is FF 54 18 00 then M1', () => {
  const M1 = fromHex('e165134ac70c7d4ba602470d60f528189724102f');
  const p = S.logonProofPacket(M1);
  assert.equal(hex(p.subarray(0, 4)), 'ff541800');
  assert.equal(hex(p.subarray(4)), hex(M1));
});

test('the 0x54 reply parses to status and the 20-byte M2, and rejects another length', () => {
  const M2 = fromHex('48e046732cfc1fa81b741a0f6cac7fe8dcb5d7bd');
  const p = Uint8Array.from([0xff, 0x54, 28, 0, 0, 0, 0, 0, ...M2]);
  const r = S.parseLogonProofReply(p);
  assert.equal(r.status, 0);
  assert.equal(hex(r.M2), hex(M2));
  const bad = Uint8Array.from([0xff, 0x54, 28, 0, 2, 0, 0, 0, ...M2]);
  assert.equal(S.parseLogonProofReply(bad).status, 2);
  assert.throws(() => S.parseLogonProofReply(p.subarray(0, 24)));
});
