// 1.13c D2Game+0x07AD1: one statically linked VC7.1 CRT code-page selector.
// This is state-parameterized so the DLL's CRT globals and NLS callees remain explicit seams.
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2GameMbSelectionState {
    uint32_t locale;             // DAT_6fd39fd0
    uint32_t multibyte;          // DAT_6fd39fd8
    uint8_t ctype[257];          // DAT_6fd39fdf..DAT_6fd3a0df; data byte is indexed at ch+1
    uint32_t codepage;           // DAT_6fd3a0e4
    uint32_t extra[3];           // DAT_6fd3a0f0/0xf4/0xf8
    uint32_t pseudo_page_flag;   // DAT_6fd3007c: caller requested a pseudo code page
    uint8_t case_map[256];       // DAT_6fd3a100, written by the 0x7936 tail
};

struct D2GameSetmbcpCpInfo {
    uint32_t max_char_size;      // Win32 CPINFO +0
    uint8_t default_char[2];     // +4
    uint8_t lead_byte[12];       // +6
};
static_assert(offsetof(D2GameSetmbcpCpInfo, lead_byte) == 6);
static_assert(sizeof(D2GameSetmbcpCpInfo) == 0x14);

struct D2GameSetmbcpRecord {
    uint32_t codepage;
    uint32_t extra[3];
    uint8_t ranges[4][8];
};
static_assert(offsetof(D2GameSetmbcpRecord, extra) == 4);
static_assert(offsetof(D2GameSetmbcpRecord, ranges) == 0x10);
static_assert(sizeof(D2GameSetmbcpRecord) == 0x30);

// The five 0x30-byte CRT records at 0x6FD1F9B8 and the category masks at
// 0x6FD1F9B0. Their contents match the D2Gfx/D2Client copies cross-checked in
// their function specs; code-page records are immutable retail data.
static constexpr uint8_t kGroupFlags[4] = {1, 2, 4, 8};
static constexpr D2GameSetmbcpRecord kRecords[5] = {
    {932, {0x82798260, 0x00000021, 0}, {{0xa6,0xdf}, {0xa1,0xa5}, {0x81,0x9f,0xe0,0xfc}, {0x40,0x7e,0x80,0xfc}}},
    {936, {0xa3daa3c1, 0x00000020, 0}, {{0}, {0}, {0x81,0xfe}, {0x40,0xfe}}},
    {949, {0xa3daa3c1, 0x00000020, 0}, {{0}, {0}, {0x81,0xfe}, {0x41,0xfe}}},
    {950, {0xa2e4a2cf, 0xa2e5001a, 0x005ba2e8}, {{0}, {0}, {0x81,0xfe}, {0x40,0x7e,0xa1,0xfe}}},
    {1361,{0xda5eda51, 0xda5f0020, 0x0032da6a}, {{0}, {0}, {0x81,0xd3,0xd8,0xde,0xe0,0xf9}, {0x31,0x7e,0x81,0xfe}}}
};

struct D2GameSetmbcpEnv {
    int32_t (*get_cp_info)(uint32_t codepage, D2GameSetmbcpCpInfo* info);
    uint32_t (*locale_id)(uint32_t codepage); // 0x78DE
    void (*set_sbcs)(D2GameMbSelectionState* state); // 0x790D
    void (*derive_tables)(D2GameMbSelectionState* state); // 0x7936
};

// 1.13c D2Game+0x07AD1: __setmbcp_nolock. Known pages select the matching CRT
// record, classify its four range groups, publish the page/locale metadata, then
// derive ctype/case tables. Other pages use GetCPInfo; unsupported pages preserve
// state and return -1 unless the caller's pseudo-page flag permits SBCS fallback.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_7ad1(
    uint32_t codepage, D2GameMbSelectionState* state, const D2GameSetmbcpEnv* env) {
    if (codepage != 0) {
        int32_t record_index = -1;
        for (int32_t i = 0; i < 5; ++i) {
            if (kRecords[i].codepage == codepage) { record_index = i; break; }
        }
        if (record_index >= 0) {
            std::memset(state->ctype, 0, sizeof state->ctype);
            const auto& record = kRecords[record_index];
            for (uint32_t group = 0; group < 4; ++group) {
                const uint8_t* pair = record.ranges[group];
                for (uint32_t offset = 0; offset + 1 < 8 && pair[offset] && pair[offset + 1]; offset += 2) {
                    const uint32_t first = pair[offset];
                    const uint32_t last = pair[offset + 1];
                    if (first <= last)
                        for (uint32_t byte = first; byte <= last; ++byte)
                            state->ctype[byte + 1] |= kGroupFlags[group];
                }
            }
            state->codepage = codepage;
            state->multibyte = 1;
            state->locale = env->locale_id(codepage);
            std::memcpy(state->extra, record.extra, sizeof state->extra);
        } else {
            D2GameSetmbcpCpInfo info{};
            if (env->get_cp_info(codepage, &info) == 1) {
                std::memset(state->ctype, 0, sizeof state->ctype);
                state->codepage = codepage;
                state->locale = 0;
                if (info.max_char_size < 2) {
                    state->multibyte = 0;
                } else {
                    for (uint32_t offset = 0; offset + 1 < sizeof info.lead_byte &&
                         info.lead_byte[offset] != 0 && info.lead_byte[offset + 1] != 0; offset += 2) {
                        const uint32_t first = info.lead_byte[offset];
                        const uint32_t last = info.lead_byte[offset + 1];
                        if (first <= last)
                            for (uint32_t byte = first; byte <= last; ++byte)
                                state->ctype[byte + 1] |= 4;
                    }
                    for (uint32_t byte = 1; byte < 0xff; ++byte) state->ctype[byte + 1] |= 8;
                    state->locale = env->locale_id(codepage);
                    state->multibyte = 1;
                }
                state->extra[0] = state->extra[1] = state->extra[2] = 0;
            } else if (state->pseudo_page_flag == 0) {
                return -1;
            } else {
                env->set_sbcs(state);
            }
        }
    } else {
        env->set_sbcs(state);
    }
    env->derive_tables(state);
    return 0;
}
