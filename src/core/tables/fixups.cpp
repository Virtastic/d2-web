#include "fixups.h"
#include <cstring>
#include <cstdio>
#include <vector>

namespace d2 {

// D2LANG_GetStringByReferenceString(key) then Unicode::strncpy(dst, str, n) into a field that
// was memset to zero. A key absent from all three tables makes D2Lang return the placeholder
// "<key> -not xlated call ken " (a Blizzard-era reminder that survives in retail: the Uber
// level warps in levels.txt hit it), so that is what lands in memory and what we reproduce.
static const char16_t* kNotTranslated = u" -not xlated call ken ";
static void putUnicode(uint8_t* dst, size_t nChars, const std::string& key, const LangTables& lang) {
    memset(dst, 0, nChars * 2);
    if (key.empty()) return;                 // an empty reference resolves to "" in retail, no placeholder
    const std::u16string* s = lang.find(key);
    std::u16string fallback;
    if (!s) { fallback = utf8ToU16(key) + kNotTranslated; s = &fallback; }
    size_t n = s->size() < nChars ? s->size() : nChars;
    for (size_t i = 0; i < n; i++) { dst[i * 2] = (uint8_t)((*s)[i] & 0xFF); dst[i * 2 + 1] = (uint8_t)((*s)[i] >> 8); }
}
static std::string cstr(const uint8_t* p, size_t max) { return std::string((const char*)p, strnlen((const char*)p, max)); }

// levels (D2LevelsTxt, 544 bytes): DATATBLS_LoadLevelsTxt
//   +0x16E wszLevelName[40] <- tbl[szLevelName @0xF5],  [39] = 0
//   +0x1BE wszLevelWarp[40] <- tbl[szLevelWarp @0x11D], [39] = 0
//   +0x33/+0x34/+0x35 = count of leading non-negative entries in wNormalMonsters[25] @0x36,
//   wNightmareHellMonsters[25] @0x68, wUniqueMonsters[25] @0x9A (int16 each)
static void fixLevels(uint8_t* r, size_t count, size_t recSize, const LangTables& lang) {
    for (size_t i = 0; i < count; i++, r += recSize) {
        putUnicode(r + 0x16E, 40, cstr(r + 0xF5, 40), lang);  r[0x16E + 39 * 2] = r[0x16E + 39 * 2 + 1] = 0;
        putUnicode(r + 0x1BE, 40, cstr(r + 0x11D, 40), lang); r[0x1BE + 39 * 2] = r[0x1BE + 39 * 2 + 1] = 0;
        const uint32_t lists[3] = { 0x36, 0x68, 0x9A };
        for (int l = 0; l < 3; l++) {
            uint8_t n = 0;
            for (; n < 25; n++) { int16_t v; memcpy(&v, r + lists[l] + n * 2, 2); if (v < 0) break; }
            r[0x33 + l] = n;
        }
    }
}

// charstats (D2CharStatsTxt, 196 bytes): DATATBLS_InitUnicodeClassNamesInCharStatsTxt
//   +0x00 wszClassName[16] <- tbl[szClassName @0x20], or "<name>" when the key is absent
static void fixCharStats(uint8_t* r, size_t count, size_t recSize, const LangTables& lang) {
    for (size_t i = 0; i < count; i++, r += recSize) {
        std::string key = cstr(r + 0x20, 16);
        if (lang.find(key)) putUnicode(r, 16, key, lang);
        else { std::u16string v = u"<" + utf8ToU16(key.substr(0, 15)) + u">"; memset(r, 0, 16); for (size_t k = 0; k < v.size() && k < 16; k++) { r[k * 2] = (uint8_t)v[k]; r[k * 2 + 1] = (uint8_t)(v[k] >> 8); } }
    }
}

// The DRLG file-name fields (lvltypes/lvlprest/lvlsub, 60-byte char arrays): '/' -> '\', then a
// name longer than one char gets "DATA\GLOBAL\TILES\\" in front (LevelsTbls.cpp, all three loaders).
static void fixTilePath(uint8_t* field) {
    size_t len = strnlen((const char*)field, 60);
    for (size_t k = 0; k < len; k++) if (field[k] == '/') field[k] = '\\';
    if (len > 1) {
        static const char prefix[] = "DATA\\GLOBAL\\TILES\\";
        char buf[60] = {};
        size_t pl = sizeof(prefix) - 1, n = len < 59 - pl ? len : 59 - pl;
        memcpy(buf, prefix, pl); memcpy(buf + pl, field, n);
        memcpy(field, buf, 60);
    }
}
static void fixLvlTypes(uint8_t* r, size_t count, size_t recSize) { for (size_t i = 0; i < count; i++, r += recSize) for (int j = 0; j < 32; j++) fixTilePath(r + j * 60); }
static void fixLvlPrest(uint8_t* r, size_t count, size_t recSize) { for (size_t i = 0; i < count; i++, r += recSize) for (int j = 0; j < 6; j++) fixTilePath(r + 0x44 + j * 60); }
static void fixLvlSub(uint8_t* r, size_t count, size_t recSize)   { for (size_t i = 0; i < count; i++, r += recSize) fixTilePath(r + 4); }

// objects (D2ObjectsTxt, 448 bytes): DATATBLS_LoadObjectsTxt
//   +0x40 wszName[64] <- tbl[szName @0x00 (64)]; +0xD8 dwFrameCnt[8] <<= 8
static void fixObjects(uint8_t* r, size_t count, size_t recSize, const LangTables& lang) {
    for (size_t i = 0; i < count; i++, r += recSize) {
        putUnicode(r + 0x40, 64, cstr(r, 64), lang);
        for (int j = 0; j < 8; j++) { uint32_t v; memcpy(&v, r + 0xD8 + j * 4, 4); v <<= 8; memcpy(r + 0xD8 + j * 4, &v, 4); }
    }
}


// ---- monstats (D2MonStatsTxt, 424 bytes): DATATBLS_LoadMonStatsTxt + LoadMonEquipTxt --------
// Three things retail writes after the .bin is in memory:
//   +0x2A nMonEquipTxtRecordId: -1, then the index of the FIRST monequip record naming the monster
//   +0x4A nMaxChainId / +0x4B nChainId: walk the BaseId/NextInClass chain (+0x02/+0x04, int16)
//   +0x36 nWalkAnimSpeed / +0x38 nRunAnimSpeed: from AnimData.d2 for the BASE monster's
//        <Code><WL|RN><monstats2.BaseW> COF, scaled by this monster's Velocity/Run (+0x32/+0x34)
//        over the base's; classic rows (< 410) take run = base walk speed / 2 instead of the RN record
// Auxiliary inputs (all through the VFS): data\global\excel\{monequip,monmode,monstats2}.bin and
// data\global\AnimData.d2 (256 buckets of u32 count + 160-byte records; bucket = byte sum of the
// upper-cased 8-char name, D2MOO AnimTbls.cpp).
struct Bin { std::vector<uint8_t> data; uint32_t count = 0; size_t rec = 0;
    const uint8_t* at(size_t i) const { return data.data() + 4 + i * rec; } };
static bool loadBin(const MpqVfs& vfs, const char* name, size_t recSize, Bin& b) {
    std::string path = std::string("data\\global\\excel\\") + name + ".bin";
    if (!vfs.read(path.c_str(), b.data) || b.data.size() < 4) return false;
    memcpy(&b.count, b.data.data(), 4); b.rec = recSize;
    return b.data.size() == 4 + (size_t)b.count * recSize;
}
struct AnimData { std::vector<uint8_t> d; size_t bucket[256] = {};
    bool load(const MpqVfs& vfs) {
        if (!vfs.read("data\\global\\AnimData.d2", d)) return false;
        size_t pos = 0;
        for (int b = 0; b < 256; b++) { if (pos + 4 > d.size()) return false; bucket[b] = pos; uint32_t n; memcpy(&n, d.data() + pos, 4); pos += 4 + (size_t)n * 160; }
        return pos == d.size();
    }
    // dwAnimSpeed of the record named `path` (<= 8 chars); retail's default record gives 256.
    int32_t speed(const char* path) const {
        char key[8] = {}; uint8_t h = 0;
        for (int i = 0; i < 8 && path[i]; i++) { key[i] = path[i] >= 'a' && path[i] <= 'z' ? path[i] - 32 : path[i]; h += (uint8_t)key[i]; }
        uint32_t n; memcpy(&n, d.data() + bucket[h], 4);
        for (uint32_t i = 0; i < n; i++) { const uint8_t* r = d.data() + bucket[h] + 4 + i * 160; if (!memcmp(r, key, 8)) { int32_t v; memcpy(&v, r + 12, 4); return v; } }
        return 256;
    }
};
static void trim4(char* out, const uint8_t* code) { int n = 0; for (int i = 0; i < 3; i++) if (code[i] != ' ' && code[i]) out[n++] = (char)code[i]; out[n] = 0; }

static bool fixMonStats(uint8_t* r, size_t count, size_t recSize, const MpqVfs& vfs) {
    Bin equip, mode, stats2; AnimData anim;
    if (!loadBin(vfs, "monequip", 28, equip) || !loadBin(vfs, "monmode", 52, mode) || !loadBin(vfs, "monstats2", 308, stats2) || !anim.load(vfs)) return false;
    auto R = [&](size_t i) { return r + i * recSize; };
    auto i16 = [](const uint8_t* p) { int16_t v; memcpy(&v, p, 2); return v; };
    auto w16 = [](uint8_t* p, int16_t v) { memcpy(p, &v, 2); };

    for (size_t i = 0; i < count; i++) w16(R(i) + 0x2A, -1);
    for (uint32_t e = 0; e < equip.count; e++) {
        int16_t mon = i16(equip.at(e));
        if (mon >= 0 && (size_t)mon < count && i16(R(mon) + 0x2A) < 0) w16(R(mon) + 0x2A, (int16_t)e);
    }
    for (size_t i = 0; i < count; i++) {
        int chain = 0; bool found = false; int id = i16(R(i) + 0x02);
        int next;
        do {
            if (id < 0 || (size_t)id >= count) break;
            if ((size_t)id == i) { found = true; R(i)[0x4B] = (uint8_t)chain; }
            next = i16(R(id) + 0x04); chain++;
            if (id == next) break;
            id = next;
            if (chain > 255) break;
        } while (next >= 0);
        R(i)[0x4A] = (uint8_t)chain;
        (void)found;
    }
    char walkTok[4], runTok[4]; trim4(walkTok, mode.at(2) + 0x20); trim4(runTok, mode.at(15) + 0x20);
    for (size_t i = 0; i < count; i++) {
        int base = i16(R(i) + 0x02);
        if (base < 0 || (size_t)base >= count) { w16(R(i) + 0x02, (int16_t)i); base = (int)i; }
        char code[4], baseW[4] = { 'h', 't', 'h', 0 }; trim4(code, R(base) + 0x10);
        uint16_t ex; memcpy(&ex, R(base) + 0x18, 2);
        if (ex < stats2.count) trim4(baseW, stats2.at(ex) + 0x10);
        char path[16];
        snprintf(path, sizeof path, "%s%s%s", code, walkTok, baseW);
        int walk = anim.speed(path);
        if (base != (int)i && i16(R(base) + 0x32) > 0) walk = (int)((unsigned)(walk * i16(R(i) + 0x32)) / (unsigned)i16(R(base) + 0x32));
        w16(R(i) + 0x36, (int16_t)(walk <= 0 ? 0 : walk >= 32767 ? 32767 : walk));
        int run;
        if (i < 410) run = i16(R(base) + 0x36) / 2;
        else { snprintf(path, sizeof path, "%s%s%s", code, runTok, baseW); run = anim.speed(path); }
        if (base != (int)i && i16(R(base) + 0x34) > 0) run = (int)((unsigned)(run * i16(R(i) + 0x34)) / (unsigned)i16(R(base) + 0x34));
        w16(R(i) + 0x38, (int16_t)(run <= 0 ? 0 : run >= 32767 ? 32767 : run));
    }
    return true;
}

// ---- itemstatcost (D2ItemStatCostTxt, 324 bytes): DATATBLS_LoadItemStatCostTxt -------------
// The op graph, built in place (D2MOO ItemsTbls.cpp):
//   +0x5E unk0x5E[64] (u16) = 0xFFFF, then for every stat with 0 < op < 14 whose op base
//        (+0x56) is a valid stat: base.bIsBaseOfOtherStatOp(+0x51) = 1, first free slot of
//        base.unk0x5E (value >= count) = this stat; op 4/5 sets this.bHasOpApplyingToItem(+0x53)
//   for each of this stat's op stat1..3 (+0x58, u16, stop at the first >= count):
//        this.bIsBaseOfOtherStatOp = 1; first pOpStatData slot (+0xDE, 16 x {u16 opBase, u16 stat,
//        u8 op, u8 param}) of the TARGET with op == 0 gets {opBase, thisStat, op, param};
//        target.bHasOpStatData(+0x52) = 1; if this or target is maxhp(7)/maxmana(9)/maxstamina(11),
//        this.dwItemStatFlags(+0x04) |= HP_MANA_STAMINA(bit 5) and HP(6)/MANA(7)/STAMINA(8)
static void fixItemStatCost(uint8_t* r, size_t count, size_t recSize) {
    auto R = [&](size_t i) { return r + i * recSize; };
    auto u16 = [](const uint8_t* p) { uint16_t v; memcpy(&v, p, 2); return v; };
    auto w16 = [](uint8_t* p, uint16_t v) { memcpy(p, &v, 2); };
    for (size_t i = 0; i < count; i++) memset(R(i) + 0x5E, 0xFF, 128);
    for (size_t id = 0; id < count; id++) {
        uint8_t* cur = R(id);
        uint8_t op = cur[0x54];
        if (op == 0 || op >= 14) continue;
        uint16_t opBase = u16(cur + 0x56);
        if (opBase < count) {
            uint8_t* base = R(opBase);
            base[0x51] = 1;
            for (int k = 0; k < 64; k++) if (u16(base + 0x5E + k * 2) >= count) { w16(base + 0x5E + k * 2, (uint16_t)id); break; }
            if (op == 4 || op == 5) cur[0x53] = 1;
        }
        for (int j = 0; j < 3; j++) {
            uint16_t opStat = u16(cur + 0x58 + j * 2);
            if (opStat >= count) break;
            cur[0x51] = 1;
            uint8_t* tgt = R(opStat);
            for (int k = 0; k < 16; k++) {
                uint8_t* slot = tgt + 0xDE + k * 6;
                if (slot[4] != 0) continue;
                w16(slot, opBase); w16(slot + 2, (uint16_t)id); slot[4] = op; slot[5] = cur[0x55];
                tgt[0x52] = 1;
                uint32_t flags; memcpy(&flags, cur + 4, 4);
                if (id == 7 || opStat == 7)        flags |= (1u << 6) | (1u << 5);
                else if (id == 9 || opStat == 9)   flags |= (1u << 7) | (1u << 5);
                else if (id == 11 || opStat == 11) flags |= (1u << 8) | (1u << 5);
                memcpy(cur + 4, &flags, 4);
                break;
            }
        }
    }
}

bool applyTableFixups(const std::string& table, uint8_t* records, size_t count, size_t recSize, const LangTables& lang, const MpqVfs& vfs) {
    if (table == "levels" && recSize == 544)    { fixLevels(records, count, recSize, lang); return true; }
    if (table == "charstats" && recSize == 196) { fixCharStats(records, count, recSize, lang); return true; }
    if (table == "objects" && recSize == 448)   { fixObjects(records, count, recSize, lang); return true; }
    if (table == "lvltypes" && recSize == 1928) { fixLvlTypes(records, count, recSize); return true; }
    if (table == "lvlprest" && recSize == 432)  { fixLvlPrest(records, count, recSize); return true; }
    if (table == "lvlsub" && recSize == 348)    { fixLvlSub(records, count, recSize); return true; }
    if (table == "monstats" && recSize == 424)  { return fixMonStats(records, count, recSize, vfs); }
    if (table == "itemstatcost" && recSize == 324) { fixItemStatCost(records, count, recSize); return true; }
    return false;
}

} // namespace d2
