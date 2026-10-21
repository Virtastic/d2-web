// D2Common 1.13c batch. Table pointer and the 0x38B00 callee are supplied by the caller.
#include <cstdint>
#include <emscripten.h>

struct D2Common394E0Env {
 uint32_t tables; // sgptDataTables
 int32_t (*stat_value)(uint32_t); // 0x38B00 on the record address
};
// 1.13c D2Common+0x394E0: value of stat record idx (0x144-byte entries) when gate is set.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_394e0(uint32_t gate, int32_t idx,
 const D2Common394E0Env* e) {
 if (!gate || !e || !e->tables || idx < 0) return 0;
 auto t = reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(e->tables));
 if (idx >= *reinterpret_cast<const int32_t*>(t + 0xbd4)) return 0;
 uint32_t rec = *reinterpret_cast<const uint32_t*>(t + 0xbcc) + static_cast<uint32_t>(idx) * 0x144;
 return rec && e->stat_value ? e->stat_value(rec) : 0;
}
