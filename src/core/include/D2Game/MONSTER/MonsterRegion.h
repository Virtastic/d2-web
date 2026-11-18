#pragma once

#include <Units/Units.h>
#include <Drlg/D2DrlgDrlg.h>

struct D2UnkMonCreateStrc;		// d2-web: the 0x47450 callee seam below names it (MonsterSpawn.h defines it)
struct D2UnkMonCreateStrc2;

#pragma pack(1)

struct D2MonRegDataStrc
{
	uint16_t nMonHcIdx;							//0x00
	uint8_t nRarity;							//0x02
	uint8_t nComponentVariantsMax;				//0x03
	uint8_t nComponentVariants[3][16];			//0x04
};

struct D2MonsterRegionStrc
{
	uint8_t nAct;								//0x00
	uint8_t unk0x01[3];						//0x01
	int32_t unk0x04;							//0x04
	int32_t unk0x08;							//0x08
	int32_t unk0x0C;							//0x0C
	uint8_t nMonCount;							//0x10
	uint8_t nTotalRarity;						//0x11
	uint8_t nSpawnCount;						//0x12
	uint8_t unk0x13;							//0x13
	D2MonRegDataStrc pMonData[13];			//0x14
	uint32_t dwMonDen;							//0x2B8
	uint8_t nBossMin;							//0x2BC
	uint8_t nBossMax;							//0x2BD
	uint8_t nMonWander;						//0x2BE
	uint8_t unk0x2BF;							//0x2BF
	uint32_t dwlevel;							//0x2C0
	uint32_t unk0x2C4;							//0x2C4
	uint32_t dwUniqueCount;					//0x2C8
	uint32_t dwMonSpawnCount;					//0x2CC
	uint32_t dwMonKillCount;					//0x2D0
	int32_t unk0x2D4;							//0x2D4
	uint8_t nQuest;							//0x2D8
	uint8_t unk0x2D9[3];						//0x2D9
	uint32_t dwDungeonLevel;					//0x2DC
	uint32_t dwDungeonLevelEx;					//0x2E0
};

#pragma pack()

