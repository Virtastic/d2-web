// 1.13c D2Common+0x55400: text-compile sound-name table. When the compile-mode gate is set, create the linker (0x924C,
// stdcall, 12 stack words; the block also carries the linker address) and load the "sounds" table into it (0x5EF40,
// stdcall, 5 stack words: zero, name, the same 12-word block, the out address, mode 2). The function's one stack argument
// is never read (ret 4). Host addresses are passed in so the probe can present retail's module addresses.
#include <cstdint>

struct D2Common55400State {
    uint32_t  bCompile;    // DAT_6FDF145C (0xA145C): compile-mode gate
    uintptr_t pLinker;     // DAT_6FDF0070 (0xA0070): linker returned by 0x924C
    uintptr_t pResult;     // DAT_6FDF006C (0xA006C): result of the load
    uintptr_t pOutAddr;    // address of DAT_6FDF0074 (0xA0074): the load's out pointer
    uintptr_t addrLinker;  // address of DAT_6FDF0070, block word 6
    uintptr_t addrSound;   // "Sound" string, block word 2
    uintptr_t addrFile;    // source-file string, block word 0
    uintptr_t addrTable;   // DAT_6FDDA3E0 table address, block word 7
    uintptr_t addrSounds;  // "sounds" string (the load's name)
};

typedef uintptr_t (*D2Common55400Create)(const uint32_t* blk);                                                  // 0x924C
typedef int32_t (*D2Common55400Load)(uintptr_t zero, uintptr_t szName, const uint32_t* blk, uintptr_t pOut, uint32_t mode); // 0x5EF40

// 1.13c D2Common+0x55400 (the stdcall argument is dropped by the caller's ret 4).
extern "C" void D2Common_55400_Load(D2Common55400State* s, D2Common55400Create pCreate, D2Common55400Load pLoad)
{
    if (s->bCompile == 0) return;
    // Block: file, line 0xC99, "Sound", 0x11, 0, 0, &linker, table, 0, 0, 0, 0 (slot 11 is the zero the load receives as ECX).
    uint32_t blk[12] = {
        (uint32_t)s->addrFile, 0xC99u, (uint32_t)s->addrSound, 0x11u, 0u, 0u,
        (uint32_t)s->addrLinker, (uint32_t)s->addrTable, 0u, 0u, 0u, 0u
    };
    s->pLinker = pCreate(blk);
    s->pResult = (uintptr_t)pLoad(blk[11], s->addrSounds, blk, s->pOutAddr, 2u);
}
