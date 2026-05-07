You are an expert C++ / OpenGL developer on Project-M.

### Current Status
- Phase 1 is complete
- Phase 2 Task 1 (Easing) is in progress or just finished

### Task 2: Improve Texture Filtering During Transitions

Ensure both `iChannel0` and `iChannel1` use high-quality linear filtering for smooth transitions.

---

### Changes Needed in `PresetTransition.cpp`

Add the following code when binding the textures (replace or enhance the current binding logic):

```cpp
// === High-quality texture filtering for transitions (Milkdrop-style) ===
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, oldTex);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

glActiveTexture(GL_TEXTURE1);
glBindTexture(GL_TEXTURE_2D, newTex);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

// Optional: Anisotropic filtering on desktop (safe to leave out for Emscripten)
#ifdef GL_EXT_texture_filter_anisotropic
    GLfloat maxAniso = 0.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
    if (maxAniso > 1.0f) {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);
    }
#endif
```

Add a clear comment block explaining the purpose:

```cpp
// Set high-quality linear filtering on both transition textures.
// This makes wipes, curls, and other effects look much smoother,
// matching the quality of classic Milkdrop transitions.
```

---

### Optional Bonus (C++ side for Easing)

If you haven't already added it, also add this in `Draw()`:

```cpp
// Easing type: 0=Linear, 1=Smoothstep (default), 2=Ease-in, 3=Ease-out
float easingType = 1.0f;
shader->setUniform("easingType", easingType);
```

---

**Rules:**
- Keep changes minimal and clean
- Prioritize Emscripten compatibility
- Add good comments

Start with the texture filtering changes above. Report back when done.