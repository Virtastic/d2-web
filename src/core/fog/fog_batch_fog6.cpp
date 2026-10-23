// Fog batch s-fog-next-6: 1E7D0, 17E00, 17F60, 14040, 1F8C0 (alpha lane: no vector).
// Retail links in queue nodes are 32-bit addresses; host seams are injected like fog_sub_1ca70.cpp.
#include <cstdint>
#include <cstring>
void fog_free_block(void* block, const char* file, uint32_t line);

// 1.13c Fog+0x14040  clear bit n of the byte array at base (gdwInvBitMasks[n&7] == ~(1<<(n&7))).
extern "C" void fog_sub_14040(uint8_t* base, int32_t n) {  // 1.13c Fog+0x14040
    base[n >> 3] &= (uint8_t)~(1u << (n & 7));
}

// 1.13c Fog+0x1F8C0  CRC-16/CCITT (poly 0x1021, init 0xFFFF, MSB-first table, 'shift 2' variant:
// crc = (crc>>2) ^ tbl[(tolower(c)^crc)&0xFF]) over the lower-cased string; empty string -> 0xFFFF.
namespace {
struct Crc { uint16_t t[256]; constexpr Crc() : t() {
    for (int i = 0; i < 256; ++i) { uint32_t v = (uint32_t)i << 8;
        for (int k = 0; k < 8; ++k) v = (v & 0x8000) ? ((v << 1) ^ 0x1021) & 0xFFFF : (v << 1) & 0xFFFF;
        t[i] = (uint16_t)v; } } };
constexpr Crc kCrc;
}
extern "C" uint16_t fog_sub_1f8c0(const uint8_t* s) {  // 1.13c Fog+0x1F8C0
    uint32_t crc = 0xFFFF; uint16_t out = 0xFFFF;
    for (;; ++s) {
        uint32_t c = *s; if (c - 'A' < 26u) c += 32;   // C-locale tolower
        if ((c & 0xFF) == 0) break;
        out = (uint16_t)((crc >> 2) ^ kCrc.t[(c ^ crc) & 0xFF]); crc = out;
    }
    return out;
}

// 1.13c Fog+0x1E7D0 -> 0x179B0  pop the head packet of queue `q` (EBX in retail) into dst.
// q: +0 u32 error flag, +4 CRITICAL_SECTION, +0x1C head, +0x20 tail. Packet: u16 size, +4 next, +8 data.
// Locking is a no-op here (single thread). Oversized packet: sets error flag, copies nothing, still pops.
extern "C" uint16_t fog_sub_1e7d0(int32_t enable, uint8_t* q, uint8_t* dst, uint32_t cap) {  // 1.13c Fog+0x1E7D0
    if (!enable) return 0;
    uint32_t headAddr; std::memcpy(&headAddr, q + 0x1C, 4);
    uint8_t* head = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(headAddr));
    if (!head) return 0;
    uint16_t size; std::memcpy(&size, head, 2);
    if ((uint16_t)cap < size) { uint32_t one = 1; std::memcpy(q, &one, 4); size = 0; }
    else std::memcpy(dst, head + 8, size);
    uint16_t ret; std::memcpy(&ret, head, 2);
    uint32_t next; std::memcpy(&next, head + 4, 4);
    fog_free_block(head, "", 0);
    std::memcpy(q + 0x1C, &next, 4);
    if (next == 0) { uint32_t z = 0; std::memcpy(q + 0x20, &z, 4); }
    return ret;
}

// 1.13c Fog+0x17E00  forwards to Storm#289 with the last two stack args swapped.
static void (*g_storm289)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t) = nullptr;
extern "C" void fog_sub_17e00(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f, uint32_t g) {  // 1.13c Fog+0x17E00
    if (g_storm289) g_storm289(a, b, c, d, e, g, f);
}

// 1.13c Fog+0x17F60  GetSavePath: registry "Diablo II"\"Save Path", else <install>\Save\ , always ends in '\\', dirs created.
// Seams: registry get/set, file-exists, install path (Fog+0x17E60), strcat, mkdir chain (Fog+0x17C40).
struct FogSavePathHost {
    int  (*regGet)(const char* key, const char* val, char* buf, uint32_t cap);
    void (*regSet)(const char* key, const char* val, const char* s);
    bool (*exists)(const char* path);
    void (*installPath)(char* buf, uint32_t cap);
    void (*mkdirs)(char* path);
};
extern "C" void fog_sub_17f60(const FogSavePathHost* h, char* p, uint32_t cap) {  // 1.13c Fog+0x17F60
    if (!h->regGet("Diablo II", "Save Path", p, cap)) *p = 0;
    if (!h->exists(p)) *p = 0;
    if (*p == 0) {
        h->installPath(p, cap);
        size_t n = std::strlen(p);
        if (n + 6 <= cap) std::memcpy(p + n, "Save\\", 6); else if (cap) p[cap - 1] = 0;  // SStrNCat truncates
        h->regSet("Diablo II", "Save Path", p);
    }
    if (cap) {
        size_t n = std::strlen(p);
        if ((n == 0 || p[n - 1] != '\\') && n + 1 < cap) { p[n] = '\\'; p[n + 1] = 0; }
    }
    h->mkdirs(p);
}
