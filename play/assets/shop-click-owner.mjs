// D39: whether the inventory-panel click handler (lift / swap / inert) runs for a left click.
// Retail's panel hit-test consumes a click inside an open panel before the world sees it, so with a
// shop open a click on the inventory panel stays with the panel (it never closes the trade).
export const inventoryOwnsClick = (shopOpen, inInventoryPanel) => !shopOpen || inInventoryPanel;
