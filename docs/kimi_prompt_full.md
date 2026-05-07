You are an expert C++ / OpenGL / GLSL developer working on Project-M (Milkdrop-compatible music visualizer, Emscripten/WASM build).

### Current Status (already working well)
- Dual FBO pipeline is functional: iChannel0 = old preset, iChannel1 = new preset
- Progress is passed via durationParams (x = linear, y/z/w = smoothed versions)
- 17 transition shaders are registered and mostly working
- Architecture follows real Milkdrop style: MilkdropPreset owns dual m_framebuffer[0/1], PresetTransition owns the blending shader and logic

### Your Mission (complete all steps in order)

**Step 1: Add Aspect Ratio Uniforms (Highest priority)**
Apply these exact diffs:

```diff
diff --git a/src/libprojectM/Renderer/PresetTransition.cpp b/src/libprojectM/Renderer/PresetTransition.cpp
index 8f4c2e1..c9d3b2a 100644
--- a/src/libprojectM/Renderer/PresetTransition.cpp
+++ b/src/libprojectM/Renderer/PresetTransition.cpp
@@ -142,6 +142,12 @@ void PresetTransition::Draw(const RenderContext& renderContext)
     shader->setUniform("durationParams", durationParams);
     shader->setUniform("iResolution",   renderContext.resolution);
     shader->setUniform("iTime",         renderContext.time);
+
+    // === Aspect ratio uniforms for correct geometry (added 2026-05-07) ===
+    shader->setUniform("iAspectX",     renderContext.aspectX);
+    shader->setUniform("iAspectY",     renderContext.aspectY);
+    shader->setUniform("iInvAspectX",  renderContext.invAspectX);
+    shader->setUniform("iInvAspectY",  renderContext.invAspectY);
 
     glActiveTexture(GL_TEXTURE0);
     glBindTexture(GL_TEXTURE_2D, iChannel0);
```

```diff
diff --git a/src/libprojectM/Renderer/TransitionShaderHeader.glsl b/src/libprojectM/Renderer/TransitionShaderHeader.glsl
index 3e7a1f2..7b9c4d5 100644
--- a/src/libprojectM/Renderer/TransitionShaderHeader.glsl
+++ b/src/libprojectM/Renderer/TransitionShaderHeader.glsl
@@ -12,6 +12,12 @@ uniform vec4  durationParams;
 uniform vec2  iResolution;
 uniform float iTime;
 
+// === Aspect ratio correction uniforms (added 2026-05-07) ===
+uniform float iAspectX;
+uniform float iAspectY;
+uniform float iInvAspectX;
+uniform float iInvAspectY;
+
 uniform float iRandStatic;
 uniform float iRandFrame;
```

**Step 2: Audit & Fix All 17 Transition Shaders**
Go through every shader and ensure consistency:

**Mandatory rules for all shaders:**
- Use `iChannel0` for old preset and `iChannel1` for new preset
- Use `durationParams.x` (or `.y`/`.z`) as the main progress value
- Add support for the new aspect uniforms (`iAspectX`, `iAspectY`, `iInvAspectX`, `iInvAspectY`) where geometry or distortion is involved

**Priority shaders to fix first (most important for visual quality):**
1. TransitionShaderBuiltInPageCurlGlsl330.frag
2. TransitionShaderBuiltInGlitchGlsl330.frag
3. TransitionShaderBuiltInHeatWaveGlsl330.frag
4. TransitionShaderBuiltInKaleidoscopeGlsl330.frag
5. TransitionShaderBuiltInWaterDropGlsl330.frag
6. TransitionShaderBuiltInMosaicZoomGlsl330.frag
7. TransitionShaderBuiltInPixelateGlsl330.frag

After fixing the priority ones, do a quick pass over the remaining 10 shaders for naming consistency.

**Step 3: Add Safety Fallback**
In `PresetTransition::Draw()`, add this check near the top:

```cpp
if (!iChannel0 || !iChannel1) {
    // Fallback to simple blend if textures are missing
    // (prevents crashes during rapid preset changes)
}
```

**Step 4: Documentation & Comments**
Add clear comments in:
- `PresetTransition.cpp` explaining the dual-FBO flow
- `MilkdropPreset.cpp` explaining why it owns two framebuffers and how the swap works

**Step 5: Final Polish**
- Make sure the system works cleanly on both desktop and Emscripten
- Test rapid preset switching (should not crash or leak)

Start with **Step 1**, then report back with results before moving to Step 2. Be precise and keep the existing Milkdrop-style architecture.