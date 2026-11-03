#pragma once
#include <cstdint>
// D2Common 0x20BC0 / 0x20E80 / 0x21300 / 0x21420 seams (the retail callees, 1.13c conventions).
// 0x20E80 (ECX = unit, EBX = item, stack out_a, out_b; ret 8) callees: 0x441E0 (stdcall unit, inv, &out_a slot, -1, 1),
// 0x231E0 (stdcall item; al = slot), 0x20830 (stdcall inv, id), 0x719A0 (stdcall class), 0x71960 (stdcall code, &out),
// 0x1FB50 (ESI = [rec+0x90], EDI = [rec+0x80]; no stack args; eax = composite id).
struct D2Common20E80Env {
    int32_t (*weaponClass)(uint32_t unit, uint32_t inv, uint32_t* pOut, int32_t a, int32_t b);
    uint8_t (*slot)(uint32_t item);
    uint32_t (*component)(uint32_t inv, int32_t id);
    uint32_t (*itemsTxt)(uint32_t cls);
    uint32_t (*itemByCode)(uint32_t code, uint32_t* pOut);
    uint32_t (*composite)(uint32_t esi, uint32_t edi);
};
void D2Common_20E80_UpdateItemTables(uint32_t unit, uint32_t outA, uint32_t outB, uint32_t item, const D2Common20E80Env* env);

// 0x21300 (stdcall unit, out_a, out_b, item; ret 0x10). Globals: items.txt record count (0x9FB94) and base (0x9FB98), stride 0x1A8.
struct D2Common21300Env {
    uint32_t itemCount, itemTable;
    uint32_t (*component)(uint32_t inv, int32_t id);                            // 0x20830
    uint8_t (*slot)(uint32_t item);                                             // 0x231E0
    uint32_t (*composite)(uint32_t esi, uint32_t edi);                          // 0x1FB50
    void (*update)(uint32_t unit, uint32_t item, uint32_t outA, uint32_t outB); // 0x20E80 (ECX = unit, EBX = item, stack out_a, out_b)
    int32_t (*color)(uint32_t unit, uint32_t item, uint32_t pColor, int32_t z); // 0x251C0 (stdcall)
};
void D2Common_21300_UnitItemSlots(uint32_t unit, uint32_t outA, uint32_t outB, uint32_t item, const D2Common21300Env* env);

// 0x20BC0 (stdcall inventory, reference item, start node; ret 0xC). dataTables = *0x99E1C (sgptDataTables).
struct D2Common20BC0Env {
    uint32_t dataTables;
    uint32_t (*recordId)(uint32_t unit, uint32_t zero, uint32_t one);   // 0x2FB60 (stdcall)
    uint32_t (*layout)(uint32_t rec, uint32_t zero, uint32_t out);      // 0x81FB0 (stdcall)
    uint32_t (*grid)(uint32_t gridNo, uint32_t inv, uint32_t layout);   // 0x1E1B0 (EAX = grid number, EBX = inventory, stack layout)
    uint32_t (*itemType)(uint32_t itemId);                              // 0x230A0 (stdcall)
    int32_t (*statTotal)(uint32_t edi, uint32_t rec);                   // 0x38A80 (EDI = 0x460000, stack record)
    int32_t (*value)(uint32_t item);                                    // 0x260F0 (stdcall)
};
uint32_t D2Common_20BC0_InventoryCriteriaScan(uint32_t inv, uint32_t ref, uint32_t start, const D2Common20BC0Env* env);

// 0x21420 (stdcall unit, out_a, out_b; ret 0xC). Globals: items.txt count/base (0x9FB94/0x9FB98), data tables (0x99E1C: type count +0xBFC,
// stride +0xC00, masks +0xC04), item types base (0xA13D4, stride 0x34), Fog bit mask 11 (0x800).
struct D2Common21420Env {
    uint32_t itemCount, itemTable, dataTables, typesTable, bitMask11;
    void (*context)(uint32_t node, uint32_t pOut);                              // 0x1E420 (EAX = node, ECX = 12-byte out)
    void (*refresh)(uint32_t unit, uint32_t outA, uint32_t outB, uint32_t node); // 0x21300 (stdcall)
    uint32_t (*typeIndex)(uint32_t i, uint32_t ctx);                            // 0x43710 (stdcall i, &ctx)
    uint32_t (*composite)(uint32_t esi, uint32_t edi);                          // 0x1FB50
    int32_t (*color)(uint32_t unit, uint32_t node, uint32_t pColor, int32_t z); // 0x251C0 (stdcall)
};
void D2Common_21420_RefreshItemNodes(uint32_t unit, uint32_t outA, uint32_t outB, const D2Common21420Env* env);
