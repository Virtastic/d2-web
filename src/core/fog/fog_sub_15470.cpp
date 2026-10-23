// 1.13c Fog+0x15470 -- sEnumModules(pid): try the toolhelp module snapshot (Fog+0x15240, pid in
// EAX); if that yields no list fall back to the PSAPI walk (Fog+0x14F50, pid on the stack). Then log
// one "    Base:%08lXh  Size:%7lXh  Name:%-15.15s  Path:%s" line per node (next at +0x20c) through
// Fog+0x190A0 and return the head. wasm has no process module list, so both enumerators yield null,
// the loop never runs and the head is null.
#include <cstdint>

void* fog_sub_15470(uint32_t pid) {  // 1.13c Fog+0x15470
    (void)pid;
    return nullptr;
}
