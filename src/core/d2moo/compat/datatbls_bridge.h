#pragma once
namespace d2 {
class MpqVfs; struct LangTables;
// Fills sgptDataTables from the VFS (levels, leveldefs, lvlprest, lvltypes, lvlwarp, lvlmaze,
// lvlsub, objects, monstats, superuniques, monpreset). False + stderr on any missing table.
bool drlgLoadTables(MpqVfs& vfs, const LangTables& lang);
}
