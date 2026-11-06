// 1.13c D2Launch+0x128D0: the Aux Battle.net flag. Reads the registry string "Diablo II"/"Aux Battle.net"
// (Storm Ordinal_422, 100-byte buffer), parses "%d.%d.%d.%d" into q[0..3] with q[3] preset to 0x22; returns bit 7 of ~q[3] when q[3] bit 4
// is set, else 0 (also 0 when the registry read yields nothing). Both callees are seams so a replay probe pins them.
#include <cstdint>
extern "C" uint32_t d2l_128d0(int32_t (*reg422)(const char* key, const char* val, int32_t type, char* buf, int32_t cap),
                              int32_t (*scan)(const char* buf, const char* fmt, int32_t* q)) {
    char buf[100];
    int32_t q[4];
    if (!reg422("Diablo II", "Aux Battle.net", 0, buf, 100)) return 0;
    q[3] = 0x22;
    scan(buf, "%d.%d.%d.%d", q);
    const uint8_t al = (uint8_t)q[3];
    if (!(al & 0x10)) return 0;
    return (uint32_t)(uint8_t)~al >> 7;
}
