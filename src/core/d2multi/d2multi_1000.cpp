// 1.13c D2Multi+0x1000 -- _sprintf (MSVC 2003 CRT).
// Builds a string FILE over dest (_ptr = _base = dest, _cnt = 0x7fffffff, _flag = 0x42), runs the shared formatter
// (_output, 0x2334) over the argument words, then NUL-terminates when dest is non-null (--_cnt, '\0' at _ptr, or
// __flsbuf(0) when _cnt went negative). Returns the formatter's count. The FILE's write_char is retail 0x22A6:
// a string FILE with a null _base swallows the bytes and only counts them.
#include <cstdint>
#include "d2multi_out_env.h"

struct D2MultiStream32 { uint32_t ptr; int32_t cnt; uint32_t base; uint32_t flag; int32_t file; uint32_t charbuf; int32_t bufsiz; uint32_t tmpfname; };
extern "C" int32_t d2multi_flsbuf_env(int32_t ch, D2MultiStream32* f);

namespace {
void file_char(d2multi_out_env* e, int ch) {                  // 0x22A6
    D2MultiStream32* f = (D2MultiStream32*)e->ctx;
    if ((f->flag & 0x40) == 0 || f->base != 0) {
        int32_t r;
        if (--f->cnt < 0) r = d2multi_flsbuf_env((int8_t)ch, f);
        else { *(uint8_t*)e->addr(f->ptr) = (uint8_t)ch; f->ptr += 1; r = ch & 0xff; }
        if (r == -1) { *e->count = -1; return; }
    }
    *e->count += 1;
}
void file_multi(d2multi_out_env* e, int ch, int n) {          // 0x22D9
    while (n-- > 0) { file_char(e, ch); if (*e->count == -1) return; }
}
void file_string(d2multi_out_env* e, const char* p, int n) {  // 0x22FD
    D2MultiStream32* f = (D2MultiStream32*)e->ctx;
    if ((f->flag & 0x40) != 0 && f->base == 0) { *e->count += n; return; }
    while (n-- > 0) { file_char(e, (uint8_t)*p++); if (*e->count == -1) return; }
}
}  // namespace

// tmpl supplies the formatter's host callbacks (addr, wctomb, float hooks, mbctype, null strings, udivrem);
// the port fills the stream, the count and the FILE sinks. dest is a retail 32-bit address (0 = count only).
extern "C" int32_t d2multi_sprintf_1000(const d2multi_out_env* tmpl, uint32_t dest, const uint8_t* fmt, const uint32_t* va) {
    D2MultiStream32 f{};
    f.cnt = 0x7fffffff;
    f.flag = 0x42;
    f.base = dest;
    f.ptr = dest;
    int written = 0;
    d2multi_out_env e = *tmpl;
    e.ctx = &f;
    e.count = &written;
    e.write_char = file_char;
    e.write_multi_char = file_multi;
    e.write_string = file_string;
    int32_t n = d2multi_output(&e, fmt, va);
    if (dest != 0) {
        if (--f.cnt < 0) d2multi_flsbuf_env(0, &f);
        else *(uint8_t*)e.addr(f.ptr) = 0;
    }
    return n;
}
