// Compat: the D2CMP tile-library slice the DRLG uses, reimplemented from the 1.13c D2CMP.dll
// (exports #10007 LoadTileLibrarySlot, #10059 GetTiles, #10011/#10027/#10036/#10045/#10051
// entry getters -- D2MOO's 1.10f ordinals 10087/10088/10085/10078/10082/10081/10079).
// "Tile library". What matters for parity is the ORDER GetTiles returns
// entries in (the rarity roll walks it), so the hash is replicated node for node:
//   * a library is the DT1 file bytes; entries are the 0x60-byte tile headers in place
//   * libraries are cached process-wide by exact (case-sensitive) name, never reloaded
//   * a hash block has 128 buckets; bucket = (2*type - seq + style) & 0x7F
//   * nodes (style,seq,type) and refs are pushed at the HEAD of their lists as tiles are
//     inserted in file order, so GetTiles yields a library's matches in REVERSE file order
//   * a slot array holds 32 hash blocks; LoadTileLibrarySlot fills the first empty slot,
//     GetTiles walks the slots in order and concatenates
#include <D2Common.h>
#include <D2CMP.h>
#include <string>
#include <vector>
#include "../../../assets/mpqvfs.h"

namespace d2 { extern MpqVfs* g_drlgVfs; }

namespace {
struct Ref  { uint8_t* pTile; Ref* pNext; };
struct Node { int32_t nStyle, nSequence, nType; Ref* pRef; Node* pNext; };
struct HashBlock { Node* pNodes[128]; };
struct Record { std::string name; std::vector<uint8_t> data; HashBlock* hash; };
std::vector<Record*> g_records;   // 0x6fe88328 list, oldest first (lookup order is irrelevant: names are unique)

inline int32_t tileType(const uint8_t* e)  { int32_t v; memcpy(&v, e + 0x14, 4); return v; }
inline int32_t tileStyle(const uint8_t* e) { int32_t v; memcpy(&v, e + 0x18, 4); return v; }
inline int32_t tileSeq(const uint8_t* e)   { int32_t v; memcpy(&v, e + 0x1C, 4); return v; }
inline int32_t tileRarity(const uint8_t* e){ int32_t v; memcpy(&v, e + 0x20, 4); return v; }
inline uint8_t bucket(int type, int style, int seq) { return (uint8_t)(2 * (uint8_t)type - (uint8_t)seq + (uint8_t)style) & 0x7F; }

// 0x6fe22f20: insert one entry
void insert(HashBlock* h, uint8_t* e) {
    Node** slot = &h->pNodes[bucket(tileType(e), tileStyle(e), tileSeq(e))];
    for (Node* n = *slot; n; n = n->pNext) {
        if (n->nSequence == tileSeq(e) && n->nStyle == tileStyle(e) && n->nType == tileType(e)) {
            for (Ref* r = n->pRef; r; r = r->pNext) if (r->pTile == e) return;   // 0x6fe22cd0: same pointer twice
            Ref* r = new Ref{ e, n->pRef }; n->pRef = r;
            return;
        }
    }
    Node* n = new Node{ tileStyle(e), tileSeq(e), tileType(e), nullptr, *slot };   // 0x6fe22c30
    *slot = n;
    n->pRef = new Ref{ e, nullptr };
}

// 0x6fe19720: load-or-find by exact name
HashBlock* loadLibrary(const char* name) {
    for (Record* r : g_records) if (r->name == name) return r->hash;
    Record* r = new Record; r->name = name;
    if (!d2::g_drlgVfs || !d2::g_drlgVfs->read(name, r->data) || r->data.size() < 0x114) {
        fprintf(stderr, "D2CMP: cannot read tile library %s\n", name);
        abort();
    }
    int32_t ver; memcpy(&ver, r->data.data(), 4);
    if (ver != 7) { fprintf(stderr, "D2CMP: %s is DT1 version %d, want 7\n", name, ver); abort(); }
    int32_t nTiles, tilesOff; memcpy(&nTiles, r->data.data() + 0x10C, 4); memcpy(&tilesOff, r->data.data() + 0x110, 4);
    r->hash = new HashBlock(); memset(r->hash, 0, sizeof(HashBlock));
    for (int i = 0; i < nTiles; i++) insert(r->hash, r->data.data() + tilesOff + 0x60 * (size_t)i);   // 0x6fe25c70
    g_records.push_back(r);
    return r->hash;
}
} // namespace

