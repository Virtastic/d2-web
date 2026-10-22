// 1.13c Storm+0x3BDD0: SReg raw value query. Registry access is host-seamed (stateful, no vector).
#include <cstdint>
#include <cstring>

struct StormRegEnv {
    uint32_t (*open)(uint32_t hive, const char* path, void** key);   // RegOpenKeyExA(KEY_READ), err code
    uint32_t (*query)(void* key, const char* name, uint32_t* type, uint8_t* data, uint32_t* size);
    void (*close)(void* key);
    void (*set_last_error)(uint32_t);
};

// 1.13c Storm+0x3BDD0
extern "C"
uint32_t Storm_SRegQueryRaw(StormRegEnv& e, const char* key, const char* name, uint8_t flags,
                            uint32_t* type, uint8_t* data, uint32_t size, uint32_t* length) {
    char path[0x104];
    void* h = nullptr;
    uint32_t err = 0x3f3;
    *length = 0;
    path[0] = 0;
    if (!(flags & 0x10)) {
        const char* root = (flags & 2) ? "Software\\Battle.net\\" : "Software\\Blizzard Entertainment\\";
        std::strncpy(path, root, 0x103); path[0x103] = 0;
    }
    size_t n = std::strlen(path);
    if (n < 0x103) { std::strncpy(path + n, key, 0x103 - n); path[0x103] = 0; }
    if (!(flags & 4) && (err = e.open(0x80000001u, path, &h)) == 0) {
        *length = size;
        err = e.query(h, name, type, data, length);
        e.close(h);
        if (err == 0) return 1;
    }
    if (!(flags & 1)) {
        err = e.open(0x80000002u, path, &h);
        if (err != 0) { e.set_last_error(err); return 0; }
        *length = size;
        err = e.query(h, name, type, data, length);
        e.close(h);
    }
    if (err == 0) return 1;
    e.set_last_error(err);
    return 0;
}
