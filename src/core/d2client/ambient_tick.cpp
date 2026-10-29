// D2Client 0x4F860 wired into the client tick: the session-side Services for the ambient/weather sound updater.
// Retail calls it first thing in 0x5F020 with now = the frame counter (0 on the first call). It draws from the client hero seed
// (unit +0x20/+0x24, the same seed the weather pass steps), so the caller passes that seed. The sound-engine calls
// (0x88xxx) are not simulated; the f88a70 submits (EBX = Sounds row) are queued for the platform layer to drain.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "weather_sim.h"

#include "sub_4f860.h"

namespace {
using namespace d2client_4f860;
constexpr U kTbl = 0x1000, kRec = 0x58, kMaxRec = 64, kUD = 0x100000;
struct Amb final : Services {
    uint8_t tbl[kMaxRec * kRec] = {};
    U key = 0, period = 0, nrec = 0, nextHandle = 1;
    bool weatherLevel = false;
    int32_t vol = 0;
    uint32_t* seed = nullptr;   // {lo, hi}
    int32_t q[256 * 2]; int qn = 0;
    U rd32(U a) override {
        if (a == kUD + 0x21c) return key;
        if (a >= kTbl && a + 4 <= kTbl + sizeof tbl) { U v; memcpy(&v, tbl + (a - kTbl), 4); return v; }
        return 0;
    }
    U game(U) override { return 1; }
    U unit(U) override { return 1; }
    U act(U) override { return period; }
    U unitData(U) override { return kUD; }
    bool f80450() override { return weatherLevel; }
    bool weather(int32_t* scaled) override { *scaled = int32_t(double(weatherLevel ? d2client::weather::intensity() : 0.0f) * 1000.0); return true; }
    void f88810(U, U) override {}
    void f88770(U, U) override {}
    void f88230(U, U, U, U, U) override {}
    U f88a70(U row, U kind) override {
        if (qn < 256) { q[qn * 2] = int32_t(row); q[qn * 2 + 1] = int32_t(kind); ++qn; }
        return nextHandle++;
    }
    void f87ed0(U, U v) override { vol = int32_t(v); }
    U f87f00(U) override { return U(vol); }
    void f88bf0(U) override {}
    void f88ba0(U, int32_t, int32_t, int32_t) override {}
    U rand(U n) override {   // 0x89640: LCG on the hero seed
        if (!n) return 0;
        uint64_t v = uint64_t(seed[0]) * 0x6ac690c5ull + seed[1];
        seed[0] = uint32_t(v); seed[1] = uint32_t(v >> 32);
        return (n & (n - 1)) ? U(uint32_t(v) % n) : ((n - 1) & U(v));
    }
    void f5cf90(U) override {}
    void f4f750() override {}
    void f4f720(U) override {}
};
Amb A;
Env E{};
U Now = 0;
}

namespace d2client_ambient {
uint32_t Fb[2] = {0x00b63052u, 0x50f180c0u};
uint32_t* fallbackSeed() { return Fb; }
void reset() { Fb[0] = 0x00b63052u; Fb[1] = 0x50f180c0u; E = Env{}; Now = 0; A.qn = 0; A.nextHandle = 1; A.vol = 0; }
void setArea(U key) { A.key = key; }
// One 0x5F020 call. period = D2Common #10396; seed = the client hero seed.
void tick(uint32_t* seed, U period, bool weatherLevel) {
    A.seed = seed; A.period = period; A.weatherLevel = weatherLevel;
    E.bbfc = 1; E.c280 = 1; E.c3b8 = period; E.c254 = kTbl; E.c258 = A.nrec;
    sub_4F860(Now++, E, A);
}
}

extern "C" {
// SoundEnviron.txt row `idx`, in the in-memory 0x58 layout: +4 night amb, +8 day amb, +0xC night event, +0x10 day event, +0x14 delay.
EMSCRIPTEN_KEEPALIVE void d2_ambient_area(int key) { d2client_ambient::setArea(U(key)); }
EMSCRIPTEN_KEEPALIVE void d2_ambient_record(int idx, int nightAmb, int dayAmb, int nightEvt, int dayEvt, int delay) {
    if (idx < 0 || idx >= int(kMaxRec)) return;
    uint32_t* r = reinterpret_cast<uint32_t*>(A.tbl + idx * kRec);
    r[1] = nightAmb; r[2] = dayAmb; r[3] = nightEvt; r[4] = dayEvt; r[5] = delay;
    if (U(idx + 1) > A.nrec) A.nrec = idx + 1;
}
// Drain the queued submits: pairs {Sounds row, kind} (kind 0 area loop, 1 weather handle, 2 one-shot). Returns the pair count.
EMSCRIPTEN_KEEPALIVE int d2_ambient_take(int32_t* out, int cap) {
    int n = A.qn < cap ? A.qn : cap;
    memcpy(out, A.q, size_t(n) * 8);
    A.qn = 0;
    return n;
}
}
