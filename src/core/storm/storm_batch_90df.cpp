// Storm VC7.1 Release CRT leaves (0x90DF-0x9D60). Retail globals, callees and kernel32 calls are seams
// after the retail argument order; 32-bit links are uint32_t, memory goes through StormMem.
#include <cstdint>

struct StormMem { uint32_t (*rd32)(uint32_t); void (*wr32)(uint32_t, uint32_t); };

// 1.13c STORM+0x90DF __lock: lock table DAT_6fc402c0, 8-byte entries {cs ptr, flag}; lazily init then enter.
void storm_lock(const StormMem& m, uint32_t table, int32_t id, int32_t (*mtinitlocknum)(int32_t),
                void (*amsg_exit)(int32_t), void (*enter_cs)(uint32_t)) {
    if (m.rd32(table + id * 8) == 0 && mtinitlocknum(id) == 0) amsg_exit(0x11);
    enter_cs(m.rd32(table + id * 8));
}

// 1.13c STORM+0x9110 _strcmp: -1/0/1 on first differing unsigned byte.
int32_t storm_strcmp(const uint8_t* a, const uint8_t* b) {
    for (;; ++a, ++b) {
        if (*a != *b) return *a < *b ? -1 : 1;
        if (!*a) return 0;
    }
}

// 1.13c STORM+0x946D __ioterm: for each of 64 stdio block pointers (DAT_6fc47ba0..), delete the live
// critical sections (0x24-byte entries, flag at +8, CS at +0xC; 0x480 bytes), free the block, zero the slot.
void storm_ioterm(const StormMem& m, uint32_t slots, void (*delete_cs)(uint32_t), void (*free_)(uint32_t)) {
    for (uint32_t s = slots; s < slots + 0x100; s += 4) {
        uint32_t base = m.rd32(s);
        if (!base) continue;
        for (uint32_t e = base; e < base + 0x480; e += 0x24)
            if (m.rd32(e + 8)) delete_cs(e + 0xc);
        free_(m.rd32(s));
        m.wr32(s, 0);
    }
}

// 1.13c STORM+0x94B9 __setenvp: build the char* environ array from the "\0\0"-terminated block (DAT_6fc42468).
// Returns 0 / -1; envp_out = DAT_6fc424bc. Entries starting '=' are skipped.
int32_t storm_setenvp(const StormMem& m, uint32_t* block, uint32_t* envp_out, uint32_t* env_inited,
                      int32_t mbc_inited, void (*initmbctable)(), uint32_t (*malloc_)(uint32_t),
                      void (*free_)(uint32_t), uint32_t (*strlen_)(uint32_t), void (*strcpy_)(uint32_t, uint32_t),
                      uint8_t (*rd8)(uint32_t)) {
    if (!mbc_inited) initmbctable();
    if (!*block) return -1;
    int32_t n = 0;
    for (uint32_t p = *block; rd8(p); p += strlen_(p) + 1) if (rd8(p) != '=') ++n;
    uint32_t arr = malloc_(n * 4 + 4);
    *envp_out = arr;
    if (!arr) return -1;
    for (uint32_t p = *block, o = arr;; ) {
        if (!rd8(p)) {
            free_(*block); *block = 0; m.wr32(o, 0); *env_inited = 1; return 0;
        }
        uint32_t len = strlen_(p);
        if (rd8(p) != '=') {
            uint32_t s = malloc_(len + 1);
            m.wr32(o, s);
            if (!s) { free_(*envp_out); *envp_out = 0; return -1; }
            strcpy_(s, p);
            o += 4;
        }
        p += len + 1;
    }
}

// 1.13c STORM+0x96EC __setargv: two-pass parse_cmdline (0x9580): count, malloc, fill. Returns 0 / -1.
// parse(argv_or_0, &nargs_and_bytes): fills *argc and *bytes (retail in_ECX).
int32_t storm_setargv(int32_t mbc_inited, void (*initmbctable)(), uint32_t progname_buf, uint32_t* pgmptr,
                      void (*get_module_file)(uint32_t), void (*parse)(uint32_t, int32_t*, int32_t*),
                      uint32_t (*malloc_)(uint32_t), int32_t* argc_out, uint32_t* argv_out) {
    if (!mbc_inited) initmbctable();
    get_module_file(progname_buf);
    *pgmptr = progname_buf;
    int32_t argc = 0, bytes = 0;
    parse(0, &argc, &bytes);
    uint32_t argv = malloc_(bytes + argc * 4);
    if (!argv) return -1;
    parse(argv, &argc, &bytes);
    *argc_out = argc - 1;
    *argv_out = argv;
    return 0;
}

