import test from 'node:test';
import assert from 'node:assert/strict';
import { readFileSync, writeFileSync, mkdtempSync, copyFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join, dirname } from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';
import { execFileSync } from 'node:child_process';
import { versionLabel, RETAIL_LABEL } from './build-version.mjs';
import { TITLE_VERSION } from './frontend.mjs';
import { AREA_NAME_BLOCK, areaNameBlockLines } from './area-name-block.mjs';

const here = dirname(fileURLToPath(import.meta.url));
const pkg = JSON.parse(readFileSync(join(here, '../../electron/package.json'), 'utf8'));

async function stagedModule() {
  const dir = mkdtempSync(join(tmpdir(), 'bv-'));
  const f = join(dir, 'build-version.mjs');
  copyFileSync(join(here, 'build-version.mjs'), f);
  execFileSync(process.execPath, [join(here, '../../ci/stamp-version.mjs'), f]);
  return import(pathToFileURL(f).href);
}

test('the shipped label equals "v " + electron/package.json version', async () => {
  const m = await stagedModule();
  assert.equal(m.versionLabel({ versionLabel: 'release' }), `v ${pkg.version}`);
  assert.equal(m.versionLabel(undefined), `v ${pkg.version}`);
});

test('an unstaged build says v dev, never v 1.13', () => {
  assert.equal(versionLabel({ versionLabel: 'release' }), 'v dev');
});

test('the harness/parity path still draws v 1.13 on the title screen and the automap block', () => {
  const saved = globalThis.__d2Entry;
  try {
    globalThis.__d2Entry = { versionLabel: 'retail' };
    assert.equal(RETAIL_LABEL, 'v 1.13');
    assert.equal(TITLE_VERSION.text, 'v 1.13');
    assert.equal(AREA_NAME_BLOCK.version, 'v 1.13');
    assert.equal(areaNameBlockLines({ name: 'A' }).find((l) => l.key === 'version').text, 'v 1.13');
    globalThis.__d2Entry = { versionLabel: 'release' };
    assert.equal(TITLE_VERSION.text, 'v dev');
    assert.equal(areaNameBlockLines({ name: 'A' }).find((l) => l.key === 'version').text, 'v dev');
  } finally { globalThis.__d2Entry = saved; }
});

test('entry.js gives the test runner the retail label and a player the release one', () => {
  const src = readFileSync(join(here, '../entry.js'), 'utf8');
  assert.match(src, /test && !asPlayer[^\n]*'retail' : 'release'/);
});
