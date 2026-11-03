// 1.13c D2Game+0x8D5F0: skill-base-level packet 0x21.
// Register inputs: EAX = target client (0 = resolve it from the unit), ESI = unit, BX = skill id; stack: arg1 byte, arg2 byte (ret 8).
// Without a target the unit must be a player (type 1) whose #21260 record has word 0 == 0; the client is the player data's +0x9C word.
// The 12-byte packet is {0x21, unit type, arg2, unit id (4), skill id (2), arg1, #11146(unit, skill) low byte, residue byte}; sent through 0x8A3E0.
#include <cstdint>
#include <cstring>
struct D2Game8d5f0Env {
    uint32_t (*sub21260)(uint32_t unit);                  // 0x21260, unit in ECX
    uint32_t (*playerData)(uint32_t rec);                 // Ordinal 10920 via 0xA4C0
    uint32_t (*skillLevel)(uint32_t unit, uint32_t skill); // Ordinal 11146 via 0xAFCA; only the low byte is stored
    void (*send)(uint32_t client, const uint8_t* buf, uint32_t len); // 0x8A3E0, client in EAX
};
static uint32_t rd32w(uint32_t a, uint32_t off) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + off), 4); return v; }
extern "C" void d2game_8d5f0_run(uint32_t target, uint32_t unit, uint32_t skillId, uint32_t arg1, uint32_t arg2, const D2Game8d5f0Env* env) { // 1.13c D2Game+0x8D5F0
    if (!unit) return;
    if (!target) {
        if (rd32w(unit, 0) != 1) return;
        const uint32_t rec = env->sub21260(unit);
        if (!rec || rd32w(rec, 0) != 0) return;
        target = rd32w(env->playerData(rec), 0x9C);
        if (!target) return;
    }
    uint8_t b[0xC] = {};
    b[0] = 0x21;
    b[1] = (uint8_t)rd32w(unit, 0);
    b[2] = (uint8_t)arg2;
    const uint32_t id = rd32w(unit, 0xC);
    std::memcpy(b + 3, &id, 4);
    const uint16_t sk = (uint16_t)skillId;
    std::memcpy(b + 7, &sk, 2);
    b[9] = (uint8_t)arg1;
    b[0xA] = (uint8_t)env->skillLevel(unit, sk);
    env->send(target, b, 0xC);   // b[0xB] is stack residue in retail; the capture masks it
}
