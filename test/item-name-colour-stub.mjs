// Installs the engine exports the shell would install, for tests that run without the wasm engine. Same rules as the ports:
// d2_client_item_name_colour (D2Client+0x93450, src/core/d2client/item_name_colour_93450.cpp) and
// d2_client_inv_grid_cell (D2Client+0x96AA0, the unsigned division).
import { setItemNameColourEngine } from '../play/assets/item-tooltip.mjs';
import { setGridCellEngine } from '../play/assets/grid-cell.mjs';
const kQualCol = [3, 2, 9, 4, 8, 10];
setItemNameColourEngine((q, f) => ((q - 4) >>> 0) <= 5 ? kQualCol[q - 4] : (f & 0x800 || f & 0x400000) ? 5 : 0);
setGridCellEngine((l, t, w, h, x, y, c, r) => { const cx = Math.floor(((x - l) >>> 0) / w), cy = Math.floor(((y - t) >>> 0) / h); return cx >= c || cy >= r ? null : { x: cx, y: cy }; });
