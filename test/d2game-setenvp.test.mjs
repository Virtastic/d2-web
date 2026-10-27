// D2Game+0x6339 __setenvp: host-compiled check (skips '=' entries, copies, frees block, NULL-terminates).
import { execFileSync } from 'node:child_process';
import { mkdtempSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';
const d = mkdtempSync(join(tmpdir(), 'setenvp-'));
const t = join(d, 't.cpp');
writeFileSync(t, `
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cstddef>
struct S{uint32_t r;char*b;char**e;uint32_t i;void(*m)();void*(*a)(uint32_t,size_t);void(*f)(void*);};
extern "C" int32_t d2game_setenvp(S*);
static int inits=0,frees=0;
int main(){
  const char src[]="A=1\\0=C:=x\\0B=2\\0";
  char*blk=(char*)malloc(sizeof src);memcpy(blk,src,sizeof src);
  S s{0,blk,0,0,[]{inits++;},[](uint32_t,size_t n){return malloc(n);},[](void*p){frees++;free(p);}};
  assert(d2game_setenvp(&s)==0&&inits==1&&frees==1&&!s.b&&s.i==1);
  assert(!strcmp(s.e[0],"A=1")&&!strcmp(s.e[1],"B=2")&&!s.e[2]);
  S n{1,0,0,0,0,0,0}; assert(d2game_setenvp(&n)==-1);
  return 0;}`);
execFileSync('g++', ['-std=c++17', t, 'src/core/d2game/setenvp_6339.cpp', '-o', join(d, 't')]);
execFileSync(join(d, 't'));
console.log('ok');
