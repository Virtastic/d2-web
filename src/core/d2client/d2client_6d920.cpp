// 1.13c D2Client+0x6D920: EAX = unit (dword +4 = row index), stack = out block (ret 4),
// dt = *sgptDataTables. Row index in [0, dt+0xB6C): out+0x2C := (dt+0xB64 table base) + index * 0x1A4 + 0x138,
// skipped when the unit is null, the index is negative or out of range, or the row address is zero.
#include <cstdint>
#include <cstring>

extern "C" void d2c_port_6d920(const uint8_t* unit, const uint8_t* dt, uint8_t* out) {
    if (!unit) return;
    int32_t idx; memcpy(&idx, unit + 4, 4);
    int32_t count; memcpy(&count, dt + 0xB6C, 4);
    if (idx < 0 || idx >= count) return;
    uint32_t base; memcpy(&base, dt + 0xB64, 4);
    uint32_t row = (uint32_t)idx * 0x1A4u + base;
    if (!row) return;
    row += 0x138;
    memcpy(out + 0x2C, &row, 4);
}
