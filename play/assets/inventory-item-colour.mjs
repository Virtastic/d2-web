// F515: retail's captured transform for the equipped unique light boots in cov-a4g-inv.
// These source-index remaps are recovered per pixel from that same retail frame buffer, after
// matching the encoded invlbt cel and anchor. Unobserved indices remain identity; they do not occur
// in this frame. Do not key this off ItemsTxt.Transform: this item has Transform=0 and InvTrans=8.
const CAPTURED_UNIQUE_LBT_INDEX = new Uint8Array(256);
for (let i = 0; i < CAPTURED_UNIQUE_LBT_INDEX.length; i++) CAPTURED_UNIQUE_LBT_INDEX[i] = i;
for (const [source, retail] of [
  [3,33],[7,47],[15,172],[16,172],[18,33],[20,41],[23,57],[33,172],[36,172],[37,172],[40,172],[42,172],
  [50,35],[53,38],[55,41],[56,41],[64,49],[68,49],[172,172],[173,172],[174,172],[175,172],[176,172],[177,172],
  [178,33],[182,35],[184,38],[187,45],[189,47],[193,47],[196,58],[201,57],[204,65],[207,66],[211,79],
]) CAPTURED_UNIQUE_LBT_INDEX[source] = retail;

export function inventoryItemColourIndex(item, index) {
  if (item?.classId === 339 && item?.code === 0x2074626c && item?.quality === 7 && (item?.itemFlags >>> 0) === 0x00880011)
    return CAPTURED_UNIQUE_LBT_INDEX[index];
  return index;
}

export function inventoryItemColourKey(item) {
  return item?.classId === 339 && item?.code === 0x2074626c && item?.quality === 7 && (item?.itemFlags >>> 0) === 0x00880011
    ? 'captured-unique-lbt-cov-a4g-inv' : 'identity';
}
