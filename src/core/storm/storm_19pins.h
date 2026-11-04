// Pinned module words of the Storm DirectDraw front end (0x19040 / 0x19150 / 0x19520), copied in and out by Storm_19_FnRun
// (storm_batch_19040.cpp) for the FN capture probes (src/core/fn/storm_19x.h).
#pragma once
#include <cstdint>
struct Storm19Pins {
    int32_t err;                 // 0x55388
    void* display;               // 0x553E4
    void* pal;                   // 0x553E8
    int32_t capDlg;              // 0x553D4
    void* hwnd;                  // 0x553D8
    void* hpal;                  // 0x553DC
    int32_t rectLock;            // 0x557F0
    uint32_t rect[4];            // 0x557F4
    void* draw[4];               // 0x55804
    uint32_t palette[256];       // 0x553F0
};
