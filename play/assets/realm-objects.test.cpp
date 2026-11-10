// Feeds a joined game's object packets to the client's object units and prints the units as the page reads
// them back (the records of d2_realm_client_object_units). The table rows are given on stdin, so the units are
// built from the game's own Objects table values. stdin, one command a line:
//   txt <class> <token> <orientation> <subclass> <lockable> <isdoor> then for each of the 8 modes
//       <selectable> <framecnt> <framedelta> <cycleanim> <start> <hascollision>
//   shrine <record> <code>   the Shrines table record an assign packet's interact byte names
//   room <0|1>               whether the rooms objects are assigned into are revealed (1 at the start)
//   p <hex>                  a server packet: 0x51 assign, 0x0E state, 0x4D action, 0x0A remove
//   tick <n>                 n client ticks of every unit
//   dump                     "units <count> <hex of the records>"
#include "../../src/core/realm/d2gs_client_object.cpp"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

static std::map<int, D2ObjectsTxt*> g_txt;
static D2ShrinesTxt g_shrines[64];
static D2ObjectsTxt* row(int id) {
    D2ObjectsTxt*& r = g_txt[id];
    if (!r) r = static_cast<D2ObjectsTxt*>(std::calloc(1, sizeof(D2ObjectsTxt)));
    return r;
}
D2ObjectsTxt* __stdcall DATATBLS_GetObjectsTxtRecord(int id) { auto it = g_txt.find(id); return it == g_txt.end() ? nullptr : it->second; }
D2ShrinesTxt* __stdcall DATATBLS_GetShrinesTxtRecord(int id) { return id >= 0 && id < 64 ? &g_shrines[id] : nullptr; }
int __stdcall UNITROOM_AddUnitToRoom(D2UnitStrc* unit, D2ActiveRoomStrc*) { unit->pRoomNext = unit; return 1; }
void __stdcall UNITROOM_RemoveUnitFromRoom(D2UnitStrc* unit) { unit->pRoomNext = nullptr; }
D2UnitStrc* __stdcall UNITS_AllocUnit(void*, int type) {
    auto* unit = static_cast<D2UnitStrc*>(std::calloc(1, sizeof(D2UnitStrc)));
    unit->dwUnitType = type;
    return unit;
}
void __stdcall UNITS_FreeUnit(D2UnitStrc* unit) { std::free(unit); }
void __stdcall UNITS_AllocStaticPath(D2UnitStrc* unit) { unit->pStaticPath = static_cast<D2StaticPathStrc*>(std::calloc(1, sizeof(D2StaticPathStrc))); }
void __stdcall UNITS_FreeStaticPath(D2UnitStrc* unit) { std::free(unit->pStaticPath); }
void __stdcall UNITS_InitializeStaticPath(D2UnitStrc* unit, D2ActiveRoomStrc* room, int x, int y) {
    unit->pStaticPath->pRoom = room; unit->pStaticPath->tGameCoords.nX = x; unit->pStaticPath->tGameCoords.nY = y;
}
D2ActiveRoomStrc* __stdcall UNITS_GetRoom(D2UnitStrc* unit) { return unit->pStaticPath->pRoom; }
// The mode's cel run: the table's count (already 8.8), its start cel and its 8.8 step.
void __stdcall UNITS_SetAnimStartFrame(D2UnitStrc* unit) {
    const D2ObjectsTxt* txt = unit->pObjectData->pObjectTxt;
    unit->dwFrameCountPrecise = txt->dwFrameCnt[unit->dwAnimMode];
    unit->nSeqCurrentFramePrecise = txt->nStart[unit->dwAnimMode] << 8;
    unit->wAnimSpeed = txt->wFrameDelta[unit->dwAnimMode];
}
void __stdcall UNITS_BlockCollisionPath(D2UnitStrc*, D2ActiveRoomStrc*, int, int) {}
void __stdcall UNITS_FreeCollisionPath(D2UnitStrc*) {}

