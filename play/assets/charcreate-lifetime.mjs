// The create-character screen's state (picked class, typed name, the class animations' poses, the
// checkboxes) lives only while that screen is up. Retail builds the screen when it opens and tears it
// down when it closes, so the next open starts with no class, an empty name and every hero idle.
// The shell keeps one instance; this is the rule that drops it once the screen is left.
export const keepWhileOpen = (open, instance) => (open ? instance : null);
