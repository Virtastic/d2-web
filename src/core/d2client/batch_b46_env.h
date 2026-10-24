#pragma once
#include <cstdint>
struct B46Env {
    // tables and globals (host-resolved addresses)
    uint8_t *tbl2268, *tbl21a8, *tbl22b0, *tbl22bc;   // 0xC strides
    uint8_t *dataTables;                              // *sgptDataTables
    int32_t *g_bbfc;                                  // DAT_6fbbcbbfc (player unit ptr slot value)
    int32_t g_cc0a0; int8_t g_c5591; int32_t g_cc300, g_cc304;
    int32_t *g_cc300p, *g_cc304p; const uint32_t *k38f4, *k7578; int32_t g_bf78;
    // callees
    uint8_t* (*f1260)(); int32_t (*o10707)(); uint8_t (*o11077)(); int32_t (*o10331)(); int32_t (*o10826)(int32_t);
    int32_t (*o10265)(int32_t); void (*o10024)(int32_t); void (*exit_)(int32_t);
    int32_t* (*f58c20)(uint32_t); void (*f55420)(); void (*f54b00)(uint32_t);
    void (*f55fb0)(); void (*o10770)(int32_t); void (*f2a6c0)();
    int32_t (*o10426)(); void (*o10853)(); void (*fdc70)(int32_t, int32_t*, int32_t*);
    int8_t (*o10215)(int32_t); void (*o10499)(int32_t);
    void (*o10065)(int32_t); void (*o10043)(int32_t, int32_t);
    int32_t (*o10651)(); int32_t (*o11142)();
};
