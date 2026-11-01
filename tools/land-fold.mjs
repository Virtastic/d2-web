#!/usr/bin/env node
// tools/land-fold.mjs [--repo <dir>] [--base <rev>] [--head <branch>] [--cap <n>] -- folds local commits so that
// `<base>..<head>` fits the public push cap (the pre-push hook refuses more than D2_PUSH_MAX_COMMITS commits).
//
// Each fold merges one adjacent pair into one commit: the oldest pair whose subjects share an area (the text before
// the first ':') is taken first; if no pair shares an area, the oldest adjacent pair is taken. The folded commit keeps
// the older commit's author and the later commit's committer and tree; its subject is `<area>: <a>; <b>` for a shared
// area, else `<a>; <b>`. Every later commit is rebuilt with `git commit-tree` on its own tree, so the final tree is
// byte-identical to the original head's tree (asserted). The branch moves by `git update-ref` with the old value as
// the compare-and-swap, so a concurrent move fails instead of being overwritten. The working tree and index are not
// touched. Exit 0 when the range fits (folded or already fitting), 1 on a refused fold, 2 on bad arguments.
import { execFileSync } from 'node:child_process';
import { fileURLToPath } from 'node:url';

const USAGE = `usage: node tools/land-fold.mjs [--repo <dir>] [--base <rev>] [--head <branch>] [--cap <n>]
  --repo  checkout to operate on (default: current directory)
  --base  the revision already on the remote (default: origin/main)
  --head  the local branch to fold (default: main)
  --cap   the most commits the range may hold (default: D2_PUSH_MAX_COMMITS, else 10)
`;

const SEP = '\x1e';   // record separator between commits in the log output
const FIELD = '\x00'; // NUL between fields inside a record

export function git(cwd, ...args) {
  return execFileSync('git', ['-C', cwd, ...args], { encoding: 'utf8', stdio: ['ignore', 'pipe', 'pipe'], maxBuffer: 1 << 26 });
}

// The range as an ordered list, oldest first. Message bodies are kept verbatim; the subject is the first line.
export function readChain(repo, base, head) {
  const out = git(repo, 'log', '--reverse', '--format=%T%x00%an%x00%ae%x00%aI%x00%cn%x00%ce%x00%cI%x00%B%x1e', `${base}..${head}`);
  return out.split(SEP).filter((r) => r.trim()).map((rec) => {
    const [tree, an, ae, ai, cn, ce, ci, msg] = rec.replace(/^\n/, '').split(FIELD);
    const message = msg.replace(/\n+$/, '');
    const nl = message.indexOf('\n');
    const subject = nl < 0 ? message : message.slice(0, nl);
    const body = nl < 0 ? '' : message.slice(nl + 1).replace(/^\n+/, '');
    return { tree, author: { name: an, email: ae, date: ai }, committer: { name: cn, email: ce, date: ci }, subject, body };
  });
}

export const areaOf = (subject) => {
  const m = /^([^:]+):/.exec(subject);
  return m ? m[1].trim() : null;
};

// The pair to fold next: the oldest adjacent pair sharing an area, else the oldest adjacent pair.
export function pickPair(items) {
  for (let i = 0; i + 1 < items.length; i++) {
    const a = areaOf(items[i].subject);
    if (a !== null && a === areaOf(items[i + 1].subject)) return i;
  }
  return 0;
}

export function foldPair(a, b) {
  const area = areaOf(a.subject);
  const subject = area !== null && area === areaOf(b.subject)
    ? `${area}: ${a.subject.slice(area.length + 1).trim()}; ${b.subject.slice(area.length + 1).trim()}`
    : `${a.subject}; ${b.subject}`;
  const body = [a.body, b.body].filter(Boolean).join('\n\n');
  return { tree: b.tree, author: a.author, committer: b.committer, subject, body };
}

// Folds `items` until at most `cap` remain. Pure: returns the new list.
export function planFold(items, cap) {
  let list = items.slice();
  while (list.length > Math.max(1, cap)) {
    const i = pickPair(list);
    list.splice(i, 2, foldPair(list[i], list[i + 1]));
  }
  return list;
}

