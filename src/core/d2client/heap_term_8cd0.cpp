// D2Client/__heap_term: alpha Stage A implementation from the cached 1.13c decompile.
#include <cstdint>
#include <emscripten.h>

struct D2ClientSbhRegion {
    uint8_t opaque[0x0c];
    uint32_t reservation_base; // +0x0c, retail 32-bit address passed to VirtualFree
    uint32_t block_header;     // +0x10, retail 32-bit address passed to HeapFree
};
static_assert(sizeof(D2ClientSbhRegion) == 0x14, "1.13c small-block region stride");
static_assert(__builtin_offsetof(D2ClientSbhRegion, reservation_base) == 0x0c &&
              __builtin_offsetof(D2ClientSbhRegion, block_header) == 0x10,
              "1.13c small-block region fields");

struct D2ClientHeapTermEnv {
    uint32_t heap_type;             // DAT_6FBD398C: 3 selects the small-block heap
    uint32_t region_count;          // DAT_6FBD3970
    uint32_t regions_address;       // DAT_6FBD3974, retail 32-bit pointer
    D2ClientSbhRegion* regions;     // resolved backing for that retail pointer
    uint32_t heap_handle;           // DAT_6FBD3988, retained as retail's 32-bit handle
    int32_t (*virtual_free)(uint32_t address, uint32_t size, uint32_t free_type);
    int32_t (*heap_free)(uint32_t heap, uint32_t flags, uint32_t address);
    int32_t (*heap_destroy)(uint32_t heap);
};

// 1.13c D2Client+0x08CD0 (__heap_term): on heap type 3, decommit and release each
// region's 1 MiB reservation, free its block header, then the region array; finally destroy
// the process heap. The callback arguments preserve the retail 32-bit handles and call order.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_heap_term_8cd0(D2ClientHeapTermEnv* env) {
    if (env->heap_type == 3) {
        for (uint32_t i = 0; i < env->region_count; ++i) {
            const D2ClientSbhRegion& region = env->regions[i];
            env->virtual_free(region.reservation_base, 0x100000, 0x4000); // MEM_DECOMMIT
            env->virtual_free(region.reservation_base, 0, 0x8000);        // MEM_RELEASE
            env->heap_free(env->heap_handle, 0, region.block_header);
        }
        env->heap_free(env->heap_handle, 0, env->regions_address);
    }
    env->heap_destroy(env->heap_handle);
}
