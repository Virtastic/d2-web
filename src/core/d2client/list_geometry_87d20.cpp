// Prior art used: ; D2MOO has no D2Client implementation.
// Recovered from the cached 1.13c functions. Register-only arguments are explicit;
// each list argument is its owning live head (retail globals 11BF30 / 11BC14).
#include "list_geometry_87d20.h"
#include <bit>
#include <emscripten.h>
using namespace d2client_leaf;

// 1.13c D2Client+0x87D20
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_87d20(EntryNode* head, uint32_t key) {
    for (auto* n = head; n; n = n->next)
        if (n->entry->key == key) return n->entry->id;
    return 0;
}
// 1.13c D2Client+0x87E60
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_87e60(EntryNode* head, uint32_t id) {
    if (id) for (auto* n = head; n; n = n->next)
        if (n->entry->id == id) return n->entry->key;
    return 0;
}
// 1.13c D2Client+0x87F00
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_87f00(EntryNode* head, uint32_t id) {
    if (id) for (auto* n = head; n; n = n->next)
        if (n->entry->id == id) return n->entry->value;
    return 0;
}
// 1.13c D2Client+0x87ED0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_87ed0(EntryNode* head, uint32_t id, uint32_t value) {
    if (id) for (auto* n = head; n; n = n->next)
        if (n->entry->id == id) { n->entry->value = value; return; }
}
// 1.13c D2Client+0x69090
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_69090(const ValueUnit* unit, int32_t id) {
    if (unit && unit->list) for (auto* n = unit->list->first; n; n = n->next)
        if (n->id == id) return n->record->value14;
    return 0;
}
// 1.13c D2Client+0x7A040
extern "C" EMSCRIPTEN_KEEPALIVE bool d2_client_7a040(const Square* square, int32_t x, int32_t y) {
    const auto dx = std::bit_cast<int32_t>(uint32_t(x) - uint32_t(square->x_ea68));
    const auto dy = std::bit_cast<int32_t>(uint32_t(y) - uint32_t(square->y_ea6c));
    return dx >= 0 && dy >= 0 && dx < square->extent_ea78 && dy < square->extent_ea78;
}
// 1.13c D2Client+0x8BEB0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_8beb0(uint32_t base_11b9a4, uint32_t add_dbc4c, uint32_t flag_119854) {
    return base_11b9a4 + add_dbc4c - (flag_119854 == 0 ? 0x41u : 0x40u);
}
// 1.13c D2Client+0x6D6D0
extern "C" EMSCRIPTEN_KEEPALIVE uint8_t* d2_client_6d6d0(uint8_t* data, int32_t row, int32_t column) {
    if (!data) return nullptr;
    const uint32_t offset = (uint32_t(data[1]) * uint32_t(row) + 9u + uint32_t(column)) *
                            uint32_t(data[0]) + uint32_t(data[1]) + 0x1cu;
    return data + offset;
}
// 1.13c D2Client+0x9CE00
extern "C" EMSCRIPTEN_KEEPALIVE char16_t* d2_client_9ce00(TextNode* head, int32_t key) {
    for (auto* n = head; n; n = n->next)
        if (n->key3c == key || n->key40 == key) return n->text66;
    return nullptr;
}
