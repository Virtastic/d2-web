// 1.13c Fog+0x154D0 -- sLoadSymbols(hProcess): takes the module list from Fog+0x15470 (linked list,
// next at +0x20c, image path at +0, name at +0x104, base +0x204, size +0x208), logs
// "Load symbols..." and calls the imagehlp SymLoadModule pointer (DAT_6ffa6e40) once per module,
// logging success/error; returns the list head. wasm has no process module list and no imagehlp,
// so the list is empty: the loop never runs and the head is null.
#include <cstdint>

void* fog_sub_154D0(uint32_t hProcess) {  // 1.13c Fog+0x154D0
    (void)hProcess;
    return nullptr;
}
