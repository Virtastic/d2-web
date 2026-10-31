// Callee-order rules shared by the engine and the FN probes. Each rule takes its callees as an Ops object (or
// callables) so the engine runs it on real units and a probe runs it on scripted/blob state.
#pragma once
#include <cstdint>

// 1.13c D2Game+0xE0BD0: the target of a unit's path, or null when it is the unit itself.
template <class P> inline P SUNIT_TargetOrNull(P pUnit, P pTarget) { return pTarget == pUnit ? P{} : pTarget; }

// 1.13c D2Game+0xDF980: free the attacker combat list, then change the anim mode.
template <class FreeFn, class ChangeFn> inline void SUNIT_SetCombatModeSteps(FreeFn freeCombatList, ChangeFn changeAnimMode)
{
    freeCombatList();
    changeAnimMode();
}

// 1.13c D2Game+0x8C890: a 0x9D ITEMINSOCKET packet for every socketed child, flags carrying the deleted bit.
// Ops: socketable(item), flagSet(flags, bit), inventory(item), first(item, inv), asItem(child), setDeleted(flags&),
// send(client, item, child, flags), next(child).
template <class Ops, class C, class I>
inline int32_t SendItemSocketPacketsRule(Ops& o, C pClient, I pItem, uint32_t dwFlag)
{
    int32_t nFlags = (int32_t)dwFlag;
    if (!o.socketable(pItem) || o.flagSet(nFlags, 0x20)) return 0;
    auto pInv = o.inventory(pItem);
    if (!pInv) return 0;
    for (auto i = o.first(pItem, pInv); i; i = o.next(i))
    {
        auto pChild = o.asItem(i);
        o.setDeleted(nFlags);
        o.send(pClient, pItem, pChild, (uint32_t)nFlags);
    }
    return 1;
}

// 1.13c D2Game+0x8B0F0: the 0x9D item-action packet (action, component, item id, owning unit, item bitstream).
// Ops: component(unit, item), header(action, comp, unit, item), getFlags(item), setFlag(item, flag, on),
// serialize(item, gamble) -> bytes, emit(len) -> false when too long, sockets(client, item, flags).
template <class Ops, class C, class U, class I>
inline void SendPacket0x9DRule(Ops& o, C pClient, U pUnit, I pItem, char nAction, uint32_t dwFlag, int32_t bGamble)
{
    const auto nComponent = o.component(pUnit, pItem);
    o.header(nAction, nComponent, pUnit, pItem);
    const uint32_t nOldItemFlags = o.getFlags(pItem);
    o.setFlag(pItem, dwFlag, 1);
    const uint32_t nLength = (uint32_t)o.serialize(pItem, bGamble) + 13;
    if (nLength > 0xFC) return;                 // retail: Fog assert + exit
    o.setFlag(pItem, 0xFFFFFFFFu, 0);
    o.setFlag(pItem, nOldItemFlags, 1);
    o.emit(pClient, nLength);
    o.sockets(pClient, pItem, dwFlag);
}
