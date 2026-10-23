// D2Win CRT leaf functions (batch b55-win-a). Globals/allocators are seams.
#include <cstddef>
#include <cstdint>

struct d2win_b55a_File { char* _ptr; int _cnt; char* _base; int _flag; int _file; int _charbuf; int _bufsiz; char* _tmpfname; };

struct d2win_b55a_Env {
    uint32_t  nhandles;                 // DAT_6f9abb2c
    uint8_t** handleTable;              // DAT_6f9abb40, 32 entries of 0x24 bytes per block
    void*   (*nhMalloc)(size_t);        // __nh_malloc(size, DAT_6f901398)
    int       getbufCount;              // DAT_6f9012c0
    uintptr_t cookie;                   // DAT_6f8ff6f0
    void    (*reportFailure)();
};
static d2win_b55a_Env g_d2win_b55a_env;
void d2win_b55a_SetEnv(const d2win_b55a_Env& e) { g_d2win_b55a_env = e; }

// 1.13c D2Win+0x3eb0
size_t d2win_b55a_strlen(const char* s) { size_t n = 0; while (s[n]) n++; return n; }

// 1.13c D2Win+0x3FE2
void* d2win_b55a_malloc(size_t size) { return g_d2win_b55a_env.nhMalloc(size); }

// 1.13c D2Win+0x03d17
int d2win_b55a_isatty(int fh) {
    if (g_d2win_b55a_env.nhandles <= (uint32_t)fh) return 0;
    return (int)(char)g_d2win_b55a_env.handleTable[(uint32_t)fh >> 5][4 + ((uint32_t)fh & 0x1f) * 0x24] & 0x40;
}

// 1.13c D2Win+0x3CD3
void d2win_b55a_getbuf(d2win_b55a_File* f) {
    g_d2win_b55a_env.getbufCount++;
    char* p = (char*)d2win_b55a_malloc(0x1000);
    f->_base = p;
    if (!p) { f->_flag |= 4; f->_base = (char*)&f->_charbuf; f->_bufsiz = 2; }
    else    { f->_flag |= 8; f->_bufsiz = 0x1000; }
    f->_cnt = 0;
    f->_ptr = f->_base;
}

// 1.13c D2Win+0x40ac
void d2win_b55a_security_check_cookie(uintptr_t c) {
    if (c == g_d2win_b55a_env.cookie) return;
    g_d2win_b55a_env.reportFailure();
}
