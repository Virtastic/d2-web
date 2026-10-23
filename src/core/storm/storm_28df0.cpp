// 1.13c Storm+0x28DF0 -- block-cache fill: read one cache block at an aligned offset.
#include <cstdint>
struct S28DFCache { uint8_t pad[0x118]; void* owner; uint32_t off; uint32_t blockSize; uint8_t* data; uint32_t result; };
struct S28DFInfo { uint32_t pad[2]; uint32_t size; uint32_t flags; };
struct S28DFFile { void* pad[1]; S28DFCache* cache; S28DFInfo* info; };  // retail dwords 0x44 / 0x45
// seam: Storm+0x28450 (file, off, dst, len) -> bytes read; unmapped, empty provider state
uint32_t Seam28450_28DF0(void*, uint32_t, uint8_t*, uint32_t) { return 0; }

uint32_t Storm_28DF0(S28DFFile* f, uint32_t off) {
    S28DFCache* c = f->cache;
    if (c->owner == f && c->off == off) return c->result;
    uint32_t len = c->blockSize;
    if (f->info->size < len + off && ((f->info->flags >> 8) & 0xff) == 0) len = f->info->size - off;
    c->owner = f;
    c->off = off;
    c->result = Seam28450_28DF0(f, off, c->data, len);
    return c->result;
}
