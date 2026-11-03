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
- D2Game+0x6A9D0 OBJECTS_OperateFunction08_Door: the rule is `src/core/d2game/d2game_6a9d0.cpp` (every callee a seam in retail order); the collision results are tested as 16-bit, the rate check is unsigned 32-bit.
- D2Game+0x6CEE0 OBJECTS_OperateFunction22_Well: no null checks; anim step uses the byte-truncated 2*parm[2]; always returns 0.
- D2Game+0x6D960 OBJECTS_OperateFunction33_WirtsBody: ENDANIM fires (FrameCnt[1] >> 8) + 1 ticks out (not OBJECTS_GetOperatingTicks); no null checks.
- D2Game+0x6DA00 OBJECTS_OperateFunction02_Shrine: a null shrine-table handler (code 0) falls back to Refill instead of asserting; no null checks on pOp/pObject/pPlayer.
- D2Game+0x838B0 AITHINK_Fn003_Zombie: out of combat, a failed walk roll runs MoveToTarget in mode 0xF at velocity 100.
- D2Game+0x8C280 D2GAME_PACKETS_SendPacket0x0D_6FC3C920: 13-byte packet, asserts unit type < 6.
- D2Game+0x9C160 PARTY_IteratePartyMembers: asserts pGame non-null (1.10f copy asserted the inverse on pPartyControl).
- D2Game+0x9C4F0 sub_6FCB9D10: asserts pPartyControl.
- D2Game+0x9C4F0 party add (`sub_6FCB9D10`): the hostility helper is called member-first, then player-first, as retail does.
- D2Game+0x922A0 sub_6FCF5680 and D2Game+0x91E30 sub_6FCF5870: the skill record is read only when the skill id is above 0.
- D2Game+0x92770 SKILLS_SrvDo033_PsychicHammer: no lower clamp of the skill level to 1 (the retail clamp reduces to the level itself).
- D2Game+0x92ED0 sub_6FCF9580: a monequip row with no valid location is skipped without a roll.
- D2Game+0xB7090 (AI/AiBaal.cpp, AIBAAL_GetTarget): the act walk over list 8 starts from pGame->pTargetNodes[8], not the node left over from the lists 0-7 pass.
- D2Game+0x83130 (AI/AiThink.cpp, AITHINK_Fn006_Fallen): the neutral-mode test after the corpse scan uses the mode read at entry; the minion command buffer is zero-filled (retail leaves all but param 0 uninitialised).
- D2Game+0xA4610 (AI/AiThink.cpp, AITHINK_GetTargetForBoss): the act walk over list 8 starts from pGame->pTargetNodes[8], as in AIBAAL_GetTarget.
- D2Game+0xD0060 (PLAYER/PlrMsg.cpp, D2GAME_PACKETCALLBACK_Rcv0x36_HireMerc_6FC86BD0): matches the 1.13c disassembly; no change.
- D2Game+0xBDB10 D2GAME_NPC_IdentifyAllItems_6FCC9C90: the code-3 reply is sent once, after the loop.
- D2Game+0xDEEE0 SUNITDMG_KillMonster: matches; the trailing D2Common ordinal is a no-op in 1.13c.
- D2Game+0x212F0 AIGENERAL_AllocCommandsForMinions: minion loop skips a null allocation (1.10f dereferenced it); no pCurrentCmd write; monster guards on caller and owner; the new command pointer goes to the minion game block +0x20.
- D2Common+0x1BA60 (DataTbls/HoradricCube.cpp, DATATBLS_CubeMainOutputParser): a set-item hit returns FALSE after filling its fields, without parsing output modifiers.
- D2Common+0x25810 ITEMS_CalculateAdditionalCostsForChargedSkills: the base multiplier is (req level / 6) * 2 + layer bits + 2; a large operand is divided by 1024 before the cost multiplier.
- D2Common+0x25970 ITEMS_CalculateAdditionalCostsForItemSkill: for a large sell cost the multiplier is divided by 1024 (4096 for buy) before it scales the cost.
- D2Common+0x28850 ITEMS_CalculateAdditionalCostsForBonusStats: for a large operand the cost times value is divided first, then multiplied by the row multiplier.
- D2Common+0x267F0 ITEMS_GetWeaponAttackSpeed: matches the 1.13c decompile; the public wrapper passes includeItemFasterAttackRate=TRUE and the Impl helper is unchanged; validated against the vector.
- D2Game+0x64B60 ACT1Q3_PlayerOrPartyHasMalus: the party walk only runs for a player unit attached to a game (pGame set, unit type player).

