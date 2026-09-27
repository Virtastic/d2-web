// Objects.txt is the authority for an object's draw pivot. The engine supplies the
// object subtile and animation; this client-only reader supplies no world state.
const OBJECT_RECORD_BYTES = 448;
const OFFSET_X = 0x148;
const OFFSET_Y = 0x14c;
const SUBCLASS = 0x167;
const WAYPOINT_SUBCLASS = 0x40;

function objectRecord(Module, classId) {
  const table = Module.ccall('d2_datatbl', 'string', ['string'], ['objects']);
  const [ptr, count] = String(table || '').split(':').map(Number);
  if (!Number.isInteger(ptr) || !Number.isInteger(count) || classId < 0 || classId >= count) return null;
  const at = ptr + classId * OBJECT_RECORD_BYTES;
  const view = new DataView(Module.HEAPU8.buffer, at, OBJECT_RECORD_BYTES);
  if ((view.getUint8(SUBCLASS) & WAYPOINT_SUBCLASS) === 0) return null;
  return { xOffset: view.getInt32(OFFSET_X, true), yOffset: view.getInt32(OFFSET_Y, true) };
}

// Record the anchor passed to the DCC renderer beside the exact object subtile and
// Objects.txt offset that produced it. This is evidence, not a second placement path.
export function waypointPadAnchor(Module, object, drawn, x, y) {
  const offset = objectRecord(Module, object.classId);
  if (!offset || !drawn) return null;
  const layers = drawn.layers.filter((layer) => /\\(?:tr|s1)\\/i.test(layer.path || ''));
  if (!layers.length) return null;
  return {
    classId: object.classId, token: object.token, mode: drawn.mode, frame: drawn.frame,
    objectSubtile: { x: object.x, y: object.y }, tableOffset: offset,
    drawnAnchor: { x: Math.round(x + offset.xOffset), y: Math.round(y + offset.yOffset) },
    layers: layers.map(({ component, path, mode }) => ({ component, path, mode })),
  };
}
