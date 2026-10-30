// Prints the known-mpqs.json entry for each MPQ given: node tools/mpq-fingerprint.mjs <file> [...]
import { inspect } from '../play/assets/byo-detect.mjs';
import { openAsBlob } from 'node:fs';
import { basename } from 'node:path';
for (const p of process.argv.slice(2)) {
  const b = await openAsBlob(p);
  const f = new File([b], basename(p));
  const i = await inspect(f);
  console.log(JSON.stringify({ file: i.name, role: i.role, size: i.size, blockN: i.blockN, sha256: i.sha256 }));
}
