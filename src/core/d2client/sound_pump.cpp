// Session-replay driver for the D2Client sound pump (1.13c 0x5F020), emitting our engine's D2Sound call records in the
// retail A-capture line shape ("<tick> A <ordinal> <ecx>,<edx>[ stack]") so D2_SESSION_FULL=1 can compare them.
// What runs: 0x45660 (music) then 0x88DD0 (sound-entry tick) from their ports, with an empty entry list. Not run here:
// 0x4F860 (ambient), 0x5ED90 (the Sounds.txt sweep) and 0x5D080 (slot loop) -- unported/needing the Sounds table -- and
// the init (tick 0) and tick-1 sweep calls, so those retail A records stay unmatched. D2Sound's volume (10001) answers 0.
// A register value >= 0x100000 prints as 0 (the capture's rule); EDX of 10001/10063 is the pump's leftover `now`.
#include <emscripten.h>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include "cl_env.h"
#include "d2client_5dfd0.h"
#include "d2client_batch_5ebc0.h"
#include "../tables/shared.h"
namespace d2client_batch_5ebc0 { void sub_5ED90(Svc& s, G& g, int32_t now); }
namespace d2client_45660 {
struct G { uint32_t *player, *c340, *c344, *c348, *c34c, *c350, *c354, *c358, *c35c, *c360, *c364, *c368, *c36c, *c370, *c374,
    *tbl254, *cnt258, *lo25c, *hi260, *tick97f8, *arr75b8; };
struct Calls {
    uint32_t (*ord10331)(uint32_t); uint32_t (*ord10826)(uint32_t); const uint8_t* (*ord10014)(uint32_t);
    uint32_t (*ordD270)(); uint32_t (*ordD26A)();
    void (*f388b0)(); void (*f38a70)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
    uint32_t (*f55440)(uint32_t); void (*f38930)(uint32_t);
    uint32_t (*f37d20)(uint32_t); uint32_t (*f37af0)(); uint32_t (*f37de0)(uint32_t); uint32_t (*f38080)(uint32_t);
    const uint8_t* (*f0da50)(uint32_t); void (*ad3e20)();
};
void sub_45660(uint32_t now, G& g, Calls& c);
}
extern "C" void d2c_88dd0(uint32_t now, const uint32_t* gb, const cl::Env& e);
namespace {
std::string out;
uint32_t curTick, curEdx;
uint32_t ordOf(uint32_t rva) {
    switch (rva) {
        case 0xd270: return 10001; case 0xd25e: return 10063; case 0xd258: return 10062; case 0xd2ac: return 10018;
        case 0xd276: return 10034; case 0xd26a: return 10011; case 0xd2a6: return 10061; case 0xd2a0: return 10041;
    }
    return 0;
}
void logA(uint32_t ord, uint32_t ecx, uint32_t edx) {
    char b[64];
    snprintf(b, sizeof b, "%u A %u %x,%x\n", curTick, ord, ecx >= 0x100000 ? 0u : ecx, edx >= 0x100000 ? 0u : edx);
    out += b;
}
// The Sounds.txt sweep's service seam (0x5ED90 -> 0x5EBC0): the file reads are the shared VFS, the D2Sound calls are logged.
struct SweepSvc : d2client_batch_5ebc0::Svc {
    d2client_batch_5ebc0::G* g = nullptr;
    int32_t idx = 0; std::vector<uint8_t> file; bool have = false;
    uint8_t* rec() { return g->cc24c + (size_t)idx * 0x8e; }
    void f5da80(uint8_t*) override {}
    void f5db30(int32_t i, char* o) override {   // 0x5DB30: DATA\GLOBAL|LOCAL \SFX|MUSIC \ filename
        idx = i; have = false; std::string p = (i >= 0xb76 && i <= 0x1230) ? "DATA\\LOCAL" : "DATA\\GLOBAL";
        p += (i >= 0x1231 && i <= 0x125a) ? "\\MUSIC\\" : "\\SFX\\";
        p += (const char*)(g->cc24c + (size_t)i * 0x8e);
        if (p.size() >= 0x50) p.clear();
        strncpy(o, p.c_str(), 79); o[79] = 0;
        have = !p.empty() && d2::sharedVfs().read(p.c_str(), file);
    }
    void f5dae0() override { logA(10062, 0, (uint32_t)idx); logA(10018, 0, (uint32_t)idx); }
    void f5cd10() override {                        // 0x5CD10: parse the wav header, D2Sound 10025 on 22050 Hz / 16 bit
        uint8_t* r = rec(); const uint8_t* w = file.data();
        auto u16 = [&](size_t q) { return (uint32_t)(w[q] | w[q + 1] << 8); };
        auto u32 = [&](size_t q) { return u16(q) | u16(q + 2) << 16; };
        if (file.size() >= 36 && !memcmp(w, "RIFF", 4) && !memcmp(w + 12, "fmt ", 4) && u32(24) == 22050 && u16(34) == 16) {
            uint32_t one = 1, zero = 0; memcpy(r + 0x74, &one, 4); r[0x50] = u16(22) == 2; memcpy(r + 0x78, &zero, 4);
            out += std::to_string(curTick) + " A 10025 0,0,*\n";
        } else r[0x81] = 1;
    }
    int32_t f5ea90(int32_t, int32_t, int32_t, int32_t) override { return 0; }
    int32_t fabd810(int32_t h, int32_t* sz) override { *sz = (int32_t)file.size(); return h; }
    int32_t o10102() override { return have; }
    int32_t o10105() override { return (int32_t)file.size(); }
    void o10103() override {}
    int32_t o10016() override { return idx + 1; }
    int32_t o10091() override { return 0; }
    int32_t o10095() override { return 0; }
    int32_t o10092(int32_t) override { return 0; }
    void f5ebc0(int32_t i, int32_t eax, int32_t p1) override { d2client_batch_5ebc0::sub_5EBC0(*this, *g, i, eax, p1); }
    void fatal302() override {}
    int32_t o10190(int, int, int) override { return 0; }
    void o10065(int32_t) override {} void free(int32_t, void*) override {}
    void o10010(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) override {}
    void* alloc(int32_t, int32_t, int32_t) override { return nullptr; }
    void o10112() override {} void o10111(int32_t, void*, int32_t) override {}
};
uint32_t hostRd(void*, uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
void hostWr(void*, uint32_t a, uint32_t v) { memcpy((void*)(uintptr_t)a, &v, 4); }
uint32_t hostH(void*, uint32_t rva, uint32_t, uint32_t ecx, uint32_t edx, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t) {
    const uint32_t o = ordOf(rva);
    if (o) logA(o, ecx, (o == 10001 || o == 10063) ? curEdx : edx);
    return 0;   // D2Sound volume 0, no sound handle
}
uint32_t d270() { logA(10001, 0, curEdx); return 0; }
uint32_t zero1(uint32_t) { return 0; }
uint32_t one1(uint32_t) { return 1; }
uint32_t zero0() { return 0; }
void nop0() {}
void nop5(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t) {}
void nop1(uint32_t) {}
const uint8_t* zbuf(uint32_t) { static uint8_t z[0x400]; return z; }
bool armed = false, inited = false;
d2client_batch_5ebc0::G sg; SweepSvc sw;
uint32_t ambTick = 0;
void initOnce() {
    if (inited) return;
    inited = true;
    d2client_sounds::load();                                   // 0x5DFD0
    sg.cc24c = d2client_sounds::records(); sg.cc250 = d2client_sounds::recordCount(); sg.cc278 = 0x500000; sw.g = &sg;
    curTick = 0;                                               // tick-0 init: 0x5EF90, 0x5D730, 0xFF7C0 (hard-wired record sequence)
    logA(10041, 0, 0); logA(10059, 0, 0x70); logA(10041, 0, 0x70); logA(10065, 1, 0); logA(10041, 1, 0); logA(10065, 1, 0); logA(10061, 0xff, 0x70);
}
}
namespace d2client_ambient { void useEnvTable(); bool keyMoved(); void setArea(uint32_t); }
extern "C" int d2_ambient_take(int32_t* out, int cap);
// Called by the client tick before 0x4F860 (replay only: armed by the A-trace reset): load the Sounds tables and aim the ambient
// updater at the level's SoundEnviron row. Nothing happens in the play path.
void d2client_sound_stage_feed(uint32_t soundEnv) {
    if (!armed) return;
    initOnce();
    d2client_ambient::useEnvTable(); d2client_ambient::setArea(soundEnv);
}
// Called after 0x4F860: its D2Sound calls, in the A-capture shape. An accepted area-loop submit (Sounds row R) is two #10034
// records (the second carries the row as snd=R); a changed area key is #10059 (0x5CF90).
void d2client_sound_stage_log() {
    if (!armed) return;
    curTick = ++ambTick;
    int32_t q[64 * 2]; const int n = d2_ambient_take(q, 64);
    for (int i = 0; i < n; ++i) {
        if (q[i * 2 + 1] != 0) continue;
        out += std::to_string(curTick) + " A 10034 0,0\n";
        out += std::to_string(curTick) + " A 10034 0,0 snd=" + std::to_string(q[i * 2]) + "\n";   // snd= is decimal in the capture (snd=70 is row 70)
    }
    if (d2client_ambient::keyMoved()) logA(10059, 0, 0);
}
extern "C" {
EMSCRIPTEN_KEEPALIVE void d2_snd_trace_reset() { out.clear(); armed = true; }
EMSCRIPTEN_KEEPALIVE const char* d2_snd_trace_ptr() { return out.c_str(); }
EMSCRIPTEN_KEEPALIVE uint32_t d2_snd_trace_len() { return (uint32_t)out.size(); }
// one pump pass at session tick `tick` (>= 1); the sound "now" counter is tick - 1 (0 at the first pass)
EMSCRIPTEN_KEEPALIVE void d2_client_sound_pump(uint32_t tick) {
    static uint32_t m[32], tbl[1], pl = 1, tickv, arr, blk0[6], blk1[0x60], blk2[3], blk3[2], fdiv, plbox;
    blk0[1] = 1; plbox = 1;
    initOnce();
    uint32_t now = tick - 1;
    curTick = tick;
    d2client_45660::G g{ &pl, &m[0], &m[1], &m[2], &m[3], &m[4], &m[5], &m[6], &m[7], &m[8], &m[9], &m[10], &m[11], &m[12], &m[13],
        &tbl[0], &m[14], &m[15], &m[16], &tickv, &arr };
    d2client_45660::Calls c{ one1, one1, zbuf, d270, zero0, nop0, nop5, zero1, nop1, zero1, zero0, zero1, zero1, nullptr, nop0 };
    curEdx = 0;
    d2client_45660::sub_45660(now, g, c);
    const uint32_t gb[6] = { (uint32_t)(uintptr_t)blk0, (uint32_t)(uintptr_t)blk1, (uint32_t)(uintptr_t)blk2,
                             (uint32_t)(uintptr_t)blk3, (uint32_t)(uintptr_t)&fdiv, (uint32_t)(uintptr_t)&plbox };
    cl::Env e{}; e.h = hostH; e.rd = hostRd; e.wr = hostWr;
    if (sg.cc24c) d2client_batch_5ebc0::sub_5ED90(sw, sg, (int32_t)now);   // 0x5ED90 sweep
    curEdx = now;
    d2c_88dd0(now, gb, e);
}
}
