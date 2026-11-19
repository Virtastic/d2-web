#pragma once

#include "D2CommonDefinitions.h"
#include <D2Seed.h>
#include "Path.h"

#pragma pack(1)

#pragma pack()

struct D2PathMovStrc
{
	int nDirectionIndex;
	int nPoints; //nLoopLen in 1.10f
	D2PathPointStrc tCurrentCoord;
	D2PathPointStrc tTargetCoord;
	BOOL bReachedDeadEnd; // Only used for debugging ?
	int* pPrevDirection;
	int* pNextDirection;
	int nLastSyncPointWithOtherPathIndex;
	BOOL bPathFinished;
	D2PathPointStrc pPoints[200];
};

// 1.13c D2Common+0x18400: splice pMov's points into pPoints in place of the subpath ending at nSubPathLastIdx
void __vectorcall PATH_ReplaceSubpathPoints(D2PathMovStrc* pMov, D2PathPointStrc* pPoints, int* pSubPathStartIdx, int nSubPathLastIdx, int* nMaxIndex);
// 1.13c D2Common+0x186D0: step a walker to the next non-colliding target cell
BOOL __vectorcall PATH_FindNonCollidingTargetPoint(D2PathInfoStrc* pInfo, D2PathMovStrc* pMov);

//1.10f: D2Common.0x6FDABAC0
//1.13C: D2Common.0x6FD68830
int __fastcall PATH_FindSubpathWithoutObstacles(D2PathInfoStrc* pInfo, D2PathPointStrc tSubPathStart, D2PathPointStrc* pPathPoints, int* pSubPathStartIdx, int* nMaxIndex, int nMaxLength, int nMajorDirection);
//1.10f: D2Common.0x6FDAC170
//1.13C: D2Common.0x6FD68310
int __fastcall PATH_SimplifyToLines(D2PathPointStrc* pOutPathPoints, D2PathPointStrc* pInputPoints, D2PathPointStrc tStartCoord, signed int nbTempPoints);
//1.10f: D2Common.0x6FDAC270
//1.13C: D2Common.0x6FD68C40
int __fastcall PATH_ComputePathOrSlideAlongObstacles(D2PathInfoStrc* ptPathInfo);
