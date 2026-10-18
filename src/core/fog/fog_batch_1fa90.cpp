// src/core/fog/fog_batch_1fa90.cpp -- six 1.13c Fog.dll alpha-lane rows reimplemented from the
// decompiles in build-wasm/ghidra-fog/decomp/0x<RVA>.c, with the 1.13c fastcall/stack argument
// order recovered from the disassembly of retail/D2/D2SE/CORES/1.13c/Fog.dll. Base 0x6FF50000.
//
// These are the DataArrays handle creation/reset/lookup trio (Source/Fog/Src/DataArrays.cpp) and
// the Fog memory-block acquire/release pair. Their module-internal callees are separate, still
// UNMAPPED Fog rows (+0x2D90 strncpy, +0x1B560 alloc, +0x1B0C0 free, +0x1FC20 capture-ip,
// +0xCD8E stack dump, +0x10F30 fatal report, +0x684A0 abort, +0x1BA9 _exit, +0x1FB20 lookup), so
// each is a seam on the env struct passed first.
//
// Fog+0x1FA90 builds a data-array handle: name[0x100], count at +0x100, stride at +0x104, flags
// and bookkeeping zeroed to +0x120, and the inline data pointer +0x11C = handle + 0x120. The
// allocation size retail passes in EAX is count*stride + 0x120.
#include <cstddef>
#include <cstdint>
#include <cstring>

#include <Windows.h>   // deterministic GetTickCount

namespace {

// Fog+0x6FF7xxxx module state. Only the performance accumulator index and the tick are read back;
// the accumulator array is the 0x17FF8-byte span between +0x32F48 and the +0x4AF40 tick.
uint32_t g_perfAccumulatorIndex;                   // +0x5A6E7C
uint8_t g_perfAccumulator[0x17FF8];                // +0x32F48, 6-dword entries
uint32_t g_perfTick;                               // +0x4AF40

constexpr const char* kDataArraysFile = "..\\Source\\Fog\\Src\\DataArrays.cpp";

// The shared fatal tail of +0x1FCD0 / +0x1FD80: capture the IP, dump the stack, then report level
// 6 at the given line and exit. Retail does not return.
struct FogDataArrayApi {
    void* (*alloc)(uint32_t size, uint32_t zero, const char* file, uint32_t line);   // +0x1B560
    char* (*strncpy_)(char* dest, const char* src, uint32_t count);                  // +0x2D90
    void (*free_block)(void** slot, const char* file, uint32_t line);                // +0x1B0C0
    uint32_t (*capture_ip)(void);                                                    // +0xFC20
    void (*stack_dump)(void);                                                        // +0xCD8E
    void (*fatal)(int level, const char* file, uint32_t line, const char* format, ...); // +0x10F30
    void (*abort)(void);                                                             // +0x684A0
    void (*exit_process)(int code);                                                  // +0x1BA9
    int32_t (*lookup)(void* array, int32_t index);                                   // +0x1FB20
};

// The handle layout +0x1FA90 returns and +0x1FA60/+0x1FCD0/+0x1FD80 consume.
#pragma pack(push, 1)
struct FogDataArray {
    char name[0x100];      // +0x00
    int32_t count;         // +0x100
    int32_t stride;        // +0x104
    uint32_t init;         // +0x108
    uint32_t at10c;        // +0x10C
    uint32_t index10;      // +0x110
    uint32_t at114;        // +0x114
    uint16_t at118;        // +0x118
    uint8_t pad11a[2];     // +0x11A
    void* data;            // +0x11C
};
#pragma pack(pop)
static_assert(sizeof(void*) != 4 || offsetof(FogDataArray, count) == 0x100, "count");
static_assert(sizeof(void*) != 4 || offsetof(FogDataArray, init) == 0x108, "init");
static_assert(sizeof(void*) != 4 || offsetof(FogDataArray, data) == 0x11C, "data");
static_assert(sizeof(void*) != 4 || sizeof(FogDataArray) == 0x120, "handle header");

[[noreturn]] void fog_dataarray_fatal(const FogDataArrayApi* api, uint32_t line) {
    const uint32_t ip = api->capture_ip();
    api->stack_dump();
    api->fatal(6, "", line, "Unrecoverable internal error %08x", ip);
    api->abort();
    api->exit_process(-1);
    __builtin_unreachable();
}

}  // namespace

