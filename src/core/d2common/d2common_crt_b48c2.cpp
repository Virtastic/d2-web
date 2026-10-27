// D2Common VC7.1 CRT glue, read against the cached retail 1.13c decompiles.
// Host-seam pattern as d2common_crt_tiny.cpp: Win32 calls and image globals are passed in.
// Not ported (host-only, no portable behaviour): __ValidateEH3RN 0x62AA (VirtualQuery/PE walk),
// ___crtGetEnvironmentStringsA 0x58C0 (GetEnvironmentStrings*/WideCharToMultiByte).
#include <cstdint>
#include <cstddef>

// 1.13c D2Common+0x059E2
extern "C" uint32_t d2common_heap_select(uint32_t os_platform, uint32_t os_major) {
    return (os_platform == 2 && 4 < os_major) ? 1 : 3;
}

// --- D2Common __sbh_* state (CRT small-block heap), file-local ---
static uint32_t g_sbh_entry;    // +0x1F4A8 DAT_6fdf14a8
static uint32_t g_sbh_count;    // +0x1F4AC DAT_6fdf14ac
static uint32_t g_sbh_regions;  // +0x1F4B0 DAT_6fdf14b0
static uint32_t g_sbh_max;      // +0x1F4B4 DAT_6fdf14b4
static uint32_t g_sbh_last;     // +0x1F4B8 DAT_6fdf14b8
static uint32_t g_sbh_cap;      // +0x1F4BC DAT_6fdf14bc
static uint32_t g_sbh_heap;     // +0x1F70C DAT_6fdf170c (Win32 heap handle)

// 1.13c D2Common+0x07CD9 ___sbh_heap_init: take the 0x140-byte region table from the Win32 heap;
// on failure return 0, else reset the sbh globals (max request = arg, capacity 0x10) and return 1.
extern "C" int32_t d2common_sbh_heap_init(uint32_t maximum, uint32_t (*heap_alloc)(uint32_t, uint32_t, uint32_t)) {
    uint32_t r = heap_alloc(g_sbh_heap, 0, 0x140);
    g_sbh_regions = r;
    if (r == 0) return 0;
    g_sbh_entry = 0; g_sbh_count = 0; g_sbh_last = r; g_sbh_max = maximum; g_sbh_cap = 0x10;
    return 1;
}

// Read-back of the sbh globals {entry,count,regions,max,last,cap} for the host test.
extern "C" void d2common_sbh_state(uint32_t out[6]) {
    out[0] = g_sbh_entry; out[1] = g_sbh_count; out[2] = g_sbh_regions;
    out[3] = g_sbh_max;   out[4] = g_sbh_last;  out[5] = g_sbh_cap;
}

struct D2CommonHeapHost {
    void*    heap;        // DAT_6fdf170c
    uint32_t heap_mode;   // DAT_6fdf1710
    uint32_t os_platform, os_major; // DAT_6fdef5a4 / DAT_6fdef5b0
    void* (*HeapCreate)(uint32_t flags, uint32_t initial, uint32_t max);
    void  (*HeapDestroy)(void* h);
    int   (*sbh_heap_init)(uint32_t threshold); // D2Common+0x7CD9
};

// 1.13c D2Common+0x059FC
extern "C" int d2common_heap_init(D2CommonHeapHost* h, int mt_flag) {
    h->heap = h->HeapCreate(mt_flag == 0, 0x1000, 0);
    if (!h->heap) return 0;
    h->heap_mode = d2common_heap_select(h->os_platform, h->os_major);
    if (h->heap_mode == 3 && h->sbh_heap_init(0x3f8) == 0) {
        h->HeapDestroy(h->heap);
        return 0;
    }
    return 1;
}

struct D2CommonHeapTermHost {
    void*    heap;            // DAT_6fdf170c
    uint32_t heap_mode;       // DAT_6fdf1710
    int32_t  region_count;    // DAT_6fdf14ac
    uint32_t* regions;        // DAT_6fdf14b0, 20-byte entries
    void (*VirtualFree)(void* p, uint32_t size, uint32_t type);
    void (*HeapFree)(void* heap, uint32_t flags, void* p);
    void (*HeapDestroy)(void* heap);
};

// 1.13c D2Common+0x05A4D
extern "C" void d2common_heap_term(D2CommonHeapTermHost* h) {
    if (h->heap_mode == 3) {
        // entry: +0xC = reserved base (void*), +0x10 = descriptor block (void*); stride 5 dwords
        uint8_t* e = (uint8_t*)h->regions + 0xc;
        for (int i = 0; i < h->region_count; i++, e += 20) {
            void* base = (void*)(uintptr_t)*(uint32_t*)e;
            h->VirtualFree(base, 0x100000, 0x4000);
            h->VirtualFree(base, 0, 0x8000);
            h->HeapFree(h->heap, 0, (void*)(uintptr_t)*(uint32_t*)(e + 4));
        }
        h->HeapFree(h->heap, 0, h->regions);
    }
    h->HeapDestroy(h->heap);
}

