// D2Gfx VC7.1 CRT leaves: pointer validators and abort, read against the cached retail 1.13c decompiles.
// Host seam: Win32 IsBad*Ptr and the CRT callees run through callbacks, effects executed not dropped.
#include <cstdint>

struct D2GfxValidateHost {
    int32_t (*is_bad_write_ptr)(void* p, uint32_t n);
    int32_t (*is_bad_code_ptr)(void* p);
    void (*fn_360c)(int32_t code);
    int32_t (*raise)(int32_t sig);
    void (*exit_)(int32_t code);
};

// 1.13c D2Gfx+0xC986 _ValidateWrite: 1 when the range is writable
extern "C" int32_t d2gfx_validate_write(D2GfxValidateHost* h, void* p, uint32_t n) {
    return h->is_bad_write_ptr(p, n) == 0;
}

// 1.13c D2Gfx+0xC9A2 _ValidateExecute: 1 when the code pointer is callable
extern "C" int32_t d2gfx_validate_execute(D2GfxValidateHost* h, void* p) {
    return h->is_bad_code_ptr(p) == 0;
}

// 1.13c D2Gfx+0xC9BA _abort: sub_360C(10), raise(SIGABRT=22), __exit(3)
extern "C" void d2gfx_abort(D2GfxValidateHost* h) {
    h->fn_360c(10);
    h->raise(0x16);
    h->exit_(3);
}
