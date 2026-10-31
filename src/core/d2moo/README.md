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

- D2Game+0x6A560 OBJMODE_WellHeal: split out of the well operate; refills life/mana/stamina by parm[1]/256 of max, clears poison and freeze, heals pets, returns 1 when anything was used else 0.
- D2Game+0x6CEE0 OBJECTS_OperateFunction22_Well: no null checks; anim step uses the byte-truncated 2*parm[2]; always returns 0.
- D2Game+0x838B0 AITHINK_Fn003_Zombie: out of combat, a failed walk roll runs MoveToTarget in mode 0xF at velocity 100.
- D2Game+0x8C280 D2GAME_PACKETS_SendPacket0x0D_6FC3C920: 13-byte packet, asserts unit type < 6.
- D2Game+0x9C160 PARTY_IteratePartyMembers: asserts pGame non-null (1.10f copy asserted the inverse on pPartyControl).
- D2Game+0x9C4F0 sub_6FCB9D10: asserts pPartyControl.
- D2Game+0x9C4F0 party add (`sub_6FCB9D10`): the hostility helper is called member-first, then player-first, as retail does.
- D2Game+0xB7090 (AI/AiBaal.cpp, AIBAAL_GetTarget): the act walk over list 8 starts from pGame->pTargetNodes[8], not the node left over from the lists 0-7 pass.
- D2Game+0xBDB10 D2GAME_NPC_IdentifyAllItems_6FCC9C90: the code-3 reply is sent once, after the loop.
- D2Game+0xDEEE0 SUNITDMG_KillMonster: matches; the trailing D2Common ordinal is a no-op in 1.13c.
- D2Game+0x212F0 AIGENERAL_AllocCommandsForMinions: minion loop skips a null allocation (1.10f dereferenced it); no pCurrentCmd write; monster guards on caller and owner; the new command pointer goes to the minion game block +0x20.
- D2Common+0x25810 ITEMS_CalculateAdditionalCostsForChargedSkills: the base multiplier is (req level / 6) * 2 + layer bits + 2; a large operand is divided by 1024 before the cost multiplier.
- D2Common+0x25970 ITEMS_CalculateAdditionalCostsForItemSkill: for a large sell cost the multiplier is divided by 1024 (4096 for buy) before it scales the cost.
- D2Common+0x28850 ITEMS_CalculateAdditionalCostsForBonusStats: for a large operand the cost times value is divided first, then multiplied by the row multiplier.
- D2Common+0x1BA60 (DataTbls/HoradricCube.cpp, DATATBLS_CubeMainOutputParser): a set-item hit returns FALSE after filling its fields, without parsing output modifiers.
- D2Common+0x267F0 ITEMS_GetWeaponAttackSpeed: matches the 1.13c decompile; the public wrapper passes includeItemFasterAttackRate=TRUE and the Impl helper is unchanged; validated against the vector.
