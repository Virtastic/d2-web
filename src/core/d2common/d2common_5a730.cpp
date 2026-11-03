// 1.13c D2Common+0x5A730 -- the treasure-class-ex linker build (MonsterTbls.cpp, line 0x60E).
// Seams: the linker create (import thunk), the no-argument build step at 0x547D0, the stdcall
// one-argument step at 0x5A030 (fed the function's own stack argument), the step at 0x53530, and the
// fatal report for a record count that overflows the 16-bit index (line 0x616).
#include <cstdint>

struct D2Common5A730State {
    uintptr_t   pLinker;     // DAT_6FDF0890: written from the create call
    uint32_t    nRecs;       // DAT_6FDF0898: record count, read by the overflow test
    uint32_t    nArg;        // the function's one stack argument, passed on to 0x5A030
    const char* szFile;      // the MonsterTbls.cpp source path the linker is tagged with
};
typedef uintptr_t (*D2Common5A730Create)(const char* szFile, uint32_t nLine);
typedef void (*D2Common5A730Step)(void);
typedef void (*D2Common5A730Arg)(uint32_t nArg);
typedef void (*D2Common5A730Fatal)(uint32_t nMsg);

extern "C" void D2Common_5A730_Build(D2Common5A730State* s, D2Common5A730Create pCreate,
    D2Common5A730Step pStep547D0, D2Common5A730Arg pStep5A030, D2Common5A730Step pStep53530,
    D2Common5A730Fatal pFatal)
{
    s->pLinker = pCreate(s->szFile, 0x60E);
    pStep547D0();
    pStep5A030(s->nArg);
    pStep53530();
    if (s->nRecs >= 0xFFFF)          // 0xFFFE < count in the decompile (unsigned compare)
        pFatal(0x616);
}
