// The MPQ hash and decrypt path of the engine's archive reads runs retail Storm's rows in place of
// StormLib's own: wasm-build/link-d2.sh links with -Wl,--wrap=<name> for each StormLib symbol below,
// so every call StormLib makes to it (the hash-table lookup through ha->pfnHashString, the table and
// sector decryption, the file key) lands here.
//   crypt table   Storm+0x27FC0 (seed 0x100001, five 256-dword slices)
//   HashString    Storm+0x25ED0 (StormLib's hash type is the slice offset: type >> 8 is retail's index)
//   DecryptBlock  Storm+0x25F30
// StormLib's InitializeMpqCryptography still fills its private StormBuffer for the encrypt and
// key-detection paths retail D2 never takes; the two tables hold the same values.
#include <cstdint>

uint32_t Storm_Sub25ED0(const uint32_t* tbl, int32_t type, const char* s);
uint32_t Storm_Sub27FC0(uint32_t* t, uint32_t seed);
extern "C" void Storm_Sub25F30(uint32_t key, void* buffer, uint32_t bytes, const uint32_t* cryptTable);

namespace {
// Retail indexes the slice with a signed char: a byte >= 0x80 reads up to 0x80 dwords below the
// slice. The pad keeps that read inside this object (zero, where retail reads its own data segment).
struct CryptTable {
    uint32_t pad[0x80];
    uint32_t t[0x500];
};
const uint32_t* cryptTable() {
    static CryptTable c = [] {
        CryptTable k{};
        Storm_Sub27FC0(k.t, 0x100001);
        return k;
    }();
    return c.t;
}
}

// StormLib DWORD HashStringSlash(const char*, DWORD): the default ha->pfnHashString. Retail
// uppercases a-z only and leaves '/' as it is, the same rule.
extern "C" uint32_t __wrap__Z15HashStringSlashPKcj(const char* name, uint32_t hashType) {
    return Storm_Sub25ED0(cryptTable(), static_cast<int32_t>(hashType >> 8), name);
}

// StormLib DWORD HashString(const char*, DWORD): only the "(hash table)" / "(block table)" keys reach
// it outside DecryptFileKey; neither has a '/', the one byte StormLib maps and retail does not.
extern "C" uint32_t __wrap__Z10HashStringPKcj(const char* name, uint32_t hashType) {
    return Storm_Sub25ED0(cryptTable(), static_cast<int32_t>(hashType >> 8), name);
}

// StormLib void DecryptMpqBlock(void*, DWORD bytes, DWORD key).
extern "C" void __wrap__Z15DecryptMpqBlockPvjj(void* data, uint32_t bytes, uint32_t key) {
    Storm_Sub25F30(key, data, bytes, cryptTable());
}

// StormLib DWORD DecryptFileKey(const char*, ULONGLONG pos, DWORD size, DWORD flags): the leaf name
// (after the last '\\' or '/') hashed under slice 3; MPQ_FILE_KEY_V2 (0x20000) adds the low 32 bits
// of the file's archive offset and XORs the size.
extern "C" uint32_t __wrap__Z14DecryptFileKeyPKcyjj(const char* name, uint64_t pos, uint32_t size, uint32_t flags) {
    const char* leaf = name;
    for (const char* p = name; *p; ++p)
        if (*p == '\\' || *p == '/') leaf = p + 1;
    uint32_t key = Storm_Sub25ED0(cryptTable(), 3, leaf);
    if (flags & 0x00020000u) key = (key + static_cast<uint32_t>(pos)) ^ size;
    return key;
}
