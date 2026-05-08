# Starter: Port HeatWave to 2-Pass

**Transition:** HeatWave  
**Goal:** Split into 2 passes for much better visual quality

---

## Recommended Split

**Pass 0 (Base Layer):**
- Main heat distortion / ripple effect
- Base color and old/new preset blending
- Core wave geometry

**Pass 1 (Enhancement Layer):**
- Heat shimmer / caustics using `iLastPassTex`
- Enhanced bright highlights along wave edges
- Subtle glow and color shift
- Soft vignette or heat haze

---

## GLSL Structure (Suggested)

```glsl
void main() {
    if (iPass == 0) {
        // === PASS 0: Base Heat Wave ===
        vec2 distortedUV = uv + waveOffset * intensity;
        vec4 base = mix(texture(iChannel0, distortedUV), 
                        texture(iChannel1, distortedUV), 
                        iProgressEased);
        gl_FragColor = base;
    } 
    else {
        // === PASS 1: Heat Shimmer + Highlights ===
        vec4 previous = texture(iLastPassTex, uv);
        
        // Add shimmer, highlights, glow using previous pass result
        float shimmer = sin(uv.x * 40.0 + iTime * 8.0) * 0.03;
        vec4 enhanced = previous + vec4(shimmer, shimmer * 0.6, 0.0, 0.0);
        
        // Soft glow along wave edges
        float edge = ...; // calculate wave edge
        enhanced += vec4(0.8, 0.6, 0.2, 0.0) * edge * 0.4;
        
        gl_FragColor = enhanced;
    }
}
```

---

## First Step for kimi

1. Duplicate `TransitionShaderBuiltInHeatWaveGlsl330.frag`
2. Rename it to something like `TransitionShaderBuiltInHeatWave2PassGlsl330.frag`
3. Implement the 2-pass split as shown above
4. Register it with `passCount = 2` in `TransitionShaderManager`

---

**Ready when you are.** Hand this to kimi when you're ready to start.