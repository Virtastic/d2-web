// The hover tooltip of a belt potion: the text retail's inventory tooltip draws for an item, placed above
// the belt box it sits in. Before this, only the inventory, stash, shop and ground items had one, so a
// belt potion showed nothing on hover, and a phone's first tap on a potion (input-touch.mjs inspect rule)
// showed nothing either.
//
// Pure except for the drawing: the box under a point is the belt click's own test (gameplay-shell.mjs
// beltSlot in the click handler), and the lines are the engine's through the shell's tipFields, the same
// fields the inventory tooltip reads. The placement is item-tooltip.mjs's placeItemTip, unchanged.
import { createItemTooltip, itemTooltipSpec } from './item-tooltip.mjs';

// The belt box a point is in (its index in `boxes`, bottom row first), or -1. `origin` is hudOrigin(w, h, 'bottom').
export function beltSlotAt(boxes, origin, x, y) {
  if (!boxes?.length) return -1;
  const bx = x - origin.x, by = y - origin.y;
  return boxes.findIndex((b) => bx >= b.left && bx < b.right && by >= b.top && by < b.bottom);
}

// The plan for a hovered belt potion: { id, slot, cell, lines, colours } or null. `cell` is the box in retail's
// 800x600 coordinates (the belt is centre-bottom anchored, so its boxes are already in that frame).
// `tip(item)` is the shell's tipFields(item.id, item.code); `items` the snapshot's items.
export function beltHoverPlan({ boxes, origin, x, y, items, tip }) {
  const slot = beltSlotAt(boxes, origin, x, y);
  if (slot < 0) return null;
  const item = (items || []).find((it) => it.mode === 2 && it.gridX === slot);
  if (!item) return null;
  const b = boxes[slot];
  const spec = itemTooltipSpec({ ...tip(item), itemFlags: item.itemFlags });
  return { id: item.id, slot, cell: { x: b.left, y: b.top, w: b.right - b.left, h: b.bottom - b.top }, lines: spec.lines, colours: spec.colours };
}

// The drawing half: the inventory's own item tooltip (same fonts, same box, same placement). `act` is the
// snapshot's act (the font palette). Draws over the HUD, after the panels, as the inventory's tooltip does.
export function createBeltTooltip(read, documentObject = globalThis.document) {
  const tooltip = createItemTooltip(read, documentObject);
  return {
    draw(ctx, act, plan, width, height) {
      if (!plan) return null;
      const record = tooltip.draw(ctx, act, plan.cell, width, height, plan.lines, plan.colours);
      return record ? { id: plan.id, slot: plan.slot, ...record } : null;
    },
  };
}
