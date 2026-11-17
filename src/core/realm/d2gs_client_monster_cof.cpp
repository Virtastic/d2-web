// The COF file a client monster draws, named by the engine's own composite-path routine
// (1.13c D2Common+0x43C30, D2Common_10884_COMPOSIT_unk): the class code from MonStats, the mode token from
// MonMode.txt (knockback resolves GH there), and the weapon class from MonStats2 (BaseW for the attack modes).
// The page reads the path instead of rebuilding it from a token table and a weapon-class guess.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include <D2Composit.h>
#include <D2DataTbls.h>
#include <D2Monsters.h>
#include <Units/Units.h>

// The path (valid until the next call), or an empty string when the class or mode has no record.
extern "C" EMSCRIPTEN_KEEPALIVE const char* d2_realm_client_monster_cof_path(uint32_t classId, uint32_t mode) {
    static char path[260];
    path[0] = 0;
    if (mode > 15 || !DATATBLS_GetMonStatsTxtRecord(static_cast<int>(classId))) return path;
    int weaponCode = 0;
    D2Common_10884_COMPOSIT_unk(nullptr, static_cast<int>(classId), static_cast<int>(mode), UNIT_MONSTER, nullptr, path, &weaponCode, TRUE, 0);
    return path;
}
