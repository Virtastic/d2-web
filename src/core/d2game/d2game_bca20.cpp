// 1.13c D2Game+0xBCA20 -- EBX = pGame. Null game or null list (+0x60) returns 0. Otherwise walks the list (0xA3DC head, 0xA4CC next taken before the
// node is used, 0xA46C node data); for each data word of type 4 for which 0xA952(data, &out) is nonzero it calls 0xA9F4(game, data) then
// 0xA8C2(game, data, out). Returns 1.
#include <cstdint>
#include <cstring>
struct GBCA20Env {
    uint32_t (*head)(uint32_t list);                           // 0xA3DC (stdcall 1)
    uint32_t (*next)(uint32_t it);                             // 0xA4CC (stdcall 1)
    uint32_t (*data)(uint32_t it);                             // 0xA46C (stdcall 1)
    uint32_t (*probe)(uint32_t data, uint32_t* out);           // 0xA952 (stdcall 2; writes *out)
    void (*prepare)(uint32_t game, uint32_t data);             // 0xA9F4 (stdcall 2)
    void (*apply)(uint32_t game, uint32_t data, uint32_t out); // 0xA8C2 (stdcall 3)
};
static uint32_t rd32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
// 1.13c D2Game+0xBCA20
uint32_t g_bca20(uint32_t game, const GBCA20Env& env)
{
    if (!game || !rd32(game + 0x60)) return 0;
    uint32_t it = env.head(rd32(game + 0x60));
    while (it) {
        const uint32_t nx = env.next(it);
        const uint32_t d = env.data(it);
        uint32_t out = 0;
        if (d && rd32(d) == 4 && env.probe(d, &out) != 0) { env.prepare(game, d); env.apply(game, d, out); }
        it = nx;
    }
    return 1;
}
