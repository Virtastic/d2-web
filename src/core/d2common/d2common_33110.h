#pragma once
// D2Common 0x33110 (#10819) UNITS_UpdateAnimRateAndVelocity, written from the 1.13c disassembly. The callees are a table so
// the engine binds the D2MOO functions (Units.cpp) and the FN probe binds recorders. Comments name the retail callee RVA.
#include <cstdint>
#include "Units/Units.h"
#include "D2Inventory.h"

struct D2Common33110Env {
    void (*convertMode)(D2UnitStrc* pUnit, int* pType, int* pClass, int* pMode);   // 0x501E0 (#11013)
    void (*setAnimData)(D2UnitStrc* pUnit, int nType, int nClass, int nMode);      // 0x32670 (#11122)
    int  (*fcrMode)(D2UnitStrc* pUnit, int nType, int nMode);                      // 0x30B80 (EAX type, ECX mode, EDX unit)
    int  (*basePct)(int nAnimStat, D2UnitStrc* pUnit);                             // 0x323E0 (EAX stat row, stack unit)
    int  (*blockMode)(int nType, int nMode);                                       // 0x2EF90
    int  (*checkState)(D2UnitStrc* pUnit, int nState);                             // 0x37DB0 (EAX unit, ECX state)
    int  (*hitMode)(int nType, int nMode);                                         // 0x2EFC0
    int  (*knockMode)(int nType, int nMode);                                       // 0x2F010
    int  (*baseAnimSpeed)(int nType, int nClass, int nMode);                       // 0x2FB10 (EAX class, ECX type, EDX mode)
    void (*setVelocity)(D2DynamicPathStrc* pPath, int nVelocity);                  // 0x34D40
    int  (*velMode)(int nType, int nMode, D2UnitStrc* pUnit, int nClass);          // 0x30C70 (ECX mode, EDX type, ESI class, stack unit)
    int  (*stat)(D2UnitStrc* pUnit, int nStat, int nLayer);                        // 0x38B70
    int  (*baseVelocity)(int nType, int nClass);                                   // 0x30D50 (ECX type, EAX class)
    int  (*atkMode)(int nType, int nMode, D2UnitStrc* pUnit, int nClass);          // 0x30BD0 (ECX mode, EDX type, stack unit, class)
    void (*mergeDual)(D2UnitStrc* pUnit, int bMerge);                              // 0x32EF0
    int  (*canDual)(D2UnitStrc* pUnit);                                            // 0x30530
    D2UnitStrc* (*bodyItem)(D2InventoryStrc* pInventory, int nBodyLoc);            // 0x1E1B0 grid 0 (EBX inventory), slot read
    int  (*canEquip)(D2UnitStrc* pItem);                                           // 0x23EF0
    int  (*itemType)(D2UnitStrc* pItem, int nItemType);                            // 0x24430
    int  (*checkMask)(D2UnitStrc* pUnit, int nStateMask);                          // 0x33C20 (EAX unit, ECX mask table)
    int  (*meleeSpeed)(D2UnitStrc* pUnit);                                         // 0x50CE0 (#11043)
    int  (*allowsAnim)(int nType, int nMode, int nClass);                          // 0x2FAC0 (EAX type, ECX mode, stack class)
};

void D2Common_33110_UpdateAnimRate(D2UnitStrc* pUnit, const D2Common33110Env& x);
