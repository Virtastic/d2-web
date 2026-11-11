#!/usr/bin/env node
// Writes electron/package.json's version into a staged copy of play/assets/build-version.mjs.
// usage: node ci/stamp-version.mjs <staged build-version.mjs>
import { readFileSync, writeFileSync } from 'node:fs';
import { fileURLToPath } from 'node:url';
import { dirname, join } from 'node:path';

export function stampVersion(source, version) {
  if (!/^\d+(\.\d+)*$/.test(version)) throw new Error(`not a release version: ${version}`);
  const out = source.replace(/export const RELEASE_VERSION = '[^']*';/, `export const RELEASE_VERSION = '${version}';`);
  if (out === source && !source.includes(`'${version}'`)) throw new Error('RELEASE_VERSION line not found');
  return out;
}

if (process.argv[1] === fileURLToPath(import.meta.url)) {
  const root = join(dirname(fileURLToPath(import.meta.url)), '..');
  const { version } = JSON.parse(readFileSync(join(root, 'electron/package.json'), 'utf8'));
  const file = process.argv[2];
  if (!file) { console.error('usage: stamp-version.mjs <file>'); process.exit(2); }
  writeFileSync(file, stampVersion(readFileSync(file, 'utf8'), version));
  console.log(`stamped v ${version} into ${file}`);
}
