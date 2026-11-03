// 1.13c Storm+0x10CB0 (cdecl, no args): ask the SNet provider for its status block. Builds a 12-byte request with 0x10AA0 (EDI = a 256-byte stack buffer,
// one stack word = &count; it leaves {0xC, n, DAT 0x55170} in the buffer and 0xC in count), then calls the provider table's slot +0x40 (stdcall, 10 words):
// (&0x55240, &0x552C0, &0x551B8, DAT 0x55344, (GetTickCount - DAT 0x55348) / 1000, DAT 0x55174, DAT 0x55354, DAT 0x55238, buffer, count) and returns its result.
#include <cstdint>
struct Storm10CB0G { uint32_t a240, a2c0, a1b8, v344, bias, v174, v354, v238, tick; };
extern "C" uint32_t Storm13c_10CB0(const Storm10CB0G& g, void (*f10aa0)(uint32_t* buf, uint32_t* count),
                                   uint32_t (*cb)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t*, uint32_t)) {
    uint32_t buf[64];
    uint32_t count;
    f10aa0(buf, &count);
    return cb(g.a240, g.a2c0, g.a1b8, g.v344, (g.tick - g.bias) / 1000u, g.v174, g.v354, g.v238, buf, count);
}
