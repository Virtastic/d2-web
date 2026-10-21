// Prior art: none (D2MOO has no D2Client module); cached 1.13c decompile is the authority.
#include <cstdint>
// Env seam: Fog Ordinal_423 reads ("Diablo II","Help Menu", default 1) into *out.
struct HelpMenuEnv { void (*readPref)(const char* key, const char* name, int32_t def, int32_t* out); };
static int32_t g_helpMenu = -1; // DAT_6fba5204
// 1.13c D2Client+0x36450
extern "C" int32_t d2_client_36450(const HelpMenuEnv* env) {
    if (g_helpMenu == -1) {
        g_helpMenu = 0;
        env->readPref("Diablo II", "Help Menu", 1, &g_helpMenu);
    }
    return g_helpMenu;
}
