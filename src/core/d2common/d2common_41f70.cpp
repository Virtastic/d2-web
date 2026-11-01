// 1.13c D2Common+0x41F70
// Item-name lookup front end: a 12-byte local is zeroed, the nine-argument formatter 0x43C30 (the five stack arguments, then the name
// buffer local+4, the local itself, 0 and 1) fills the name, 0x41DD0 looks that name up in the 256-bucket table, and the row is returned,
// or the table global + 0x404 when there is none. `pTable` is the table global's value; tokens are 32-bit, `resolve` turns one into an address.
#include <cstdint>
#include <cstring>

extern "C" uint32_t D2Common_41DD0(char* pName, const uint8_t* pTable, const void* (*resolve)(uint32_t));

extern "C" uint32_t D2Common_41F70(uint32_t p1, uint32_t a, uint32_t b, uint32_t c, uint32_t out,
                                   uint32_t tableToken, const uint8_t* pTable,
                                   void (*format)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, char*, uint8_t*, uint32_t, uint32_t),
                                   const void* (*resolve)(uint32_t))
{
    uint8_t local[12];
    std::memset(local, 0, sizeof local);
    format(p1, a, b, c, out, reinterpret_cast<char*>(local + 4), local, 0, 1);
    uint32_t row = D2Common_41DD0(reinterpret_cast<char*>(local + 4), pTable, resolve);
    return row ? row : tableToken + 0x404;
}
