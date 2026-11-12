// 1.13c Storm+0x149E0: wait until every id in ids[] (EBX, ESI = count) has its ack flag set in the 16-word table at
// Storm+0x45178, pumping the three network sweeps (0x124B0, 0x14250, 0x13170) per pass. Stack (any, timeout), ret 8.
// The table is zeroed on entry; a pass with n == 0 returns 1, all n set returns 1, any set with any == 0 returns 1;
// timeout != 0xFFFFFFFF expires once GetTickCount() - start >= timeout (0). Sleep(10) precedes every pass but the first.
// GetTickCount, Sleep and the pumps are seams (host clock and network state).
#include <cstdint>

struct Storm149E0Seams {
    uint32_t (*tick)();
    void (*sleep)(uint32_t ms);
    void (*pump124b0)();
    void (*pump14250)();
    void (*pump13170)();
};

uint32_t g_storm_ack_flags[16];   // Storm+0x45178

extern "C" uint32_t Storm_Sub149E0(const uint32_t* ids /*ebx*/, uint32_t n /*esi*/, int32_t any, uint32_t timeout,
                                   const Storm149E0Seams& s) {
    for (auto& f : g_storm_ack_flags) f = 0;
    const uint32_t t0 = s.tick();
    for (bool first = true;; first = false) {
        if (!first) s.sleep(10);
        s.pump124b0();
        s.pump14250();
        s.pump13170();
        if (n == 0) return 1;
        uint32_t hit = 0;
        for (uint32_t i = 0; i < n; ++i)
            if (ids[i] < 16 && g_storm_ack_flags[ids[i]] != 0) ++hit;
        if (n <= hit) return 1;
        if (hit != 0 && any == 0) return 1;
        if (timeout != 0xFFFFFFFFu && timeout <= s.tick() - t0) return 0;
    }
}
