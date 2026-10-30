#!/usr/bin/env node
// The bring-your-own-files patch shim: the 1.13c Patch_D2.mpq, base64-encoded, served beside the page as
// assets/patch-113c.patch. The page mounts it as Patch_D2.mpq over whatever patch the player gave, so the
// engine always reads 1.13c data; the player's base archives are the same at every patch level. The output
// is generated at stage/deploy time and never committed.
//   node tools/patch-shim.mjs <Patch_D2.mpq> <out.patch>
import { readFileSync, writeFileSync } from 'node:fs';

const [, , src, out] = process.argv;
if (!src || !out) { console.error('usage: patch-shim.mjs <Patch_D2.mpq> <out.patch>'); process.exit(2); }
const mpq = readFileSync(src);
if (mpq.readUInt32LE(0) !== 0x1a51504d) { console.error(`${src}: not an MPQ`); process.exit(1); }
writeFileSync(out, mpq.toString('base64'));
console.log(`patch shim: ${src} (${mpq.length} bytes) -> ${out}`);
