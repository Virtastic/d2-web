// bnet-srp.mjs -- the Battle.net SRP account logon: the NLS v1 client math and the two packets it rides on.
//
// SID_AUTH_ACCOUNTLOGON (0x53) sends the client public value A and the account name; the server answers with a
// status, the salt and its public value B. SID_AUTH_ACCOUNTLOGONPROOF (0x54) sends the client proof M1; the server
// answers with a status and its proof M2. Every integer on the wire is little-endian. Names and passwords are
// uppercased (ASCII only) before they feed the hash. The module is pure: no DOM, no Node-only API, so it runs
// in the browser and in the tests alike. SHA-1 is implemented here because the page has no synchronous one.

export const SRP_N_HEX = 'F8FF1A8B619918032186B68CA092B5557E976C78C73212D91216F6658523C787';
export const SRP_N = BigInt('0x' + SRP_N_HEX);
export const SRP_G = 47n;

const SHA1_INIT = [0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0];

// Standard SHA-1 (FIPS 180-4) over a byte array; returns the 20-byte digest.
export function sha1(data) {
  const bits = BigInt(data.length) * 8n;
  const padLen = ((data.length + 9 + 63) >> 6) << 6;
  const buf = new Uint8Array(padLen);
  buf.set(data);
  buf[data.length] = 0x80;
  const dv = new DataView(buf.buffer);
  dv.setUint32(padLen - 8, Number(bits >> 32n));
  dv.setUint32(padLen - 4, Number(bits & 0xffffffffn));
  const h = SHA1_INIT.slice();
  const w = new Uint32Array(80);
  for (let off = 0; off < padLen; off += 64) {
    for (let i = 0; i < 16; i++) w[i] = dv.getUint32(off + i * 4);
    for (let i = 16; i < 80; i++) {
      const t = w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16];
      w[i] = (t << 1) | (t >>> 31);
    }
    let [a, b, c, d, e] = h;
    for (let i = 0; i < 80; i++) {
      let f, k;
      if (i < 20) { f = (b & c) | (~b & d); k = 0x5a827999; }
      else if (i < 40) { f = b ^ c ^ d; k = 0x6ed9eba1; }
      else if (i < 60) { f = (b & c) | (b & d) | (c & d); k = 0x8f1bbcdc; }
      else { f = b ^ c ^ d; k = 0xca62c1d6; }
      const t = (((a << 5) | (a >>> 27)) + f + e + k + w[i]) >>> 0;
      e = d; d = c; c = (b << 30) | (b >>> 2); b = a; a = t;
    }
    h[0] = (h[0] + a) >>> 0; h[1] = (h[1] + b) >>> 0; h[2] = (h[2] + c) >>> 0;
    h[3] = (h[3] + d) >>> 0; h[4] = (h[4] + e) >>> 0;
  }
  const out = new Uint8Array(20);
  const odv = new DataView(out.buffer);
  for (let i = 0; i < 5; i++) odv.setUint32(i * 4, h[i]);
  return out;
}

export function concatBytes(...parts) {
  const out = new Uint8Array(parts.reduce((n, p) => n + p.length, 0));
  let o = 0;
  for (const p of parts) { out.set(p, o); o += p.length; }
  return out;
}

const utf8 = (s) => new TextEncoder().encode(s);

// ASCII-only uppercase, as the client's per-byte toupper does (the C locale leaves bytes >= 0x80 alone).
export function asciiUpper(s) {
  return s.replace(/[a-z]/g, (c) => String.fromCharCode(c.charCodeAt(0) - 32));
}

export function leToBig(bytes) {
  let n = 0n;
  for (let i = bytes.length - 1; i >= 0; i--) n = (n << 8n) | BigInt(bytes[i]);
  return n;
}

export function bigToLe(n, len) {
  const out = new Uint8Array(len);
  let v = n;
  for (let i = 0; i < len; i++) { out[i] = Number(v & 0xffn); v >>= 8n; }
  return out;
}

export function modPow(base, exp, mod) {
  let r = 1n;
  let b = ((base % mod) + mod) % mod;
  let e = exp;
  while (e > 0n) {
    if (e & 1n) r = (r * b) % mod;
    b = (b * b) % mod;
    e >>= 1n;
  }
  return r;
}

const xorBytes = (a, b) => a.map((v, i) => v ^ b[i]);

// The fixed NLS constant I = SHA1(N) xor SHA1(g): N over its 32 little-endian bytes, g over the single byte 0x2F.
export const SRP_I = xorBytes(sha1(bigToLe(SRP_N, 32)), sha1(Uint8Array.of(Number(SRP_G))));

