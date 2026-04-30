# Preset Directory Configuration

The `presetDir` hidden HTML div controls which folder the Contabo API pulls random presets from.

## HTML Setup

Add the hidden config div alongside the other hidden fields:

```html
<!-- projectm-core.html -->
<div id="dataContainer" style="display:none;">
  ...
  <div id="presetDir">default</div>
</div>

<!-- projectm.1ink -->
<div hidden id=presetDir>default</div>
```

## Values

| `presetDir` value | API query param | Behaviour |
|-------------------|-----------------|-----------|
| `default` | `dir=any` | API returns a random preset from the default pool (`milkLRG`, `milkSML`, etc.) |
| `folder_name` | `dir=folder_name` | API returns a random preset from `https://glsl.1ink.us/folder_name` |

## Examples

### Default behaviour (mixed pool)
```html
<div id="presetDir">default</div>
```
Fetches from:
```
https://storage.noahcohn.com/api/presets/random?dir=any
```

### Custom folder
```html
<div id="presetDir">custom_milk</div>
```
Fetches from:
```
https://storage.noahcohn.com/api/presets/random?dir=custom_milk
```
Which serves presets stored at `https://glsl.1ink.us/custom_milk/`.

## Implementation Details

- `getPresetDir()` reads `#presetDir` innerHTML and maps `default` → `any`
- `fetchApiPreset(apiBase)` calls `${apiBase}/api/presets/random?dir=${encodeURIComponent(dir)}`
- Both **Random Preset** and **Random Preset (API)** buttons now always use the API path via `loadRandomApiPreset()`
- Local `Module.switchPreset()` is kept available but is no longer used for random switching
