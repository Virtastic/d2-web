// D2Gfx VC7.1 CRT leaves, read against the cached retail 1.13c decompiles.
// Prior art: D2MOO D2Gfx/DllMain.cpp has the user entry, not these CRT leaves.
// The existing D2Gfx CRT host-seam pattern preserves calls and image globals
// explicitly; callbacks execute the effects rather than recording or dropping them.
#include <cstdint>

struct D2GfxCrtTinyHost {
    void (*doexit)(uint32_t code, uint32_t quick, uint32_t retcaller);
    void (*unlock)(int32_t locknum);
    int32_t (*setmbcp)(int32_t codepage);
    void (*report_failure)();
    uint32_t mbctable_initialized; // image RVA 0x1EA34
    uint32_t stack_cookie;         // image RVA 0x106E0
};

// 1.13c D2Gfx+0x0151C
extern "C" void d2gfx_exit(D2GfxCrtTinyHost* host, uint32_t code) {
    host->doexit(code, 1, 0);
}

// 1.13c D2Gfx+0x01A68
extern "C" void d2gfx_unlock_10(D2GfxCrtTinyHost* host) {
    host->unlock(10);
}

// 1.13c D2Gfx+0x05870
extern "C" int32_t d2gfx_initmbctable(D2GfxCrtTinyHost* host) {
    if (host->mbctable_initialized == 0) {
        host->setmbcp(-3);
        // Retail marks initialized even when __setmbcp reports failure.
        host->mbctable_initialized = 1;
    }
    return 0;
}

// 1.13c D2Gfx+0x04293
extern "C" void d2gfx_security_check_cookie(D2GfxCrtTinyHost* host, uint32_t cookie) {
    if (cookie != host->stack_cookie) host->report_failure();
}

// 1.13c D2Gfx+0x013BD
extern "C" void d2gfx_unlock_8(D2GfxCrtTinyHost* host) {
    host->unlock(8);
}
