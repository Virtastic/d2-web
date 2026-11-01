#!/usr/bin/env node
// The bring-your-own-files patch shim: the 1.13c Patch_D2.mpq, base64-encoded, served beside the page as
// assets/patch-113c.patch. The page mounts it as Patch_D2.mpq over whatever patch the player gave, so the
// engine always reads 1.13c data; the player's base archives are the same at every patch level. The output
// is generated at stage/deploy time and never committed.
//   node tools/patch-shim.mjs <Patch_D2.mpq> <out.patch>   check the patch is 1.13c, write the shim
//   node tools/patch-shim.mjs --check <out.patch>          check a staged shim holds the 1.13c patch
import { readFileSync, writeFileSync } from 'node:fs';
import { createHash } from 'node:crypto';

// Patch_D2.mpq as the 1.13c patch installs it. Any other version (or a modded file) is refused.
const PATCH_113C = { size: 8229959, sha256: '41a5fc95977369e3d5f32127cc4c0d571daad9cf706b6b6523935b705afc5a6d' };

function verify(mpq, what) {
  if (mpq.length < 4 || mpq.readUInt32LE(0) !== 0x1a51504d) fail(`${what}: not an MPQ archive`);
  const sha256 = createHash('sha256').update(mpq).digest('hex');
  if (mpq.length !== PATCH_113C.size || sha256 !== PATCH_113C.sha256)
    fail(`${what}: not the 1.13c Patch_D2.mpq (${mpq.length} bytes, sha256 ${sha256}; expected ${PATCH_113C.size} bytes, sha256 ${PATCH_113C.sha256}). Update the game to 1.13c and use its Patch_D2.mpq.`);
}
function fail(msg) { console.error(`FATAL: ${msg}`); process.exit(1); }

const [, , a, b] = process.argv;
if (a === '--check' && b) {
  verify(Buffer.from(readFileSync(b, 'latin1'), 'base64'), b);
  console.log(`patch shim: ${b} holds the 1.13c Patch_D2.mpq`);
} else if (a && b) {
  const mpq = readFileSync(a);
  verify(mpq, a);
  writeFileSync(b, mpq.toString('base64'));
  console.log(`patch shim: ${a} (${mpq.length} bytes, 1.13c) -> ${b}`);
} else {
  console.error('usage: patch-shim.mjs <Patch_D2.mpq> <out.patch> | --check <out.patch>');
  process.exit(2);
}
