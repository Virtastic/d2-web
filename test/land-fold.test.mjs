// tools/land-fold.mjs: folds a local range down to the push cap on a throwaway repo. The final tree must be
// byte-identical to the original head's tree, the working tree and index must not move, and the branch must move
// only by compare-and-swap.
import { test } from 'node:test';
import assert from 'node:assert/strict';
import { execFileSync } from 'node:child_process';
import { mkdtempSync, mkdirSync, writeFileSync, readFileSync, rmSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join, dirname } from 'node:path';
import { fileURLToPath } from 'node:url';
import { areaOf, planFold, foldPair, landFold } from '../tools/land-fold.mjs';

const tool = fileURLToPath(new URL('../tools/land-fold.mjs', import.meta.url));
const env = {
  ...process.env,
  GIT_AUTHOR_NAME: 'alpha author', GIT_AUTHOR_EMAIL: 'alpha@example.invalid',
  GIT_COMMITTER_NAME: 'beta committer', GIT_COMMITTER_EMAIL: 'beta@example.invalid',
};
const git = (cwd, ...a) => execFileSync('git', ['-C', cwd, ...a], { encoding: 'utf8', env });

// A repo whose `base` branch is the remote-side tip and whose `main` carries one commit per subject. Each commit
// writes its own file, so every intermediate tree differs and a wrong fold would show in the final tree.
function fixture(subjects) {
  const dir = mkdtempSync(join(tmpdir(), 'land-fold-'));
  git(dir, 'init', '-q', '-b', 'main');
  const put = (name, text) => { mkdirSync(dirname(join(dir, name)), { recursive: true }); writeFileSync(join(dir, name), text); };
  put('README.md', 'base\n');
  git(dir, 'add', '-A');
  git(dir, 'commit', '-q', '-m', 'base');
  git(dir, 'branch', 'base');
  subjects.forEach((s, i) => {
    put(`f${i}.txt`, `${s}\n`);
    git(dir, 'add', '-A');
    git(dir, 'commit', '-q', '-m', s, '-m', `body of ${i}`);
  });
  return dir;
}
const subjectsOf = (dir) => git(dir, 'log', '--reverse', '--format=%s', 'base..main').split('\n').filter(Boolean);
const treeOf = (dir, rev) => git(dir, 'rev-parse', `${rev}^{tree}`).trim();
const cleanup = (dir) => rmSync(dir, { recursive: true, force: true });

test('areaOf reads the text before the first colon', () => {
  assert.equal(areaOf('alpha: one'), 'alpha');
  assert.equal(areaOf('alpha one'), null);
});

test('pickPair takes the oldest same-area pair, else the oldest pair', () => {
  const items = (subjects) => subjects.map((subject) => ({ subject }));
  assert.equal(planFold(items(['a: 1', 'b: 2', 'b: 3', 'a: 4']), 3).map((x) => x.subject)[1], 'b: 2; 3');
  assert.deepEqual(planFold(items(['a: 1', 'b: 2', 'c: 3']), 2).map((x) => x.subject), ['a: 1; b: 2', 'c: 3']);
  assert.equal(foldPair({ subject: 'x: 1', body: '', tree: 't', author: {}, committer: {} },
    { subject: 'x: 2', body: '', tree: 't2', author: {}, committer: {} }).subject, 'x: 1; 2');
});

test('folds same-area pairs down to the cap; the final tree is byte-identical', () => {
  const subjects = ['alpha: one', 'alpha: two', 'beta: three', 'gamma: four', 'gamma: five', 'beta: six',
    'delta: seven', 'delta: eight', 'eps: nine', 'zeta: ten', 'eta: eleven', 'eta: twelve'];
  const dir = fixture(subjects);
  try {
    const before = git(dir, 'rev-parse', 'main').trim();
    const tree = treeOf(dir, 'main');
    const status = git(dir, 'status', '--porcelain');
    const r = landFold({ repo: dir, base: 'base', head: 'main', cap: 10 });
    assert.equal(r.before, 12);
    assert.equal(r.after, 10);
    assert.equal(r.folded, 2);
    assert.equal(treeOf(dir, 'main'), tree, 'final tree is byte-identical');
    assert.notEqual(git(dir, 'rev-parse', 'main').trim(), before, 'the branch moved');
    assert.deepEqual(subjectsOf(dir), ['alpha: one; two', 'beta: three', 'gamma: four; five', 'beta: six',
      'delta: seven', 'delta: eight', 'eps: nine', 'zeta: ten', 'eta: eleven', 'eta: twelve']);
    // the author of the folded commit is the older commit's; the committer is the later one's
    assert.equal(git(dir, 'log', '--format=%an', '-1', 'main~9').trim(), 'alpha author');
    // working tree and index untouched
    assert.equal(git(dir, 'status', '--porcelain'), status);
    assert.equal(readFileSync(join(dir, 'f11.txt'), 'utf8'), 'eta: twelve\n');
  } finally {
    cleanup(dir);
  }
});

test('with no same-area pair the oldest adjacent pair is folded and joined with "; "', () => {
  const dir = fixture(['alpha: one', 'beta: two', 'gamma: three']);
  try {
    const tree = treeOf(dir, 'main');
    const r = landFold({ repo: dir, base: 'base', head: 'main', cap: 2 });
    assert.equal(r.after, 2);
    assert.equal(treeOf(dir, 'main'), tree);
    assert.deepEqual(subjectsOf(dir), ['alpha: one; beta: two', 'gamma: three']);
  } finally {
    cleanup(dir);
  }
});

test('a range that already fits the cap is not rewritten', () => {
  const dir = fixture(['alpha: one', 'beta: two']);
  try {
    const before = git(dir, 'rev-parse', 'main').trim();
    const r = landFold({ repo: dir, base: 'base', head: 'main', cap: 10 });
    assert.equal(r.folded, 0);
    assert.equal(git(dir, 'rev-parse', 'main').trim(), before);
  } finally {
    cleanup(dir);
  }
});

test('the CLI exits 0 after folding and 2 on a bad argument', () => {
  const dir = fixture(['alpha: one', 'alpha: two', 'alpha: three']);
  try {
    const out = execFileSync(process.execPath, [tool, '--repo', dir, '--base', 'base', '--head', 'main', '--cap', '2'],
      { encoding: 'utf8', env });
    assert.match(out, /3 commits -> 2 \(folded 1\)/);
    assert.deepEqual(subjectsOf(dir), ['alpha: one; two', 'alpha: three']);
    let code = 0;
    try {
      execFileSync(process.execPath, [tool, '--bogus'], { encoding: 'utf8', env, stdio: 'pipe' });
    } catch (e) { code = e.status; }
    assert.equal(code, 2);
  } finally {
    cleanup(dir);
  }
});
