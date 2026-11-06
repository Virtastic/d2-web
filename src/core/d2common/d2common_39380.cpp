// 1.13c D2Common+0x39380: EAX = owner, ESI = stat id, one stack word (layer, low 16 bits used), ret 4. Zero owner returns 0. The stat row is
// *(dt + 0xBCC) + id * 0x144 when 0 <= id < *(dt + 0xBD4) and non-null (dt = sgptDataTables, retail 0x99E1C); the key EDI = (id << 16) + layer.
// With a row: A = 0x38A80(EBX = owner, EDI = key, row) else 0; the result is A minus 0x38B00(EBX = owner, EDI = key, row) when the row exists
// again (the table is read twice, as retail does), else A. Both callees are seams (ret 4 each).
#include <cstdint>
#include <cstring>

struct D2Common39380Env {
    int32_t (*total)(uint32_t owner, uint32_t key, uint32_t row);   // 0x38A80
    int32_t (*base)(uint32_t owner, uint32_t key, uint32_t row);    // 0x38B00
};

namespace {
inline uint32_t Rd(const uint8_t* b, uint32_t o) { uint32_t v; memcpy(&v, b + o, 4); return v; }
inline uint32_t Row(const uint8_t* dt, int32_t id) {
    if (id < 0 || id >= static_cast<int32_t>(Rd(dt, 0xBD4))) return 0;
    return static_cast<uint32_t>(id) * 0x144u + Rd(dt, 0xBCC);
}
}

extern "C" int32_t D2Common_39380_Raw(uint32_t owner, int32_t id, uint32_t layer, const uint8_t* dt, const D2Common39380Env* env)
{
    if (owner == 0) return 0;
    const uint32_t key = (static_cast<uint32_t>(id) << 16) + (layer & 0xFFFFu);
    int32_t a = 0;
    uint32_t row = Row(dt, id);
    if (row != 0) a = env->total(owner, key, row);
    row = Row(dt, id);
    if (row != 0) return a - env->base(owner, key, row);
    return a;
}
