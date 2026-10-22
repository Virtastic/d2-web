// D2Common VC7.1 CRT leaves, read against the cached retail 1.13c decompiles.
// Host-seam pattern as d2gfx_crt_tiny.cpp: the callee/global is passed in, not dropped.
#include <cstdint>

struct D2CommonCrtTinyHost {
    void* (*nh_malloc)(uint32_t size, uint32_t newmode); // D2Common+0x5F53
    void (*report_failure)();                            // D2Common+0x6BDB
    uint32_t newmode;      // image global read by _malloc (DAT_6fdef968)
    uint32_t stack_cookie; // image global (DAT_6fde9950)
};

// 1.13c D2Common+0x05F7F
extern "C" void* d2common_malloc(D2CommonCrtTinyHost* host, uint32_t size) {
    return host->nh_malloc(size, host->newmode);
}

// 1.13c D2Common+0x06C0C
extern "C" void d2common_security_check_cookie(D2CommonCrtTinyHost* host, uint32_t cookie) {
    if (cookie != host->stack_cookie) host->report_failure();
}

struct D2CommonFreeHost {
    uint32_t heap_mode;                       // DAT_6fdf1710 (3 = small-block heap active)
    void (*lock4)();                          // D2Common+0x2EF9 with n=4
    void* (*sbh_find_block)(void* p);         // D2Common+0x7D4C
    void (*sbh_free_block)(void* hdr, void* p); // D2Common+0x7D21
    void (*unlock4)();                        // D2Common+0x4C60
    void (*heap_free)(void* p);               // HeapFree(DAT_6fdf170c, 0, p)
};

// 1.13c D2Common+0x04C0D
extern "C" void d2common_free(D2CommonFreeHost* h, void* p) {
    if (!p) return;
    if (h->heap_mode == 3) {
        h->lock4();
        void* hdr = h->sbh_find_block(p);
        if (hdr) h->sbh_free_block(hdr, p);
        h->unlock4();
        if (hdr) return;
    }
    h->heap_free(p);
}

extern "C" void D2Common_8C7A_SecurityFailure(int32_t nCode); // D2Common+0x8C7A, defined in d2common_sub_8c7a.cpp

// 1.13c D2Common+0x06BDB
// _report_failure(): report code 1 through 0x8C7A, then ExitProcess(3) (0x8C7A already exits with 3; never returns).
extern "C" void D2Common_6BDB_ReportFailure() {
    D2Common_8C7A_SecurityFailure(1);
}
