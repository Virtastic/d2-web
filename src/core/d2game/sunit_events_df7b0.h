// 1.13c D2Game+0xDF7B0, written from the decompile as a rule over an environment E:
// schedule the per-frame animation events of a unit's current mode. The vendored D2MOO 1.10f body
// (src/core/d2moo/src/D2Game/UNIT/SUnit.cpp:sub_6FCBCE70) is this same rule with the walk loop rotated and
// EVENT_SetEvent inlined into the direct D2GAME_InitTimer call; a probe's E logs and scripts the calls
// (src/core/fn/hookenv.h).
//
// Reads: pGame+0xA8 dwGameFrame; pUnit+0x30 pAnimSeq, +0x4C wAnimSpeed (int16) / +0x48 dwFrameCountPrecise
// when pAnimSeq is null, +0x3C dwSeqSpeed / +0x34 dwSeqFrameCount otherwise; pUnit+0x50 pAnimData, whose
// pFrameFlags array is inline at +0x10 (144 entries). Writes pUnit+0x44 nSeqCurrentFramePrecise.
//
// Every D2Common ordinal (0xA622 UNITS_GetFrameBonus, 0xA5F8 DATATBLS_GetSequenceEvent) and every queued
// event (0x8EA80 D2GAME_InitTimer, the inlined EVENT_SetEvent) is one E::h call. At an event call
// ecx = pGame, edx = pUnit and the stack holds (type, expire, callback, customId, customParam); the
// descriptor's `cd1` hook logs only the registers and stack words 1..4, so the type is not a record: a
// customId of 0 with param 0 is the ENDANIM, every other entry a MODECHANGE.
#pragma once
#include <cstdint>

namespace df7b0 {
// 0x8EA80: __fastcall(pGame, pUnit, type, expire, callback, customId, customParam), ret 0x14.
template <class E> static inline void QueueEvent(E& e, uint32_t game, uint32_t unit, uint32_t expire, uint32_t customId, uint32_t param)
{
    e.h(0x8EA80, 0, game, unit, 0, 0, expire, 0, customId, param);
}

// 0xDF7B0: fastcall(pGame on the stack, pUnit in ESI). F = pGame->dwGameFrame, t = UNITS_GetFrameBonus(pUnit) << 8
// (8.8 fixed point). Walk the clock from t in +speed steps until it reaches the length; each step advances the
// game frame by one and queues a MODECHANGE for every newly reached integer index (skipped once index >= 144).
// Then the ENDANIM one frame past the last frame -- with `frame == F -> ++frame` when the walk never ran --
// and the animation clock pinned back to F.
template <class E> void Schedule(E& e, uint32_t game, uint32_t unit)
{
    const int32_t gameFrame = (int32_t)e.rd32(game + 0xA8);
    int32_t clock = (int32_t)e.h(0xA622, 0, 0, 0, 0, 0, unit) << 8;
    const uint32_t seq = e.rd32(unit + 0x30);
    int32_t speed, count;
    if (seq == 0) { speed = e.rd16s(unit + 0x4C); count = (int32_t)e.rd32(unit + 0x48); }
    else { speed = (int32_t)e.rd32(unit + 0x3C); count = (int32_t)e.rd32(unit + 0x34); }
    if (speed == 0) { QueueEvent(e, game, unit, (uint32_t)gameFrame + 1, 0, 0); return; }

    const uint32_t animData = e.rd32(unit + 0x50);
    int32_t frame = gameFrame, index = clock >> 8, param = 0, hi;
    for (;;)
    {
        do
        {
            clock += speed;
            if (count <= clock)
            {
                if (frame == (int32_t)e.rd32(game + 0xA8)) ++frame;
                QueueEvent(e, game, unit, (uint32_t)frame + 1, 0, 0);
                e.wr32(unit + 0x44, (uint32_t)((int32_t)e.rd32(game + 0xA8) << 8));
                return;
            }
            hi = clock >> 8;
            ++frame;
        } while (hi < index);

        do
        {
            if (index >= 144) break;
            uint32_t event;
            if (seq == 0)
            {
                event = e.rd8(animData + 0x10 + (uint32_t)index);
            }
            else
            {
                uint32_t out = 0;
                e.h(0xA5F8, 0, 0, 0, 0, 0, seq, (uint32_t)(index << 8), e.stk(&out));
                event = out;
            }
            if (event == 1 || event == 2 || event == 4) { QueueEvent(e, game, unit, (uint32_t)frame, event, (uint32_t)param); ++param; }
            else if (event == 3) QueueEvent(e, game, unit, (uint32_t)frame, 3, 0);
            ++index;
        } while (index <= hi);
    }
}
}
