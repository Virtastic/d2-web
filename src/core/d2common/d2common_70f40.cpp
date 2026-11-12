// Two leaf exports as retail leaves the registers. The first is a stdcall no-op that discards its single stack word: EAX on return
// is EAX on entry. The second returns the address of the gamble table header, a standalone module global in retail (image offset
// 0x9FBF0); the caller-facing accessors in exports.cpp serve the engine, these state the register result.
#include <cstdint>

// 1.13c D2Common+0x70F40
extern "C" uint32_t d2common_70f40(uint32_t eax, uint32_t /*discarded*/) { return eax; }

// 1.13c D2Common+0x71280: modBase is the address the module is loaded at.
extern "C" uint32_t d2common_71280(uint32_t modBase) { return modBase + 0x9FBF0u; }
