#pragma once

#include <cstdint>

// Native pointers address a 36-byte retail image; its pointer-shaped slots stay u32.
void* d2_bnclient_gateway_access_assign(void* destination, const void* source);

// The Storm/kernel32/Fog imports the BNGatewayAccess functions reach, as host callbacks (rows s3622-s3635).
struct BnetGatewayHost {
    void (*log_printf)(const char*, ...);                                                     // 0x17420
    void (*log_indent)(int32_t, uint32_t);                                                    // 0x17480
    void (*mem_zero)(void*, uint32_t);                                                        // Storm #494 0x806C
    int (*sfile_load_file)(const char*, void**, int32_t*, int32_t, int32_t);                  // Storm #279 0x803C
    int (*sreg_load_data)(const char*, const char*, uint32_t, uint32_t, uint32_t, uint32_t*); // Storm #421 0x8054
    int (*sreg_save_data)(const char*, const char*, uint32_t, const void*, uint32_t);         // Storm #424 0x8066
    int (*sreg_delete_value)(const char*, const char*, uint32_t);                             // Storm #428 0x805A
    uint32_t (*mem_alloc)(uint32_t, const char*, uint32_t, uint32_t);                         // Storm #401 0x8018
    int (*mem_free)(uint32_t, const char*, uint32_t, uint32_t);                               // Storm #403 0x801E
    uint32_t (*sstr_copy)(char*, const char*, uint32_t);                                      // Storm #501 0x804E
    uint32_t (*sstr_len)(const char*);                                                        // Storm #506 0x8042
    int32_t (*sstr_cmpi)(const char*, const char*, uint32_t);                                 // Storm #509 0x8060
    uint32_t (*get_time_zone_information)(void*);                                             // kernel32
    uint16_t (*get_user_default_lang_id)();                                                   // kernel32
    uint32_t (*caller_return_address)(uint32_t);                                              // Fog #10265 0x80E4
    void (*display_halt)(uint32_t, uint32_t);                                                 // Fog #10024 0x80DE
    void (*terminate)(int32_t);                                                               // 0x1569 _exit
};

int32_t bn_get_system_time_zone(const BnetGatewayHost& host);
