// NPC class names are resolved from static monster/string tables. The open automap can request the
// same labels on every rendered frame, so retain a bounded working set for the loaded client.
export function createNpcNameLabelCache(resolve, limit = 512) {
  if (typeof resolve !== 'function') throw new TypeError('NPC name resolver must be a function');
  if (!Number.isInteger(limit) || limit < 1) throw new RangeError('NPC name cache limit must be positive');
  const entries = new Map();
  return {
    get(classId) {
      if (entries.has(classId)) {
        const label = entries.get(classId);
        entries.delete(classId);
        entries.set(classId, label);
        return label;
      }
      const label = resolve(classId);
      entries.set(classId, label);
      if (entries.size > limit) entries.delete(entries.keys().next().value);
      return label;
    },
    clear() { entries.clear(); },
    get size() { return entries.size; },
  };
}
