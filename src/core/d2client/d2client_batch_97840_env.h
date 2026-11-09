#pragma once
#include <cstdint>
struct D2cClientEnv {
    void (*sub_97770)();                                 // 0x97770
    void (*ord10065)(uint32_t);                          // D2Gfx/Win ordinal 10065
    void (*ord10043)(uint32_t, uint32_t);                // free(line, zero): 0x7A hud handle at 0x97840 (node-less, see free10043)
    uint32_t (*ord10227)();                              // client-ready query
    uint32_t (*ord10265)(uint32_t);                      // string table lookup
    void (*ord10024)(const void*, uint32_t);             // fatal message
    void (*exit_)(int);                                  // _exit(-1)
    uint32_t (*ord10048)();                              // panel owner
    void (*ord10006)();
    void (*sub_72790)(uint32_t, uint32_t, uint32_t);     // 0x72790
    void (*sub_47b20)();                                 // 0x47B20 (listed callee, not reached on read path)
    void (*sub_f7b20)();                                 // FUN_6faf7b20
    uint32_t (*sub_55440)();                             // 0x55440
    void (*sub_eccc0)(uint32_t, const void*);            // open notify
    void (*sub_ecc60)(uint32_t, const void*);            // close notify
    void (*sub_3bbf0)();                                 // 0x3BBF0 (97CD0 close only)
    void (*sub_3a4d0)(int32_t, int32_t, int32_t);        // 0x3A4D0 layout
    void (*draw)(const uint32_t* ctx, int32_t x, int32_t y, int32_t color, int32_t mode, int32_t pal);  // ord 10041
    void (*sub_408c0)();  void (*sub_40eb0)();  void (*sub_3c8b0)();  void (*sub_3ea20)();
    void (*sub_3b2f0)(uint32_t);  void (*sub_452a0)();  void (*sub_3e8e0)();
    void (*sub_43450)(uint32_t, uint32_t);  uint32_t (*sub_6eaf0)();
    // Fog ordinal 10043 as retail calls it: ecx = allocation, edx = source file (RVA), stack = line, zero.
    void (*free10043)(uint32_t allocation, uint32_t file_rva, uint32_t line, uint32_t zero);
};
