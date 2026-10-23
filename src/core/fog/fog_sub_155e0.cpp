// 1.13c Fog+0x155E0 -- sShowStack: crash-report stack crawl (imagehlp StackWalk/SymInitialize,
// logs "Stack Crawl:" lines through Fog+0x190A0). Retail returns 0 early when the registry key
// "NoStackDump" is set or imagehlp.dll is not loaded (DAT_6ffa6e54 == 0), else walks <= 0x40 frames.
// wasm has no imagehlp, no registry, no native frames: the "imagehlp not loaded" branch is the
// only reachable one, so it returns 0 (log line "sShowStack: imagehlp.dll not loaded" omitted: no log sink).
#include <cstdint>

uint32_t fog_sub_155E0() {  // 1.13c Fog+0x155E0
    return 0;
}
