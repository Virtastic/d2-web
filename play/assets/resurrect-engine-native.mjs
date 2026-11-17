// UNIT-ONLY: the Resurrect row's insert (src/core/d2client/npc_menu_resurrect.cpp) compiled natively with g++, in the shape the shell
// installs from the wasm export. Returns the engine function, or null when no compiler is present.
import { execFileSync } from 'node:child_process';
import { mkdtempSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join, dirname } from 'node:path';
import { fileURLToPath } from 'node:url';

export function nativeResurrectEngine() {
  const src = join(dirname(fileURLToPath(import.meta.url)), '../../src/core/d2client/npc_menu_resurrect.cpp');
  const dir = mkdtempSync(join(tmpdir(), 'resur-'));
  writeFileSync(join(dir, 'main.cpp'), `#include "${src}"
#include <cstdio>
#include <cstdlib>
int main(int, char** v){unsigned a[5]={0,0,0,0,0};unsigned n=strtoul(v[1],0,10);for(int i=0;i<5;i++)a[i]=strtoul(v[2+i],0,10);printf("%d",d2_client_npc_menu_resurrect_at(n,a[0],a[1],a[2],a[3],a[4]));}`);
  try { execFileSync('g++', ['-o', join(dir, 'm'), join(dir, 'main.cpp')]); } catch { return null; }
  return (hs) => Number(execFileSync(join(dir, 'm'), [hs.length, ...Array.from({ length: 5 }, (_, i) => hs[i] ?? 0)].map(String)));
}