// 1.13c Fog+0x0FB90 (ordinal 10058)
// Reset the performance accumulator: clear the first two dwords of each 6-dword entry while the
// index is positive, then stamp the module tick with GetTickCount(). The tail is not conditioned
// on the index.
extern "C"
void fog_sub_FB90(void) {
    uint32_t i = g_perfAccumulatorIndex;
    uint32_t* entry = reinterpret_cast<uint32_t*>(g_perfAccumulator);
    for (; i != 0; --i) {
        entry[0] = 0;
        entry[1] = 0;
        entry += 6;
    }
    g_perfTick = GetTickCount();
}

// 1.13c Fog+0x1FA90 (ordinal 10060)
// Allocate count*stride + 0x120 bytes, zero the 0x120-byte header, copy the name (strncpy, 0x100),
// record count/stride and point data at the inline tail. Allocation failure reports level 2 at
// DataArrays.cpp:0x34 and returns NULL.
extern "C"
void* fog_sub_1FA90(const FogDataArrayApi* api, const char* name, int32_t count, int32_t stride) {
    const uint32_t size = static_cast<uint32_t>(count) * static_cast<uint32_t>(stride) + 0x120u;
    FogDataArray* array = static_cast<FogDataArray*>(api->alloc(size, 0, kDataArraysFile, 0x34));
    if (array == nullptr) {
        api->fatal(2, kDataArraysFile, 0x34, "");
        return nullptr;
    }
    std::memset(array, 0, 0x120);
    api->strncpy_(array->name, name, 0x100);
    array->stride = stride;
    array->count = count;
    array->data = array + 1;
    array->init = 0;
    return array;
}

// 1.13c Fog+0x1FA60 (ordinal 10061)
// Zero the 0x120-byte header in place, then release the block through the Fog allocator
// (DataArrays.cpp:0x4C).
extern "C"
void fog_sub_1FA60(const FogDataArrayApi* api, void* block) {
    std::memset(block, 0, 0x120);
    api->free_block(&block, kDataArraysFile, 0x4C);
}

// 1.13c Fog+0x1FCD0 (ordinal 10063)
// A handle that was never initialised is an unrecoverable internal error at DataArrays.cpp:0x68
// (capture IP, stack dump, fatal, abort, _exit(-1)); a live one resets index to -1, clears the
// counter at +0x114 and the word at +0x10C, sets +0x118 = 0x8000, and zeroes the first word of
// every count entries at stride.
extern "C"
void fog_sub_1FCD0(const FogDataArrayApi* api, FogDataArray* array) {
    if (array->init == 0) fog_dataarray_fatal(api, 0x68);
    array->index10 = 0xFFFFFFFFu;
    array->at114 = 0;
    array->at10c = 0;
    array->at118 = 0x8000;
    for (int32_t i = 0; i < array->count; ++i) {
        *reinterpret_cast<uint16_t*>(static_cast<uint8_t*>(array->data) +
                                     static_cast<size_t>(array->stride) * static_cast<size_t>(i)) = 0;
    }
}

// 1.13c Fog+0x1FD70 (ordinal 10062)
// Mark the handle initialised (+0x108 = 1) and tail-jump into the reset (+0x1FCD0).
extern "C"
void fog_sub_1FD70(const FogDataArrayApi* api, FogDataArray* array) {
    array->init = 1;
    fog_sub_1FCD0(api, array);
}

// 1.13c Fog+0x1FD80 (ordinal 10066)
// Look up an entry; a missed lookup is the same unrecoverable internal error at
// DataArrays.cpp:0xFD. Retail leaves the found entry to the callee and only checks for NULL.
extern "C"
void fog_sub_1FD80(const FogDataArrayApi* api, FogDataArray* array, int32_t index) {
    if (api->lookup(array, index) == 0) fog_dataarray_fatal(api, 0xFD);
}
