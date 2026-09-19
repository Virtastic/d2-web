// Writes play/smoke.mpq (synthetic, via tools/mkmpq.mjs) and play/smoke.json (what s00 must
// read back out of it). 3 MB of deterministic bytes: spans two streamfs chunks (2 MB), so a
// read that only ever touched the first chunk produces the wrong hash rather than passing.
import { buildMpq, fnv1a } from './mkmpq.mjs';
import { writeFileSync } from 'node:fs';
const data = new Uint8Array(3 * 1024 * 1024);
let x = 0x9E3779B9; for (let i = 0; i < data.length; i++) { x ^= x << 13; x ^= x >>> 17; x ^= x << 5; data[i] = x & 0xFF; }
writeFileSync('play/smoke.mpq', buildMpq([{ name: 'smoke.txt', data }, { name: 'other.bin', data: new Uint8Array([1, 2, 3]) }]));
writeFileSync('play/smoke.json', JSON.stringify({ smoke: `${data.length}:${fnv1a(data)}` }) + '\n');
console.log('play/smoke.mpq + smoke.json:', `${data.length}:${fnv1a(data)}`);
