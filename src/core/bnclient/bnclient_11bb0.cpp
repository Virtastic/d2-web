// 1.13c BNClient+0x11BB0: the proof compare. SHA-1 over the three pinned globals
// (a = DAT_1ECF8[0x20], b = DAT_1ED40[0x14], c = DAT_1ED18[0x28], through 0xA610) against the 20 bytes at param.
#include <cstdint>
#include <cstring>

extern "C" void d2_bnclient_sub_a610(uint8_t* digest, uint8_t* a, uint8_t* b, uint8_t* c);   // 1.13c BNClient+0xA610

extern "C" uint32_t bnclient_proof_check_11bb0(const uint8_t* param, uint8_t* a, uint8_t* b, uint8_t* c) {
    uint8_t digest[20];
    d2_bnclient_sub_a610(digest, a, b, c);
    return memcmp(digest, param, 20) == 0 ? 1u : 0u;
}
