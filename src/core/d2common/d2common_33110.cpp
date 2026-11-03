// 1.13c D2Common+0x33110 (#10819) UNITS_UpdateAnimRateAndVelocity(pUnit, szFile, nLine), stdcall, ret 0xC. Written from the
// 1.13c disassembly by D2UnitStrc field (x86 offsets: +0x00 type, +0x04 class, +0x10 mode, +0x2C path, +0x3C seq speed,
// +0x4C anim speed, +0x50 anim data, +0x60 inventory). 1.10f split this into inlined helpers; 1.13c keeps one body, and its
// rate products are divided UNSIGNED (a negative product saturates to 0x7FFF where 1.10f clamped it to 0 or 1).
#include "d2common_33110.h"
#include "D2DataTbls.h"

static_assert(offsetof(D2AnimDataRecordStrc, dwAnimSpeed) == 0x0C, "anim data speed at +0xC");

namespace {
constexpr int kStatVelocityPercent = 0x43, kStatAttackRate = 0x44, kStatOtherAnimRate = 0x45;
constexpr int kStateHolyShield = 0x65, kMaskMeleeOnly = 38, kItemTypeWeapon = 0x2D;
constexpr int kBodyRArm = 4, kBodyLArm = 5;

// (a * b) / 100 as retail does it: a 32-bit wrapping product, an unsigned divide
inline uint32_t mulDivU(int32_t a, int32_t b) { return (uint32_t(a) * uint32_t(b)) / 100u; }
// (a * b) / 100 signed (truncating), 32-bit wrapping product
inline int32_t mulDivS(int32_t a, int32_t b) { return int32_t(uint32_t(a) * uint32_t(b)) / 100; }
inline int16_t capU(uint32_t q) { return q == 0 ? 0 : int16_t(q >= 0x7FFF ? 0x7FFF : q); }
inline int16_t capS(int32_t v) { return v <= 0 ? 0 : int16_t(v >= 0x7FFF ? 0x7FFF : v); }
}

void D2Common_33110_UpdateAnimRate(D2UnitStrc* pUnit, const D2Common33110Env& x)
{
    if (!pUnit) return;
    int nType = int32_t(pUnit->dwUnitType);
    if (nType >= 5 || nType == 4) return;   // signed: a negative type goes on

    int nClass = int32_t(pUnit->dwClassId), nMode = int32_t(pUnit->dwAnimMode);
    x.convertMode(pUnit, &nType, &nClass, &nMode);
    x.setAnimData(pUnit, nType, nClass, nMode);
    const D2AnimDataRecordStrc* pAnim = pUnit->pAnimData;

    if (x.fcrMode(pUnit, nType, nMode)) {                    // cast: FCR, +100, capped 175; also the sequence speed
        int nRate = x.basePct(2, pUnit) + 100;
        if (nRate > 175) nRate = 175;
        const int16_t s = capU(mulDivU(pAnim->dwAnimSpeed, nRate));
        pUnit->wAnimSpeed = s;
        pUnit->dwSeqSpeed = uint32_t(int32_t(s));
        return;
    }
    if (x.blockMode(nType, nMode)) {                         // block: FBR + 50 (100 under holy shield), at least 1
        const int nRate = x.basePct(3, pUnit);
        int nBase = 50;
        const int nRaw = int32_t(pUnit->dwUnitType);
        if (nRaw >= 0 && (nRaw <= 1 || nRaw == 3) && sgptDataTables->nStatesTxtRecordCount > kStateHolyShield
            && x.checkState(pUnit, kStateHolyShield))
            nBase = 100;
        const uint32_t q = mulDivU(nBase + nRate, pAnim->dwAnimSpeed);
        pUnit->wAnimSpeed = q < 1 ? 1 : capU(q);
        return;
    }
    if (x.hitMode(nType, nMode)) {                           // get-hit: FHR + 50
        const int nRate = x.basePct(1, pUnit);
        pUnit->wAnimSpeed = capU(mulDivU(nRate + 50, pAnim->dwAnimSpeed));
        return;
    }
    if (x.knockMode(nType, nMode)) {                         // knockback: the base walk/run rate, velocity 0x1000
        pUnit->wAnimSpeed = capS(x.baseAnimSpeed(nType, nClass, nMode));
        x.setVelocity(pUnit->pDynamicPath, 0x1000);
        return;
    }
    if (x.velMode(nType, nMode, pUnit, nClass)) {            // walk/run: velocity percent + FRW, at least 25; no path = nothing
        D2DynamicPathStrc* pPath = pUnit->pDynamicPath;
        if (!pPath) return;
        int nPct = x.stat(pUnit, kStatVelocityPercent, 0);
        nPct += x.basePct(4, pUnit);
        if (nPct < 25) nPct = 25;
        pUnit->wAnimSpeed = capS(mulDivS(x.baseAnimSpeed(nType, nClass, nMode), nPct));
        x.setVelocity(pPath, mulDivS(x.baseVelocity(nType, nClass), nPct));
        return;
    }
    if (x.atkMode(nType, nMode, pUnit, nClass)) {            // attack: attack rate + IAS, dual-wield average, 15..175
        x.mergeDual(pUnit, 1);
        int nRate = x.stat(pUnit, kStatAttackRate, 0);
        nRate += x.basePct(0, pUnit);
        if (x.canDual(pUnit)) {
            D2InventoryStrc* pInv = pUnit->pInventory;
            const bool bInv = pInv && pInv->dwSignature == D2C_InventoryHeader;
            D2UnitStrc* pRight = bInv ? x.bodyItem(pInv, kBodyRArm) : nullptr;
            if (!x.canEquip(pRight)) pRight = nullptr;
            D2UnitStrc* pLeft = bInv ? x.bodyItem(pInv, kBodyLArm) : nullptr;
            if (!x.canEquip(pLeft)) pLeft = nullptr;
            if (x.itemType(pRight, kItemTypeWeapon) && x.itemType(pLeft, kItemTypeWeapon)) {
                const int nR = x.stat(pRight, kStatAttackRate, 0);
                const int nL = x.stat(pLeft, kStatAttackRate, 0);
                nRate += int32_t(uint32_t(nL) + uint32_t(nR)) / 2 - nR;
            }
        }
        if (pUnit->dwUnitType == 0 && pUnit->dwAnimMode == 0x12) nRate -= 30;   // the raw fields, not the converted ones
        if (nRate < 15) nRate = 15; else if (nRate > 175) nRate = 175;
        int nBase = 0;
        if (x.checkMask(pUnit, kMaskMeleeOnly)) nBase = x.meleeSpeed(pUnit);
        if (!nBase) nBase = pAnim->dwAnimSpeed;
        const int16_t s = capU(mulDivU(nBase, nRate));
        pUnit->dwSeqSpeed = uint32_t(int32_t(s));
        pUnit->wAnimSpeed = s;
        return;
    }
    if (x.allowsAnim(nType, nMode, nClass)) {                // other: the anim data rate as is
        pUnit->wAnimSpeed = capS(pAnim->dwAnimSpeed);
        return;
    }
    int nRate = x.stat(pUnit, kStatOtherAnimRate, 0);         // other: other-anim-rate stat, 15..175
    if (nRate < 15) nRate = 15; else if (nRate > 175) nRate = 175;
    pUnit->wAnimSpeed = capU(mulDivU(pAnim->dwAnimSpeed, nRate));
}
