// A gold pile's ground label is "<amount> <Gold>": the item name prefixed with its STAT_GOLD (stat 14) value, plain
// white (gold is quality 2, no colour). Retail's D2Client item-name builder; the amount is the pile's, never the base name alone.
export const goldLabel = (name, amount) => (Number.isFinite(amount) && amount > 0 ? `${amount} ${name}` : name);
