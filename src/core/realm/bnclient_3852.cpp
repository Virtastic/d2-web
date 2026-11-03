// 1.13c BNClient+0x03852 ___isctype_mt (cdecl, 119 B). Own file (the rule for ports of this batch).
#include <cstdint>
#include <emscripten.h>

struct BnCrtGetStringType {
    int32_t (*get_string_type_a)(uint32_t info_type, const uint8_t* bytes, int32_t count,
                                 uint16_t* out, uint32_t codepage);   // 0x6D18 ___crtGetStringTypeA
};

// 1.13c BNClient+0x03852 (119 B, callers 0x7C5F, 0x1C76, 0x16D9, 0x19FE, 0x1E1F): ___isctype_mt(c, mask)
// -- test the ctype mask of one character. For c in [-1, 0xFF] the answer is the ctype table word at
// index c & mask (the locale's table pointer lives at locale+0x48; index -1 is the EOF entry, so retail's
// table pointer is 2 bytes into the 0x100-word array). Otherwise the code is a multibyte sequence: when
// the table word of its high byte has bit 15 clear it is one byte (c & 0xFF), else two high byte first;
// those bytes go through ___crtGetStringTypeA (CT_CTYPE1) and the returned word & mask is the answer, 0
// when that call fails. The table pointer is an argument so the rule stays a pure function of it.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t bn_isctype_mt(const uint16_t* ctype, int32_t c, uint32_t mask,
                                            uint32_t codepage,
                                            const BnCrtGetStringType* env) {
    if ((uint32_t)(c + 1) < 0x101u) return ctype[c] & mask;
    uint8_t bytes[2];
    int32_t count;
    if ((ctype[(c >> 8) & 0xff] & 0x8000u) == 0) {
        bytes[0] = (uint8_t)c;
        count = 1;
    } else {
        bytes[0] = (uint8_t)(c >> 8);
        bytes[1] = (uint8_t)c;
        count = 2;
    }
    uint16_t flags = 0;
    if (env->get_string_type_a(1, bytes, count, &flags, codepage) == 0) return 0;
    return flags & mask;
}

