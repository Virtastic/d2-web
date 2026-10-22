// D2Lang batch b69. Skipped (1D6C: 32-bit locinfo layout; SEH/direct-Win32/CRT-internal): 24B3 1B55 55D5 5A1F 5C86 3C7D 25CD 2215.
#include <cstdint>
#include <cstring>
#include <initializer_list>

struct CrtFile { char* _ptr; int _cnt; char* _base; int _flag; };  // MSVC 7.1 FILE prefix
extern "C" unsigned d2lang_flsbuf(int ch, CrtFile* f);  // D2Lang+0x272E __flsbuf
extern "C" void d2lang_free(void* p);                   // CRT _free
extern "C" int** d2lang_getptd_locinfo_slot(void);      // &__getptd()->ptlocinfo
extern "C" int d2lang_wctomb_mt(int* loc, char* mb, uint16_t wc);  // D2Lang+0x5F00 ___wctomb_mt
extern "C" int* d2lang_updatetlocinfo(void);            // D2Lang+0x1E2D ___updatetlocinfo

static int* g_ptlocinfoDefault;   // D2Lang PTR_DAT_6fc0f0ac
static char** g_lconvC;           // D2Lang PTR_PTR_6fc0f55c (C-locale lconv)
static char* g_lconvDef[7];       // D2Lang PTR_DAT_6fc0f538..550

// 1.13c D2Lang+0x2847: AL=ch, ECX=FILE*, ESI=&charsWritten (hidden registers) -> explicit params.
void d2lang_write_char(uint8_t ch, CrtFile* f, int* written)
{
    if ((f->_flag & 0x40) == 0 || f->_base != nullptr) {
        unsigned r;
        if (--f->_cnt < 0) r = d2lang_flsbuf((char)ch, f);
        else { *f->_ptr++ = (char)ch; r = ch; }
        if (r == 0xffffffffu) { *written = -1; return; }
    }
    ++*written;
}

// 1.13c D2Lang+0x287A: EAX=&charsWritten (hidden), ch/ECX/ESI forwarded to write_char.
void d2lang_write_multi_char(int* written, uint8_t ch, CrtFile* f, int count)
{
    while (count > 0) {
        --count;
        d2lang_write_char(ch, f, written);
        if (*written == -1) return;
    }
}

// 1.13c D2Lang+0x289E: EAX=&charsWritten, EDI=FILE* (hidden registers) -> explicit params.
void d2lang_write_string(int* written, CrtFile* f, const char* s, int count)
{
    if ((f->_flag & 0x40) == 0 || f->_base != nullptr) {
        while (count > 0) {
            --count;
            d2lang_write_char((uint8_t)*s++, f, written);
            if (*written == -1) return;
        }
    } else {
        *written += count;
    }
}

// 1.13c D2Lang+0x4026
void d2lang_free_lconv_mon(char** lc)
{
    if (!lc) return;
    for (int i = 0; i < 7; i++) {
        char* p = lc[3 + i];
        if (p != g_lconvC[3 + i] && p != g_lconvDef[i]) d2lang_free(p);
    }
}

// 1.13c D2Lang+0x5F60
int d2lang_wctomb(char* mb, uint16_t wc)
{
    int* li = *d2lang_getptd_locinfo_slot();  // ptd->ptlocinfo
    if (li != g_ptlocinfoDefault) li = d2lang_updatetlocinfo();
    return d2lang_wctomb_mt(li, mb, wc);
}
