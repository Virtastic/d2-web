// Test seam: the gateway-access unit calls the runtime's strtoul by the engine's name; the full routine
// (batch_1555) needs the per-thread CRT data, so this test links a decimal-only stand-in.
#include <cstdint>
#include <cstdlib>
uint32_t bn_strtoul(const char* s, const char** end, int radix) {
    char* e = nullptr;
    unsigned long v = std::strtoul(s, &e, radix);
    if (end) *end = e;
    return (uint32_t)v;
}
