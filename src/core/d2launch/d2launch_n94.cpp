// D2Launch n94: CRT leaves with seams for malloc/_unlock_fhandle.
#include <cstdint>
#include <cstdlib>

struct CrtFile94 { char* ptr; int32_t cnt; char* base; uint32_t flag; int32_t file; int32_t charbuf; int32_t bufsiz; char* tmpfname; };

uint32_t g_getbufCount94;                                  // DAT_6fa64f90
void (*g_unlock_fhandle94)(int32_t) = nullptr;             // seam: __unlock_fhandle (0x6B05)

// 1.13c D2LAUNCH+0x058E3
void d2launch_getbuf(CrtFile94* f) {
  g_getbufCount94++;
  f->base = (char*)malloc(0x1000);
  if (!f->base) {
    f->flag |= 4;
    f->base = (char*)&f->charbuf;
    f->bufsiz = 2;
  } else {
    f->flag |= 8;
    f->bufsiz = 0x1000;
  }
  f->cnt = 0;
  f->ptr = f->base;
}

// 1.13c D2LAUNCH+0x058BF  (EBX = fh)
void d2launch_sub_58bf(int32_t fh) { if (g_unlock_fhandle94) g_unlock_fhandle94(fh); }

// 1.13c D2LAUNCH+0x07866  (EBX = fh)
void d2launch_sub_7866(int32_t fh) { if (g_unlock_fhandle94) g_unlock_fhandle94(fh); }