- D2Game SCmd 0x6FC3CBC0/CDE0/CA90/CCB0 (monster move and attack-start packets, compat/game_net_stubs.cpp): write the full 16/21-byte packet with the path type remap, clamped velocity and max distance.
- D2Game+0xD3CB0 (stack search, ItemMode.cpp sub_6FC437F0): 1.13c searches the backpack directly for non-quiver stackables; quiver-type items search equipped first, then backpack (D57).
- D2Game+0x8B010 (packet 0x3E item stat update, game_net_stubs.cpp): bit-packed writer replaces the empty stub.
- D2Common+0x24B20 ITEMS_CheckIfAutoBeltable: matches the 1.13c decompile; no change.
- D2Common+0x25810 and D2Common+0x25970 (charged-skill and item-skill costs): arithmetic matches; the stat copy they rely on, STATLIST_CopyStats, now returns 0 for a unit with no stat list instead of dereferencing a null pointer.
- D2Common+0x7E4F0 DATATBLS_LoadCollisionField (FieldTbls.cpp): center X is half the height and center Y half the width (1.13c 0x1414/0x1418); ours had them swapped, which moved every field lookup. 0x7E480 getter and 0x7E290 lookup match after the swap.
- D2Common+0x7E4A0 DATATBLS_FreeCollisionFieldTable: 1.13c clears the field globals and asserts the data is already released; the release moved to the unload caller (DATATBLS_UnloadAllBins) so it still happens once before the clear.
- D2Common+0x6B2B0 MISSILE_EvaluateMissileFormula: arguments, level and id defaults, and the expression table guard match 1.13c; no change. The callback at 0x6B160 is not decompiled (Ghidra refuses the label), so its identity with MISSILE_GetCalcParamValue is unverified.
- D2Common+0x26BA0 ITEMS_GetRunesTxtRecordFromItem: returns NULL unless the socketed inventory's signature is 0x1020304 (1.13c check added).
- D2Common+0x28850 ITEMS_CalculateAdditionalCostsForBonusStats: all encode cases match the 1.13c arithmetic; encode 2/3 skips the stat when the skill record is missing (1.13c's null read contributes zero).
- D2Common+0x2C910 ITEMS_UpdateSets: body matches; the 1.13c routing through 0x2C7C0 and 0x2C630 is ported in src/core/d2common/d2common_2c910.cpp (D2Common_2C910_UpdateSets); the play path still calls this 1.10f-routed copy.
- D2Game+0x68D70 OBJECTS_OperateFunction06_TowerTome (QUESTS/ACT1/A1Q5.cpp): returns 0 on every path (1.10f returned 1 after activating the tome); validated against the vector.
- D2Game+0x5CD50 OBJECTS_OperateFunction67_FrozenAnya (QUESTS/ACT5/A5Q3.cpp): returns 0 on every path (1.10f returned 1 when the quest is missing and at the end).
- D2Game+0x56B10 (QUESTS/ACT5/A5Q3.cpp `ACT5Q3_QuestSeqHelper`): the quest-sequence tail that 0x5C690 ACT5Q3_SpawnDrehyaInTown calls is its own function in 1.13c; split out of the 1.10f body, same behaviour.
- D2Game+0x5BF70 ACT5Q5_RewardPlayer (QUESTS/ACT5/A5Q5.cpp): the exp-versus-delta compare is unsigned (jb) in 1.13c.
- D2Game+0xC6070 ACT2Q1_UnitIterate_SetPrimaryGoalDoneForPartyMembers: matches the 1.13c decompile; validated against the vector (in-room path).
- `src/D2Game/GAME/Game.cpp` `GAME_UpdateGamesProgress` (`// 1.13c D2Game+0x2E1E0`): the elapsed-time compare against the target frame time is signed (retail casts both sides to int); 1.10f compared unsigned.
- `src/core/d2game/d2game_n88.cpp` `D2Game_2C2D0` (`// 1.13c D2Game+0x2C2D0`, game creation): the D2Net#10021 call between 0x6A9B0 and the `b[0x6a]==1` test is now a seam. Row 0x33290 (object sweep) is `OBJECTS_RemoveAll`; the monster sweep is 0xB0640. `GAME_ProcessNetworkMessages` still calls the 1.10f `sub_6FC38140` in its third loop where 1.13c calls 0x29CB0.
- `src/D2Game/GAME/Game.cpp` `GAME_GetGameGUIDFromGameId` (`// 1.13c D2Game+0x2A3B0`): the handle table is indexed by the game id directly (1.10f indexed id - 1) and is 1025 entries wide. `D2Game_10050` (`// 1.13c D2Game+0x2A9A0`, teardown): deletes the client-list, by-name and game-list locks, clears the clients flag and item cache, resets the game-id cursor word, game data table and act word.
- D2Game+0xF22C0 AITACTICS_IdleInNeutralMode (AI/AiTactics.cpp): the mode-change record is built inline (used skill cleared, flag byte +0x15 = 100, mode 1, unit and target = unit) instead of through MONSTERMODE_GetModeChangeInfo, which can set 101.
- D2Common+0x31B40 UNITS_GetEquippedWeaponFromMonster: the candidate is the 1.13c 0x20700 rule (right hand cell 4 unless it is an item with flags 0x100/0x4000, else left hand cell 5 if equippable; weapon type 0x2D), not INVENTORY_GetEquippedWeapon. The global DAT_6FDED378 is gBodyLocInventoryGridInfo = {13,1}; the 0x20700 port now checks the grid dimensions against it and the cell-4 gate is applied as retail does.
- D2Game+0xEF6B0 MONSTERSPAWN_SpawnRandomMonsterForLevel (MONSTER/MonsterSpawn.cpp): the no-normal-monsters path and the no-spawn-flag path assert (lines 0x4F0 and 0x507) where 1.10f warned and returned 0.
- SKILLITEM_HandleItemEffectSkill (D2Game+0xF14F0): returns 0 when nSkillId is 0 (retail tests the id before the record lookup).
- 1.13c out-of-line helpers split from 1.10f bodies, one marker each: SKILLS_ValidateUsedSkillTarget (D2Game+0x9FC00, from the used-skill prologue), NPC_RefillHirelingListIfExhausted (D2Game+0xBD160), ACT3Q3_SpawnFetishBoss (D2Game+0xE8780), ITEMMODE_ReturnItemToGround (D2Game+0xD2840, the five-line drop-back that the pickup paths repeated) and ITEMMODE_UnpackDroppedCube (D2Game+0xD78C0, the cube tail of sub_6FC44030). D2GAME_RemoveItemIfOnCursor (D2Game+0xD2C70) sends the cursor clear with no client for a holder that is not a player.
- D2Common+0x2D660 DRLG_UpdateRoomExCoordinates (`sub_2D660`, DrlgDrlg.cpp): the four asserts now `return;` after the warning, as retail exits there (the warning helper only prints). Valid input is unchanged; the FN probe (`src/core/fn/d2common_2d660.cpp`) copies the 1.13c blobs into the 1.10f structs.
- D2Common+0xB290 (DrlgPreset.cpp `DRLGPRESET_SpawnRiverSoundColumn`): the river-sound column loop is its own function as in 1.13c, so the row is measured on its own.
- D2Common+0x19950 DRLGWARP_GetWarpIdArrayFromLevelId (DrlgDrlgWarp.cpp): a zero level in the warp override chain asserts and exits as retail does (1.10f warned and went on).
- D2Common+0x2C7C0 (Items.cpp `sub_6FDA4380`, the set-bonus walk under ITEMS_UpdateSets): the switch-on walk stops before the sixth set state (1.10f went to six), and a start index below zero toggles nothing.
- D2Common+0x51000 D2Common_11016 (D2Skills.cpp): the skill range byte is read sign-extended (1.13c movsx), so a byte >= 0x80 is negative.
- D2Common+0x4D140 COLLISION_GetFreeCoordinatesImpl (D2Collision.cpp): matches 1.10f: each lookup that finds a room makes it the base of the next (retail writes it over the room argument).
- D2Common+0x54430 (core/d2common/d2common_batch_53780.cpp): DATATBLS_LoadTreasureClassExTxt calls the 1.13c item parser per slot; the 1.10f inline parser matched argument keys by prefix and stopped on an empty value.


- D2Common+0x5D8B0 DATATBLS_UnloadStatesTxt (DataTbls/DataTbls.cpp): no NULL store back to pStatesTxt or pStatesLinker after the free, as retail (PRIORART batch). The -4 pool adjustment is kept as a host adaptation (FOG_FreePool is free()).
- D2Common+0x82D20, +0x82D60 and +0x82F00 (overlay accessors, DataTbls/OverlayTbls.cpp DATATBLS_GetOverlayTxtRecord): the table pointer is tested as well as the index; with no table the result is zero (1.13c).
- D2Game+0xE800 (`sub_E800`, ITEMS/Items.cpp): arguments documented as retail registers (EAX = item, EDI = game); logic unchanged, measured by an FN probe.
- D2Game+0xC0B90 (`sub_6FCC84D0`, SUnitNpc.cpp): 1.13c inlines the pet item stat-list re-merge (0xBCA20) before the pets re-evaluate; added.
- D2Game+0xCB3D0 (`D2GAME_PACKETCALLBACK_Rcv0x58_QuestCompleted`, PlrMsg.cpp): quest id bound is 0x2A, not MAX_QUEST_STATUS.
- D2Common+0x4DAF0 DUNGEON_ClientToGameTileCoords and D2Common+0x4DA40 DUNGEON_ClientTileDrawPositionToGameCoords (D2Dungeon.cpp): both return int (retail returns the Y numerator times 0x66666667 in EAX, the divide-by-160 residue), where the 1.10f port was void. The Y quotient is kept for the non-negative path; the negative path of 4DA40 sets Y to the quotient minus one, as retail.
- D2Common+0x4DC70 (d2common_batch_4dc70.cpp): the batch export is renamed D2Common_4DC70_Interp (int32 p1, lo, hi) so the FN probe name is free; the body is unchanged and matches the decompile.
- D2Game+0x716D0 (PLAYER/PlrTrade.cpp sub_6FC90AE0, PRIORART batch): the item loop reads the next inventory link before moving the item to the trade inventory, and a non-item cursor is skipped without a trace, as retail.
- D2Game+0x53B40 (PLAYER/PlrTrade.cpp PLRTRADE_CopyTradeSaveDataToBuffer, PRIORART batch 5): takes pGame and a use-trade-record flag; the cached trade record is used only for a player unit with the flag set, and a missing, oversized or zero-length record falls back to the item writer (0x537D0, sub_6FC8A0F0) instead of returning 0.
- D2Common+0x26BA0 (`ITEMS_GetRunesTxtRecordFromItem`, Items.cpp): a socketed inventory with no first item returns no rune record before the socket walk, as retail (the vendored code walked an empty chain and could match a rune with no required sockets).
- D2Game+0x272C0 (SKILLS/SkillBar.cpp SKILLS_FindLeapAttackTarget): when the path target and the remembered target both fail, 1.13c runs the aura-target search (range melee+4, filter 0x20003) as 1.10f does; the search had been dropped here and is restored.
- D2Game+0x4D710 (d2game_batch_452c0b.cpp D2GAME_4D710): the closing timer start takes the caster's dword at +0xd0 (ebx), not its low byte.
- D2Common+0x3AAB0 (D2StatList.cpp D2Common_10574): already in the wanted SET state returns 0 without expiring or re-posting; a change returns 1 (the vendored copy returned 1 for both).

- STATLIST_MergeBaseStats (D2Common+0x3A600): base entries now fold through D2Common_3A330_AddStat (1.13c), not STATLIST_AddStat.
- D2Common+0x335F0 (UnitSetupSpeeds): rewritten over D2UnitStrc by field name (the word-array port was wrong for the widened unit); +0x32EF0 port renamed D2Common_32EF0_MergeDualWield. Frame bonus still goes through UNITS_GetFrameBonus (1.10f); 1.13c 0x32220 reads a weapon-class table instead -- player branch not captured.
- D2Common+0x32E10 (Units.cpp UNITS_CharacterStartRunningOrKnockback): a null unit allocates the temp statlist from no pool, owner type 6, owner id -1 (1.10f dereferenced it); the old word-array placeholder in d2common_batch_wave1018.cpp is retired.
- D2Game+0xA2F90 (sub_6FD14F70): the boss bit (pData+0x16 & 8) is tested before the unique flag as in 1.13c (reads only; order only). +0xA2EE0, +0xA2DE0, +0xA3120 and +0xA3090 are marked 1.13c.

- D2Game+0xA34E0 (d2game_batch_9eeb0.cpp): callee argument slots corrected against the disassembly (10785/10945 take s2, 10807/10945 take s1). D2Game+0xA39E0: port renamed d2game_a39e0_gate so the FN probe owns d2_fn_d2game_a39e0.
- D2Game+0x11A70 (ITEMS/Items.cpp D2GAME_DropItemAtUnit_6FC4FEC0): the drop spot comes from D2GAME_GetFreeSpaceEx (0xE930) on the unit coords, not the 1.10f COLLISION free-coordinates search at (x+2,y+3); the ObjMode DropItemWithCodeAndQuality copy was wrongly marked with this RVA.
- D2Common+0x3A330 AddStat: the real ABI is stdcall(list, statId, delta, layer); STATLIST_MergeBaseStats (0x3A600) and the 0x47EF0/0x47FA0/0x475E0 batches now pass the layer through D2Common_3A330_AddStatLayer (the old 3-argument call passed the stat id as the packed key). D2Common+0x39140 is the vendored STATLIST_GetStatValue (stdcall list, id, layer); its separate batch copy was removed. D2Common+0x45F90 (d2common_45f90.cpp), +0x2C630 (d2common_2c630.cpp) and +0x2C910 (D2Common_2C910_UpdateSets) are real ports with FN descriptors.

- D2Game+0x1F2D0, +0x204E0, +0x20760, +0x24440, +0x2E4F0, +0x2FEC0, +0x365E0, +0x366A0: retail-layout (32-bit offset) ports in src/core/d2game/d2game_<rva>.cpp, each with an FN probe; the vendored D2MOO twins are unchanged. Deltas against 1.10f: 0x204E0 splits the monster (0x1F600) and item (0x203D0) branches out of the compress; 0x24440 finds the free spot through 0xE930 and drops through 0xE990; 0x2FEC0 inlines the unit allocation (rolls the seed before the placement test, 32-unit cap); 0x366A0/0x365E0 take EAX = item code/item, ECX = game/player register arguments.
- D2Game+0x4D420 (SkillSpawnMissileFan), +0x4F970 (SetSummonPassiveStats), +0x50B70 (SkillNec_ApplySkillToUnit), +0x526F0 (Server_ClientTimeoutCheck), +0x53640 (the legacy compact-stat save section): 1.13c rules written from the disassembly as templates over the probe's HookEnv in src/core/d2game/d2game_<rva>.h, each with an FN probe. The vendored D2MOO twins (SkillNec.cpp SetSummonPassiveStats, PlrSave) and the older batch sketches (d2game_batch_452c0b.cpp, d2game_batch_4ebe0.cpp, d2game_n89.cpp) are unchanged and point at the headers; d2game_batch_53640.cpp's export is renamed d2game_53640_batch so the probe owns d2_fn_d2game_53640.
- `src/core/d2game/d2game_batch_2dc70.cpp` `d2_game_4a780` (`// 1.13c D2Game+0x4A780`): resolves the game through D2Net #10015 then 0x2AAE0 (the port passed the client id to 0x2AAE0); clock and client-list lock are seams for the FN probe.
- D2Game+0x5DA70 A5Q4 event handler: the module table is at 0x10C468 (the old port used 0x12C468) and the 0x337D0 id comes from the player unit.
- D2Game+0x5E940 quest 0x21 object spawn: q32 port replaces the Env port.
- D2Game+0x5FE30 A5Q3 event handler: rewritten from the disassembly (old argument orders wrong); the data+0x88 loop is unreachable in FN mode.
- D2Game+0x5D960, 0x5FB50, 0x5FC20, 0x64B60, 0x664F0: q32 ports from the disassembly (64B60 callees are 0xE660/0x9BEB0/0x9C160; 664F0 replaces the d2game_batch_573d0.cpp Env port).
