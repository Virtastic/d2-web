// D2Common 1.13c batch. Register-call helper: EAX = delimiter set, ESI = char** cursor.
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <emscripten.h>

extern "C" {
// 1.13c D2Common+0x1B930: split the next token off *cur at a delimiter, advance cursor, return atol(token).
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_1b930(char** cur, const char* delims) {
 char* s = *cur;
 char* p = s + strcspn(s, delims);
 if (*p) { *p = 0; *cur = p + 1; } else *cur = nullptr;
 return static_cast<int32_t>(atol(s));
}
}
