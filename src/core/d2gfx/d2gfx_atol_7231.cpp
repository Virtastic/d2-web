// 1.13c D2Gfx+0x07231 _atol (136 bytes), recovered from the 1.13c Ghidra decompile.
#include <cstdint>
#include "d2gfx_atol_7231.h"

extern "C" int32_t d2gfx_atol_7231(const char* str, const D2GfxAtolHost* host) {
    // Retail order: __getptd, read _ptiddata+0x64, compare with the module's
    // default locale, and reset a non-default thread locale under the CRT lock.
    const auto* ptd = static_cast<const uint8_t*>(host->getptd(host->context));
    uint32_t locale32 = *reinterpret_cast<const uint32_t*>(ptd + 0x64);
    const void* locale = host->resolve_locale(host->context, locale32);
    if (locale32 != host->default_locale(host->context)) {
        // ___updatetlocinfo returns the retail pointer value in 32-bit eax.
        // Resolve that token rather than treating it as a wasm pointer.
        locale32 = host->updatetlocinfo(host->context);
        locale = host->resolve_locale(host->context, locale32);
    }

    const uint8_t* p = reinterpret_cast<const uint8_t*>(str);
    while (true) {
        const uint32_t c = *p;
        uint32_t flags;
        // pthreadlocinfo +0x28 is lc_category[1].refcount; +0x48 is the
        // two-byte ctype table. _atol supplies a byte, so ___isctype_mt's
        // 0..255 path is table-only and cannot reach ___crtGetStringTypeA.
        if (*reinterpret_cast<const int32_t*>(static_cast<const uint8_t*>(locale) + 0x28) < 2) {
            const uint32_t table = *reinterpret_cast<const uint32_t*>(
                static_cast<const uint8_t*>(locale) + 0x48);
            const uint16_t* ctype = host->resolve_ctype(host->context, table);
            flags = ctype[c];
        } else {
            flags = host->isctype_mt(host->context, nullptr, locale,
                                     static_cast<int32_t>(c), 8);
        }
        if ((flags & 8) == 0) break;
        ++p;
    }

    const uint8_t first = *p;
    const bool negative = first == '-';
    if (first == '-' || first == '+') ++p;

    uint32_t value = 0;
    for (;;) {
        const uint32_t c = *p;
        if (c < '0' || c > '9') break;
        value = value * 10u + (c - '0');
        ++p;
    }
    if (negative) value = 0u - value;
    return static_cast<int32_t>(value);
}
