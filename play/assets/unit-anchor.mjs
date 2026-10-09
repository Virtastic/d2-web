// A unit with a dynamic path has the engine's own 16.16 position (d2_session_unit_coords).
// Drawing it at the whole subtile made a walking unit hold still for several frames and then jump
// a square. Subtract the half subtile between the unit centre and the integer grid used by static
// projection. Rendering and pointer hit tests share this anchor; stationary units keep their
// integer position exposed by the session.
export function unitAnchor(entity) {
  return entity.preciseX != null
    ? [entity.preciseX - 0.5, entity.preciseY - 0.5]
    : [entity.x, entity.y];
}
