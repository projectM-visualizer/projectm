# Multi-Pass Transition Starter Code (Project-M)

**Date:** 2026-05-07  
**Goal:** Provide ready-to-use starter code for adding 2-pass support to Project-M transitions.

---

## 1. Proposed API (in PresetTransition)

```cpp
// Add these methods to PresetTransition class

void BeginPass(int passNumber);
void EndPass();
GLuint GetPassTexture(int passNumber) const;
int GetCurrentPass() const;
```

---

## 2. C++ Skeleton (PresetTransition.cpp)

Add this logic inside the `Draw()` method or as new helper methods:

```cpp
// === Multi-Pass Support (Phase 1 - 2 passes) ===

void PresetTransition::BeginPass(int passNumber)
{
    if (passNumber < 0 || passNumber > 1) return;

    m_currentPass = passNumber;

    if (passNumber == 0)
    {
        // First pass - render to main framebuffer or dedicated pass0 FBO
        m_pass0Fbo->Bind();
    }
    else if (passNumber == 1)
    {
        // Second pass - render to pass1 FBO, using pass0 as input
        m_pass1Fbo->Bind();
        // Bind previous pass texture as iLastPassTex
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, m_pass0Fbo->GetColorTexture());
    }
}

void PresetTransition::EndPass()
{
    if (m_currentPass == 0)
    {
        m_pass0Fbo->Unbind();
    }
    else if (m_currentPass == 1)
    {
        m_pass1Fbo->Unbind();
    }

    m_currentPass = -1;
}

GLuint PresetTransition::GetPassTexture(int passNumber) const
{
    if (passNumber == 0) return m_pass0Fbo ? m_pass0Fbo->GetColorTexture() : 0;
    if (passNumber == 1) return m_pass1Fbo ? m_pass1Fbo->GetColorTexture() : 0;
    return 0;
}
```

---

## 3. GLSL Helper Macros (add to TransitionShaderHeaderGlsl330.frag)

```glsl
// Multi-pass helpers
#define iPass          _prjm_pass
#define iLastPassTex   _prjm_lastPassTex
#define iPassTime      _prjm_passTime

// In main() or per-pass code:
uniform int   _prjm_pass;
uniform sampler2D _prjm_lastPassTex;
uniform float _prjm_passTime;
```

---

## 4. First Small Diff (Recommended Starting Point)

**File:** `src/libprojectM/Renderer/PresetTransition.cpp`

Add member variables:
```cpp
std::shared_ptr<Framebuffer> m_pass0Fbo;
std::shared_ptr<Framebuffer> m_pass1Fbo;
int m_currentPass = -1;
```

Then implement `BeginPass()` / `EndPass()` as shown above.

---

## 5. Next Steps After This Starter

1. Create the two extra framebuffers in constructor
2. Add `iPass` and `iLastPassTex` uniforms
3. Update one transition shader (e.g. PageCurl) to use 2 passes as proof of concept
4. Test on both desktop and Emscripten

---

**Ready to hand to kimi-cli when you're ready.**