// The client's secret x = SHA1(salt || SHA1(USER:PASS)), read as a little-endian integer.
export function srpX(salt, user, pass) {
  const inner = sha1(utf8(`${asciiUpper(user)}:${asciiUpper(pass)}`));
  return leToBig(sha1(concatBytes(salt, inner)));
}

// The client public value A = g^a mod N, as 32 little-endian bytes.
export function srpPublic(a) {
  return bigToLe(modPow(SRP_G, a, SRP_N), 32);
}

// u is the big-endian reading of the first four bytes of SHA1(B), as the client computes it.
export function srpU(B) {
  const h = sha1(B);
  return ((h[0] << 24) | (h[1] << 16) | (h[2] << 8) | h[3]) >>> 0;
}

// The shared secret S = (B - g^x)^(a + u*x) mod N, as 32 little-endian bytes.
export function srpS(a, B, x, u) {
  const v = modPow(SRP_G, x, SRP_N);
  const base = (SRP_N + leToBig(B) - v) % SRP_N;
  return bigToLe(modPow(base, a + BigInt(u) * x, SRP_N), 32);
}

// The session key K: the even-indexed and odd-indexed bytes of S are each hashed, and the two 20-byte
// digests are written back interleaved, giving 40 bytes.
export function srpK(S) {
  const even = new Uint8Array(16);
  const odd = new Uint8Array(16);
  for (let i = 0; i < 16; i++) { even[i] = S[2 * i]; odd[i] = S[2 * i + 1]; }
  const h0 = sha1(even);
  const h1 = sha1(odd);
  const K = new Uint8Array(40);
  for (let i = 0; i < 20; i++) { K[2 * i] = h0[i]; K[2 * i + 1] = h1[i]; }
  return K;
}

// The client proof M1 = SHA1(I || SHA1(USER) || salt || A || B || K).
export function srpM1(user, salt, A, B, K) {
  return sha1(concatBytes(SRP_I, sha1(utf8(asciiUpper(user))), salt, A, B, K));
}

// The server proof M2 = SHA1(A || M1 || K), which the client checks against the server's reply.
export function srpM2(A, M1, K) {
  return sha1(concatBytes(A, M1, K));
}

// The full client computation for one logon, from the random exponent a and the server's salt and B.
export function srpClient({ user, pass, a, salt, B }) {
  const A = srpPublic(a);
  const x = srpX(salt, user, pass);
  const u = srpU(B);
  const K = srpK(srpS(a, B, x, u));
  const M1 = srpM1(user, salt, A, B, K);
  return { A, M1, K, M2: srpM2(A, M1, K) };
}

// ---- packets (BNCS framing: FF id len16LE body; len includes the four header bytes) ----

function frame(id, body) {
  const len = body.length + 4;
  const out = new Uint8Array(len);
  out[0] = 0xff;
  out[1] = id;
  out[2] = len & 0xff;
  out[3] = len >> 8;
  out.set(body, 4);
  return out;
}

// SID_AUTH_ACCOUNTLOGON: A (32 bytes) then the account name with its NUL terminator.
export function accountLogonPacket(A, user) {
  return frame(0x53, concatBytes(A, utf8(user), Uint8Array.of(0)));
}

// Reads a 0x53 reply. A reply is 72 bytes: status (LE32), salt (32), B (32).
export function parseAccountLogonReply(packet) {
  if (packet[0] !== 0xff || packet[1] !== 0x53) throw new Error('not a SID_AUTH_ACCOUNTLOGON reply');
  const len = packet[2] | (packet[3] << 8);
  if (len !== 72 || packet.length !== len) throw new Error(`bad 0x53 reply length ${len}`);
  const status = (packet[4] | (packet[5] << 8) | (packet[6] << 16) | (packet[7] << 24)) >>> 0;
  return { status, salt: packet.slice(8, 40), B: packet.slice(40, 72) };
}

// SID_AUTH_ACCOUNTLOGONPROOF: the 20-byte client proof M1.
export function logonProofPacket(M1) {
  return frame(0x54, M1);
}

// Reads a 0x54 reply. A reply is 28 bytes: status (LE32), server proof M2 (20).
export function parseLogonProofReply(packet) {
  if (packet[0] !== 0xff || packet[1] !== 0x54) throw new Error('not a SID_AUTH_ACCOUNTLOGONPROOF reply');
  const len = packet[2] | (packet[3] << 8);
  if (len !== 28 || packet.length !== len) throw new Error(`bad 0x54 reply length ${len}`);
  const status = (packet[4] | (packet[5] << 8) | (packet[6] << 16) | (packet[7] << 24)) >>> 0;
  return { status, M2: packet.slice(8, 28) };
}
