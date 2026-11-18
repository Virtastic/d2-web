// BNClient.dll alpha-lane batch b59-bnc-a: two small rows transcribed from the cached decompile.
// The other ten claimed rows are register-argument (EAX/ECX) or have garbled stack decompiles and
// are skipped.
#include <cstdint>

// Storm.dll!#403 SMemFree(ptr, file, line, flags), as the retail thunk 0x801E calls it.
typedef void (*BnB59SmemFreeFn)(void* ptr, const char* file, uint32_t line, uint32_t flags);

// DAT_6FF3F640 / DAT_6FF3F63C: optional callbacks; DAT_6FF3F8D0 / DAT_6FF3F8DC: counters.
static void (*g_bn_cb_once)();       // DAT_6FF3F640
static void (*g_bn_cb_each)();       // DAT_6FF3F63C
static int32_t g_bn_news_count;      // DAT_6FF3F8D0
static int32_t g_bn_news_base;       // DAT_6FF3F8DC

// 1.13c BNClient+0x12810
void bn_sub_12810() {
    if (g_bn_cb_once) g_bn_cb_once();
    if (g_bn_cb_each) {
        for (int32_t n = g_bn_news_count - g_bn_news_base; n > 0; --n) g_bn_cb_each();
    }
}

// 1.13c BNClient+0x12EC0
// CACHE.CPP line 0x15A: frees the block; returns 0 for NULL, else 1.
uint32_t bn_sub_12EC0(void* p, BnB59SmemFreeFn smem_free) {
    if (!p) return 0;
    smem_free(p, "..\\Source\\D2BNClient\\CACHE.CPP", 0x15a, 0);
    return 1;
}
