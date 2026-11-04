// src/core/d2launch/d2launch_n22_ui.h -- the UI-side host seams of src/core/d2launch/d2launch_batch_n22.cpp (shared with its FN probes).
#pragma once
#include <cstdint>

// UI-side seams (ordinals / FUN_ callees)
struct N22Ui {
    int32_t (*Ord10089)(int32_t ctl); void (*Ord10124)(int32_t ctl, int32_t v);  // 0x9A3C, 0x99FA (ECX = ctl, EDX = v)
    void (*Ord10147)(); void (*Ord10106)(); void (*Ord10022)(int);
    int (*Ord502)(); void (*Ord10003_fn)(); int (*Ord10227)();
    void (*ae60)(); void (*a8c0)(); void (*b600)(); void (*bda0)(); void (*c3f0)(); void (*cc50)();
    void (*cb00)(const char*, int); void (*fn5090)(const void*, void (*)(), int); void (*fn7df0)();
    void (*ord10003_str)(uint32_t id, void** out);   // D2Lang 10003 (ECX = id)
    uint32_t (*GetTickCount)();
    int32_t (*gate68)();                                  // (*(DAT_6fa656c4+0x68))()
    int32_t* dat_ec4; int32_t* dat_eb8; int32_t* dat_ebc; int32_t* dat_ee4; int32_t* dat_ad4;
    int32_t* list_ef4;                                    // DAT_6fa65ef4 chain head (nodes of 4 ints)
    void (*ordinal_10182)(int, int, int); int32_t* dat_ab0; int32_t* dat_ed0; int32_t* dat_db0;
    int32_t* dat_ed4; int32_t* dat_dbc; int32_t* dat_710; int32_t* dat_ec8;
    void (*ordinal_10131)();
    int32_t* dat_aa0;                                     // DAT_6fa65aa0 (the control 9BD0 passes in ECX)
};