//D2Game.0x6FC66260
// 1.13c D2Game+0x47450
int32_t __fastcall sub_6FC66260(D2GameStrc* pGame, D2ActiveRoomStrc* pRoom, D2RoomCoordListStrc* pRoomCoordList, int32_t nSuperUniqueId, int32_t* pX, int32_t* pY, int32_t a7);
// 1.13c D2Game+0x46C30 (extracted out of the 1.10f sub_6FC66260 body; also called from 0x471E0)
void __fastcall sub_46C30(D2ActiveRoomStrc* pRoom, D2RoomCoordListStrc* pRoomCoordList, int32_t* pLeft, int32_t* pTop, int32_t* pRight, int32_t* pBottom);
// 1.13c D2Game+0x46B20 (extracted out of the 1.10f sub_6FC66260 body; only called from 0x47450)
int32_t __fastcall sub_46B20(D2GameStrc* pGame, D2ActiveRoomStrc* pRoom, int32_t nX, int32_t nY);
// d2-web: 1.13c D2Game+0x46B20 callee seams; all null in the game. The five
// D2Common lookups sub_46B20 makes need a live game, so the probe scripts them out of the room blob
//; #11158 (tile -> subtile) runs for real on both sides.
struct D2LevelsTxt;
struct D2Spawn46B20Seam
{
    int32_t (*levelId)(D2ActiveRoomStrc*);                                            // D2Common #10826
    int32_t* (*warpCoordinates)(D2ActiveRoomStrc*);                                   // D2Common #10779
    D2LevelsTxt* (*levelsRecord)(int nLevelId);                                       // D2Common #10014
    uint8_t (*actNo)(int nLevelId);                                                   // D2Common #10459
    void (*actSpawnLocation)(D2DrlgActStrc*, int, int, int*, int*);                   // D2Common #10942
};
extern D2Spawn46B20Seam g_d2Spawn46B20;
// 1.13c D2Game+0x47450 callee seams; both null in the game
struct D2Spawn47450Seam
{
    int32_t (*warpCheck)(D2GameStrc*, D2ActiveRoomStrc*, int32_t, int32_t);  // 0x46B20
    D2UnitStrc* (*spawn)(D2UnkMonCreateStrc*);                              // 0xEEF10
};
extern D2Spawn47450Seam g_d2Spawn47450;
// 1.13c D2Game+0x47CF0 callee seams; all null in the game
struct D2Spawn47CF0Seam
{
    int32_t (*levelId)(D2ActiveRoomStrc*);                                                            // 0xA556
    int32_t (*questCheck)(D2GameStrc*, int32_t);                                                      // 0x53970
    D2UnitStrc* (*spawnSuperUnique)(D2GameStrc*, D2ActiveRoomStrc*, int32_t, int32_t, int32_t);       // 0x251F0
    D2UnitStrc* (*spawnMonster)(D2GameStrc*, D2ActiveRoomStrc*, int32_t, int32_t, int32_t, int32_t, int32_t, int16_t);  // 0xEF650
    int32_t (*randomId)(D2ActiveRoomStrc*, int32_t);                                                  // 0xADFC
    // the four 0x46F70-only callees (the outlined regular path); null in the game
    uint32_t (*checkMonStats2Flag)(int32_t, int32_t);                                                 // 0x13B0
    void (*allocUnit)(int32_t, int32_t, int32_t, int32_t, D2GameStrc*, D2ActiveRoomStrc*, char, int32_t, uint32_t);   // 0xE1D90
    int32_t (*unitX)(D2UnitStrc*);                                                                    // 0x1100
    int32_t (*unitY)(D2UnitStrc*);                                                                    // 0x1130
};
extern D2Spawn47CF0Seam g_d2Spawn47CF0;
//D2Game.0x6FC66F70 (outlined regular path of 0x6FC66560)
D2UnitStrc* __fastcall D2GAME_SpawnRegularMonster_6FC66F70(D2GameStrc* pGame, D2ActiveRoomStrc* pRoom, int32_t nClassId, int32_t nX, int32_t nY, int32_t nMode);
//D2Game.0x6FC66560
D2UnitStrc* __fastcall D2GAME_SpawnPresetMonster_6FC66560(D2GameStrc* pGame, D2ActiveRoomStrc* pRoom, int32_t nClassId, int32_t nX, int32_t nY, int32_t nMode);
//D2Game.0x6FC66FC0
D2MonStats2Txt* __fastcall MONSTERREGION_GetMonStats2TxtRecord(int32_t nMonsterId);
//D2Game.0x6FC67010
uint32_t __fastcall MONSTERREGION_CheckMonStats2Flag(int32_t nMonsterId, int32_t nFlag);
//D2Game.0x6FC67080
int32_t __fastcall MONSTERREGION_SanitizeMonsterId(int32_t nMonsterId);
//D2Game.0x6FC670A0
int32_t __fastcall sub_6FC670A0(int32_t nMonsterId);
//D2Game.0x6FC67190
void __fastcall D2GAME_PopulateRoom_6FC67190(D2GameStrc* pGame, D2ActiveRoomStrc* pRoom);
//D2Game.0x6FC67570
int32_t __fastcall sub_6FC67570(D2GameStrc* pGame, D2ActiveRoomStrc* pRoom, D2RoomCoordListStrc* pRoomCoordList, int32_t nMonsterId, D2UnkMonCreateStrc2* a5, int32_t* pX, int32_t* pY);
//D2Game.0x6FC677D0
D2UnitStrc* __fastcall sub_6FC677D0(D2GameStrc* pGame, D2ActiveRoomStrc* pRoom, D2RoomCoordListStrc* pRoomCoordList, int32_t nMonsterId, uint8_t nMin, uint8_t nMax);
// 1.13c D2Game 0x47820 (split out of sub_6FC679F0; replay kind 3446)
void sub_47820(D2GameStrc* pGame, D2ActiveRoomStrc* pRoom, int32_t nLevelId, struct D2LevelsTxt* pLevelsTxtRecord);
// 1.13c D2Game+0x47A20 callee seams (replay kind 3450, s3252); all null in the game
struct D2Populate47A20Seam
{
    int32_t (*gate)(D2GameStrc*, D2ActiveRoomStrc*);                                                             // 0x46CD0
    D2RoomCoordListStrc* (*coordList)(D2ActiveRoomStrc*);                                                        // 0xB22E
    int32_t (*levelId)(D2ActiveRoomStrc*);                                                                       // 0xB204
    int32_t (*preset)(D2GameStrc*, D2MonsterRegionStrc*, D2ActiveRoomStrc*, D2MonStatsTxt**, int32_t, int32_t);  // 0xDF4A0
    int32_t (*bossType)(D2MonsterRegionStrc*, D2ActiveRoomStrc*);                                                // 0xDF080
    int32_t (*fallenScarab)(int32_t);                                                                            // 0x46D40
    int32_t (*group)(D2GameStrc*, D2ActiveRoomStrc*, D2RoomCoordListStrc*, int32_t, uint8_t, uint8_t);           // 0x47700
    int32_t (*boss)(D2GameStrc*, D2ActiveRoomStrc*, int32_t, D2UnkMonCreateStrc2*);                              // 0x47900
    int32_t (*champion)(D2GameStrc*, D2ActiveRoomStrc*, D2RoomCoordListStrc*, int32_t);                          // 0x479B0
};
extern D2Populate47A20Seam g_d2Pop47A20;
// 1.13c D2Game+0x46CD0 callee seams (replay kind 3451, s3253); all null in the game
struct D2Gate46CD0Seam
{
    int32_t (*roomLevel)(D2ActiveRoomStrc*);            // 0xA556
    int32_t (*populatedLevel)(D2ActiveRoomStrc*);       // 0xB204
    int32_t (*countRooms)(D2DrlgActStrc*, int32_t);     // 0xB18C
    int32_t (*chaosCleared)(D2GameStrc*);               // 0xB8940
};
extern D2Gate46CD0Seam g_d2Gate46CD0;
int32_t D2GAME_Gate46CD0(D2GameStrc* pGame, D2ActiveRoomStrc* pRoom);

