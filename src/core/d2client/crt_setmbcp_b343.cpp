// Code-page selection for D2Client's statically linked CRT.
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2ClientMbSelection {
    uint32_t locale, multibyte, codepage, extra[3];
    uint8_t ctype[257];
};
struct D2ClientSelectionCpInfo {
    uint32_t max_char_size;
    uint8_t default_char[2], lead[12];
};
static_assert(offsetof(D2ClientSelectionCpInfo, lead) == 6);
static_assert(sizeof(D2ClientSelectionCpInfo) == 20);
struct D2ClientMbSelectionEnv {
    int32_t (*get_cp_info)(uint32_t, D2ClientSelectionCpInfo*);
    void (*derive_case)(D2ClientMbSelection*);
};
struct CodePageRecord {
    uint32_t codepage, extra[3];
    uint8_t ranges[4][8];
};
static_assert(offsetof(CodePageRecord, extra) == 4);
static_assert(offsetof(CodePageRecord, ranges) == 0x10);
static_assert(sizeof(CodePageRecord) == 0x30);
// Retail D2Client+0xDBA28 records, checked against the DLL's section bytes.
static constexpr CodePageRecord records[] = {
    {932, {0x82798260, 0x21, 0}, {{0xa6,0xdf}, {0xa1,0xa5}, {0x81,0x9f,0xe0,0xfc}, {0x40,0x7e,0x80,0xfc}}},
    {936, {0xa3daa3c1, 0x20, 0}, {{0}, {0}, {0x81,0xfe}, {0x40,0xfe}}},
    {949, {0xa3daa3c1, 0x20, 0}, {{0}, {0}, {0x81,0xfe}, {0x41,0xfe}}},
    {950, {0xa2e4a2cf, 0xa2e5001a, 0x005ba2e8}, {{0}, {0}, {0x81,0xfe}, {0x40,0x7e,0xa1,0xfe}}},
    {1361, {0xda5eda51, 0xda5f0020, 0x0032da6a}, {{0}, {0}, {0x81,0xd3,0xd8,0xde,0xe0,0xf9}, {0x31,0x7e,0x81,0xfe}}}
};
uint32_t D2Client_CodepageLangId(int32_t);

// 1.13c D2Client+0x0B343
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2client_setmbcp_b343(
    uint32_t codepage, uint32_t pseudo_codepage, D2ClientMbSelection* state,
    const D2ClientMbSelectionEnv* env) {
    if (codepage) {
        for (const auto& record : records) {
            if (record.codepage != codepage) continue;
            std::memset(state->ctype, 0, sizeof state->ctype);
            for (uint32_t group = 0; group < 4; ++group) {
                const auto* ranges = record.ranges[group];
                for (uint32_t pair = 0; pair < 8 && ranges[pair] && ranges[pair+1]; pair += 2)
                    for (uint32_t byte = ranges[pair]; byte <= ranges[pair+1]; ++byte)
                        state->ctype[byte+1] |= static_cast<uint8_t>(1u << group);
            }
            state->codepage = codepage;
            state->multibyte = 1;
            state->locale = D2Client_CodepageLangId(codepage);
            std::memcpy(state->extra, record.extra, sizeof state->extra);
            env->derive_case(state);
            return 0;
        }
        D2ClientSelectionCpInfo cp{};
        if (env->get_cp_info(codepage, &cp) == 1) {
            std::memset(state->ctype, 0, sizeof state->ctype);
            state->codepage = codepage;
            state->locale = 0;
            state->multibyte = cp.max_char_size >= 2;
            if (state->multibyte) {
                for (uint32_t pair = 0; pair < 12 && cp.lead[pair] && cp.lead[pair+1]; pair += 2)
                    for (uint32_t byte = cp.lead[pair]; byte <= cp.lead[pair+1]; ++byte)
                        state->ctype[byte+1] |= 4;
                for (uint32_t byte = 1; byte < 255; ++byte) state->ctype[byte+1] |= 8;
                state->locale = D2Client_CodepageLangId(codepage);
            }
            std::memset(state->extra, 0, sizeof state->extra);
            env->derive_case(state);
            return 0;
        }
        if (!pseudo_codepage) return -1;
    }
    state->locale = state->multibyte = state->codepage = 0;
    std::memset(state->extra, 0, sizeof state->extra);
    std::memset(state->ctype, 0, sizeof state->ctype);
    env->derive_case(state);
    return 0;
}
