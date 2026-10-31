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

// 1.13c D2CMP+0x09720
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

// 1.13c D2CMP+0x14EC0 -- row index into the 256-byte-record static table (base DAT_6fe34aa8); -1 = retail's NULL.
// Table bytes are retail data, so the pointer itself is not reproduced; the index is.
int D2CMP_14EC0_TableIndex(int a, int b) { return (a < 9 && b < 0x15 && a != 0 && (a < 3 || a > 4)) ? a * 0x69 + b : -1; }

// 1.13c D2CMP+0x17020 (verified against decompile)
// Cache bucket lookup. Retail gets bucket index + key hash from 0x16E80 (register args); the caller supplies them.
// Node: +4 id, +8..+18 five key ints, +1C hash, +2C next (void*). Key: +4 flags, +8 kind, +C idA, +14 idB, +18..+28 five ints.
const uint8_t* D2CMP_17020_CacheFind(const uint8_t* pTable, const uint8_t* pKey, uint32_t nBucket, uint32_t nHash) {
    auto I = [](const uint8_t* p, int o) { int v; memcpy(&v, p + o, 4); return v; };
    const uint8_t* n; memcpy(&n, pTable + 0x8000 + (size_t)nBucket * sizeof n, sizeof n);
    const int kind = I(pKey, 8);
    for (; n; memcpy(&n, n + 0x2c, sizeof n)) {
        bool ok = true;
        for (int k = 0; k < 5; k++) ok = ok && I(pKey, 0x18 + 4 * k) == I(n, 8 + 4 * k);
        if (kind == 3 || kind == 4) ok = ok && I(pKey, 0xc) == I(n, 4);
        else if (kind == 6 && (I(pKey, 4) & 2)) ok = ok && I(pKey, 0x14) == I(n, 4);
        if (ok && nHash == (uint32_t)I(n, 0x1c)) return n;
    }
    return nullptr;
}

// 1.13c D2CMP+0xc0b0 -- sprite-cache init (SpriteCache.cpp): 0x1400 zeroed nodes chained next/prev,
// head/tail kept at [9]/[10], critical section created once.
// Stateful, register args (ecx,eax,edx -> param1,eax,param2): no FN vector; mapped from the decompile.
// Slots are named by retail's dword index ([n]). Retail's node is 0x1c bytes with 32-bit links at +0x14
// (next) and +0x18 (prev); here the links are native pointers, so the node is wasm-width and its byte
// layout differs, while the chain (node i -> i+1, node 0 prev null, node 0x13ff next null), the zeroed
// fields and every slot value are the same. Node pool and lock are host-allocated (zeroed), as Fog #10045.
struct D2SpriteNode {
    uint32_t z[5];               // +0x00..+0x13 zero (retail +0,+4,+8,+0xC,+0x10)
    D2SpriteNode* next;          // retail +0x14
    D2SpriteNode* prev;          // retail +0x18
};
struct D2SpriteCache {
    uint32_t s0;                 // [0]  param_1 (ECX)
    uint32_t s1;                 // [1]  in_EAX
    uint32_t s2, s3;             // [2],[3] 0
    D2SpriteNode* pNodes;        // [4]  node array
    uint32_t s5_8[4];            // [5..8] 0
    D2SpriteNode* pHead;         // [9]
    D2SpriteNode* pTail;         // [10]
    uint32_t s11;                // [11] param_2 (EDX)
    void* pLock;                 // [12] CRITICAL_SECTION stand-in, created once
    uint32_t s13_14[2];          // [13],[14] 0
};
void D2CMP_SpriteCacheInit(D2SpriteCache* p, uint32_t param1, uint32_t eax, uint32_t param2) {
    p->s0 = param1; p->s1 = eax; p->s2 = p->s3 = 0; p->s11 = param2;
    p->pNodes = (D2SpriteNode*)calloc(0x1400, sizeof(D2SpriteNode));   // retail: Fog #10045, zeroed
    for (uint32_t i = 0; i < 0x1400; i++) {
        p->pNodes[i].next = i < 0x13ff ? &p->pNodes[i + 1] : nullptr;
        p->pNodes[i].prev = i > 0 ? &p->pNodes[i - 1] : nullptr;
    }
    p->pHead = p->pNodes; p->pTail = &p->pNodes[0x13ff];
    p->s5_8[0] = p->s5_8[1] = p->s5_8[2] = p->s5_8[3] = 0;
    p->s13_14[0] = p->s13_14[1] = 0;
    if (!p->pLock) p->pLock = calloc(1, 0x18);   // retail: Fog #10042 + InitializeCriticalSection
}

// 1.13c D2CMP+0x08710 -- statically linked CRT __convertcp(fromCP, toCP, src, &len, dst, dstLen).
// Retail goes through MultiByteToWideChar/WideCharToMultiByte; we have no codepages, so both are
// treated as single-byte identity (ponytail: Latin-1 ceiling). from==to -> NULL. len==-1 -> NUL-terminated.
// dst==NULL -> malloc'd result (caller frees), *len updated unless -1; else copy into dst, return dst.
#include <stdlib.h>
#include <string.h>
extern "C" void* d2cmp_convertcp_8710(unsigned from, unsigned to, const char* src, unsigned* len, char* dst, int dstLen)
{
	if (from == to) return nullptr;
	// 1.13c D2CMP+0x06120 (_strlen = libc strlen)
	unsigned n = *len == 0xFFFFFFFFu ? (unsigned)strlen(src) + 1 : *len;
	if (dst) { if ((int)n > dstLen) return nullptr; memcpy(dst, src, n); return dst; }
	char* out = (char*)malloc(n);
	if (!out) return nullptr;
	memcpy(out, src, n);
	return out;
}

// 1.13c D2CMP+0x0EDD0
// Register-convention blend row loop (EAX=dst, ECX=src stream, ESI=blend table, EDI=remap table).
// Stream byte b: b>=0x80 -> skip (b&0x7f) dst px, 0 ends the row; else (b&7) + 8*(b>>3) pixels follow,
// each dst = blend[dst*256 + remap[src]] (esi base). Rows advance by stride; `rows` rows.
// Returns the src stream pointer after the last row.
extern "C" const uint8_t* d2cmp_edd0_blend_rows(uint8_t* dst, const uint8_t* src, const uint8_t* blend,
                                                const uint8_t* remap, int rows, int stride) {
	for (; rows; --rows) {
		uint8_t* next = dst + stride;
		for (;;) {
			uint8_t b = *src++;
			if (b & 0x80) {
				if (!(b & 0x7f)) break;
				dst += b & 0x7f;
				continue;
			}
			for (int n = (b & 7) + (b >> 3) * 8; n > 0; --n, ++dst, ++src)
				*dst = blend[*dst * 256 + remap[*src]];
		}
		dst = next;
	}
	return src;
}
