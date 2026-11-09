// n67: STALLED rows implemented from the decompile. Callees (other D2Game functions, D2Common
// ordinals, CRT) are host function-pointer seams in N67Seams; register-passed args are explicit.
#include <cstdint>
#include <cstring>

struct N67Seams {
    // packet sender D2Game+0x8A3E0 (pBuf, len)
    void (*sendPacket)(const void* pBuf, int len);
    // D2Common ordinals by number (generic cdecl-ish seam, args as intptr)
    intptr_t (*ord)(int ordinal, intptr_t a, intptr_t b, intptr_t c);
    // other D2Game functions by RVA
    intptr_t (*fn)(uint32_t rva, intptr_t a, intptr_t b, intptr_t c, intptr_t d, intptr_t e);
    void (*fatal)(int line); // assert path: Ordinal_10265/10024 then _exit(-1)
};

// 1.13c D2GAME+0x8A670
void n67_SendPacket1(const N67Seams* s, uint8_t id) { s->sendPacket(&id, 1); }

// 1.13c D2GAME+0x8C380
void n67_SendPacket5(const N67Seams* s) { uint32_t b[2] = {0, 0}; s->sendPacket(b, 5); }

// 1.13c D2GAME+0x8B220
void n67_SendPacket0x81(const N67Seams* s, uint32_t p2, uint8_t p4, uint32_t p5, uint16_t p6, uint32_t p7, uint32_t p8)
{
    uint8_t b[20];
    b[0] = 0x81; b[1] = p4; memcpy(b + 2, &p6, 2);
    memcpy(b + 4, &p5, 4); memcpy(b + 8, &p7, 4); memcpy(b + 12, &p2, 4); memcpy(b + 16, &p8, 4);
    s->sendPacket(b, 0x14);
}

// 1.13c D2GAME+0xE12D0
int* n67_CreatePresetUnit(const N67Seams* s, uint32_t eax, int type, int a3, void* a4, int a5, int a6)
{
    int* u;
    if (type == 1) {
        u = (int*)s->fn(0x47CF0, (intptr_t)a4, a3, a5, a6, 0);
    } else {
        if (type == 2) {
            if ((int)eax > 0x23d) { u = (int*)s->fn(0x2E830, 0, 0, 0, 0, 0); goto done; }
            if (eax == 0x23d) return nullptr;
        }
        u = (int*)s->fn(0xE1D90, type, eax, (uint8_t)a5, a6, a3);
        if (type == 4) {
            if (!u) return nullptr;
            uint32_t tmp[33] = {0};
            tmp[2] = (uint32_t)s->ord(10826, (intptr_t)a4, 0, 0);
            tmp[0x30 / 4 * 0 + 12] = 2; // local_60 (offset 0x30 into the 0x84-byte block)
            if (s->fn(0x11070, a3, (intptr_t)tmp, 0, 0, 0) == 0) {
                s->fn(0xE20A0, a3, (intptr_t)u, 0, 0, 0);
                return nullptr;
            }
        }
    }
done:
    if (u) u[0x31] |= 0x3000000;
    return u;
}

// 1.13c D2GAME+0xE13B0
int* n67_PresetUnitFromDesc(const N67Seams* s, const uint32_t* d /*ESI*/, int ebx, void* edi, int frame /*local_20 base*/)
{
    s->ord(10561, 0, 0, 0);
    int* u = n67_CreatePresetUnit(s, d[0], d[5], ebx, edi, d[2] + (int)(intptr_t)edi, frame + d[6]);
    if (u) {
        if (d[5] == 1) {
            u = (u[0] == 1 && u[5]) ? *(int**)(intptr_t)(u[5] + 0x28) : nullptr;
            if (d[4]) return (int*)s->ord(10171, 0, 0, 0);
        } else if (d[5] == 2) {
            u = (int*)s->fn(0x35E50, (intptr_t)u, d[4], 0, 0, 0);
        }
    }
    return u;
}

// 1.13c D2GAME+0xDE750
void n67_DE750(const N67Seams* s, int param_1, int* unit /*ESI*/, intptr_t ebp)
{
    if (!unit || !(unit[0] == 0 || unit[0] == 1)) return;
    if (!((int)s->ord(10587, 0, 0, 0) < param_1)) return;
    if (unit[0] == 0) {
        int a = unit[1];
        uint32_t u2 = (uint32_t)s->ord(10587, 0, 0, 0);
        uint32_t u3 = (uint32_t)s->ord(10399, a, param_1, 0);
        if (u2 < u3) s->fn(0xDE6D0, 0, 0, 0, 0, 0);
        return;
    }
    if (unit[5] == 0) return;
    int iv = *(int*)(intptr_t)(unit[5] + 0x28);
    if (!iv || !*(int*)(intptr_t)(iv + 0x28)) return;
    int* p = (int*)s->fn(0xE03A0, *(int*)(intptr_t)(iv + 0x28), *(uint32_t*)(intptr_t)(iv + 0x2c), 0, 0, 0);
    if (!p || *p != 0) return;
    int r = (int)s->ord(10066, 0, 0, 0);
    int cmp = unit[1]; // iVar1 left over from the earlier branch in the decompile
    if (cmp < r) {
        int rec = (int)s->fn(0x96570, 0, 0, 0, 0, 0);
        if (rec) {
            intptr_t self = *(intptr_t*)(ebp + 0x70);
            int it = (int)s->ord(11156, self, *(int*)(intptr_t)(rec + 8), 0xc);
            if (it) {
                s->ord(10587, 0, 0, 0);
                s->ord(10448, 0, *(int*)(intptr_t)(it + 0x20), 0);
                s->fn(0xDDCB0, self, self, (intptr_t)p, (intptr_t)unit, 0xd);
            }
        }
    }
}

