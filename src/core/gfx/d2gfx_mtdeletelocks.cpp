// 1.13c D2Gfx+0x01967 (__mtdeletelocks): MSVC 2003 CRT lock-table teardown. Pass 1 over the 0x24
// slots of the table at +0x10048 (8-byte records: pointer, flag): a non-null slot with flag != 1
// (heap-allocated lock) is DeleteCriticalSection'd, _free'd and its pointer zeroed. Pass 2: a
// non-null slot with flag == 1 (static lock) is only DeleteCriticalSection'd, pointer kept.
// Table, delete and free are seams (GUESS: caller supplies module storage; kernel CS delete is a
// host no-op under wasm).
#include <cstdint>
#include <emscripten.h>

extern "C" EMSCRIPTEN_KEEPALIVE
void d2gfx_mtdeletelocks(uint32_t* table, void (*delete_cs)(uint32_t), void (*free_fn)(uint32_t)) {
    for (int i = 0; i < 0x24; ++i) {
        uint32_t p = table[i * 2];
        if (p != 0 && table[i * 2 + 1] != 1) {
            delete_cs(p);
            free_fn(p);
            table[i * 2] = 0;
        }
    }
    for (int i = 0; i < 0x24; ++i) {
        uint32_t p = table[i * 2];
        if (p != 0 && table[i * 2 + 1] == 1) delete_cs(p);
    }
}
