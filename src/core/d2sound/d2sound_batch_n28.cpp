// D2sound CRT batch n28: 27BD, 28DC, D0CE, 47B4. External state via seams.
#include <cstdint>
#include <cstddef>

struct n28_Env {
    void (*initCs)(void*);                 // InitializeCriticalSection
    void* (*heapAlloc)(size_t);            // D2sound+0x2861
    int (*callNewH)(size_t);               // D2sound+0x403F
    void (*fn5623)(int);                   // D2sound+0x5623
    void (*exitProcess)(uint32_t);
};
static n28_Env* g_n28;
void n28_SetEnv(n28_Env* e) { g_n28 = e; }

// 1.13c D2sound+0x27BD
uint32_t n28_crtInitCritSecNoSpinCount(void* cs) { g_n28->initCs(cs); return 1; }

// 1.13c D2sound+0x28DC
void* n28_nh_malloc(size_t size, int nhFlag) {
    if (size < 0xffffffe1u) {
        do {
            void* p = g_n28->heapAlloc(size);
            if (p) return p;
            if (nhFlag == 0) return nullptr;
        } while (g_n28->callNewH(size) != 0);
    }
    return nullptr;
}

// 1.13c D2sound+0xD0CE
uint32_t n28_sptype(int lo, uint32_t hi) {
    if (hi == 0x7ff00000) { if (lo == 0) return 1; }
    else if (hi == 0xfff00000 && lo == 0) return 2;
    if (((hi >> 16) & 0x7ff8) == 0x7ff8) return 3;
    if ((((hi >> 16) & 0x7ff8) != 0x7ff0) || ((hi & 0x7ffff) == 0 && lo == 0)) return 0;
    return 4;
}

// 1.13c D2sound+0x47B4
void n28_report_failure() { g_n28->fn5623(1); g_n28->exitProcess(3); }
