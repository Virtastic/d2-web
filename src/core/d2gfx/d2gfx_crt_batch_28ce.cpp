// D2Gfx 1.13c CRT helpers. Host callbacks stand for the DLL's TLS and lock APIs;
// addresses and field widths below remain the retail x86 values.
#include <cstdint>
#include <cstring>

struct D2GfxCrtBatchHost {
    void (*tls_free)(uint32_t index);
    void (*mtdeletelocks)();
    void (*unlock)(uint32_t lock_number);
};

// 1.13c D2Gfx+0x028CE
extern "C" void d2gfx_mtterm(uint32_t* tls_index, const D2GfxCrtBatchHost* host) {
    if (*tls_index != UINT32_MAX) {
        host->tls_free(*tls_index);
        *tls_index = UINT32_MAX;
    }
    host->mtdeletelocks();
}

// 1.13c D2Gfx+0x028EB
extern "C" void d2gfx_initptd(void* ptd, uint32_t initaddr_rva) {
    auto* bytes = static_cast<uint8_t*>(ptd);
    std::memcpy(bytes + 0x54, &initaddr_rva, sizeof(initaddr_rva));
    const uint32_t initial_rand = 1;
    std::memcpy(bytes + 0x14, &initial_rand, sizeof(initial_rand));
}

// 1.13c D2Gfx+0x02AA1
extern "C" void d2gfx_unlock_0d(const D2GfxCrtBatchHost* host) {
    host->unlock(0x0D);
}

// 1.13c D2Gfx+0x02AAD
extern "C" void d2gfx_unlock_0c(const D2GfxCrtBatchHost* host) {
    host->unlock(0x0C);
}

// 1.13c D2Gfx+0x0335D
extern "C" uint32_t d2gfx_heap_select(uint32_t platform, uint32_t version) {
    return platform == 2 && version > 4 ? 1u : 3u;
}
