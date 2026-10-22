// Batch b67: raw 1.13c byte offsets; pointer slots are read as uintptr_t.
#include <cstdint>
#include <cstdlib>
#include <initializer_list>

extern "C" __attribute__((weak)) uint32_t Ordinal_10265(uint32_t);
extern "C" __attribute__((weak)) void Ordinal_10024(const void*, uint32_t);
extern const char DAT_6fd19ff9[];

void D2GAME_2D140(void* a);                // 1.13c D2Game+0x2D140
void D2GAME_2D500(void* game, uint32_t f); // 1.13c D2Game+0x2D500
void D2GAME_29820(const void* name);       // 1.13c D2Game+0x29820

namespace {
template <class T> T& at(void* p, uint32_t off) { return *reinterpret_cast<T*>(static_cast<char*>(p) + off); }
[[noreturn]] void fatal(uint32_t line)
{
    Ordinal_10024(DAT_6fd19ff9, Ordinal_10265(line));
    std::exit(-1);
}
}

// 1.13c D2Game+0x2A680 (esi = table of 0x80 list heads; counts nodes linked via +0xE4)
int D2GAME_2A680_CountLinked(void* table)
{
    int n = 0;
    for (int i = 0; i < 0x80; ++i) {
        for (uintptr_t p = at<uintptr_t>(table, i * sizeof(uintptr_t)); p; p = at<uintptr_t>(reinterpret_cast<void*>(p), 0xE4))
            ++n;
    }
    return n;
}

// 1.13c D2Game+0x2D790 (esi = client, edi = game; game+0x6A mode 1 or 2)
void D2GAME_2D790_Drain(void* client, void* game)
{
    uint8_t m = at<uint8_t>(game, 0x6A);
    if (m != 1 && m != 2) return;
    if (!client) fatal(0xb8a);
    while ((at<uint8_t>(client, 0x3D4) & 0x10) && at<uintptr_t>(client, 0x17C))
        D2GAME_2D140(game);
}

// 1.13c D2Game+0x2D980 (edi = game; walks list at +0x88 via +0x4A8)
void D2GAME_2D980_FreeAll(void* game)
{
    uintptr_t p = at<uintptr_t>(game, 0x88);
    while (p) {
        p = at<uintptr_t>(reinterpret_cast<void*>(p), 0x4A8);
        D2GAME_2D500(game, 1);
    }
}

// 1.13c D2Game+0xCBDB0 (eax = record; 16 name bytes at +8; calls 29820 if any is zero)
void D2GAME_CBDB0_CheckName(void* rec)
{
    int i = 0;
    while (i < 0x10 && at<uint8_t>(rec, 8 + i)) ++i;
    if (i < 0x10) D2GAME_29820(rec);
}
