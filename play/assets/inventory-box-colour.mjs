// F516: retail's carried-item cell background uses index 138 for Act IV 1x1 items in
// cov-a4g-inv. Keep that rule scoped to the measured act and size until another act has a
// comparable carried-inventory capture. The existing two-cell case remains supported.
export function inventoryBoxColour(item, act) {
  const cells = (item.invWidth || 1) * (item.invHeight || 1);
  return cells === 2 || (act === 4 && cells === 1) ? 138 : 234;
}