// 1.13c D2GAME+0x98120
void n67_98120(const N67Seams* s, intptr_t game, int* unit, intptr_t retaddrArg, int dataTableCount)
{
    intptr_t pl = s->ord(10920, (intptr_t)unit, 0, 0);
    if (!pl) { s->fatal(0x2a0); return; }
    uint32_t id = unit ? (uint32_t)unit[3] : 0xffffffffu;
    int** list = *(int***)(pl + 0x44);
    if (!list) return;
    int idx = (int)s->fn(0x96600, 0, 0, 0, 0, 0);
    if (idx != 7) s->fn(0x97B50, retaddrArg, (intptr_t)unit, id, 0, 0);
    if (idx >= 0 && idx < dataTableCount) {
        uint32_t** head = (uint32_t**)((char*)*list + idx * 0xc);
        for (uint32_t* n = *head; n; n = (uint32_t*)(uintptr_t)n[5]) {
            if (n[1] == id) {
                n[0] |= 1;
                if (unit && unit[0] == 0) s->ord(10920, (intptr_t)unit, 0, 0);
                s->ord(11097, (intptr_t)unit, 0, 0);
                s->fn(0x8A690, 0, 0, 0, 0, 0);
                break;
            }
        }
        s->fn(0xDFC10, 0, 0, 0, 0, 0);
    }
    (void)game;
}

// 1.13c D2GAME+0x8EBC0
void n67_ExecuteMonsterEvents(const N67Seams* s, uint8_t* ev /*this*/, intptr_t game /*param_1*/, uint8_t* list /*param_2*/, int eax)
{
    auto run = [&](uint8_t* e) {
        e[2] |= 1;
        auto cb = *(void (**)(uint8_t, uint32_t, uint32_t))(e + 0x2c);
        if (!cb) s->fn(0x769A0, game, *(uint32_t*)(e + 0x14), *(uint32_t*)(e + 0x18), 0, 0);
        else cb(e[0], *(uint32_t*)(e + 0x14), *(uint32_t*)(e + 0x18));
        e[2] &= 0xfe;
    };
    *(uint32_t*)(list + 0xa18) = 0;
    if (eax == 0) {
        while (ev) {
            *(uint32_t*)(list + 0xa18) = *(uint32_t*)(ev + 0x1c);
            run(ev);
            if (ev[2] & 8) s->fn(0x8E6F0, 0, 0, 0, 0, 0);
            ev = (uint8_t*)(uintptr_t) * (uint32_t*)(list + 0xa18);
        }
    } else {
        while (ev) {
            *(uint32_t*)(list + 0xa18) = *(uint32_t*)(ev + 0x1c);
            if (*(int*)(ev + 4) == *(int*)(game + 0xa8)) { run(ev); s->fn(0x8E6F0, 0, 0, 0, 0, 0); }
            ev = (uint8_t*)(uintptr_t) * (uint32_t*)(list + 0xa18);
        }
    }
}

// 1.13c D2GAME+0x8C810  regs: EAX=pItem, EBX=pGame, EDI=unit; [esp+4]=flag (ret 4). pGame is checked against
// #10920(unit)+0x9c for a unit of type 0; then item mode is read (#10020), cleared (#10728), the item is
// re-flagged (#10012), packet 0x9D action 5 is sent via 0x8B0F0 (ECX=unit, ESI=item, AL=5, stack ebx/flag/0),
// and #10012 restores the saved byte. Returns the last #10012 result (EAX at ret).
intptr_t n67_RemoveItem(const N67Seams* s, int* unit /*EDI*/, int ebx, intptr_t item /*EAX*/, uint32_t flag)
{
    int cur = 0;
    if (unit && *unit == 0) cur = *(int*)(s->ord(10920, (intptr_t)unit, 0, 0) + 0x9c);
    if (cur != ebx) s->fatal(0x802);
    intptr_t saved = s->ord(10020, item, 0, 0) & 0xff;
    intptr_t m = s->ord(10728, item, 0, 0);
    s->ord(10012, item, m, 0);
    s->fn(0x8B0F0, ebx, flag, (intptr_t)unit, item, 5);
    return s->ord(10012, item, saved, 0);
}

// 1.13c D2GAME+0x78610
void n67_78610(const N67Seams* s, intptr_t thiz, int param_1, int mode /*EAX*/)
{
    if (!param_1) { s->fatal(0x139); return; }
    intptr_t u = *(intptr_t*)(thiz + 0x2c);
    s->ord(10498, u, 0, 0);
    switch (mode) {
    case 3:
        if (s->ord(10973, thiz, 10, 0) != 0) break;
        [[fallthrough]];
    case 2: case 6: {
        intptr_t x = s->ord(10331, thiz, 0, 0);
        mode = (s->ord(10057, x, 0, 0) ? 4 : 0) + 2;
        break; }
    case 0x13:
        s->ord(10647, u, 8, 0);
        s->ord(10527, u, 5, 0);
    }
    s->ord(10334, u, thiz, 0);
    s->ord(11103, thiz, 0, 0);
    if (s->ord(10800, u, 0, 0) == 0) { s->fn(0x782F0, param_1, thiz, 0, 0, 0); return; }
    s->fn(0xDA750, 0, 0, 0, 0, 0);
    s->ord(11090, thiz, mode, 0);
    s->fn(0xE0750, 0, 0, 0, 0, 0);
    s->fn(0xE0B40, thiz, 0, 0, 0, 0);
}
