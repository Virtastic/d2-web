// Every explicitly relative #include ("./", "../") in tracked engine source must resolve to a tracked file, never to a gitignored one:
// a header that only exists in a developer's untracked tree builds there and breaks every clean checkout.
import { test } from 'node:test';
import assert from 'node:assert/strict';
import { execFileSync } from 'node:child_process';
import { readFileSync } from 'node:fs';
import { dirname, join, normalize } from 'node:path';

const git = (args, input) => execFileSync('git', args, { encoding: 'utf8', input, maxBuffer: 1 << 28 });

test('tracked engine source includes no gitignored header', () => {
  const tracked = new Set(git(['ls-files', 'src']).split('\n').filter(Boolean));
  const refs = [];
  for (const f of tracked) {
    if (!/\.(c|cc|cpp|h|hh|hpp|inc)$/.test(f)) continue;
    let text; try { text = readFileSync(f, 'utf8'); } catch { continue; }
    for (const m of text.matchAll(/^\s*#\s*include\s+"(\.\.?\/[^"]+)"/gm)) {
      const p = normalize(join(dirname(f), m[1]));
      if (p.startsWith('src/') && !tracked.has(p)) refs.push([f, p]);
    }
  }
  if (!refs.length) return;
  let ignored = '';
  try { ignored = git(['check-ignore', '--no-index', '--stdin'], refs.map(([, p]) => p).join('\n') + '\n'); } catch { /* exit 1: none ignored */ }
  const bad = new Set(ignored.split('\n').filter(Boolean));
  const hits = refs.filter(([, p]) => bad.has(p)).map(([f, p]) => `${f} -> ${p}`);
  assert.deepEqual(hits, [], 'track the header (add a .gitignore exception) or drop the include');
});
