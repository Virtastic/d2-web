// 1.13c D2Client+0x5B260: Hireables UI init. Loads the hireable icon cels, allocates the zeroed 20-byte-per-hireable table.
#include <cstdint>
struct Client5B260Callees {
    uint32_t (*icon)(int32_t nameId);              // 0xBF6C0 (name in ESI): loads "Hireables\<name>"; nameId indexes kNames
    int32_t (*expansion)();                        // 0xBF8C -> Fog #10227
    uint8_t* (*alloc)(uint32_t size);              // 0xC010 -> Fog #10045("..\Source\D2Client\UI\Hireables.cpp", 0x96, 0)
};
// Icon name ids, in call order: 0..4 classic (Amazon, Sorceress, Barbarian, Paladin, Necromancer), 5 Assassin, 6 Druid.
// g[0] = DAT +0x1177C4 (table ptr, low 32 bits), g[1..7] = DAT +0x1177C8.. (icon handles). count = *(*sgptDataTables+0xBF0).
extern "C" void d2_client_5b260(uint32_t* g, uint32_t count, const Client5B260Callees& c) {
    for (int i = 1; i < 8; ++i) g[i] = 0;
    for (int i = 0; i < 5; ++i) g[1 + i] = c.icon(i);
    if (c.expansion()) for (int i = 5; i < 7; ++i) g[1 + i] = c.icon(i);
    uint8_t* p = c.alloc(count * 20);
    g[0] = (uint32_t)(uintptr_t)p;
    for (uint32_t i = 0; i < count * 20; ++i) p[i] = 0;
}
