// Draw order of a wall and a unit sharing a cell. Retail (D2Client+0x9EE90, cell filler +0x7A5D0/+0x7A340/+0x7A500):
// per cell the wall list is drawn first, then the unit list, so a hero standing in a wagon/fence tile is NOT
// overdrawn by that tile's own wall pieces. Rows (world Y) then columns (world X) still order cells.
import assert from 'node:assert/strict';
import { overlayKey } from '../play/assets/session-terrain.mjs';
const cmp = (a, b) => { for (let i = 0; i < 4; i++) if (a[i] !== b[i]) return a[i] - b[i]; return 0; };
const wall = (y, x, ordinal = 1) => overlayKey({ tile: { worldY: y, worldX: x, ordinal } });
const hero = (wy, wx) => overlayKey({ sprite: { wx, wy, pass: 'player' }, depth: 100 });
assert(cmp(wall(40, 20), hero(204, 103)) < 0, 'same cell: wall then hero');
assert(cmp(hero(204, 103), wall(40, 21)) < 0, 'later column wall still covers hero');
assert(cmp(hero(204, 103), wall(41, 0)) < 0, 'later row wall still covers hero');
assert(cmp(wall(39, 99), hero(204, 103)) < 0, 'earlier row wall under hero');
console.log('ok');
