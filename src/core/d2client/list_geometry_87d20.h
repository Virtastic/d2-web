#pragma once
#include <cstdint>

// Native ownership replaces retail pointer slots; integer fields retain retail widths.
namespace d2client_leaf {
struct Entry { uint32_t reserved, key, id, reserved_c[4], value; };
struct EntryNode { Entry* entry; EntryNode* next; };
struct TextNode { int32_t key3c, key40; char16_t text66[13]; TextNode* next; };
struct ValueRecord { uint32_t value14; };
struct ValueNode { int32_t id; ValueRecord* record; ValueNode* next; };
struct ValueList { ValueNode* first; };
struct ValueUnit { ValueList* list; };
struct Square { int32_t x_ea68, y_ea6c, extent_ea78; };
}
extern "C" {
uint32_t d2_client_87d20(d2client_leaf::EntryNode*, uint32_t);
uint32_t d2_client_87e60(d2client_leaf::EntryNode*, uint32_t);
uint32_t d2_client_87f00(d2client_leaf::EntryNode*, uint32_t);
void d2_client_87ed0(d2client_leaf::EntryNode*, uint32_t, uint32_t);
uint32_t d2_client_69090(const d2client_leaf::ValueUnit*, int32_t);
bool d2_client_7a040(const d2client_leaf::Square*, int32_t, int32_t);
uint32_t d2_client_8beb0(uint32_t, uint32_t, uint32_t);
uint8_t* d2_client_6d6d0(uint8_t*, int32_t, int32_t);
char16_t* d2_client_9ce00(d2client_leaf::TextNode*, int32_t);
}
