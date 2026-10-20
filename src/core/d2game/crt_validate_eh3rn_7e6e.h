#pragma once
#include <cstdint>

// Native projections of EH3 state, not packed retail x86 records. The caller
// resolves serialized addresses before constructing these objects. The saved
// frame was at retail registration-8; it is explicit here, never negative indexing.
struct D2GameEh3Scope {
    uint32_t enclosing_level;
    uintptr_t filter;
    uintptr_t handler;
};
struct D2GameEh3Registration {
    const void* saved_frame;
    const D2GameEh3Registration* previous;
    uintptr_t exception_handler;
    const D2GameEh3Scope* scope;
    uint32_t try_level;
};
// Host VirtualQuery result, not the 0x1c-byte x86 wire layout.
struct D2GameMemoryBasicInformation {
    void* base;
    void* allocation_base;
    uint32_t allocation_protect;
    uintptr_t region_size;
    uint32_t state;
    uint32_t protect;
    uint32_t type;
};
// One instance belongs to the D2Game module runtime, shared by its callers.
// No second private cache is retained by the validator.
struct D2GameEh3rnCache {
    int32_t count = 0;
    uintptr_t pages[16] = {};
    int32_t lock = 0;
};
struct D2GameEh3rnEnv {
    uintptr_t stack_limit;
    uintptr_t stack_base;
    uintptr_t (*virtual_query)(const void*, D2GameMemoryBasicInformation*);
    int32_t (*interlocked_exchange)(int32_t*, int32_t);
    D2GameEh3rnCache* cache;
};
extern "C" uint32_t d2game_validate_eh3rn_7e6e(
    const D2GameEh3Registration*, const D2GameEh3rnEnv*);
