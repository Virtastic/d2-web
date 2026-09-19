# Contributing

Thanks for helping. A few rules keep the project legal and reviewable:

- **Never commit Blizzard content**: no MPQs, DLLs, EXEs, extracted art, sound, tables or
  screenshots of game assets. `.gitignore` blocks the common extensions; check your diff anyway.
- **Never paste decompiler output.** Describe what retail does in your own words and write the code
  yourself. Cite the retail function with a `// 1.13c <DLL>+0x<address>` comment.
- **Retail 1.13c is the reference.** When behaviour is unclear, match what the original game does,
  and say how you checked in the pull request.
- Keep changes small and focused: one fix or feature per pull request.
- Run the tests before opening a pull request: `node --test test/*.test.mjs`, and `bash ci/build.sh`
  if you touched `src/`.

Bug reports: say what you did, what you expected, what happened, your browser, and the URL flags.
