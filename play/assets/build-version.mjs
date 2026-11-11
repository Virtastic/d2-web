// The version this build shows on screen (title screen, automap area-name block). ONE source:
// electron/package.json "version". ci/stage.sh rewrites RELEASE_VERSION in the STAGED copy of this
// module (ci/stamp-version.mjs); the checked-in value is 'dev', so an unstaged build says `v dev`.
// The test runner (the parity harness) keeps retail's own label, which the recorded sessions and the
// title/automap draw lists contain: play/entry.js sets __d2Entry.versionLabel = 'retail' for it
// (?version=release in a test URL asks for the release label instead).
export const RELEASE_VERSION = 'dev';
export const RETAIL_LABEL = 'v 1.13';

export function versionLabel(entry = globalThis.__d2Entry) {
  return entry?.versionLabel === 'retail' ? RETAIL_LABEL : `v ${RELEASE_VERSION}`;
}
