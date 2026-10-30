SlashGaming Diablo II Free Resolution (SGD2FreeRes 3.0.4.0) panel art: the 16 DC6 files that
play/assets/wide-hud.mjs lists (FREERES_FILES, the SIZE table).

Source: SGD2FreeRes.3.0.4.0.-.Modders.zip, data/SGD2FreeResolution/ui/panel.
Third-party mod art, not Blizzard's. Licence: GNU AGPL v3 with additional permissions (LICENSE.md, verbatim),
so redistributable; the copyright is Mir Drualga's, kept in LICENSE.md.

These files are tracked in git on purpose. ci/stage.sh copies them unconditionally to build-wasm/site/freeres/
and fails the build when one is missing; test/wide-hud-art.test.mjs checks each one decodes to its SIZE frames.
Without them the bottom bar draws only the 800-wide centre panel. D2_FREERES_DIR overrides the source directory.
