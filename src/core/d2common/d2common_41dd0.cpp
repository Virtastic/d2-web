// 1.13c D2Common+0x41DD0
// Name lookup in the 256-bucket table at the table global: the name is upper-cased in place, its byte sum
// (mod 256) picks the bucket, and the bucket's rows (0xA0 bytes each, a count dword first) are compared with 0x41D00.
// The name arrives in EBX in retail; the table is a block of 32-bit tokens, so `resolve` turns a token into an address.
#include <cstdint>
#include <cstring>

extern "C" bool D2Common_41D00(const char* pNameA, const char* pNameB);

extern "C" uint32_t D2Common_41DD0(char* pName, const uint8_t* pTable, const void* (*resolve)(uint32_t))
{
    for (char* c = pName; *c; ++c)
        if (*c >= 'a' && *c <= 'z') *c = static_cast<char>(*c - 0x20);
    uint8_t hash = 0;
    for (const char* c = pName; *c; ++c) hash = static_cast<uint8_t>(hash + static_cast<uint8_t>(*c));
    uint32_t bucket; std::memcpy(&bucket, pTable + 4 + hash * 4, 4);
    int32_t count; std::memcpy(&count, resolve(bucket), 4);
    uint32_t row = bucket + 4;
    for (; count > 0; --count, row += 0xA0)
        if (D2Common_41D00(pName, static_cast<const char*>(resolve(row)))) return row;
    return 0;
}
