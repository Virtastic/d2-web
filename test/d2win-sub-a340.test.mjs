import test from 'node:test';
import assert from 'node:assert/strict';
import { execFileSync } from 'node:child_process';
import { mkdtempSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';

// D2Win+0xA340 releases each handle box: Storm close, *box = 0, Fog free OF THE BOX, global null.
test('d2win_sub_A340 frees the box it closed, in retail order', () => {
  const d = mkdtempSync(join(tmpdir(), 'a340-'));
  writeFileSync(join(d, 'emscripten.h'), '#define EMSCRIPTEN_KEEPALIVE\n');
  writeFileSync(join(d, 't.cpp'), `
#include <cstdio>
#include <cstdint>
extern "C" { extern void* d2win_g_6f9a9cd8; extern void* d2win_g_6f9a9cc0; extern void* d2win_g_6f9a9cc4;
 extern void* d2win_g_6f9a9cb4; extern void* d2win_g_6f9a9cc8; extern void* d2win_g_6f9a9cdc; void d2win_sub_A340(); }
static int nf; static void* fr[6]; static int nc;
extern "C" void Storm_Close252(uint32_t){ nc++; }
extern "C" void Fog_Free10043(void* p,int,int){ fr[nf++]=p; }
int main(){ uint32_t b[5]={1,2,3,4,5};
 d2win_g_6f9a9cd8=&b[0]; d2win_g_6f9a9cc0=&b[1]; d2win_g_6f9a9cc4=nullptr;
 d2win_g_6f9a9cb4=&b[2]; d2win_g_6f9a9cc8=&b[3]; d2win_g_6f9a9cdc=&b[4];
 d2win_sub_A340();
 int ok = nf==5 && nc==5 && fr[0]==&b[0] && fr[1]==&b[1] && fr[2]==&b[2] && fr[3]==&b[3] && fr[4]==&b[4]
  && !b[0] && !d2win_g_6f9a9cd8 && !d2win_g_6f9a9cdc;
 puts(ok?"OK":"BAD"); }
`);
  execFileSync('g++', ['-fpermissive', '-w', '-I', d, '-o', join(d, 't'), join(d, 't.cpp'),
    'src/core/gfx/d2win_batch_93c0.cpp'], { stdio: 'pipe' });
  assert.equal(execFileSync(join(d, 't')).toString().trim(), 'OK');
});
