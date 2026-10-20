// Prior art: none; written from the cached 1.13c decompile.
#include <cstdint>
#include <cstdlib>

// unverified seam: 1.13c D2Game+0x33F30 (quest-data lookup by id, returns a table whose slot 0x3C is a callback).
extern "C" int* D2GAME_33F30_Lookup(int id);

// 1.13c D2Game+0x606E0
// Not-yet-verified: the first short-circuit and the callback call convention (no args, __fastcall decompile).
uint32_t D2GAME_606E0(uint8_t* p)
{
    if (p[0xc] != 5 && p[9] != 0)
        return 1;
    int* t = D2GAME_33F30_Lookup(*(int*)(p + 0x10));
    if (!t)
        return 0;
    void* cb = *(void**)(p + 0xf0);
    if (!cb)   // IsBadCodePtr: retail prints an error (ordinals 10265/10024) and _exit(-1)
        std::_Exit(-1);
    return ((uint32_t (*)())(uintptr_t)t[0x3c])();
}
