// src/core/d2sound/d2sound_batch_8750.cpp -- 1.13c D2Sound.dll helpers from the decompiles (0x8750, 0x8C70, 0x8EB0).
// Retail passes the record / HRESULT in EAX; here they are explicit arguments. DAT_ globals are host seams.
// Not ported (no faithful C form): 0x18FE (CRT __getptd), 0x6070/0x7CD0 (implicit regs, unported callees),
// 0x8DB0/0x8FD0 (lost call args / unported 0x87D0), 0x9D4D (no decompile), 0xA509/0xB2D0 (x87 thunks).
#include <cstdint>
#include <cstring>

static inline void wr16(void* p, uint32_t off, uint16_t v) { std::memcpy((uint8_t*)p + off, &v, 2); }
static inline void wr32(void* p, uint32_t off, uint32_t v) { std::memcpy((uint8_t*)p + off, &v, 4); }

// 1.13c D2sound+0x8750
// Fill an 18-byte WAVEFORMATEX: PCM, `channels`, 22050 Hz, 16-bit.
void D2sound_8750(void* wfx, int16_t channels) {
    std::memset(wfx, 0, 18);
    wr16(wfx, 2, (uint16_t)channels);
    wr16(wfx, 12, (uint16_t)(channels * 2));
    wr16(wfx, 0, 1);
    wr32(wfx, 4, 0x5622);
    wr16(wfx, 14, 0x10);
    wr32(wfx, 8, (uint32_t)(uint16_t)(channels * 2) * 0x5622);
}

// 1.13c D2sound+0x8C70
// DirectSound HRESULT -> name; NULL when unknown.
const char* D2sound_8C70(int32_t hr) {
    switch (hr) {
    case 0: return "DS_OK";
    case -0x7787ffce: return "DSERR_INVALIDCALL";
    case -0x7ff8fff2: return "DSERR_OUTOFMEMORY";
    case -0x7fffbffb: return "DSERR_GENERIC";
    case -0x7fffbfff: return "DSERR_UNSUPPORTED";
    case -0x7fffbffe: return "DSERR_NOINTERFACE";
    case -0x7ffbfef0: return "DSERR_NOAGGREGATION";
    case -0x7ff8ffa9: return "DSERR_INVALIDPARAM";
    case -0x7787fff6: return "DSERR_ALLOCATED";
    case -0x7787ffe2: return "DSERR_CONTROLUNAVAIL";
    case -0x7787ff6a: return "DSERR_BUFFERLOST";
    case -0x7787ffba: return "DSERR_PRIOLEVELNEEDED";
    case -0x7787ff9c: return "DSERR_BADFORMAT";
    case -0x7787ff88: return "DSERR_NODRIVER";
    case -0x7787ff7e: return "DSERR_ALREADYINITIALIZED";
    case -0x7787ff60: return "DSERR_OTHERAPPHASPRIO";
    case -0x7787ff56: return "DSERR_UNINITIALIZED";
    }
    return nullptr;
}

// 1.13c D2sound+0x8EB0
// Release (vtable slot 2) and clear the three COM globals DAT_6f9c64b8 / 6524 / 6528.
void D2sound_8EB0(void** slots[3]) {
    for (int i = 0; i < 3; i++)
        if (void* itf = *slots[i]) { (*(void (**)(void*))(*(void***)itf + 2))(itf); *slots[i] = nullptr; }
}
