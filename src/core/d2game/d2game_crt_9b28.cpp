// 1.13c D2Game.dll+0x09B28 _atol (136 B), transcribed from the cached 1.13c decompile.
// Stage A implementation + address marker only; no spec/vector in this lane.
// Prior art: the same MSVC 2003 _atol digit/sign loop is implemented for D2Client+0x0393C in
// d2client_batch_038a3.cpp and BNClient+0x07C5F in bnclient_batch_7c5f.cpp. This D2Game version also
// selects its whitespace classifier from the current locale, so that classifier is a host seam.
#include <cstdint>
#include <emscripten.h>

struct D2GameAtolEnv {
    uint32_t ctype_refcount;  // locale lc_category[1].refcount: < 2 uses the local ctype table
    const uint16_t* ctype;    // locale lc_category[3].refcount: two-byte entries, whitespace bit 8
    void* self;               // forwarded to ___isctype_mt when the locale is shared
    void* locinfo;            // current locale passed to ___isctype_mt
    uint32_t (*isctype_mt)(void* self, void* locinfo, uint32_t c, uint32_t mask);
};

static uint32_t d2game_atol_ascii_isctype(uint32_t c) {
    return (c == 0x20 || (c >= 0x09 && c <= 0x0D)) ? 8u : 0u;
}

// 1.13c D2Game+0x09B28: obtain the current thread locale, skip bytes whose ctype entry has bit 8
// (using ___isctype_mt for shared locale data), consume one optional sign, then wrap decimal
// accumulation in the retail 32-bit long. Parsing stops at the first non-digit; no digits yields 0.
extern "C" int32_t D2Game_Atol(const char* str, const D2GameAtolEnv* env) {
    const uint8_t* p = reinterpret_cast<const uint8_t*>(str);
    for (;;) {
        const uint32_t c = *p;
        const uint32_t space = (env == nullptr)
            ? d2game_atol_ascii_isctype(c)
            : (env->ctype_refcount < 2
                ? static_cast<uint32_t>(env->ctype[c] & 8u)
                : env->isctype_mt(env->self, env->locinfo, c, 8u));
        if (space == 0) break;
        ++p;
    }

    const uint8_t sign_or_digit = *p;
    const bool negative = sign_or_digit == '-';
    if (sign_or_digit == '-' || sign_or_digit == '+') ++p;

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
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_9B28(const char* str, const D2GameAtolEnv* env) { return D2Game_Atol(str, env); }
