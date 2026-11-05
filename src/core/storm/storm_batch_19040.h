// Shared declarations of the Storm DirectDraw front end / SEVT command-table ports in storm_batch_19040.cpp, for the FN capture probes.
#pragma once
#include <cstdint>
#include <Windows.h>

struct StormBatch19040Env {
    // ---- unmapped internal callees, retail argument order --------------------------------------
    uint32_t  (*f_18f00)(void);                                  // 0x18F00 surface update pump
    void      (*f_18f90)(void);                                  // 0x18F90 default palette reset
    uint32_t  (*f_18fe0)(void* hwnd);                            // 0x18FE0 window-state test (EAX)
    void      (*f_1050)(void* p);                                // 0x1050 free with "delete" tag
    void      (*f_35fc0)(void* group);                           // 0x35FC0 free a bucket-group (EDI)
    void      (*f_365e0)(void* dst, void* src);                  // 0x365E0 clone a group (src in EBX)
    uint32_t* (*f_35c70)(uint32_t a, uint32_t b);                // 0x35C70 table lookup (ESI/EDX)
    uint32_t  (*f_1520)(void* node);                             // 0x1520 remove AUBREAKCMD node
    void      (*f_4d60)(void* node);                             // 0x4D60 list unlink
    int32_t   (*f_1b960)(void* p, const char* file, int32_t line); // 0x1B960 SMemFree
    uint32_t* (*f_1bdd0)(int32_t a, uint32_t b, int32_t c, uint32_t d, uint32_t e, uint32_t f);
    uint32_t* (*f_1c390)(uint32_t flags, uint32_t size);         // 0x1C390 pool allocate
    uint32_t  (*f_2f590)(uint32_t code, const char* msg, int32_t line, uint8_t* buf, int32_t a,
                         int32_t b);                             // 0x2F590 fatal error
    uint32_t  (*f_1ad80)(void);                                  // 0x1AD80 heap id
    void      (*f_1a600)(void);                                  // 0x1A600 heap swap check
    void      (*exit_process)(uint32_t code);                    // kernel32
    // ---- gdi32 / user32 imports ---------------------------------------------------------------
    int32_t   (*set_palette_entries)(void* pal, uint32_t start, uint32_t count, void* entries);
    void*     (*get_dc)(void* hwnd);
    void*     (*select_palette)(void* hdc, void* pal, int32_t background);
    int32_t   (*realize_palette)(void* hdc);
    int32_t   (*release_dc)(void* hwnd, void* hdc);
    int32_t   (*is_window_visible)(void* hwnd);
    int32_t   (*is_iconic)(void* hwnd);
    int32_t   (*invalidate_rect)(void* hwnd, void* rect, int32_t erase);
    // ---- optional seams (null = the direct call) ----------------------------------------------------
    uint32_t* (*f_1c580)(uint32_t size, const char* file, int32_t line, uint32_t flags); // SEVT allocations
    void      (*call_handler)(uint32_t handler, uint32_t arg);   // 0x35DD0: the registered handler call
};

// SEVT command table globals (defined in storm_batch_19040.cpp).
extern uint32_t g_evtTable;        // 0x6FC45370
extern uint32_t g_evtSize;         // 0x6FC45374
extern uint32_t g_evtCount;        // 0x6FC45378
extern CRITICAL_SECTION g_evtLock; // 0x6FC458C8
extern int32_t  g_evtDirty;        // 0x6FC42F58
extern uint32_t g_breakList;       // 0x6FC458C4
extern int32_t  g_evtDepth;        // 0x6FC4536C
extern uint32_t g_breakAnchor;     // 0x6FC458BC

extern "C" {
uint32_t Storm_Sub35DD0(uint32_t, uint32_t, uint32_t, uint32_t, const StormBatch19040Env*);
uint32_t Storm_Sub36020(uint32_t, uint32_t, const StormBatch19040Env*);
uint32_t Storm_Sub36120(uint32_t, uint32_t, const StormBatch19040Env*);
uint32_t Storm_Sub36290(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, const StormBatch19040Env*);
uint32_t Storm_Sub36680(uint32_t, uint32_t, const StormBatch19040Env*);
uint32_t* Storm_Sub1C580Env(uint32_t, const char*, int32_t, uint32_t, const StormBatch19040Env*);
}
