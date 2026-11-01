// D2Common 1.13c batch. Retail passes idx in EAX and cnt in EBX (register args); explicit here.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

extern "C" {
// 1.13c D2Common+0x18400: open a gap in arr (slots start at +4) at idx, copy this's items into arr[*pos..].
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_18400(uint8_t* self, uint8_t* arr, int32_t* pos, int32_t* cnt, int32_t idx) {
 const int32_t n = *reinterpret_cast<int32_t*>(self + 4);
 const int32_t grow = (*pos - idx) - 1 + n;
 const int32_t dst = grow + idx;
 memmove(arr + 4 + dst * 4, arr + 4 + idx * 4, static_cast<size_t>((*cnt - idx) * 4 - 4));
 memcpy(arr + *pos * 4, self + 0x24, static_cast<size_t>(static_cast<uint32_t>(n) & 0x3fffffffu) * 4);
 *cnt += grow;
 *pos = dst + 1;
}
}