extern void (*g_d2Wander47660Seam)(D2GameStrc*, D2ActiveRoomStrc*, D2MonsterRegionStrc*);
//D2Game.0x6FC679F0
void __fastcall sub_6FC679F0(D2GameStrc* pGame, D2ActiveRoomStrc* pRoom);
//D2Game.0x6FC67B90
void __fastcall MONSTERREGION_InitializeAll(void* pMemPool, D2MonsterRegionStrc** ppMonsterRegion, D2SeedStrc* pSeed, uint32_t nLowSeed, uint8_t nDifficulty, int32_t bExpansion);
//D2Game.0x6FC67F30
void __fastcall MONSTERREGION_FreeAll(void* pMemPool, D2MonsterRegionStrc** ppMonsterRegion);
//D2Game.0x6FC67F90
D2MonsterRegionStrc* __fastcall MONSTERREGION_GetMonsterRegionFromLevelId(D2MonsterRegionStrc** ppMonsterRegion, int32_t nLevelId);
//D2Game.0x6FC67FA0
D2MonRegDataStrc* __fastcall sub_6FC67FA0(D2MonsterRegionStrc** ppMonsterRegion, D2ActiveRoomStrc* pRoom, D2UnitStrc* pUnit);
//D2Game.0x6FC68110
void __fastcall sub_6FC68110(D2GameStrc* pGame);
//D2Game.0x6FC68180
void __fastcall sub_6FC68180(D2MonsterRegionStrc** ppMonsterRegion, D2ActiveRoomStrc* pRoom, D2UnitStrc* pUnit, int32_t bToggleFlag);
//D2Game.0x6FC681C0
void __fastcall sub_6FC681C0(D2MonsterRegionStrc** ppMonsterRegion, D2UnitStrc* pUnit, int32_t nPreviousAlignment, int32_t nNewAlignment);
//D2Game.0x6FC68240
void __fastcall sub_6FC68240(D2MonsterRegionStrc** ppMonsterRegion, D2UnitStrc* pUnit);
//D2Game.0x6FC68280
void __fastcall sub_6FC68280(D2MonsterRegionStrc** ppMonsterRegion, D2UnitStrc* pUnit);
//D2Game.0x6FC682C0
void __fastcall sub_6FC682C0(D2MonsterRegionStrc** ppMonRegion, int32_t nLevelId1, int32_t nLevelId2, int32_t nAlignment, int32_t bDead, int32_t a6, D2UnitStrc* pUnit);
