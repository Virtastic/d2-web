// Shared plumbing of the 1.13c D2Game 0xA7A10 / 0xA8E80 / 0xA9E80 ports (src/core/d2game/d2game_a7a10.cpp, d2game_a8e80.cpp, d2game_a9e80.cpp), recovered from the
// retail disassembly like the rest of the 0xA0830-0xAE6D0 AI and skill helpers: parameter order is ecx, edx (fastcall), then the stack arguments in source order,
// then any other register input in the order eax, ebx, esi, edi. Pointer fields are int-width (at<int>): these three run on the retail 32-bit layout.
// Each callee is reached through a pointer in a0830d_env (default: the weak import itself), so a probe can substitute retail-recorded results
// (src/core/fn/d2game_a0830d_probe.h); the #define after each declaration routes the call sites through it. The env pointers are inline variables,
// so the three files share one definition of each.
#pragma once
#include <cstdint>
#include <cstdlib>
#include <unistd.h>

#define EXT(n) extern "C" __attribute__((weak)) intptr_t n(...); namespace a0830d_env { inline intptr_t (*n)(...) = &::n; }
// EXTA: an env import other TUs declare with a fixed signature; a namespaced symbol keeps the variadic calls without a wasm-ld signature clash.
#define EXTA(n) namespace b10_env { __attribute__((weak, import_module("env"), import_name(#n))) intptr_t n(...); } namespace a0830d_env { inline intptr_t (*n)(...) = &b10_env::n; }
EXT(Ordinal_10024); EXT(Ordinal_10057); EXT(Ordinal_10064); EXT(Ordinal_10101); EXT(Ordinal_10265);
#define Ordinal_10024 (a0830d_env::Ordinal_10024)
#define Ordinal_10057 (a0830d_env::Ordinal_10057)
#define Ordinal_10064 (a0830d_env::Ordinal_10064)
#define Ordinal_10101 (a0830d_env::Ordinal_10101)
#define Ordinal_10265 (a0830d_env::Ordinal_10265)
EXT(Ordinal_10306); EXT(Ordinal_10321); EXT(Ordinal_10331); EXTA(Ordinal_10392); EXT(Ordinal_10494);
#define Ordinal_10306 (a0830d_env::Ordinal_10306)
#define Ordinal_10321 (a0830d_env::Ordinal_10321)
#define Ordinal_10331 (a0830d_env::Ordinal_10331)
#define Ordinal_10392 (a0830d_env::Ordinal_10392)
#define Ordinal_10494 (a0830d_env::Ordinal_10494)
EXT(Ordinal_10507); EXTA(Ordinal_10511); EXT(Ordinal_10537); EXT(Ordinal_10546); EXTA(Ordinal_10601);
#define Ordinal_10507 (a0830d_env::Ordinal_10507)
#define Ordinal_10511 (a0830d_env::Ordinal_10511)
#define Ordinal_10537 (a0830d_env::Ordinal_10537)
#define Ordinal_10546 (a0830d_env::Ordinal_10546)
#define Ordinal_10601 (a0830d_env::Ordinal_10601)
EXT(Ordinal_10708); EXT(Ordinal_10711); EXT(Ordinal_10744); EXT(Ordinal_10785); EXT(Ordinal_10807);
#define Ordinal_10708 (a0830d_env::Ordinal_10708)
#define Ordinal_10711 (a0830d_env::Ordinal_10711)
#define Ordinal_10744 (a0830d_env::Ordinal_10744)
#define Ordinal_10785 (a0830d_env::Ordinal_10785)
#define Ordinal_10807 (a0830d_env::Ordinal_10807)
EXT(Ordinal_10828); EXT(Ordinal_10839); EXT(Ordinal_10871); EXTA(Ordinal_10910); EXT(Ordinal_10911);
#define Ordinal_10828 (a0830d_env::Ordinal_10828)
#define Ordinal_10839 (a0830d_env::Ordinal_10839)
#define Ordinal_10871 (a0830d_env::Ordinal_10871)
#define Ordinal_10910 (a0830d_env::Ordinal_10910)
#define Ordinal_10911 (a0830d_env::Ordinal_10911)
EXTA(Ordinal_10920); EXT(Ordinal_10945); EXTA(Ordinal_10973); EXT(Ordinal_10978); EXT(Ordinal_11013);
#define Ordinal_10920 (a0830d_env::Ordinal_10920)
#define Ordinal_10945 (a0830d_env::Ordinal_10945)
#define Ordinal_10973 (a0830d_env::Ordinal_10973)
#define Ordinal_10978 (a0830d_env::Ordinal_10978)
#define Ordinal_11013 (a0830d_env::Ordinal_11013)
EXT(Ordinal_11027); EXT(Ordinal_11043); EXT(Ordinal_11103); EXT(Ordinal_11108); EXT(Ordinal_11111);
#define Ordinal_11027 (a0830d_env::Ordinal_11027)
#define Ordinal_11043 (a0830d_env::Ordinal_11043)
#define Ordinal_11103 (a0830d_env::Ordinal_11103)
#define Ordinal_11108 (a0830d_env::Ordinal_11108)
#define Ordinal_11111 (a0830d_env::Ordinal_11111)
EXT(Ordinal_11138); EXTA(Ordinal_11139);
#define Ordinal_11138 (a0830d_env::Ordinal_11138)
#define Ordinal_11139 (a0830d_env::Ordinal_11139)
EXT(FUN_6fc21420); // 0x01420 skill row by id (eax = id)
#define FUN_6fc21420 (a0830d_env::FUN_6fc21420)
EXT(FUN_6fc95070); // 0x75070 AI command block init (stack n; esi buffer, edi unit)
#define FUN_6fc95070 (a0830d_env::FUN_6fc95070)
EXT(FUN_6fc95e10); // 0x75E10 (game, buffer, n)
#define FUN_6fc95e10 (a0830d_env::FUN_6fc95e10)
EXT(FUN_6fc98550); // 0x78550 (ecx game, edx unit, 5 stack) skill-fail notify
#define FUN_6fc98550 (a0830d_env::FUN_6fc98550)
EXT(FUN_6fcab710); // 0x8B710 packet 0x23 (edx unit id, client, bLeft, skill id, owner, al unit type)
#define FUN_6fcab710 (a0830d_env::FUN_6fcab710)
EXT(FUN_6fcae6f0); // 0x8E6F0 (eax event, edi game)
#define FUN_6fcae6f0 (a0830d_env::FUN_6fcae6f0)
EXTA(FUN_6fcaea80); // 0x8EA80 event queue add (ecx game, edx unit, 5 stack)
#define FUN_6fcaea80 (a0830d_env::FUN_6fcaea80)
EXT(FUN_6fcbe4f0); // 0x9E4F0 (ebx item, esi missile, edi unit)
#define FUN_6fcbe4f0 (a0830d_env::FUN_6fcbe4f0)
EXT(FUN_6fcbfc00); // 0x9FC00 (game, id; eax unit)
#define FUN_6fcbfc00 (a0830d_env::FUN_6fcbfc00)
EXT(FUN_6fcbfd00); // 0x9FD00 (game, unit, level, 0; eax id)
#define FUN_6fcbfd00 (a0830d_env::FUN_6fcbfd00)
EXT(FUN_6fcc1880); // 0xA1880 (game, missile, a1, a2, 0,0,0,1; eax 0, esi unit)
#define FUN_6fcc1880 (a0830d_env::FUN_6fcc1880)
EXT(FUN_6fcc1d10); // 0xA1D10 (game, id; edi unit)
#define FUN_6fcc1d10 (a0830d_env::FUN_6fcc1d10)
EXT(FUN_6fcc28f0); // 0xA28F0 (ecx game, edx missile, unit, a1, a2, 0, 0,0,0,1)
#define FUN_6fcc28f0 (a0830d_env::FUN_6fcc28f0)
EXT(FUN_6fcd0980); // 0xB0980 (a, b; ebx unit)
#define FUN_6fcd0980 (a0830d_env::FUN_6fcd0980)
EXT(FUN_6fcff780); // 0xDF780 game of unit (eax unit)
#define FUN_6fcff780 (a0830d_env::FUN_6fcff780)
EXT(FUN_6fd00660); // 0xE0660 (game, unit, id)
#define FUN_6fd00660 (a0830d_env::FUN_6fd00660)
EXT(FUN_6fd007a0); // 0xE07A0 (ecx game; eax unit)
#define FUN_6fd007a0 (a0830d_env::FUN_6fd007a0)
EXT(Ordinal_10522); EXT(Ordinal_10630); EXT(Ordinal_10786); EXT(Ordinal_11049);
#define Ordinal_10522 (a0830d_env::Ordinal_10522)
#define Ordinal_10630 (a0830d_env::Ordinal_10630)
#define Ordinal_10786 (a0830d_env::Ordinal_10786)
#define Ordinal_11049 (a0830d_env::Ordinal_11049)
EXT(FUN_6fc21100); // 0x01100 unit x (ecx unit)
#define FUN_6fc21100 (a0830d_env::FUN_6fc21100)
EXT(FUN_6fc21130); // 0x01130 unit y (ecx unit)
#define FUN_6fc21130 (a0830d_env::FUN_6fc21130)
EXT(FUN_6fc21180); // 0x01180 seeded roll below n (ecx seed, eax n)
#define FUN_6fc21180 (a0830d_env::FUN_6fc21180)
EXT(FUN_6fc41290); // 0x21290 (ecx unit, stack buffer; eax game)
#define FUN_6fc41290 (a0830d_env::FUN_6fc41290)
EXT(FUN_6fcbd230); // 0x9D230 (ecx game, edx unit, a, b)
#define FUN_6fcbd230 (a0830d_env::FUN_6fcbd230)
EXT(FUN_6fcc3b50); // 0xA3B50 (game, unit)
#define FUN_6fcc3b50 (a0830d_env::FUN_6fcc3b50)
EXT(FUN_6fcc4610); // 0xA4610 AI target pick (game, unit, &a, &b, 0, callback)
#define FUN_6fcc4610 (a0830d_env::FUN_6fcc4610)
EXT(FUN_6fcd0840); // 0xB0840 monster has skill (edx skill; eax unit)
#define FUN_6fcd0840 (a0830d_env::FUN_6fcd0840)
EXTA(FUN_6fd003a0); // 0xE03A0 unit by type/guid (ecx, edx, eax)
#define FUN_6fd003a0 (a0830d_env::FUN_6fd003a0)
EXT(FUN_6fd120c0); // 0xF20C0 (stack n; eax game, ebx unit)
#define FUN_6fd120c0 (a0830d_env::FUN_6fd120c0)
EXT(FUN_6fd12120); // 0xF2120 (game, target, 0, 0; edi skill)
#define FUN_6fd12120 (a0830d_env::FUN_6fd12120)
EXT(FUN_6fd121e0); // 0xF21E0 (game, a, b; eax mode, edi unit)
#define FUN_6fd121e0 (a0830d_env::FUN_6fd121e0)
EXT(FUN_6fd12220); // 0xF2220 (game, target; eax mode, edi unit)
#define FUN_6fd12220 (a0830d_env::FUN_6fd12220)
EXT(FUN_6fd122c0); // 0xF22C0 AI idle for n frames (stack n; eax game, esi unit)
#define FUN_6fd122c0 (a0830d_env::FUN_6fd122c0)
EXT(FUN_6fd12380); // 0xF2380 AI skill command (ecx unit, skill, target, x, y; al mode, ebx game)
#define FUN_6fd12380 (a0830d_env::FUN_6fd12380)
EXT(FUN_6fd12540); // 0xF2540 (ecx game, edx unit, target, 2, 0, 0, 1, 0)
#define FUN_6fd12540 (a0830d_env::FUN_6fd12540)
EXTA(Ordinal_10080); EXT(Ordinal_11005);
#define Ordinal_10080 (a0830d_env::Ordinal_10080)
#define Ordinal_11005 (a0830d_env::Ordinal_11005)
EXT(FUN_6fcd07b0); // 0xB07B0 (x, y; eax unit)
#define FUN_6fcd07b0 (a0830d_env::FUN_6fcd07b0)
EXT(FUN_6fd02780); // 0xE2780 (eax game, ebx unit, esi out xy)
#define FUN_6fd02780 (a0830d_env::FUN_6fd02780)
EXT(FUN_6fd06760); // 0xE6760 (edx unit; eax game)
#define FUN_6fd06760 (a0830d_env::FUN_6fd06760)
EXT(Ordinal_10136); EXT(Ordinal_10159); EXT(Ordinal_10408); EXT(Ordinal_10581); EXT(Ordinal_10885);
#define Ordinal_10136 (a0830d_env::Ordinal_10136)
#define Ordinal_10159 (a0830d_env::Ordinal_10159)
#define Ordinal_10408 (a0830d_env::Ordinal_10408)
#define Ordinal_10581 (a0830d_env::Ordinal_10581)
#define Ordinal_10885 (a0830d_env::Ordinal_10885)
EXT(Ordinal_11016);
#define Ordinal_11016 (a0830d_env::Ordinal_11016)
EXT(FUN_6fc798b0); // 0x598B0 (eax game)
#define FUN_6fc798b0 (a0830d_env::FUN_6fc798b0)
EXT(FUN_6fc79c50); // 0x59C50 (eax game)
#define FUN_6fc79c50 (a0830d_env::FUN_6fc79c50)
EXT(FUN_6fcbe810); // 0x9E810 (unit, level; eax target)
#define FUN_6fcbe810 (a0830d_env::FUN_6fcbe810)
EXT(FUN_6fcd17b0); // 0xB17B0 unit search (ecx query, edx game, unit, callback; eax mode)
#define FUN_6fcd17b0 (a0830d_env::FUN_6fcd17b0)
EXT(FUN_6fd020a0); // 0xE20A0 (ecx game, edx unit)
#define FUN_6fd020a0 (a0830d_env::FUN_6fd020a0)
EXT(FUN_6fd11b10); // 0xF1B10 (game, unit; al mode)
#define FUN_6fd11b10 (a0830d_env::FUN_6fd11b10)
EXT(FUN_6fd12420); // 0xF2420 (delay, skill id, skill mode; esi unit)
#define FUN_6fd12420 (a0830d_env::FUN_6fd12420)
EXT(FUN_6fd12d80); // 0xF2D80 (game, unit, 5, 1; eax target)
#define FUN_6fd12d80 (a0830d_env::FUN_6fd12d80)
EXT(FUN_6fd13d20); // 0xF3D20 (ecx y, edx monster class, x, 0)
#define FUN_6fd13d20 (a0830d_env::FUN_6fd13d20)
EXT(FUN_6fc79230); // 0x59230 (stack unit; eax game)
#define FUN_6fc79230 (a0830d_env::FUN_6fc79230)
EXT(FUN_6fc79290); // 0x59290 (stack unit; eax game)
#define FUN_6fc79290 (a0830d_env::FUN_6fc79290)
EXT(FUN_6fc7a160); // 0x5A160 (ecx game, stack unit; eax node)
#define FUN_6fc7a160 (a0830d_env::FUN_6fc7a160)
EXT(FUN_6fc7a200); // 0x5A200 (eax game)
#define FUN_6fc7a200 (a0830d_env::FUN_6fc7a200)
EXT(FUN_6fc7a290); // 0x5A290 (stack unit, out target; edi game)
#define FUN_6fc7a290 (a0830d_env::FUN_6fc7a290)
EXT(FUN_6fc7b9b0); // 0x5B9B0 (game, node, unit)
#define FUN_6fc7b9b0 (a0830d_env::FUN_6fc7b9b0)
EXT(FUN_6fcaa770); // 0x8A770 send 6-byte packet (stack client; eax buffer)
#define FUN_6fcaa770 (a0830d_env::FUN_6fcaa770)
EXT(FUN_6fcd1bc0); // 0xB1BC0 nearest target (ecx game, stack out distance; eax unit, ebx out)
#define FUN_6fcd1bc0 (a0830d_env::FUN_6fcd1bc0)
EXT(FUN_6fd0fdb0); // 0xEFDB0 (stack unit, 0; ebx mode, esi game)
#define FUN_6fd0fdb0 (a0830d_env::FUN_6fd0fdb0)
EXT(FUN_6fd130c0); // 0xF30C0 (stack game, out; esi unit)
#define FUN_6fd130c0 (a0830d_env::FUN_6fd130c0)
EXT(FUN_6fc21320); // 0x01320 MonStats2 row of a monster class (eax class)
#define FUN_6fc21320 (a0830d_env::FUN_6fc21320)
EXT(FUN_6fc57680); // 0x37680 player busy test (esi unit)
#define FUN_6fc57680 (a0830d_env::FUN_6fc57680)
EXTA(FUN_6fcfffd0); // 0xDFFD0 (stack n, other; esi unit)
#define FUN_6fcfffd0 (a0830d_env::FUN_6fcfffd0)
EXTA(Ordinal_10038); EXT(Ordinal_10188); EXT(Ordinal_10285);
#define Ordinal_10038 (a0830d_env::Ordinal_10038)
#define Ordinal_10188 (a0830d_env::Ordinal_10188)
#define Ordinal_10285 (a0830d_env::Ordinal_10285)
EXT(FUN_6fc9a090); // 0x7A090 queue overlay event (stack id; esi unit)
#define FUN_6fc9a090 (a0830d_env::FUN_6fc9a090)
EXT(FUN_6fcc4e40); // 0xA4E40 (stack superunique; esi unit)
#define FUN_6fcc4e40 (a0830d_env::FUN_6fcc4e40)
EXT(FUN_6fccfc50); // 0xAFC50 (game, id, 1; eax unit)
#define FUN_6fccfc50 (a0830d_env::FUN_6fccfc50)
EXT(Ordinal_10750); EXT(Ordinal_10826); EXT(Ordinal_10851); EXT(Ordinal_10867);
#define Ordinal_10750 (a0830d_env::Ordinal_10750)
#define Ordinal_10826 (a0830d_env::Ordinal_10826)
#define Ordinal_10851 (a0830d_env::Ordinal_10851)
#define Ordinal_10867 (a0830d_env::Ordinal_10867)
EXT(FUN_6fc57440); // 0x37440 player's linked unit guid (eax unit)
#define FUN_6fc57440 (a0830d_env::FUN_6fc57440)
EXT(FUN_6fccf840); // 0xAF840 (ecx unit; esi out[5], edi game)
#define FUN_6fccf840 (a0830d_env::FUN_6fccf840)
EXT(FUN_6fcd0a20); // 0xB0A20 (stack other; ebx unit)
#define FUN_6fcd0a20 (a0830d_env::FUN_6fcd0a20)
EXT(FUN_6fd13e30); // 0xF3E30 skill usable on (ecx unit, stack game, skill, 0; eax 0, esi other)
#define FUN_6fd13e30 (a0830d_env::FUN_6fd13e30)
EXT(FUN_6fc3cae0); // 0x1CAE0 AI approach (stack game, target, a, b, c, d; eax unit), ret 0x18
#define FUN_6fc3cae0 (a0830d_env::FUN_6fc3cae0)
EXT(FUN_6fcd1c90); // 0xB1C90 nearest unit (stack unit, state, &out1, &out2; eax game), ret 0x10
#define FUN_6fcd1c90 (a0830d_env::FUN_6fcd1c90)
EXT(FUN_6fcd0e90); // 0xB0E90 (ecx unit; eax other)
#define FUN_6fcd0e90 (a0830d_env::FUN_6fcd0e90)
EXT(FUN_6fcc41b0); // 0xA41B0 (dl kind, stack game, target; eax mode, esi unit), ret 8
#define FUN_6fcc41b0 (a0830d_env::FUN_6fcc41b0)
EXT(FUN_6fcfeee0); // 0xDEEE0 (ecx game, edx unit, stack unit, 1), ret 8
#define FUN_6fcfeee0 (a0830d_env::FUN_6fcfeee0)
EXT(FUN_6fd11c30); // 0xF1C30 (stack game, n; eax target, edi unit), ret 8
#define FUN_6fd11c30 (a0830d_env::FUN_6fd11c30)
EXT(FUN_6fc3e140); // 0x1E140 (stack game, unit, target, flag, 0, 6; esi other), ret 0x18
#define FUN_6fc3e140 (a0830d_env::FUN_6fc3e140)
EXT(FUN_6fd019c0); // 0xE19C0 (ecx x, edx room, stack game, unit, 0, 0; eax y), ret 0x10
#define FUN_6fd019c0 (a0830d_env::FUN_6fd019c0)
EXT(FUN_6fc2c1f0); // 0x0C1F0 nearest enemy (stack game, range; esi unit), ret 8
#define FUN_6fc2c1f0 (a0830d_env::FUN_6fc2c1f0)
EXT(Ordinal_10186); EXT(Ordinal_10913);
#define Ordinal_10186 (a0830d_env::Ordinal_10186)
#define Ordinal_10913 (a0830d_env::Ordinal_10913)
EXT(FUN_6fcffa90); // 0xDFA90 unit is dead (eax unit)
#define FUN_6fcffa90 (a0830d_env::FUN_6fcffa90)
EXT(Ordinal_10302); EXTA(Ordinal_10002);
#define Ordinal_10302 (a0830d_env::Ordinal_10302)
#define Ordinal_10002 (a0830d_env::Ordinal_10002)
EXT(FUN_6fcc3ae0); // 0xA3AE0 (ecx node, stack ctx; eax unit), ret 4
#define FUN_6fcc3ae0 (a0830d_env::FUN_6fcc3ae0)
EXT(FUN_6fcc4890); // 0xA4890 (ecx target, edx unit, stack game, skill, ctx; eax skillId), ret 0xc
#define FUN_6fcc4890 (a0830d_env::FUN_6fcc4890)
EXT(FUN_6fcfe0c0); // 0xDE0C0 apply damage (stack game, attacker, 1, &dmg; eax unit), ret 0x10
#define FUN_6fcfe0c0 (a0830d_env::FUN_6fcfe0c0)
EXT(FUN_6fcfcf20); // 0xDCF20 damage follow-up (stack game, attacker, &dmg; eax unit), ret 0xc
#define FUN_6fcfcf20 (a0830d_env::FUN_6fcfcf20)
EXT(Ordinal_10695); EXT(Ordinal_10428); EXT(Ordinal_10959); EXT(Ordinal_10551);
#define Ordinal_10695 (a0830d_env::Ordinal_10695)
#define Ordinal_10428 (a0830d_env::Ordinal_10428)
#define Ordinal_10959 (a0830d_env::Ordinal_10959)
#define Ordinal_10551 (a0830d_env::Ordinal_10551)
EXT(Ordinal_10680); EXT(Ordinal_10887);
#define Ordinal_10680 (a0830d_env::Ordinal_10680)
#define Ordinal_10887 (a0830d_env::Ordinal_10887)
EXT(FUN_6fc214d0); // 0x014D0 (ecx div, edx a; eax b) -> a*b/div
#define FUN_6fc214d0 (a0830d_env::FUN_6fc214d0)
EXT(FUN_6fc6ea30); // 0x4EA30 share potion life (ecx value; eax unit, esi game)
#define FUN_6fc6ea30 (a0830d_env::FUN_6fc6ea30)
EXT(FUN_6fc9a260); // 0x7A260 queue life update (stack pct; esi unit), ret 4
#define FUN_6fc9a260 (a0830d_env::FUN_6fc9a260)
EXT(Ordinal_10460); EXT(Ordinal_11147); EXT(Ordinal_11028); EXT(Ordinal_11065);
#define Ordinal_10460 (a0830d_env::Ordinal_10460)
#define Ordinal_11147 (a0830d_env::Ordinal_11147)
#define Ordinal_11028 (a0830d_env::Ordinal_11028)
#define Ordinal_11065 (a0830d_env::Ordinal_11065)
EXT(Ordinal_10698); EXT(Ordinal_10464);
#define Ordinal_10698 (a0830d_env::Ordinal_10698)
#define Ordinal_10464 (a0830d_env::Ordinal_10464)
EXT(FUN_6fcf9bf0); // 0xD9BF0 use stored item (ecx unit, edx game, stack x, y, &out; eax guid), ret 0xc
#define FUN_6fcf9bf0 (a0830d_env::FUN_6fcf9bf0)
EXT(FUN_6fcf8ba0); // 0xD8BA0 use belt item (ecx guid, stack game, unit, x, y, 0; eax &out), ret 0x14
#define FUN_6fcf8ba0 (a0830d_env::FUN_6fcf8ba0)

