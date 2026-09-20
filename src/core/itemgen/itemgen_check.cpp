// Retail-gated item-generation check (s04-itemgen): replay the retail captures through
// the vendored D2Game CreateItemEx over our tables and compare the item it makes -- quality,
// ilvl, unique id, and the ITEMS_Serialize bitstream -- with what the headless 1.13c D2Game
// produced (the retail capture tool captureItems).
#include <emscripten.h>
#include <string>
#include <vector>
#include <D2Common.h>
#include <D2DataTbls.h>
#include <D2Items.h>
#include <D2Seed.h>
#include <D2BitManip.h>
#include <GAME/Game.h>
#include <GAME/Event.h>
#include <ITEMS/Items.h>
#include <UNIT/SUnit.h>
#include "../d2moo/compat/datatbls_bridge.h"
#include "../tables/shared.h"
#include "../../../tools/vectorfmt/vectorfmt.h"

static D2GameStrc* g_game;
static std::string g_err;
static D2VecItem g_out;

extern "C" {

// Tables + a bare server game object (the fields CreateItemEx and the item code read).
EMSCRIPTEN_KEEPALIVE int d2_itemgen_init(int difficulty) {
    if (!d2::drlgLoadTables(d2::sharedVfs(), d2::sharedLang())) { g_err = "tables"; return 0; }
    if (!g_game) {
        g_game = (D2GameStrc*)calloc(1, sizeof(D2GameStrc));
        g_game->bExpansion = TRUE;
        g_game->nDifficulty = (uint8_t)difficulty;
        g_game->wItemFormat = 101;          // what the 1.13c game object carries at +0x78
        SEED_InitLowSeed(&g_game->pGameSeed, 1);
        EVENT_AllocEventQueue(g_game);      // normal D2Game bring-up; CreateItemEx schedules timers
    }
    return 1;
}

// Runs one record's inputs; fills the output half of d2_itemgen_out(). 1 on success.
EMSCRIPTEN_KEEPALIVE int d2_itemgen_run(uint32_t itemId, uint32_t ilvl, uint32_t qualityIn, uint32_t seed, uint32_t itemSeed, uint32_t itemIndex, uint32_t flags1, uint32_t extraFlags) {
    memset(&g_out, 0, sizeof g_out);
    D2ItemDropStrc drop = {};
    drop.pGame = g_game; drop.nItemLvl = (int)ilvl; drop.nId = (int)itemId; drop.nSpawnType = 4;
    drop.wUnitInitFlags = 1; drop.wItemFormat = g_game->wItemFormat; drop.bForce = TRUE;
    drop.nQuality = (int)qualityIn; drop.nItemIndex = (int)itemIndex; drop.dwFlags1 = flags1; drop.dwSeed = seed; drop.dwItemSeed = itemSeed;
    (void)extraFlags;   // 1.13c's +0x80 quality-force bits: 0 in the corpus
    D2UnitStrc* u = D2GAME_CreateItemEx_6FC4ED80(g_game, &drop, 1);
    if (!u) return 1;
    g_out.classId = u->dwClassId; g_out.seedLoOut = u->pSeed.nLowSeed; g_out.seedHiOut = u->pSeed.nHighSeed; g_out.itemFlags = u->dwFlags;
    g_out.itemSeedLoOut = ITEMS_GetItemSeed(u)->nLowSeed; g_out.itemSeedHiOut = ITEMS_GetItemSeed(u)->nHighSeed;
    g_out.qualityOut = (uint32_t)ITEMS_GetItemQuality(u);
    g_out.ilvlOut = (uint32_t)ITEMS_GetItemLevel(u);
    g_out.uniqueId = (uint32_t)ITEMS_GetFileIndex(u);   // D2Common #10620 'GetUniqueID' = the unique/set file index
    static uint8_t bits[4096]; memset(bits, 0, sizeof bits);
    g_out.bitBytes = (uint32_t)ITEMS_SerializeItemToBitstream(u, bits, sizeof bits, 1, 1, 0);   // D2Common #10987 (PlugY D2SaveItem)
    memcpy(g_out.bits, bits, g_out.bitBytes < sizeof g_out.bits ? g_out.bitBytes : sizeof g_out.bits);
    return 1;
}
EMSCRIPTEN_KEEPALIVE const D2VecItem* d2_itemgen_out() { return &g_out; }
EMSCRIPTEN_KEEPALIVE const char* d2_itemgen_err() { return g_err.c_str(); }

} // extern "C"