// 1.13c STORM+0x978E ___crtGetEnvironmentStringsA: wide path (mode 0/1) converts via WideCharToMultiByte,
// ANSI path (mode 2) copies. *mode = DAT_6fc42738. Returns 0 on failure, else malloc'd block (ours: 32-bit link).
struct EnvSeams {
    uint32_t (*get_w)();                       // GetEnvironmentStringsW
    uint32_t (*get_a)();                       // GetEnvironmentStrings
    uint32_t (*last_error)();
    int32_t (*w2mb)(uint32_t w, int32_t wn, uint32_t out, int32_t outn);
    uint32_t (*malloc_)(uint32_t);
    void (*free_)(uint32_t);
    void (*memcpy_)(uint32_t, uint32_t, uint32_t);
    void (*free_w)(uint32_t);
    void (*free_a)(uint32_t);
    uint16_t (*rd16)(uint32_t);
    uint8_t (*rd8)(uint32_t);
};
uint32_t storm_crt_get_environment_strings_a(int32_t* mode, const EnvSeams& s) {
    uint32_t w = 0;
    if (*mode == 0) {
        w = s.get_w();
        if (w) *mode = 1;
        else if (s.last_error() == 0x78) *mode = 2;
    }
    if (*mode != 1) {
        if (*mode != 2 && *mode != 0) return 0;
        uint32_t src = s.get_a();
        if (!src) return 0;
        uint32_t p = src;
        while (s.rd8(p)) { while (s.rd8(p + 1)) ++p; p += 2; }
        uint32_t n = p + 1 - src, dst = s.malloc_(n);
        if (dst) s.memcpy_(dst, src, n);
        s.free_a(src);
        return dst;
    }
    if (!w && !(w = s.get_w())) return 0;
    uint32_t p = w;
    while (s.rd16(p)) { while (s.rd16(p + 2)) p += 2; p += 4; }
    int32_t wn = (int32_t)((p - w) >> 1) + 1;
    int32_t sz = s.w2mb(w, wn, 0, 0);
    uint32_t out = 0;
    if (sz) {
        uint32_t buf = s.malloc_(sz);
        if (buf) {
            out = buf;
            if (!s.w2mb(w, wn, buf, sz)) { s.free_(buf); out = 0; }
        }
    }
    s.free_w(w);
    return out;
}

// 1.13c STORM+0x98B0 __RTC_Initialize: the table (DAT_6fc3b750..same) is empty in this image; loop never runs.
void storm_rtc_initialize(void (**begin)(), void (**end)()) {
    for (; begin < end; ++begin) if (*begin) (*begin)();
}

// 1.13c STORM+0x9938 ___heap_select: 1 (system heap) when OS platform==2 and major>4, else 3 (small-block).
int32_t storm_heap_select(int32_t platform, int32_t winmajor) { return (platform == 2 && winmajor > 4) ? 1 : 3; }

// 1.13c STORM+0x9952 __heap_init(mt): HeapCreate(mt==0, 0x1000, 0); select heap type; init SBH (0x3f8) if 3.
int32_t storm_heap_init(int32_t mt, uint32_t* heap, int32_t* heap_type, int32_t platform, int32_t winmajor,
                        uint32_t (*heap_create)(uint32_t, uint32_t, uint32_t), void (*heap_destroy)(uint32_t),
                        int32_t (*sbh_heap_init)(int32_t)) {
    *heap = heap_create(mt == 0, 0x1000, 0);
    if (!*heap) return 0;
    *heap_type = storm_heap_select(platform, winmajor);
    if (*heap_type == 3 && !sbh_heap_init(0x3f8)) { heap_destroy(*heap); return 0; }
    return 1;
}

// 1.13c STORM+0x99A3 __heap_term: for type 3 release every SBH region (20-byte entries at DAT_6fc4690c; +0xC
// base, +0x10 heap block), free the table; always HeapDestroy.
void storm_heap_term(const StormMem& m, int32_t heap_type, uint32_t heap, int32_t nregions, uint32_t regions,
                     void (*virtual_free)(uint32_t, uint32_t, uint32_t), void (*heap_free)(uint32_t, uint32_t, uint32_t),
                     void (*heap_destroy)(uint32_t)) {
    if (heap_type == 3) {
        uint32_t p = regions + 0xc;
        for (int32_t i = 0; i < nregions; ++i, p += 20) {
            virtual_free(m.rd32(p), 0x100000, 0x4000);
            virtual_free(m.rd32(p), 0, 0x8000);
            heap_free(heap, 0, m.rd32(p + 4));
        }
        heap_free(heap, 0, regions);
    }
    heap_destroy(heap);
}

// 1.13c STORM+0x9D19 __FF_MSGBANNER: when the error-mode globals ask for it, emit banner R6xxx pieces
// (0xFC, optional hook, 0xFF) via _NMSG_WRITE (0x9BA1).
void storm_ff_msgbanner(int32_t errmode, int32_t app_type, void (*nmsg_write)(int32_t), void (*hook)()) {
    if (errmode == 1 || (errmode == 0 && app_type == 1)) {
        nmsg_write(0xfc);
        if (hook) hook();
        nmsg_write(0xff);
    }
}

// 1.13c STORM+0x9D60 ___ascii_stricmp: ASCII case-insensitive compare, -1/0/1.
int32_t storm_ascii_stricmp(const uint8_t* a, const uint8_t* b) {
    for (;; ++a, ++b) {
        uint8_t x = *a, y = *b;
        if (x != y) {
            if (x - 'A' < 26u) x += 32;
            if (y - 'A' < 26u) y += 32;
            if (x != y) return x < y ? -1 : 1;
        }
        if (!*b) return 0;
    }
}
