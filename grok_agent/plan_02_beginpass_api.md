# Plan 02: Add BeginPass() / EndPass() Public API

**Phase:** B2 (Multi-pass)  
**Priority:** Medium-High  
**Est. Sessions:** 1

## Goal
Expose a clean public API so external code (and future transitions) can explicitly control passes.

## Tasks
1. Add `BeginPass(int passNumber)` and `EndPass()` methods to `PresetTransition`
2. Update internal `Draw()` logic to use these methods when available
3. Add `GetCurrentPass()` helper
4. Update documentation and example usage
5. Ensure backward compatibility (default = single pass)

## Success Criteria
- Clean, intuitive API that matches the original design
- Existing code (using `SetPassCount`) continues to work
- Easy for new transitions to adopt

## First Step
Add the method signatures to `PresetTransition.hpp` and implement empty versions in `.cpp`.