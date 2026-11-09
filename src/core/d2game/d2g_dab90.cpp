// 1.13c D2Game+0xDAB90 (ECX = game, EAX = source unit, EBX = target unit, stack = 0x70-byte record; ret 4).
// A type 0/1 source (or a type 2 source that is not class 7/0xB) queues a 0x88-byte record at target+0xAC: the allocator
// gets pool = game+0x1C, size 0x88, the source file name, line 0x7C8, 0. Fields: [0] = game, [1]/[2] = target type/id,
// [3]/[4] = source type/id, [5..0x20] = copy of the record, [0x21] = old list head, which the new node replaces.
#include <cstdint>
typedef intptr_t I;
extern "C" intptr_t D2GAME_DAB90(intptr_t game, int* eax, intptr_t ebx, const uint32_t* src,
                                 uint32_t (*alloc)(uint32_t, uint32_t, const char*, uint32_t, uint32_t))
{
    if (eax && !((*eax == 2) && (eax[1] == 7 || eax[1] == 0xb)) && (*eax == 1 || *eax == 0)) {
        uint32_t* t = (uint32_t*)ebx;
        uint32_t* n = (uint32_t*)(uintptr_t)alloc(*(uint32_t*)(game + 0x1c), 0x88, "..\\Source\\D2Game\\UNIT\\SUnitDmg.cpp", 0x7c8, 0);
        n[0] = (uint32_t)game;
        n[1] = t[0];
        n[2] = t[3];
        n[3] = (uint32_t)eax[0];
        n[4] = (uint32_t)eax[3];
        for (int i = 0; i < 0x1c; i++) n[5 + i] = src[i];
        n[0x21] = t[0xac / 4];
        t[0xac / 4] = (uint32_t)(uintptr_t)n;
    }
    return 0;
}
