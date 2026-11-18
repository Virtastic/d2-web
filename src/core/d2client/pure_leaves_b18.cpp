#include <cstdint>
#include <cstddef>
#include <emscripten.h>

// 1.13c D2Client+0x1690: returns the dword at sgptDataTables+0xA80 (the caller passes the tables pointer's field).
// 1.13c D2Client+0x1740: empty.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_1690(uint32_t field_a80) { return field_a80; }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_1740() {}
