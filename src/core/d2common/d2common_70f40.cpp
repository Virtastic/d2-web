// The gamble table header, a standalone module global in retail (image offset 0x9FBF0); the caller-facing accessor in
// exports.cpp serves the engine, this states the register result. (0x70F40 is d2common_70f40 in d2moo/src/D2CommonCrt.cpp.)
#include <cstdint>

// 1.13c D2Common+0x71280: modBase is the address the module is loaded at.
extern "C" uint32_t d2common_71280(uint32_t modBase) { return modBase + 0x9FBF0u; }
