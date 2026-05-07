You are an expert C++ / GLSL developer working on the Project-M music visualizer (Milkdrop-compatible, Emscripten/WASM).

Task: Finish & Polish the Dual-Pipeline Preset Transition System

Current status (already working):
- Dual FBOs via iChannel0 (old) + iChannel1 (new)
- Progress via durationParams
- 17 transition shaders registered
- Milkdrop-style split: MilkdropPreset owns dual framebuffers, PresetTransition owns blending

Your job (follow exactly):

1. Apply these exact diffs for aspect ratio support:

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
+    // === Aspect ratio uniforms (added for proper geometry in all transitions) ===
+    shader->setUniform("iAspectX",     renderContext.aspectX);
+    shader->setUniform("iAspectY",     renderContext.aspectY);
+    shader->setUniform("iInvAspectX",  renderContext.invAspectX);
+    shader->setUniform("iInvAspectY",  renderContext.invAspectY);
 
     // Bind textures (already working)
     glActiveTexture(GL_TEXTURE0);
```

```diff
diff --git a/src/libprojectM/Renderer/TransitionShaderHeader.glsl b/src/libprojectM/Renderer/TransitionShaderHeader.glsl
index 3e7a1f2..7b9c4d5 100644
--- a/src/libprojectM/Renderer/TransitionShaderHeader.glsl
+++ b/src/libprojectM/Renderer/TransitionShaderHeader.glsl
@@ -12,6 +12,12 @@ uniform vec4  durationParams;   // x = linear progress, y = smoothed, etc.
 uniform vec2  iResolution;
 uniform float iTime;
 
+// === Aspect ratio correction (added 2026-05-07) ===
+uniform float iAspectX;
+uniform float iAspectY;
+uniform float iInvAspectX;
+uniform float iInvAspectY;
+
 // Random & beat data (already present)
 uniform float iRandStatic;
 uniform float iRandFrame;
```

2. After applying the diffs, audit and fix all 17 transition shaders for consistency:
   - Use iChannel0 / iChannel1
   - Use durationParams.x (or .y/.z) for progress
   - Use the new iAspect* uniforms where geometry matters (especially PageCurl, Glitch, HeatWave, Kaleidoscope, WaterDrop, MosaicZoom, Pixelate)

3. Add a safety fallback in PresetTransition if textures are invalid.

4. Add clear comments explaining the dual-FBO ownership in MilkdropPreset and PresetTransition.

Start with step 1, then report back when done. Be precise and keep the Milkdrop-style architecture.