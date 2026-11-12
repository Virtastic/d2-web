// The engine's client room unit lists decide which world sprites draw with the floor and in what order the units of
// one tile draw (src/core/d2client/client_room_units.cpp). This file only feeds the frame's rooms and units to the
// engine and copies its answers onto the sprites: `floor` (drawn in the floor pass, before every wall and unit) and
// `rank` (the unit's place in its room's list; -1 when it stands in no room the client holds).
const FP_CENTRE = 0x8000;
const fraction = (v) => (Number.isFinite(v) ? Math.round((v - Math.floor(v)) * 65536) & 0xffff : FP_CENTRE);

export function orderUnitSprites(Module, sprites, rooms) {
  if (!Module?._d2c_cru_frame_begin) return false;
  const call = (n, a) => Module.ccall(n, 'number', a.map(() => 'number'), a);
  call('d2c_cru_frame_begin', []);
  for (const r of rooms ?? []) call('d2c_cru_room_rect', [r.level, r.x, r.y, r.width, r.height]);
  const slots = sprites.map((s) => (s.u && Number.isInteger(s.id) && s.wx !== undefined)
    ? call('d2c_cru_unit_at', [s.u[0], s.id >>> 0, s.u[1] ?? 0, Math.floor(s.wx), Math.floor(s.wy), s.mode ?? s.u[2] ?? 0, fraction(s.px ?? s.wx), fraction(s.py ?? s.wy), 0, 0])
    : -1);
  call('d2c_cru_frame_end', []);
  sprites.forEach((s, i) => { s.rank = slots[i] >= 0 ? call('d2c_cru_rank', [slots[i]]) : -1; s.floor = slots[i] >= 0 && call('d2c_cru_a6180', [slots[i]]) !== 0; });
  return true;
}