const messageOf = (it) => (it.body ? `${it.subject}\n\n${it.body}\n` : `${it.subject}\n`);

// Rebuilds the planned chain on `base` with commit-tree; returns the new head sha.
export function rebuild(repo, base, list) {
  let parent = git(repo, 'rev-parse', '--verify', `${base}^{commit}`).trim();
  for (const it of list) {
    const env = {
      ...process.env,
      GIT_AUTHOR_NAME: it.author.name, GIT_AUTHOR_EMAIL: it.author.email, GIT_AUTHOR_DATE: it.author.date,
      GIT_COMMITTER_NAME: it.committer.name, GIT_COMMITTER_EMAIL: it.committer.email, GIT_COMMITTER_DATE: it.committer.date,
    };
    parent = execFileSync('git', ['-C', repo, 'commit-tree', it.tree, '-p', parent], {
      encoding: 'utf8', input: messageOf(it), env, stdio: ['pipe', 'pipe', 'pipe'],
    }).trim();
  }
  return parent;
}

// The whole step. Returns { folded, before, after, head }.
export function landFold({ repo, base, head, cap }) {
  const ref = `refs/heads/${head}`;
  const oldHead = git(repo, 'rev-parse', '--verify', `${ref}^{commit}`).trim();
  const chain = readChain(repo, base, head);
  if (chain.length <= cap) return { folded: 0, before: chain.length, after: chain.length, head: oldHead };
  const origTree = git(repo, 'rev-parse', `${oldHead}^{tree}`).trim();

  const plan = planFold(chain, cap);
  const newHead = rebuild(repo, base, plan);
  const newTree = git(repo, 'rev-parse', `${newHead}^{tree}`).trim();
  if (newTree !== origTree) throw new Error(`land-fold: tree changed (${origTree} -> ${newTree}); ref not moved`);
  const count = Number(git(repo, 'rev-list', '--count', `${base}..${newHead}`).trim());
  if (count !== plan.length || count > cap) throw new Error(`land-fold: ${count} commits after folding, expected ${plan.length} within ${cap}`);

  // compare-and-swap: refuses if the branch moved since it was read above
  git(repo, 'update-ref', ref, newHead, oldHead);
  return { folded: chain.length - plan.length, before: chain.length, after: plan.length, head: newHead };
}

function parseArgs(argv) {
  const o = { repo: process.cwd(), base: 'origin/main', head: 'main', cap: Number(process.env.D2_PUSH_MAX_COMMITS || 10) };
  for (let i = 0; i < argv.length; i++) {
    const a = argv[i];
    const val = () => { if (i + 1 >= argv.length) throw new Error(`${a} needs a value`); return argv[++i]; };
    if (a === '--repo') o.repo = val();
    else if (a === '--base') o.base = val();
    else if (a === '--head') o.head = val();
    else if (a === '--cap') o.cap = Number(val());
    else if (a === '-h' || a === '--help') { process.stdout.write(USAGE); process.exit(0); }
    else throw Object.assign(new Error(`unknown argument ${a}`), { usage: true });
  }
  if (!Number.isInteger(o.cap) || o.cap < 1) throw Object.assign(new Error(`bad --cap ${o.cap}`), { usage: true });
  return o;
}

const isMain = process.argv[1] === fileURLToPath(import.meta.url);
if (isMain) {
  let opts;
  try {
    opts = parseArgs(process.argv.slice(2));
  } catch (e) {
    process.stderr.write(`land-fold: ${e.message}\n${USAGE}`);
    process.exit(2);
  }
  try {
    const r = landFold(opts);
    if (r.folded) console.log(`land-fold: ${r.before} commits -> ${r.after} (folded ${r.folded}); ${opts.head} is now ${r.head.slice(0, 10)}`);
    else console.log(`land-fold: ${r.before} commits fit the cap of ${opts.cap}; nothing folded`);
  } catch (e) {
    process.stderr.write(`${e.message}\n`);
    process.exit(1);
  }
}