struct D2CommonMsizeHost {
    uint32_t heap_mode;                 // DAT_6fdf1710
    void*    heap;                      // DAT_6fdf170c
    void (*lock4)();                    // D2Common+0x2EF9 n=4
    void (*unlock4)();                  // D2Common+0x62A1
    void* (*sbh_find_block)(void* p);   // D2Common+0x7D21
    uint32_t (*HeapSize)(void* heap, uint32_t flags, void* p);
};

// 1.13c D2Common+0x06234
extern "C" uint32_t d2common_msize(D2CommonMsizeHost* h, void* p) {
    uint32_t r = 0;
    if (h->heap_mode == 3) {
        h->lock4();
        void* blk = h->sbh_find_block(p);
        if (blk) r = *(uint32_t*)((uint8_t*)p - 4) - 9;
        h->unlock4();
        if (blk) return r;
    }
    return h->HeapSize(h->heap, 0, p);
}

// 1.13c D2Common+0x06D60 (code page in EAX)
extern "C" uint32_t d2common_cp_to_lcid(int cp) {
    if (cp == 0x3a4) return 0x411;
    if (cp == 0x3a8) return 0x804;
    if (cp == 0x3b5) return 0x412;
    if (cp == 0x3b6) return 0x404;
    return 0;
}

struct D2CommonMbcState { // image globals cleared by setSBCS
    uint8_t  ctype[0x104]; // DAT_6fdf14e0 (0x40 dwords + 1 byte, padded)
    uint32_t g_15e4, g_14cc, g_14c4, g_15f0, g_15f4, g_15f8;
};

// 1.13c D2Common+0x06D8F
extern "C" void d2common_setSBCS(D2CommonMbcState* s) {
    for (int i = 0; i < 0x101; i++) s->ctype[i] = 0;
    s->g_15e4 = 0; s->g_14cc = 0; s->g_14c4 = 0;
    s->g_15f0 = 0; s->g_15f4 = 0; s->g_15f8 = 0;
}

// 1.13c D2Common+0x056B2: cmdline parser. ctype = DAT_6fdf14e0 (index c+1, bit 4 = lead byte).
// buf/argv may be null (counting pass).
extern "C" void d2common_parse_cmdline(const uint8_t* ctype, const uint8_t* p, void** argv,
                                       uint8_t* buf, int* numargs, int* numchars) {
    bool inq = false;
    *numchars = 0;
    *numargs = 1;
    if (argv) *argv++ = buf;
    uint8_t c;
    for (;;) {
        if (*p == 0x22) { inq = !inq; c = 0x22; p++; }
        else {
            ++*numchars;
            if (buf) *buf++ = *p;
            c = *p++;
            if (ctype[c + 1] & 4) { ++*numchars; if (buf) *buf++ = *p; p++; }
            if (c == 0) { p--; goto rest; }
        }
        if (!(inq || (c != 0x20 && c != 9))) break;
    }
    if (buf) buf[-1] = 0;
rest:
    inq = false;
    while (*p) {
        while (*p == 0x20 || *p == 9) p++;
        if (!*p) break;
        if (argv) *argv++ = buf;
        ++*numargs;
        for (;;) {
            bool copy = true;
            uint32_t bs = 0;
            while (*p == 0x5c) { p++; bs++; }
            if (*p == 0x22) {
                if (!(bs & 1)) {
                    if (inq && p[1] == 0x22) p++; else copy = false;
                    inq = !inq;
                }
                bs >>= 1;
            }
            while (bs--) { if (buf) *buf++ = 0x5c; ++*numchars; }
            c = *p;
            if (c == 0 || (!inq && (c == 0x20 || c == 9))) break;
            if (copy) {
                if (!buf) { if (ctype[c + 1] & 4) { p++; ++*numchars; } }
                else {
                    if (ctype[c + 1] & 4) { *buf++ = c; p++; ++*numchars; }
                    *buf++ = *p;
                }
                ++*numchars;
            }
            p++;
        }
        if (buf) *buf++ = 0;
        ++*numchars;
    }
    if (argv) *argv = nullptr;
    ++*numargs;
}

