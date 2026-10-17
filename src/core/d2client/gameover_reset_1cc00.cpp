// 1.13c D2Client+0x1CC00: GAMEOVER registry write, then reset of the game-over globals.
#include <cstdint>
#include <cstring>
// g = the globals span starting at D2Client+0x1223D8 (VA 0x6FBD23D8), 0xFC9 bytes. reg = Ordinal_426("Diablo II","GAMEOVER",0,0).
extern "C" void d2_client_1cc00(uint8_t* g, void (*reg)(const char*, const char*, uint32_t, uint32_t)) {
    const uint32_t B = 0x6FBD23D8u, m1 = 0xFFFFFFFFu, z = 0;
    auto d = [&](uint32_t va, uint32_t v) { memcpy(g + (va - B), &v, 4); };
    auto b = [&](uint32_t va) { g[va - B] = 0; };
    reg("Diablo II", "GAMEOVER", 0, 0);
    d(0x6FBD23D8, z); d(0x6FBD2415, z); d(0x6FBD2419, z); d(0x6FBD2411, z); d(0x6FBD241D, z); d(0x6FBD2491, z); d(0x6FBD248D, z);
    memset(g + (0x6FBD2421 - B), 0, 0x1B * 4);
    d(0x6FBD23E4, z); d(0x6FBD23EC, z); d(0x6FBD3399, z);
    for (uint32_t i = 0; i < 0x29; ++i) { b(0x6FBD2496 + i); b(0x6FBD24C0 + i); }
    b(0x6FBD23F1); b(0x6FBD23F2); b(0x6FBD23F0); b(0x6FBD23F4);
    b(0x6FBD339D); b(0x6FBD339E); b(0x6FBD339F); b(0x6FBD33A0); b(0x6FBD23F3);
    d(0x6FBD23F5, m1); d(0x6FBD23FD, m1); d(0x6FBD2401, m1); d(0x6FBD2405, m1); d(0x6FBD2409, m1); d(0x6FBD240D, m1);
}
