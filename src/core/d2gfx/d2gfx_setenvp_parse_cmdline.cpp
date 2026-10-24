// 1.13c D2Gfx+0x02F66 __setenvp and D2Gfx+0x0302D parse_cmdline (MSVC 2003 CRT startup).
// Globals and callees are explicit seams: 0x28B3 _malloc, 0x2BD4 _free, 0x4150 _strlen,
// 0x58F0 _strcpy, 0x34DE-class ___initmbctable.
#include <cstdint>
#include <cstddef>

struct D2GfxSetenvpState {
    int32_t mbctable_inited;   // DAT_6fa9ea34
    char*   env_block;         // DAT_6fa90dec: double-NUL-terminated "NAME=val\0...\0"
    char**  envp;              // DAT_6fa90dc4
    int32_t env_ready;         // DAT_6fa9ea28
};
struct D2GfxSetenvpHost {
    void* (*malloc)(size_t); void (*free)(void*); size_t (*strlen)(const char*);
    void (*strcpy)(char*, const char*); void (*init_mbctable)();
};

// 1.13c D2Gfx+0x02F66
extern "C" int32_t d2gfx_setenvp_2f66(D2GfxSetenvpState* s, const D2GfxSetenvpHost* h) {
    if (s->mbctable_inited == 0) h->init_mbctable();
    int32_t n = 0;
    char* p = s->env_block;
    if (!p) return -1;
    for (; *p; p += h->strlen(p) + 1)
        if (*p != '=') ++n;
    char** out = static_cast<char**>(h->malloc(size_t(n) * sizeof(char*) + sizeof(char*)));
    s->envp = out;
    if (!out) return -1;
    for (p = s->env_block;;) {
        if (*p == 0) {
            h->free(s->env_block);
            s->env_block = nullptr;
            *out = nullptr;
            s->env_ready = 1;
            return 0;
        }
        size_t len = h->strlen(p);
        if (*p != '=') {
            char* d = static_cast<char*>(h->malloc(len + 1));
            *out = d;
            if (!d) { h->free(s->envp); s->envp = nullptr; return -1; }
            h->strcpy(d, p);
            ++out;
        }
        p += len + 1;
    }
}

// 1.13c D2Gfx+0x0302D parse_cmdline. Retail registers: EAX=cmdline, ECX=args buffer, ESI=numchars;
// stack: argv, numargs. mbctype = lead-byte table (retail _mbctype+1, bit 4 = lead byte).
extern "C" void d2gfx_parse_cmdline_302d(const uint8_t* p, uint8_t* buf, int32_t* numchars,
                                         char** argv, int32_t* numargs, const uint8_t* mbctype) {
    bool inq = false;
    *numchars = 0;
    *numargs = 1;
    if (argv) *argv++ = reinterpret_cast<char*>(buf);
    uint8_t c;
    for (;;) {  // program name: quotes toggle, no escapes
        if (*p == '"') { inq = !inq; c = '"'; ++p; }
        else {
            ++*numchars;
            if (buf) *buf++ = *p;
            c = *p++;
            if (mbctype[c] & 4) {
                ++*numchars;
                if (buf) *buf++ = *p;
                ++p;
            }
            if (c == 0) { --p; goto args; }
        }
        if (!inq && (c == ' ' || c == '\t')) break;
    }
    if (buf) buf[-1] = 0;
args:
    inq = false;
    while (*p) {
        while (*p == ' ' || *p == '\t') ++p;
        if (*p == 0) break;
        if (argv) *argv++ = reinterpret_cast<char*>(buf);
        ++*numargs;
        for (;;) {
            bool copy = true;
            uint32_t bs = 0;
            while (*p == '\\') { ++p; ++bs; }
            if (*p == '"') {
                if ((bs & 1) == 0) {
                    if (!inq || p[1] != '"') copy = false; else ++p;
                    inq = !inq;
                }
                bs >>= 1;
            }
            for (; bs; --bs) { if (buf) *buf++ = '\\'; ++*numchars; }
            c = *p;
            if (c == 0 || (!inq && (c == ' ' || c == '\t'))) break;
            if (copy) {
                if (buf) {
                    if (mbctype[c] & 4) { *buf++ = c; ++p; ++*numchars; }
                    *buf++ = *p;
                } else if (mbctype[c] & 4) { ++p; ++*numchars; }
                ++*numchars;
            }
            ++p;
        }
        if (buf) *buf++ = 0;
        ++*numchars;
    }
    if (argv) *argv = nullptr;
    ++*numargs;
}
