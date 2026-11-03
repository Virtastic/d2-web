#!/usr/bin/env node
// link-check.mjs -- a seconds-long pre-link check over src/ (no compiler, no wasm build).
//
// A single bad source file otherwise only shows up as a link error minutes into a build and blocks every
// consumer of that build. This reads the sources the way the linker will see them and reports:
//
//   LINK-DUP <name> <file:line> <file:line>      the same external function or variable defined in two .cpp files
//   LINK-LINKAGE <name> <file:line> <detail>     declared extern "C" where the only definitions have C++ linkage (or the reverse)
//   LINK-UNDEF <name> <file:line>                a variable declared, used in a function body, and defined nowhere in src/
//                                                (an undefined FUNCTION is imported by the link, not an error, so it is not reported)
//
// usage: node tools/link-check.mjs [--root DIR] [--files a.cpp,b.cpp] [--no-undef] [--no-baseline] [--write-baseline] [--json]
//   --files  report only findings that involve one of these files (the staged files; the whole tree is still scanned)
// Exit 1 when anything is reported. Names the build does not own (libc, the Storm library, emscripten) live in
// tools/link-check.external, one name per line. The scan is a heuristic: it skips #if 0 and #else branches, templates,
// static/inline/anonymous-namespace names, and class bodies, so it can miss a problem, and it should not invent one.
import { readFileSync, readdirSync, existsSync, statSync } from 'node:fs';
import { join, relative, basename, dirname } from 'node:path';
import { fileURLToPath } from 'node:url';