extern "C" char* sgptDataTables_exref;
extern "C" uint32_t gdwBitMasks_exref[32];

typedef uint8_t u8;
#define P(x) ((void*)(intptr_t)(x))
template <class T> static inline T& at(const void* p, int off) { return *(T*)((u8*)p + off); }
static inline uint32_t bm8(int off) { return *((u8*)gdwBitMasks_exref + off); }
static inline uint32_t bm32(int off) { return *(uint32_t*)((u8*)gdwBitMasks_exref + off); }
static inline u8* tables() { return (u8*)(intptr_t)*(int*)sgptDataTables_exref; }
static inline u8* skillRow(int id) {
    u8* t = tables();
    if (id < 0 || at<int>(t, 0xba0) <= id) return nullptr;
    return (u8*)(intptr_t)(id * 0x23c + at<int>(t, 0xb98));
}
static inline u8* monStatsRow(int id) {
    u8* t = tables();
    if (id < 0 || at<int>(t, 0xa80) <= id) return nullptr;
    return (u8*)(intptr_t)(id * 0x1a8 + at<int>(t, 0xa78));
}
// Retail reads through a null row here (an access violation); we trap at the same point.
static inline u8* mustRow(u8* row) { if (!row) __builtin_trap(); return row; }
// Inline seed advance (unit+0x20 / +0x24 pair, multiplier 0x6AC690C5 plus carry word), as retail writes it out.
static inline void seedStep(void* seed) {
    uint64_t s = (uint64_t)at<uint32_t>(seed, 0) * 0x6ac690c5ull + at<uint32_t>(seed, 4);
    at<uint64_t>(seed, 0) = s;
}
static const char kFile[] = "..\\Source\\D2Game\\SKILLS\\SkillUse.cpp";   // retail __FILE__ string (address only)
