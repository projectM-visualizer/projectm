You are an expert C++ / OpenGL developer on Project-M.

### Current Status
- Phase 1 (Aspect + Robustness) complete
- Phase 2 Task 1 (Easing) complete
- Phase 2 Task 2 (Texture Filtering) is in progress or just finished

### Task 3: Final Polish & Documentation

Perform a final cleanup and documentation pass on the dual-FBO + transition system.

---

## Specific Tasks

### 1. Add / Improve Comments
- Add a clear top-level comment block in `PresetTransition.cpp` explaining the full dual-FBO + easing flow.
- Improve comments in `MilkdropPreset.cpp` and `.hpp` explaining the ping-pong framebuffer ownership.
- Make sure every new uniform and function has a short Doxygen-style comment.

### 2. Code Cleanup
- Remove any unused variables or includes introduced during development.
- Ensure consistent naming (e.g. `oldTex` / `newTex` vs `iChannel0` / `iChannel1`).
- Check that all `glActiveTexture` calls are properly paired with resets to `GL_TEXTURE0`.

### 3. Final Consistency Check
- Verify all 17 transition shaders use `iProgressEased` (or the new easing system).
- Make sure no shader still references the old `iProgressCosine` or `iProgressBicubic` macros unintentionally.
- Confirm the default easing type is `Smoothstep`.

### 4. Optional: Small README Note
If appropriate, add a short note in a relevant header or README about the new Milkdrop-style easing and filtering support.

---

**Rules:**
- Keep changes minimal and clean
- Focus on readability and maintainability
- Do not change behavior unless something is clearly broken

Start when you're ready. Report back when complete.