int main() {
    alignas(16) static uint8_t roomMem[0x100];
    D2ActiveRoomStrc* room = reinterpret_cast<D2ActiveRoomStrc*>(roomMem);
    bool revealed = true;
    std::string line;
    while (std::getline(std::cin, line)) {
        std::istringstream in(line);
        std::string op;
        in >> op;
        if (op == "txt") {
            int id, v; std::string token;
            in >> id >> token;
            D2ObjectsTxt* t = row(id);
            for (size_t i = 0; i < 3 && i < token.size(); ++i) t->szToken[i] = token[i];
            in >> v; t->nOrientation = static_cast<uint8_t>(v);
            in >> v; t->nSubClass = static_cast<uint8_t>(v);
            in >> v; t->nLockable = static_cast<uint8_t>(v);
            in >> v; t->nIsDoor = static_cast<uint8_t>(v);
            for (int m = 0; m < 8; ++m) {
                in >> v; t->nSelectable[m] = static_cast<uint8_t>(v);
                in >> v; t->dwFrameCnt[m] = static_cast<uint32_t>(v) << 8;   // the compiled table holds the count as 8.8
                in >> v; t->wFrameDelta[m] = static_cast<uint16_t>(v);
                in >> v; t->nCycleAnim[m] = static_cast<uint8_t>(v);
                in >> v; t->nStart[m] = static_cast<uint8_t>(v);
                in >> v; t->nHasCollision[m] = static_cast<uint8_t>(v);
            }
            if (!in) { std::fprintf(stderr, "bad txt line\n"); return 2; }
        } else if (op == "shrine") {
            int record, code;
            in >> record >> code;
            if (!in || record < 0 || record >= 64) return 2;
            g_shrines[record].nCode = static_cast<uint8_t>(code);
        } else if (op == "room") {
            int on; in >> on; revealed = on != 0;
        } else if (op == "p") {
            std::string hex;
            in >> hex;
            std::vector<uint8_t> b;
            for (size_t i = 0; i + 1 < hex.size(); i += 2) b.push_back(static_cast<uint8_t>(std::stoi(hex.substr(i, 2), nullptr, 16)));
            const auto u32 = [&b](size_t at) { return static_cast<uint32_t>(b[at] | b[at + 1] << 8 | b[at + 2] << 16) | static_cast<uint32_t>(b[at + 3]) << 24; };
            if (b.size() == 14 && b[0] == 0x51 && b[1] == 2) {
                d2gs_client_object_destroy(d2gs_client_object_find(u32(2)));
                if (D2UnitStrc* unit = d2gs_client_object_create(u32(2), b[6] | b[7] << 8, b[12], b[13]))
                    d2gs_client_object_place(unit, revealed ? room : nullptr, b[8] | b[9] << 8, b[10] | b[11] << 8);
            } else if (b.size() == 6 && b[0] == 0x0a && b[1] == 2) {
                d2gs_client_object_destroy(d2gs_client_object_find(u32(2)));
            } else if (!b.empty() && (b[0] == 0x0e || b[0] == 0x4d)) {
                D2gsClientObjectEvent event = {};
                if (d2gs_client_object_packet(b.data(), static_cast<int>(b.size()), nullptr, &event) < 0) { std::fprintf(stderr, "refused %s\n", hex.c_str()); return 3; }
            } else { std::fprintf(stderr, "unknown packet %s\n", hex.c_str()); return 3; }
        } else if (op == "tick") {
            int n = 1; in >> n;
            for (int i = 0; i < n; ++i)
                for (D2UnitStrc* unit = d2gs_client_object_next(nullptr); unit; unit = d2gs_client_object_next(unit)) d2gs_client_object_step(unit);
        } else if (op == "dump") {
            int count = 0;
            std::string hex;
            for (D2UnitStrc* unit = d2gs_client_object_next(nullptr); unit; unit = d2gs_client_object_next(unit), ++count) {
                uint8_t record[kClientObjectViewBytes];
                d2gs_client_object_view(unit, record);
                char two[3];
                for (uint8_t byte : record) { std::snprintf(two, sizeof two, "%02x", byte); hex += two; }
            }
            std::printf("units %d %s\n", count, hex.c_str());
        }
    }
    return 0;
}
