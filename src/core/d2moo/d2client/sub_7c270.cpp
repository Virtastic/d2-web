// D2Client sub_7C270: stat-id -> string label for the Chinese/Korean/Japanese-style locales.
// Register args: EAX = obj (kind@+0, tableIdx@+4), ESI = id. lang = Ordinal_10009 result (host-supplied).
// tbls = PTR_DAT_6fb924c0[] (0x4a x {u16 id,u16 val} each); tbl2 = DAT_6fb924dc (6 x {u16 id,u16 val}).
#include <cstdint>
#include <cstdio>

// 1.13c D2Client+0x7C270. The 7B470 text sink is a parameter (the probe routes it through the hook; the port's own 7B470 would run its body).
extern "C" void d2_client_7c270(uint8_t* obj, uint32_t id, uint32_t lang,
                                const uint16_t* const* tbls, const uint16_t* tbl2,
                                void (*sink_7b470)(uint8_t* obj, const char* s, uint32_t esi)) {
    if (!obj) return;
    if (!(lang > 5 && (lang < 8 || lang == 9) && id > 0xb75 && id < 0x1231)) return;
    int32_t kind = *reinterpret_cast<int32_t*>(obj);
    char buf[12];
    if (kind == 0) {
        const uint16_t* t = tbls[*reinterpret_cast<int32_t*>(obj + 4)];
        for (uint32_t i = 0; i < 0x4a; i++)
            if (t[i * 2] == id) {
                snprintf(buf, sizeof buf, "\xff\xff%d", t[i * 2 + 1]);  // "%c%c%d", -1, -1
                sink_7b470(obj, buf, id);
                return;
            }
    } else if (kind == 1) {
        for (uint32_t i = 0; i < 6; i++)
            if (tbl2[i * 2] == id) {
                snprintf(buf, sizeof buf, "%d", tbl2[i * 2 + 1]);
                sink_7b470(obj, buf, id);
                return;
            }
    }
}
