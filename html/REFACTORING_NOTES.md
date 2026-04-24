# Project M - HTML/CSS Refactoring Notes

## Overview
The Project M visualization has been refactored to separate core music visualization code from cross-project compatibility features.

## Files Created

### projectm-core.html
**Minimal, optimized version for pure music visualization**
- Stripped down to essential UI elements
- Only includes: Canvas rendering, audio playback, preset switching, fullscreen toggle
- No file inputs, image processing, or 3D model loading
- Optimized for performance and image quality
- Fixed fullscreen canvas sizing issue
- ~400 lines vs ~1500 in original

### projectm-core.css
**Lean stylesheet with only essential visualization styling**
- Core canvas and container styles
- Button styling and positioning
- Image quality optimization (crisp-edges, pixelated rendering)
- Bezel frame and logo positioning
- ~150 lines vs ~600+ in original

## Features Removed (Cross-Project Compatibility)

### Image Processing / AI Features
- Depth analysis integration
- Image enhancement pipeline (brightness, contrast, etc.)
- Python/Pyodide integration for image processing
- APNG animation creation from visualization frames
- File input handlers for custom image analysis
- Progress bar elements for processing tasks

### 3D / Model Loading
- glTF model loading and saving
- Three.js integration
- 3D scene capture/export functionality
- Multiple visualization engines (B3, B3_B modes)

### Extended UI
- Complex menu system with tabs and sections
- Build selection dropdown
- Media type selection (Video/Image)
- Resolution mode toggle
- Multiple shader/visualizer selection
- Depth map configuration
- Frame recording buttons
- Python bytecode execution buttons
- Custom milk format loader buttons

### External Asset Linkage
- Remote CSS loading from css.1ink.us
- Remote shader loading from glsl.1ink.us
- Remote module loading from wasm.noahcohn.com
- Multiple URL dropdown selectors for presets

## File Structure Comparison

### Original (projectm.1ink)
```
- 1500+ lines of HTML
- 600+ lines of inline CSS
- 1500+ lines of JavaScript
- Complex interwoven functionality
```

### Refactored (projectm-core.html + projectm-core.css)
```
- ~400 lines of HTML (core visualization only)
- ~150 lines of CSS (essential styling only)
- ~300 lines of JavaScript (focused initialization)
- Clear separation of concerns
```

## Performance Improvements

1. **Image Rendering Quality**
   - Added `image-rendering: crisp-edges` and `image-rendering: pixelated` to canvases
   - Prevents antialiasing artifacts in visualization

2. **Canvas Sizing** (ResizeObserver-based)
   - Uses `ResizeObserver` on the canvas container for robust initial sizing
   - Fires once immediately on observation, syncing internal resolution with final CSS layout
   - Handles `devicePixelRatio` for crisp HiDPI rendering
   - Fullscreen toggle updates layout; ResizeObserver picks up the change automatically
   - Removed debounced `window.resize` handler in favor of element-level observation

3. **DOM Size**
   - Removed 100+ unused HTML elements
   - Reduced CSS rules by 75%
   - Faster parsing and rendering

## How to Use

### For Music Visualization Only
Use `projectm-core.html`:
```html
<link rel="stylesheet" href="./projectm-core.css">
```

### For Extended Features
Keep using original `projectm.1ink` (includes all cross-project features)

## Compatibility Notes

The core version is self-contained and doesn't require:
- Remote stylesheet loading
- Multiple external module loaders
- Python/Pyodide runtime
- Three.js library
- File system API

All visualization happens through the standard WebGL canvas and Web Audio API.

## Button Reference (Core Version)

| Button | Function |
|--------|----------|
| Start / Change Song | Load and play audio track |
| Random Preset | Switch to random visualization preset |
| Toggle Fullscreen | Switch between bezel mode (centered square) and fullscreen |
| Random Custom | Load a random custom preset |

## Future Enhancements

If you need to add features back:
1. Check what was in the original `projectm.1ink`
2. Only add what's needed for your use case
3. Keep CSS minimal and focused
4. Use event delegation to reduce DOM elements
5. Load features dynamically rather than inline