// #10007 (1.13c) / #10087 (1.10f)
void D2CMP_10087_LoadTileLibrarySlot(D2TileLibraryHashStrc** ppSlots, const char* szFileName) {
    D2_ASSERT(ppSlots); D2_ASSERT(szFileName);
    int i = 0;
    while (i < 32 && ppSlots[i]) i++;
    if (i >= 32) { FOG_DisplayHalt("tile library slots exhausted", __FILE__, __LINE__); return; }
    ppSlots[i] = (D2TileLibraryHashStrc*)loadLibrary(szFileName);
}

// The DT1 a loaded library came from.
//
// Not a retail export: retail has no need to name a library once it is loaded, but the browser
// client does. A tile's slot index is per-ROOM (it indexes that room's pTiles[], holding only what
// that room loaded, in its own order), so the client cannot map a slot to a file through the
// level's LvlTypes list -- doing so left preset-room levels with no floor. Resolving the hash
// pointer back to its filename gives an exact mapping instead of a guess.
extern "C" const char* D2WEB_TileLibraryNameFromHash(const D2TileLibraryHashStrc* pHash) {
    for (Record* r : g_records) if ((const D2TileLibraryHashStrc*)r->hash == pHash) return r->name.c_str();
    return nullptr;
}

// The index of a library entry in its DT1 file (entries ARE the file's 0x60-byte records): the exact
// variant the room picked. UINT32_MAX for a pointer no loaded library owns.
extern "C" uint32_t D2WEB_TileEntryFileIndex(const void* pEntry) {
    const uint8_t* e = (const uint8_t*)pEntry;
    for (Record* r : g_records) {
        int32_t off; memcpy(&off, r->data.data() + 0x110, 4);
        const uint8_t* base = r->data.data() + off;
        if (e >= base && e < r->data.data() + r->data.size()) return (uint32_t)((e - base) / 0x60);
    }
    return UINT32_MAX;
}

// The tile pick stream (s175, vector kind 26) observes these two calls the way the retail capture tool observes
// retail's through D2Common's import table. Null unless drlg_check is collecting; they only watch.
void (*g_d2webOnGetTiles)(int nType, int nStyle, int nSequence, int nEntries, int nFirstFrame) = nullptr;
void (*g_d2webOnTileRarity)(int nRarity) = nullptr;

// #10059 (1.13c) / #10088 (1.10f): 0x6fe253a0 over the slots, 0x6fe22e60 per library
int D2CMP_10088_GetTiles(D2TileLibraryHashStrc** ppSlots, int nType, int nStyle, int nSequence, D2TileLibraryEntryStrc** pTileList, int nTileListSize) {
    D2_ASSERT(ppSlots);
    int total = 0;
    for (int s = 0; s < 32 && ppSlots[s] && nTileListSize > 0; s++) {
        HashBlock* h = (HashBlock*)ppSlots[s];
        Node* n = h->pNodes[bucket(nType, nStyle, nSequence)];
        for (; n; n = n->pNext) if (n->nSequence == nSequence && n->nStyle == nStyle && n->nType == nType) break;
        if (!n) continue;
        int got = 0;
        for (Ref* r = n->pRef; r && got < nTileListSize; r = r->pNext) pTileList[total + got++] = (D2TileLibraryEntryStrc*)r->pTile;
        total += got; nTileListSize -= got;
    }
    if (g_d2webOnGetTiles) g_d2webOnGetTiles(nType, nStyle, nSequence, total, total > 0 ? tileRarity((const uint8_t*)pTileList[0]) : -1);
    return total;
}

int D2CMP_10078_GetTileStyle(D2TileLibraryEntryStrc* e)    { D2_ASSERT(e); return tileStyle((const uint8_t*)e); }
int D2CMP_10079_GetTileFlags(D2TileLibraryEntryStrc* e)    { D2_ASSERT(e); uint16_t v; memcpy(&v, (const uint8_t*)e + 6, 2); return v; }
int D2CMP_10081_GetTileRarity(D2TileLibraryEntryStrc* e)   { D2_ASSERT(e); const int v = tileRarity((const uint8_t*)e); if (g_d2webOnTileRarity) g_d2webOnTileRarity(v); return v; }
int D2CMP_10082_GetTileSequence(D2TileLibraryEntryStrc* e) { D2_ASSERT(e); return tileSeq((const uint8_t*)e); }
uint8_t* D2CMP_10085_GetTileFlagArray(D2TileLibraryEntryStrc* e) { D2_ASSERT(e); return (uint8_t*)e + 0x28; }
