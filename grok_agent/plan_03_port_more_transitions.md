# Plan 03: Port 1–2 More Transitions to 2-Pass

**Phase:** B2 (Multi-pass)  
**Priority:** High  
**Est. Sessions:** 2–3

## Goal
Prove the multi-pass system is reusable by converting real transitions.

## Recommended Transitions
- **HeatWave** (great for 2-pass: distortion in pass 0, heat shimmer in pass 1)
- **WaterDrop** (ripples in pass 0, caustics/refraction in pass 1)
- **Glitch** (base glitch in pass 0, RGB split + scanlines in pass 1)

## Tasks
1. Choose 1–2 transitions to port
2. Split logic between passes logically
3. Use `iPass` and `iLastPassTex` effectively
4. Test and tune visual quality
5. Register pass count in `TransitionShaderManager`

## Success Criteria
- At least one additional high-quality 2-pass transition working
- Clear demonstration of the framework's value

## First Step
Pick one transition (recommend HeatWave) and analyze where the split makes most sense.