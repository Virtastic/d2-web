// 1.13c BNClient+0xEA20 -- "idle and ready" test: reads DAT_1F1F4 under the 0x1F1DC critical section (a no-op on our
// single thread), then answers 1 only when DAT_1F644 is 0, the value read is 0 and DAT_1F654 is non-zero.
#include <cstdint>
__attribute__((weak)) uint32_t g_bnclient_1f1f4;
__attribute__((weak)) uint32_t g_bnclient_1f644;
__attribute__((weak)) uint32_t g_bnclient_1f654;
namespace bnclient {
uint32_t fea20()
{
    const uint32_t pending = g_bnclient_1f1f4;
    return (g_bnclient_1f644 == 0 && pending == 0 && g_bnclient_1f654 != 0) ? 1 : 0;
}
}
