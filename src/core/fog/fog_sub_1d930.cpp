// 1.13c Fog+0x1D930: thiscall(this, a, b). Validates a tagged object (this != 0, tag (+0x48 & ~0xF)
// == 0x41737960, bit 0 of +0x48 set; else fatal "Unrecoverable internal error" at lines 0x283/0x284/0x286),
// frees the block held in slot +0x3C via the pool free, then stores a at +0x3C and b at +0x40.
// NAMED only: touches struct memory, not probe-able; slots are 32-bit retail addresses.
#include <cstdint>
#include <cstring>
extern "C" void FUN_6ff6b0c0(void** slot);  // Fog+0x1B0C0 pool free (takes &ptr; null is a no-op)
void fog_error_report(int32_t type, const char* file, uint32_t line, const char* format, ...);
[[noreturn]] void fog_exit_process(int32_t code);

extern "C" void fog_sub_1d930(uint8_t* self, uint32_t a, uint32_t b) {  // 1.13c Fog+0x1D930
    auto fatal = [](uint32_t line) { fog_error_report(6, "", line, "Unrecoverable internal error %08x"); fog_exit_process(-1); };
    if (!self) fatal(0x283);
    uint32_t tag; std::memcpy(&tag, self + 0x48, 4);
    if ((tag & 0xfffffff0u) != 0x41737960u) fatal(0x284);
    if (!(tag & 1)) fatal(0x286);
    uint32_t old; std::memcpy(&old, self + 0x3c, 4);
    void* slot = reinterpret_cast<void*>(static_cast<uintptr_t>(old));
    FUN_6ff6b0c0(&slot);  // Fog+0x1B0C0
    std::memcpy(self + 0x3c, &a, 4);
    std::memcpy(self + 0x40, &b, 4);
}