const argv = process.argv.slice(2);
const opt = (n, d) => { const i = argv.indexOf(n); return i < 0 ? d : argv[i + 1]; };
const ROOT = opt('--root', join(dirname(fileURLToPath(import.meta.url)), '..'));
const ONLY = (opt('--files', '') || '').split(',').filter(Boolean).map((s) => s.replace(/^\.\//, ''));

// ---- lexing: comments, literals, preprocessor -------------------------------------------------------------------------
// Returns the text with comments/literals blanked and preprocessor lines removed; `extern "C"` survives as __EXTC__.
// Content of #else/#elif branches and #if 0 is dropped (conditional alternatives would look like duplicates).
export function clean(src) {
  let out = '', i = 0;
  const n = src.length;
  const cond = [];   // per open #if: { skip: bool (inside a dropped branch), dead: bool (the whole #if is 0) }
  const skipping = () => cond.some((c) => c.skip);
  let atLineStart = true;
  while (i < n) {
    const c = src[i], d = src[i + 1];
    if (c === '\n') { out += '\n'; i++; atLineStart = true; continue; }
    if (atLineStart && (c === ' ' || c === '\t')) { if (!skipping()) out += c; i++; continue; }
    if (atLineStart && c === '#') {
      let j = i;
      let line = '';
      for (;;) {   // a directive with continuations
        const e = src.indexOf('\n', j);
        const stop = e < 0 ? n : e;
        let seg = src.slice(j, stop);
        if (seg.endsWith('\\') || seg.endsWith('\\\r')) { line += seg.replace(/\\\r?$/, ' '); j = stop + 1; out += '\n'; if (e < 0) break; continue; }
        line += seg; j = stop; break;
      }
      line = line.replace(/\/\*.*?\*\//g, ' ').replace(/\/\/.*$/, '');
      const m = /^#\s*(\w+)\s*(.*)$/.exec(line.trim());
      if (m) {
        const [, dir, rest] = m;
        if (dir === 'if' || dir === 'ifdef' || dir === 'ifndef') cond.push({ skip: dir === 'if' && /^\(?\s*0\s*\)?$/.test(rest.trim()) });
        else if (dir === 'else' || dir === 'elif') { if (cond.length) cond[cond.length - 1].skip = true; }
        else if (dir === 'endif') cond.pop();
        else if (dir === 'include' && !skipping()) out += `__INCLUDE__(${rest.trim()})`;
      }
      i = j; atLineStart = false; continue;
    }
    atLineStart = false;
    if (c === '/' && d === '/') { while (i < n && src[i] !== '\n') i++; continue; }
    if (c === '/' && d === '*') { const e = src.indexOf('*/', i + 2); const body = src.slice(i, e < 0 ? n : e + 2); out += body.replace(/[^\n]/g, ''); i = e < 0 ? n : e + 2; if (!skipping()) out += ' '; continue; }
    if (skipping()) { i++; continue; }
    if (c === '"' || (c === 'R' && d === '"' && !/\w/.test(src[i - 1] || ' '))) {
      let lit;
      if (c === 'R') {   // raw string R"delim( ... )delim"
        const p = src.indexOf('(', i + 2), delim = src.slice(i + 2, p), e = src.indexOf(`)${delim}"`, p);
        lit = src.slice(i, e < 0 ? n : e + delim.length + 2); i += lit.length; out += '""'; out += lit.replace(/[^\n]/g, ''); continue;
      }
      let j = i + 1;
      while (j < n && src[j] !== '"' && src[j] !== '\n') { if (src[j] === '\\') j++; j++; }
      lit = src.slice(i + 1, j); i = j + 1;
      if (/\bextern\s*$/.test(out) && (lit === 'C' || lit === 'C++')) out += lit === 'C' ? '__EXTC__' : '__EXTCPP__';
      else out += '""';
      continue;
    }
    if (c === "'") {
      let j = i + 1;
      while (j < n && src[j] !== "'" && src[j] !== '\n') { if (src[j] === '\\') j++; j++; }
      i = j + 1; out += "'0'"; continue;
    }
    out += c; i++;
  }
  return out;
}

// ---- declaration scan -------------------------------------------------------------------------------------------------
// One walk over the cleaned text, tracking scope: extern "C" / namespace blocks are transparent, anything else (function
// body, class, enum, initializer) is opaque and only contributes the identifiers it uses.
const KW_SKIP = /^(typedef|using|friend|template|static_assert|enum|struct\s+\w+\s*;|class\s+\w+\s*;|union\s+\w+\s*;|asm|__asm)\b/;
export function scan(text, weakMacros = []) {
  const items = [];   // { kind: 'def'|'decl', what: 'fn'|'var', name, ns, c: bool, line, sig, internal }
  const used = new Set();
  const includes = [];
  text = text.replace(/__INCLUDE__\(([^\n]*)\)/g, (_, r) => { includes.push(r); return ''; });
  const stack = [];   // { t: 'extc'|'ns'|'opaque', name, fn: bool }
  let stmt = '', stmtLine = 1, line = 1;
  let pendingInit = false;   // a '{' that opened an initializer or tag body: the statement ends at the next ';'
  const declScope = () => stack.every((s) => s.t !== 'opaque');
  const inExtC = () => stack.some((s) => s.t === 'extc');
  const nsPath = () => stack.filter((s) => s.t === 'ns').map((s) => s.name).join('::');
  const anon = () => stack.some((s) => s.t === 'ns' && s.name === '');
  let opaqueBuf = '';
  const flushOpaque = () => { for (const m of opaqueBuf.matchAll(/[A-Za-z_]\w*/g)) used.add(m[0]); opaqueBuf = ''; };

  const parseStmt = (raw, isDef, ln, forceC) => {
    let s = raw.replace(/__INCLUDE__\(([^)]*)\)/g, (_, r) => { includes.push(r); return ' '; }).trim();
    if (!s) return;
    let c = forceC || inExtC();
    s = s.replace(/\bextern\s+__EXTC__/, () => { c = true; return 'extern'; }).replace(/__EXTC__/, () => { c = true; return ''; })
         .replace(/\bextern\s+__EXTCPP__/, 'extern').replace(/__EXTCPP__/, '').trim();
    if (!s || KW_SKIP.test(s) || /^namespace\b/.test(s) || /^(public|private|protected)\s*:/.test(s)) return;
    if (/^(struct|class|union)\b[^()=]*$/.test(s) && !isDef) return;   // forward decl / bare tag
    let weak = /__attribute__\s*\(\(\s*weak\s*\)\)/.test(s);
    for (const m of weakMacros) if (new RegExp('(^|\\s)' + m + '(\\s|$)').test(s)) { weak = true; s = s.replace(new RegExp('(^|\\s)' + m + '(?=\\s|$)'), ' ').trim(); }
    const isExtern = /^extern\b/.test(s);
    const isStatic = /(^|\s)static\b/.test(s) || /(^|\s)(inline|__inline|__forceinline|constexpr)\b/.test(s);
    const sNoExtern = s.replace(/^extern\s+/, '');
    // a function: first '(' at depth 0 with an identifier (or operator) before it, and not a (*pointer) variable
    const po = findFnParen(sNoExtern);
    if (po >= 0) {
      const head = sNoExtern.slice(0, po).trim();
      const nm = /([A-Za-z_~][\w]*(?:\s*::\s*~?[A-Za-z_]\w*)*|operator\s*\S+)\s*$/.exec(head);
      if (!nm) return;
      const name = nm[1].replace(/\s+/g, '');
      if (/^(if|for|while|switch|return|sizeof|decltype|__attribute__|alignas|__declspec)$/.test(name)) return;
      // params
      let depth = 0, pe = -1;
      for (let k = po; k < sNoExtern.length; k++) { const ch = sNoExtern[k]; if (ch === '(') depth++; else if (ch === ')') { depth--; if (!depth) { pe = k; break; } } }
      if (pe < 0) return;
      const sig = normSig(sNoExtern.slice(po + 1, pe));
      const tail = sNoExtern.slice(pe + 1);
      if (isDef === false && /^\s*=\s*0\b/.test(tail)) return;
      // constructor-style variable init `T x(args);` at namespace scope is not told apart from a prototype; harmless.
      items.push({ kind: isDef ? 'def' : 'decl', what: 'fn', name, ns: nsPath(), c, line: ln, sig, weak, internal: isStatic || anon() || /^template\b/.test(head) });
      return;
    }
    // a variable: strip initializer and array bounds, last identifier is the name
    let v = sNoExtern.replace(/=.*$/s, '').replace(/\[[^\]]*\]/g, '').replace(/\bconst\b/g, (m) => m).trim();
    const vm = /([A-Za-z_]\w*)\s*$/.exec(v);
    if (!vm || v.split(/\s+/).length < 2 && !/[*&]/.test(v)) return;
    const hasConst = /\bconst\b/.test(v) && !isExtern && !c;   // namespace-scope const has internal linkage in C++
    items.push({ kind: isExtern && !/=/.test(s) ? 'decl' : 'def', what: 'var', name: vm[1], ns: nsPath(), c, line: ln, sig: '', weak, internal: isStatic || anon() || hasConst });
  };

  let i = 0;
  const n = text.length;
  while (i < n) {
    const ch = text[i];
    if (ch === '\n') { line++; if (!declScope()) opaqueBuf += ' '; else stmt += ' '; i++; continue; }
    if (!declScope()) {
      if (ch === '{') { stack.push({ t: 'opaque' }); }
      else if (ch === '}') {
        stack.pop();
        if (declScope()) {   // left the outermost opaque block
          flushOpaque();
          const top = stack[stack.length - 1];
          if (pendingInit) { /* statement continues to ';' */ } else { stmt = ''; }
        }
      } else opaqueBuf += ch;
      i++; continue;
    }
    if (ch === '{') {
      const head = stmt.trim();
      if (/^(inline\s+)?namespace\b/.test(head) || /\bnamespace\s*$/.test(head) || /^namespace\b/.test(head)) {
        const m = /namespace\s+([\w:]+)?/.exec(head);
        stack.push({ t: 'ns', name: (m && m[1]) || '' }); stmt = ''; i++; continue;
      }
      if (/^extern\b.*__EXTC(PP)?__\s*$/.test(head) || /^__EXTC(PP)?__\s*$/.test(head)) {
        stack.push({ t: /__EXTCPP__/.test(head) ? 'ns' : 'extc', name: '' }); if (/__EXTCPP__/.test(head)) stack[stack.length - 1].name = '\0'; stmt = ''; i++; continue;
      }
      const isTag = /^(typedef\s+)?(struct|class|union|enum)\b/.test(head) && !/\)\s*$/.test(head.replace(/__attribute__\s*\(\(.*?\)\)/g, ''));
      const isInit = /=\s*$/.test(head) || /\)\s*=\s*$/.test(head);
      if (isTag || isInit || findFnParen(head.replace(/^extern\s+/, '')) < 0) { pendingInit = true; stack.push({ t: 'opaque' }); stmt += ' {} '; i++; continue; }
      // function definition
      pendingInit = false;
      parseStmt(head, true, stmtLine);
      stack.push({ t: 'opaque' }); stmt = ''; i++; continue;
    }
    if (ch === '}') { stack.pop(); stmt = ''; i++; continue; }
    if (ch === ';') {
      pendingInit = false;
      parseStmt(stmt, false, stmtLine);
      stmt = ''; i++; continue;
    }
    if (!stmt.trim()) stmtLine = line;
    stmt += ch; i++;
  }
  flushOpaque();
  return { items, used, includes };
}
function findFnParen(s) {
  let depth = 0;
  for (let k = 0; k < s.length; k++) {
    const ch = s[k];
    if (ch === '(') {
      if (depth === 0) {
        if (/^\(\s*[*&^]/.test(s.slice(k)) ) return -1;           // function-pointer variable
        if (/\b(__attribute__|__declspec|alignas|decltype)\s*$/.test(s.slice(0, k))) { depth++; continue; }
        if (/=/.test(s.slice(0, k)) && !/operator\s*=\s*$/.test(s.slice(0, k))) return -1;   // initializer with a call
        if (!/[\w>~]\s*$/.test(s.slice(0, k))) return -1;
        return k;
      }
      depth++;
    } else if (ch === ')') depth--;
  }
  return -1;
}
function normSig(p) {
  p = p.replace(/=[^,]*/g, '').trim();
  if (p === '' || p === 'void') return '';
  return splitTop(p).map((a) => {
    a = a.replace(/\s+/g, ' ').trim();
    const m = /^(.*[*&>\s])([A-Za-z_]\w*)$/.exec(a);
    if (m && m[1].trim() && !/(::|<|,)\s*$/.test(m[1]) && !/^(const|volatile|struct|class|enum|unsigned|signed|long|short|int|char|double|float|bool|void)$/.test(m[2])) a = m[1].trim();
    return a.replace(/\s*([*&])\s*/g, '$1').replace(/\bconst\b\s*/g, 'const ').trim();
  }).join(',');
}
function splitTop(s) {
  const out = []; let d = 0, cur = '';
  for (const ch of s) { if ('(<['.includes(ch)) d++; else if (')>]'.includes(ch)) d--; if (ch === ',' && d === 0) { out.push(cur); cur = ''; } else cur += ch; }
  out.push(cur); return out;
}

// ---- walk the tree ----------------------------------------------------------------------------------------------------
function walk(dir, acc = []) {
  for (const e of readdirSync(dir, { withFileTypes: true })) {
    const p = join(dir, e.name);
    if (e.isDirectory()) { if (e.name === 'node_modules' || e.name === '.git') continue; walk(p, acc); }
    else if (/\.(cpp|h|hpp|inl)$/.test(e.name)) acc.push(p);
  }
  return acc;
}

export function check(root, { undef = true } = {}) {
  const srcDir = join(root, 'src');
  if (!existsSync(srcDir)) return { findings: [] };
  const files = walk(srcDir).sort();
  const byBase = new Map();
  for (const f of files) { const b = basename(f); (byBase.get(b) || byBase.set(b, []).get(b)).push(f); }
  const info = new Map();
  for (const f of files) {
    const rel = relative(root, f);
    if (/\.test\.cpp$/.test(f) || rel.startsWith('src/core/fn/')) continue;   // FN probes are optional: the build drops one that does not link
    let sc;
    try { const raw = readFileSync(f, 'utf8'); const wm = [...raw.matchAll(/^[ \t]*#\s*define\s+(\w+)[^\n]*\bweak\b/gm)].map((m) => m[1]); sc = scan(clean(raw), wm); const rn = new Map([...raw.matchAll(/^[ \t]*#\s*define\s+(\w+)[ \t]+(\w+)[ \t]*$/gm)].map((m) => [m[1], m[2]])); if (rn.size) for (const it of sc.items) if (it.kind === "def" && rn.has(it.name)) it.name = rn.get(it.name); } catch (e) { continue; }
    info.set(f, { rel, ...sc, isCpp: f.endsWith('.cpp') });
  }
  // include closure per .cpp (quote includes resolved by basename; the build adds -Isrc, so a basename is enough)
  const incOf = (f) => (info.get(f)?.includes || []).map((r) => /["<]([^">]+)[">]/.exec(r)?.[1]).filter(Boolean).flatMap((p) => byBase.get(basename(p)) || []);
  const closure = (f) => { const seen = new Set([f]), q = [f]; while (q.length) for (const g of incOf(q.pop())) if (!seen.has(g)) { seen.add(g); q.push(g); } return seen; };

  const external = new Set(existsSync(join(root, 'tools/link-check.external')) ? readFileSync(join(root, 'tools/link-check.external'), 'utf8').split('\n').map((s) => s.replace(/#.*/, '').trim()).filter(Boolean) : []);
  const defs = new Map();   // key -> [{file,line,c,sig,what}]
  const defNames = new Map();   // name -> Set of linkages ('C' | 'X') for any def (header or cpp, inline or not)
  const addDefName = (name, c) => { (defNames.get(name) || defNames.set(name, new Set()).get(name)).add(c ? 'C' : 'X'); };
  for (const [f, fi] of info) {
    for (const it of fi.items) {
      if (it.kind !== 'def') continue;
      if (!it.ns) addDefName(it.name, it.c);
      if (it.internal) { if (!it.ns) addDefName(it.name, it.c); continue; }
      if (!fi.isCpp || it.weak) continue;   // a weak definition never collides
      const key = (it.c ? 'C ' : 'X ') + (it.ns ? it.ns + '::' : '') + it.name + (it.what === 'fn' && !it.c ? '(' + it.sig + ')' : '') + (it.what === 'var' ? '#v' : '');
      (defs.get(key) || defs.set(key, []).get(key)).push({ file: fi.rel, line: it.line, name: it.name });
    }
  }
  const findings = [];
  for (const [key, list] of defs) {
    const files2 = new Set(list.map((d) => d.file));
    if (files2.size < 2) continue;
    findings.push({ kind: 'LINK-DUP', name: list[0].name, where: [...new Map(list.map((d) => [d.file, d])).values()].map((d) => `${d.file}:${d.line}`), files: [...files2] });
  }
  if (undef) {
    const usedBy = new Map();   // name -> Set of .cpp files that use it in a body
    for (const [f, fi] of info) if (fi.isCpp) for (const u of fi.used) (usedBy.get(u) || usedBy.set(u, new Set()).get(u)).add(f);
    const reported = new Set();
    for (const [f, fi] of info) {
      if (!fi.isCpp) continue;
      for (const g of closure(f)) {
        const gi = info.get(g); if (!gi) continue;
        for (const it of gi.items) {
          if (it.kind !== 'decl' || it.internal || it.weak || it.ns || external.has(it.name)) continue;
          if (!fi.used.has(it.name)) continue;
          const D = defNames.get(it.name);
          const want = it.c ? 'C' : 'X';
          if (D && D.has(want)) continue;
          const k = `${f}|${it.name}|${want}`; if (reported.has(k)) continue; reported.add(k);
          if (!D && it.what === 'fn') continue;   // the link imports an undefined function (a runtime trap, not a link error); an undefined variable fails the link
          if (!D) findings.push({ kind: 'LINK-UNDEF', name: it.name, where: [`${gi.rel}:${it.line}`], files: [fi.rel, gi.rel], user: fi.rel });
          else findings.push({ kind: 'LINK-LINKAGE', name: it.name, where: [`${gi.rel}:${it.line}`], detail: `declared ${it.c ? 'extern "C"' : 'with C++ linkage'}, defined only ${it.c ? 'with C++ linkage' : 'extern "C"'}; used in ${fi.rel}`, files: [fi.rel, gi.rel], user: fi.rel });
        }
      }
    }
  }
  return { findings };
}

if (process.argv[1] && fileURLToPath(import.meta.url) === process.argv[1]) {
  const { findings } = check(ROOT, { undef: !argv.includes('--no-undef') });
  // tools/link-check.baseline lists the findings already on main ("KIND name" per line, # comments): they are reported
  // by --no-baseline and never block; a NEW finding does. --write-baseline prints the current set in that form.
  const bf = join(ROOT, 'tools/link-check.baseline');
  const base = new Set(!argv.includes('--no-baseline') && existsSync(bf) ? readFileSync(bf, 'utf8').split('\n').map((l) => l.replace(/#.*/, '').trim()).filter(Boolean) : []);
  if (argv.includes('--write-baseline')) { console.log([...new Set(findings.map((f) => f.kind + ' ' + f.name))].sort().join('\n')); process.exit(0); }
  const fresh = findings.filter((f) => !base.has(f.kind + ' ' + f.name));
  const sel = ONLY.length ? fresh.filter((f) => f.files.some((x) => ONLY.includes(x))) : fresh;
  // collapse the undefined/linkage reports of one name to the first user, so a header gives one line, not one per includer
  const seen = new Set(), outl = [];
  for (const f of sel) {
    const k = `${f.kind}|${f.name}|${f.where[0]}`; if (seen.has(k)) continue; seen.add(k);
    outl.push(f.kind === 'LINK-DUP' ? `LINK-DUP ${f.name} ${f.where.join(' ')}` : f.kind === 'LINK-LINKAGE' ? `LINK-LINKAGE ${f.name} ${f.where[0]} ${f.detail}` : `LINK-UNDEF ${f.name} ${f.where[0]} (used in ${f.user})`);
  }
  if (argv.includes('--json')) console.log(JSON.stringify(sel));
  else if (outl.length) console.log(outl.join('\n'));
  if (!argv.includes('--json')) console.error(`link-check: ${outl.length} finding(s)`);
  process.exit(outl.length ? 1 : 0);
}
