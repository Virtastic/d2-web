// 1.13c D2Client+0x6EE40 (116 B, fastcall ecx unused, edx = 0x202-byte chat line): push a line into the
// 8-slot history ring. The five globals are passed in so the vector can pin them.
#include <emscripten.h>
#include <cstdint>
#include <cstring>
extern "C" void d2c_port_6ee40(int32_t* count, int32_t* head, int32_t* tail, int32_t* cur, uint8_t* ring, const uint8_t* e) {
    if (*count == 8) {
        int32_t t = *tail - 1;
        *head = *tail; *tail = t;
        if (t < 0) *tail = 7;
    } else {
        if (*count != 0 && (--*head) < 0) *head = 7;
        ++*count;
    }
    *cur = *head;
    memcpy(ring + *head * 0x202, e, 0x202);
}
