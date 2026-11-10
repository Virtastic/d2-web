// Dropping the cursor item on a grid (inventory page 0, cube 3, stash 4). Retail's rule, from D2Game's handlers
// (PlrMsg.cpp Rcv0x18 / Rcv0x1F -> sub_6FC46D40): the footprint of the held item is tested against the items stored on
// that page. None overlapped: place it (0x18 {cursor, x, y, page}). Exactly one: swap (0x1F {cursor, thatItem, x, y}); the
// handler lifts that item onto the cursor and sets the held one at x,y (it refuses, as cheating, when the held item will not
// fit once the other is gone, so the client must not send what does not fit). Two or more: the drop is refused outright, the
// held item stays on the cursor and no packet is sent. A cube refuses to swap out the Horadric Cube itself (sub_6FC46D40).
export const CUBE_PAGE = 3;
// The stored items on `page` that the w x h footprint at (x, y) overlaps, in list order.
export function overlappedItems(items, page, x, y, w, h, heldId = -1) {
  return (items || []).filter((i) => i.id !== heldId && i.mode === 0 && i.invPage === page && i.gridX >= 0 && i.gridY >= 0 &&
    x < i.gridX + (i.invWidth || 1) && i.gridX < x + w && y < i.gridY + (i.invHeight || 1) && i.gridY < y + h);
}
// What a click with `held` on the cursor does at cell (x, y) of `page`: { action, item? } with action 'put' | 'swap' |
// 'refuse' | 'stack' | 'socket' | 'scroll' | 'cube'. A held Horadric Cube (Items.txt code 'box') is never swapped into the
// cube page. Dropped on the one item it overlaps, the held item combines instead of swapping when the item pair is one the
// game combines: the same stackable class stacks (0x21), a gem, rune or jewel onto a socketed item sockets (0x28), a scroll
// onto a tome fills the book (0x29), anything but the cube itself onto the carried cube goes into it (0x2A). The server's
// handler still decides whether it fits; the client only picks which packet the drop is.
export const SOCKET_TYPES = new Set(['gema', 'gemd', 'geme', 'gemr', 'gems', 'gemt', 'gemz', 'rune', 'jewl']);
const SOCKETED_FLAG = 0x800;
const codeOf = (i) => (typeof i.itemCode === 'string' ? i.itemCode : typeof i.code === 'string' ? i.code : '');
export function combineAction(held, target, page) {
  if (!held || !target || held.id === target.id) return null;
  if (codeOf(target) === 'box' && page !== CUBE_PAGE && codeOf(held) !== 'box') return 'cube';
  if (held.typeCode === 'scro' && target.typeCode === 'book') return 'scroll';
  if (held.stackable && target.stackable && held.classId !== undefined && held.classId === target.classId) return 'stack';
  if (SOCKET_TYPES.has(held.typeCode) && (target.socketed || (target.itemFlags & SOCKETED_FLAG))) return 'socket';
  return null;
}
export function decideGridPlace(items, page, x, y, held) {
  const hit = overlappedItems(items, page, x, y, held.invWidth || 1, held.invHeight || 1, held.id);
  if (hit.length === 0) return { action: 'put' };
  if (hit.length > 1) return { action: 'refuse' };
  if (page === CUBE_PAGE && codeOf(held) === 'box') return { action: 'refuse' };
  const combine = combineAction(held, hit[0], page);
  if (combine) return { action: combine, item: hit[0] };
  return { action: 'swap', item: hit[0] };
}
