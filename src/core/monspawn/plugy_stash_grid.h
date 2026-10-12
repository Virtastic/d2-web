#pragma once

#include <cstddef>

namespace d2::plugy {

struct StashGridSnapshot {
    int columns = 0;
    int rows = 0;
    int left = 0;
    int right = 0;
    int top = 0;
    int bottom = 0;
};

struct BigStashSnapshot {
    StashGridSnapshot personal;
    StashGridSnapshot highResolution;
};

template <typename GridInfo>
void readGrid(const GridInfo& grid, StashGridSnapshot& out) {
    out = {grid.nGridX, grid.nGridY, grid.nGridLeft, grid.nGridRight,
           grid.nGridTop, grid.nGridBottom};
}

template <typename GridInfo>
void writeGrid(GridInfo& grid, const StashGridSnapshot& in) {
    grid.nGridX = in.columns;
    grid.nGridY = in.rows;
    grid.nGridLeft = in.left;
    grid.nGridRight = in.right;
    grid.nGridTop = in.top;
    grid.nGridBottom = in.bottom;
}

// PlugY 10.00 rewrites rows 12 and 28 of the compiled InventoryBIN. Keep the
// retail values captured from those rows so disabled and realm sessions can
// restore them after a single-player PlugY session on the same page.
template <typename InventoryRecord>
bool configureBigStashGrid(InventoryRecord* records, std::size_t count,
                           bool activeBigStash, bool singlePlayer,
                           BigStashSnapshot& retail, bool& captured) {
    constexpr std::size_t kPersonalRow = 12;
    constexpr std::size_t kHighResolutionRow = 28;
    if (!activeBigStash || !singlePlayer) {
        if (captured && records && count > kHighResolutionRow) {
            writeGrid(records[kPersonalRow].pGridInfo, retail.personal);
            writeGrid(records[kHighResolutionRow].pGridInfo, retail.highResolution);
        }
        return true;
    }
    if (!records || count <= kHighResolutionRow) return false;

    if (!captured) {
        readGrid(records[kPersonalRow].pGridInfo, retail.personal);
        readGrid(records[kHighResolutionRow].pGridInfo, retail.highResolution);
        captured = true;
    }

    writeGrid(records[kPersonalRow].pGridInfo, {10, 10, 16, 302, 82, 371});
    writeGrid(records[kHighResolutionRow].pGridInfo, {10, 10, 96, 382, 142, 431});
    return true;
}

} // namespace d2::plugy
