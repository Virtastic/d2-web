// Retail player composites are data driven: Armor.txt supplies the body layer's
// light/medium/heavy variant (and the helmet/shield alternate graphic), while
// Weapons.txt supplies the hand graphic. The engine supplies the animation
// weapon class because dual-wield and two-handed selection depends on native
// inventory rules, not a single item's row.

const COMPONENT = ['HD', 'TR', 'LG', 'RA', 'LA', 'RH', 'LH', 'SH', 'S1', 'S2', 'S3', 'S4', 'S5', 'S6', 'S7', 'S8'];
const ARMOR_CLASS_COMPONENT = Object.freeze({ rarm: 3, larm: 4, torso: 1, legs: 2, rspad: 8, lspad: 9 });
// This mapping is walked from the player draw path; keep its pairs out of per-draw allocations.
const ARMOR_CLASS_COMPONENT_ENTRIES = Object.freeze(Object.entries(ARMOR_CLASS_COMPONENT));
// The native item compositor consumes body slots 4/5 for the hand split.
// SWITCHIN/SWITCHOUT are update flags, not its active-set selector.
const MAIN_HANDS = new Set([4, 5]);

function rows(bytes) {
  const lines = new TextDecoder().decode(bytes).replace(/\r/g, '').split('\n').filter(Boolean);
  const header = lines.shift().split('\t').map((name) => name.trim().toLowerCase());
  return lines.map((line) => {
    const values = line.split('\t'), row = {};
    for (let i = 0; i < header.length; i++) row[header[i]] = (values[i] || '').trim();
    return row;
  });
}

function itemCode(item) {
  if (typeof item?.code === 'string') return item.code.replace(/\0/g, '').trim().toLowerCase();
  const code = Number(item?.code) >>> 0;
  return String.fromCharCode(code & 255, (code >>> 8) & 255, (code >>> 16) & 255, (code >>> 24) & 255).replace(/\0/g, '').trim().toLowerCase();
}

/** Resolve worn presentation records into the exact variant requested per player component. */
export function createPlayerCompositeResolver(read) {
  let tables = null;
  function load() {
    if (tables) return tables;
    const armor = new Map(rows(read('data\\global\\excel\\armor.txt')).map((row) => [row.code.toLowerCase(), row]));
    const weapons = new Map(rows(read('data\\global\\excel\\weapons.txt')).map((row) => [row.code.toLowerCase(), row]));
    // ArmType.txt's row order is the numeric rArm/lArm/Torso/... value in Armor.txt.
    const armorClass = rows(read('data\\global\\excel\\ArmType.txt')).map((row) => row.token.toLowerCase());
    tables = { armor, weapons, armorClass };
    return tables;
  }
  return {
    resolve(items, defaults, engineWeaponClass, primaryWeaponId = 0) {
      const { armor, weapons, armorClass } = load();
      const components = { ...defaults }, source = {};
      const weaponClass = String(engineWeaponClass || '').toLowerCase();
      if (!weaponClass) throw new TypeError('engine weapon class is required for player composition');
      // Classify each active item once. This resolver runs from the player draw
      // path, so a frame with a large inventory must not rescan the complete
      // snapshot merely to find hands after assigning the body layers.
      const hands = new Map();
      for (const item of items || []) {
        if (item?.mode !== 1) continue;
        const code = itemCode(item), armorRow = armor.get(code), weaponRow = weapons.get(code);
        const record = { item, code, armorRow, weaponRow };
        const bodyLoc = Number(item.bodyLoc);
        if (MAIN_HANDS.has(bodyLoc)) hands.set(bodyLoc, record);
        if (armorRow) {
          // Helmets and shields carry their own alternate DCC token (e.g. fhl, kit).
          const direct = Number(armorRow.component);
          if ((direct === 0 || direct === 7) && armorRow.alternategfx && (direct !== 7 || MAIN_HANDS.has(bodyLoc))) {
            components[direct] = armorRow.alternategfx.toLowerCase();
            source[direct] = code;
          }
          // A torso item changes all of the body and shoulder-pad layers. Values are
          // ArmType.txt row numbers: 0 lit, 1 med, 2 hvy in retail 1.13c.
          // The six class columns belong to a torso composite (component TR). Shields
          // also carry zeroes in those columns; treating those zeroes as "lit" would
          // incorrectly erase the armour already selected by the torso item.
          if (direct === 1) for (const [field, component] of ARMOR_CLASS_COMPONENT_ENTRIES) {
            const token = armorClass[Number(armorRow[field])];
            if (token) { components[component] = token; source[component] = code; }
          }
        }
      }
      // A weapon's table component is its native presentation slot (not its
      // body location): a bow in RARM still draws in LH. For dual weapon pairs
      // the selected inventory weapon keeps its table component; a nonselected
      // item is sent to component 6, matching the client composite callback.
      const handWeapons = [];
      for (const record of hands.values()) if (record.weaponRow) handWeapons.push(record);
      const selectedId = Number(primaryWeaponId) >>> 0;
      const selected = handWeapons.find(({ item }) => (Number(item.id) >>> 0) === selectedId) || null;
      // The native client only applies its selected-item split when both table
      // records have n2Handed == 0 (Items.txt/Weapons.txt `2handed`).
      const dualWeaponPair = handWeapons.length === 2 && handWeapons.every(({ weaponRow }) => Number(weaponRow['2handed'] || 0) === 0);
      const handAssignments = [];
      for (const { item, code, weaponRow } of handWeapons) {
        if (!weaponRow?.alternategfx) continue;
        const nativeComponent = Number(weaponRow.component);
        const component = dualWeaponPair && item !== selected?.item ? 6 : nativeComponent;
        if (component !== 5 && component !== 6) continue;
        handAssignments.push({ id: Number(item.id) >>> 0, code, component,
          selected: item === selected?.item, nativeComponent });
        components[component] = weaponRow.alternategfx.toLowerCase();
        source[component] = code;
      }
      const layers = COMPONENT.map((component, index) => ({ component, variant: components[index] || '', source: source[index] || null }))
        .filter((layer) => layer.variant);
      return { weaponClass, components, source, layers, primaryWeaponId: selectedId, handAssignments };
    },
  };
}
