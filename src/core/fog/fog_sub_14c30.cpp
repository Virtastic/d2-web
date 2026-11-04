// 1.13c Fog+0x14C30: EAX = list head. Walks nodes linked at +0x20C, freeing each via the pool free (Fog+0x1B0C0, :0x39F).
// The seam below takes the pool free as an argument so the capture probe can observe the calls.
#include <cstdint>
#include <cstring>
void fog_free_block(void* block, const char* file, uint32_t line);

// The pool free takes ECX = &slot (the node pointer) and the file/line words; the seam passes the slot and the line.
struct Fog14C30Env { void (*poolFree)(uint32_t* slot, uint32_t line); };

// 1.13c Fog+0x14C30 with the pool free injected. The next link is read before the free, as retail does.
extern "C" void fog_14c30_run(uint8_t* head, const Fog14C30Env* env) {
    while (head) {
        uint32_t next; std::memcpy(&next, head + 0x20c, 4);
        uint32_t slot = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(head));
        env->poolFree(&slot, 0x39F);
        head = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(next));
    }
}

extern "C" void fog_sub_14c30(uint8_t* head) {  // 1.13c Fog+0x14C30
    Fog14C30Env e{ [](uint32_t* slot, uint32_t line) {
        fog_free_block(reinterpret_cast<void*>(static_cast<uintptr_t>(*slot)), "", line);
    } };
    fog_14c30_run(head, &e);
}
