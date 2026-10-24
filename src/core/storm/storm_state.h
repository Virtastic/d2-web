#pragma once
// Shared Storm module state (native owner). Retail identities in comments.
#include <cstdint>

// Log stream node: retail layout is {id,next,file,used,position,...,data[0x10000]} (0x1001C).
struct StormLog3b1e0 {
    uint32_t id;
    StormLog3b1e0* next;
    void* file;
    uint32_t used, position, field14, field18;
    uint8_t data[0x10000];
};

extern StormLog3b1e0* g_stormLogs[4];        // 0x6FC42FC0 bucket heads (id & 3)

// 1.13c Storm+0x349E0: flush every log stream. Retail passes ECX (scratch) as the WriteFile
// bytes-written slot; it has no other use.
extern "C" void Storm_FlushLogs_349E0(uint32_t scratch);

// SErr state shared by b54b / 3c2b0 / storm_serr.cpp: one copy, defined in storm_serr.cpp.
struct StormSrcNode { uint16_t id; void* source; StormSrcNode* next; };  // retail 12 bytes {u16,hmod,next}
extern int32_t g_stormLockInit;          // 0x6FC415F8  lazy-init counter
extern char g_stormStringBuf[0x100];     // 0x6FC45AA0  LoadString scratch
extern uint32_t g_stormMod;              // 0x6FC45390  module for STRINGTABLE / message sources
extern int32_t g_stormLastError;         // 0x6FC45388
extern StormSrcNode* g_stormSrcList;     // 0x6FC4537C
extern uint32_t g_stormSrcInit;          // 0x6FC45380

extern "C" void Storm_SErrLock_2F180();                     // 0x2F180
extern "C" const char* Storm_SErrString_2F1C0(int32_t idx); // 0x2F1C0, idx = ESI
extern "C" void Storm_SErrAddSource_2F200(uint16_t id);     // 0x2F200
extern "C" uint32_t Storm_SErrGetErrorStr_2F3F0(uint32_t msgId, char* buf, uint32_t count);
