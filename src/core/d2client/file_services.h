#pragma once
#include <cstdint>
// Native owner bindings. The read service is synchronous and is not Fog #10104.
struct D2ClientFileServices {
    int (*readSynchronous)(void*, void*, uint32_t, uint32_t*);
    uint32_t (*size)(void*, uint32_t*);
    uint32_t (*close)(void*);
    int (*fileName)(void*, char*, uint32_t);
    void (*fatal)(uint32_t);
    uint32_t (*open)(const char*, void**);
    uint32_t (*lastError)();
    void (*reportOpenError)(const char*, const char*);
    void* (*allocate)(uint32_t, const char*, uint32_t, uint32_t);
};
const D2ClientFileServices& d2_client_native_file_services();
void* d2_client_load_archive(const char*, uint32_t*);
extern "C" {
void d2_client_d520(void*, uint32_t, void*, const D2ClientFileServices*);
uint32_t d2_client_d610(void*, uint32_t*, const D2ClientFileServices*);
uint32_t d2_client_d670(void*, const D2ClientFileServices*);
uint32_t d2_client_d6a0(const D2ClientFileServices&, const char*, void**, uint32_t);
void* d2_client_d810(const D2ClientFileServices&, const char*, uint32_t*);
}

// #10016 returns a path, D810 returns a native allocation, and #10006's
// output is a native pointer slot. Never serialize these as retail u32 tokens.
// The caller must supply the CMP normalizer: no native owner is implemented yet.
struct D2ClientBatch2b420Env {
    const char* (*ordinal10070)();
    const char* (*ordinal10016)(const char*);
    void* (*helper_bd810)(const char*, uint32_t*) = d2_client_load_archive;
    void (*ordinal10006)(void*, void**, const char*, uint32_t, uint32_t, char*);
};
