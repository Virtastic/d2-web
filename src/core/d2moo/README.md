# Vendored D2MOO

Source: https://github.com/ThePhrozenKeep/D2MOO @ `5596f5cb6c5251a0a07c6637d26458b06099d516`
`source/D2Common/{include,src}`: `Drlg/*`, `D2Dungeon`, `D2Collision`, `D2Seed`, `D2StatList`,
`D2Inventory`, `D2Skills`, `D2States`, `D2Composit`, `Items/{Items,ItemMods}`, `Units/*`, all
`DataTbls/*`, `Fog/{Calc,BitManip}` and their headers; `source/D2Game`: `src/ITEMS/{Items,ItemsMagic,
ItemMode}.cpp` + `include/*` (headers only beyond ITEMS). `squall/` holds the public-domain Storm
headers D2MOO uses (`.cpp` files dropped). License: MIT (`LICENSE`).

D2MOO describes the 1.10f binaries; this copy is built as 1.13c through `../compat/`. The tree is
kept diffable against upstream: every local change is a `// d2-web:` comment on the changed line or
a `// 1.13c <DLL>+0x<offset>` marker naming the 1.13c function the code corresponds to, where its
behaviour differs from 1.10f.

- D2Game+0x9C160 PARTY_IteratePartyMembers: asserts pGame non-null (1.10f copy asserted the inverse on pPartyControl).
- D2Game+0x6A560 OBJMODE_WellHeal: split out of the well operate; refills life/mana/stamina by parm[1]/256 of max, clears poison and freeze, heals pets, returns 1 when anything was used else 0.
- D2Game+0x6CEE0 OBJECTS_OperateFunction22_Well: no null checks; anim step uses the byte-truncated 2*parm[2]; always returns 0.
- D2Game+0x838B0 AITHINK_Fn003_Zombie: out of combat, a failed walk roll runs MoveToTarget in mode 0xF at velocity 100.
- D2Game+0x8C280 D2GAME_PACKETS_SendPacket0x0D_6FC3C920: 13-byte packet, asserts unit type < 6.
- D2Game+0x9C4F0 sub_6FCB9D10: asserts pPartyControl.
- D2Game+0xBDB10 D2GAME_NPC_IdentifyAllItems_6FCC9C90: the code-3 reply is sent once, after the loop.
- D2Game+0xDEEE0 SUNITDMG_KillMonster: matches; the trailing D2Common ordinal is a no-op in 1.13c.