struct D2CommonSetargvHost {
    uint32_t mbc_inited;              // DAT_6fdf2858
    void (*initmbctable)();           // D2Common+0x7237
    uint32_t wildcard;                // DAT_6fdef884
    char*    module_path;             // DAT_6fdef780 (0x104 bytes)
    void (*GetModuleFileNameA)(char* buf, uint32_t n);
    const uint8_t* ctype;             // DAT_6fdf14e0
    void* (*malloc)(size_t n);        // D2Common+0x5F7F
    char**   prog_name_slot;          // _DAT_6fdef5d4
    int*     argc_slot;               // _DAT_6fdef5b8
    void***  argv_slot;               // _DAT_6fdef5bc
};

// 1.13c D2Common+0x0581E
extern "C" int d2common_setargv(D2CommonSetargvHost* h) {
    int nargs, nchars;
    if (h->mbc_inited == 0) h->initmbctable();
    h->wildcard = 0;
    h->GetModuleFileNameA(h->module_path, 0x104);
    *h->prog_name_slot = h->module_path;
    d2common_parse_cmdline(h->ctype, (const uint8_t*)h->module_path, nullptr, nullptr, &nargs, &nchars);
    void** argv = (void**)h->malloc((size_t)nchars + (size_t)nargs * sizeof(void*));
    if (!argv) return -1;
    // argv slots are pointer-sized here (4 bytes in the retail image); strings follow them.
    d2common_parse_cmdline(h->ctype, (const uint8_t*)h->module_path, argv,
                           (uint8_t*)argv + (size_t)nargs * sizeof(void*), &nargs, &nchars);
    *h->argc_slot = nargs - 1;
    *h->argv_slot = argv;
    return 0;
}

struct D2CommonRtlUnwindHost { void (*RtlUnwind)(void* frame, void* target_ip, void* rec, void* retval); void* target_ip; };

// 1.13c D2Common+0x05F94
extern "C" void d2common_global_unwind2(D2CommonRtlUnwindHost* h, void* frame) {
    h->RtlUnwind(frame, h->target_ip, nullptr, nullptr);
}

struct D2CommonXcptHost {
    uint32_t* table;          // ptd->_initaddr, 3 dwords per entry
    void*     initarg;        // ptd->_initarg
    uint32_t  actvar;         // ptd->_pxcptacttab
    uint32_t  count;          // DAT_6fde97a4
    uint32_t  fpe_first, fpe_count; // DAT_6fde9798 / DAT_6fde979c
    int  (*UnhandledExceptionFilter)(void* ep);
    void (*call_handler)(uint32_t handler, uint32_t sig, uint32_t sub, bool has_sub);
};

// 1.13c D2Common+0x05B0D
extern "C" int d2common_xcpt_filter(D2CommonXcptHost* h, uint32_t num, void* ep) {
    uint32_t* e = h->table;
    uint32_t* end = e + h->count * 3;
    for (; e < end; e += 3) if (*e == num) break;
    if (e >= end || *e != num) e = nullptr;
    if (!e || e[2] == 0) return h->UnhandledExceptionFilter(ep);
    uint32_t act = e[2];
    if (act == 5) { e[2] = 0; return 1; }
    if (act != 1) {
        void* saved = h->initarg;
        h->initarg = ep;
        if (e[1] == 8) {
            for (uint32_t i = h->fpe_first; i < h->fpe_count + h->fpe_first; i++) h->table[i * 3 + 2] = 0;
            uint32_t savedv = h->actvar;
            switch (*e) {
                case 0xc000008e: h->actvar = 0x83; break;
                case 0xc0000090: h->actvar = 0x81; break;
                case 0xc0000091: h->actvar = 0x84; break;
                case 0xc0000093: h->actvar = 0x85; break;
                case 0xc000008d: h->actvar = 0x82; break;
                case 0xc000008f: h->actvar = 0x86; break;
                case 0xc0000092: h->actvar = 0x8a; break;
            }
            h->call_handler(act, 8, h->actvar, true);
            h->actvar = savedv;
        } else {
            e[2] = 0;
            h->call_handler(act, e[1], 0, false);
        }
        h->initarg = saved;
    }
    return -1;
}

// 1.13c D2Common+0x07D21 ___sbh_find_block: regions = DAT_6fdf14b0, count = DAT_6fdf14ac; 20-byte
// entries, reserved base at +0xC. Returns the entry whose 1 MiB span holds p, else null.
extern "C" uint8_t* d2common_sbh_find_block(uint8_t* regions, uint32_t count, uint32_t p) {
    for (uint8_t* e = regions; e < regions + count * 0x14; e += 0x14)
        if (p - *(uint32_t*)(e + 0xc) < 0x100000) return e;
    return nullptr;
}
