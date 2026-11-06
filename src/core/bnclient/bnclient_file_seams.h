// Seams shared by the BNClient file helpers (0xCF10, 0xCFA0, 0xD160, 0xD280): the Fog file thunks and the kernel32 file-time calls.
#pragma once
#include <cstdint>
struct BnFileSeams {
    uint32_t (*open)(const char* name, uint32_t access, uint32_t share, uint32_t sa, uint32_t disp, uint32_t flags, uint32_t tmpl);  // 0x8138 Fog 10107
    uint32_t (*close)(uint32_t h);                                                                                                      // 0x8144 Fog 10109
    uint32_t (*write)(uint32_t h, const uint8_t* buf, uint32_t n, uint32_t* written, uint32_t ov);                                       // 0x814A Fog 10111
    uint32_t (*seek)(uint32_t h, uint32_t pos, uint32_t hi, uint32_t method);                                                            // 0x8132 Fog 10112
    uint32_t (*get_time)(uint32_t h, uint32_t* create, uint32_t* access, uint32_t* write);                                              // IAT 0x19084 GetFileTime
    uint32_t (*set_time)(uint32_t h, const uint32_t* create, const uint32_t* access, const uint32_t* write);                            // IAT 0x1907C SetFileTime
    uint32_t (*size)(uint32_t h, uint32_t* hi);                                                                                         // IAT 0x19198 GetFileSize
};
