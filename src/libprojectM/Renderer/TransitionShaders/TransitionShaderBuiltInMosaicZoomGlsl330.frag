// MosaicZoom — the old preset breaks into a grid of tiles that each zoom out
// (with per-tile delay and slight individual scale) to reveal the new preset.
// Bass kicks accelerate the zoom; treble adds tile rotation jitter.

float _mz_hash(vec2 p)
{
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    vec2 uv = fragCoord / iResolution.xy;
    float p = iProgressEased;

    // Random grid size.
    float gridX = floor(mod(float(iRandStatic.x) * 0.01, 8.0)) + 6.0;   // 6..13
    float gridY = floor(mod(float(iRandStatic.y) * 0.01, 8.0)) + 6.0;

    vec2 tileSize = vec2(1.0 / gridX, 1.0 / gridY);
    vec2 tileId   = floor(uv / tileSize);
    vec2 tileUV   = (uv - tileId * tileSize) / tileSize;     // 0..1 within tile

    // Per-tile staggered timing.
    float h = _mz_hash(tileId + vec2(float(iRandStatic.w)));
    float delay = h * 0.55;
    float pTile = clamp((p - delay) / max(1.0 - delay, 0.0001), 0.0, 1.0);
    pTile = smoothstep(0.0, 1.0, pTile);

    // Bass-driven snap.
    float bassPump = clamp(iBassAtt, 0.0, 2.0);
    pTile = clamp(pTile * (1.0 + 0.25 * bassPump), 0.0, 1.0);

    // Treble-jittered per-tile rotation.
    float trebPump = clamp(iTrebAtt, 0.0, 2.0);
    float rot = (h - 0.5) * 1.4 * pTile * (1.0 + 0.5 * trebPump);
    float ca = cos(rot), sa = sin(rot);

    // Zoom-out the OLD tile around its own center, while shrinking it on screen.
    // Scale grows from 1 to ~1.6 inside the tile (like punching outward), then
    // alpha fades and the new preset bleeds in underneath.
    float scale = mix(1.0, 1.6, pTile);
    vec2 local  = (tileUV - 0.5) / max(scale, 0.001);
    local = vec2(local.x * ca - local.y * sa, local.x * sa + local.y * ca);
    local += 0.5;

    // Sample the OLD preset from the tile's own region.
    vec2 oldSampleUV = tileId * tileSize + clamp(local, 0.0, 1.0) * tileSize;
    vec3 oldCol = texture(iChannel0, clamp(oldSampleUV, 0.0, 1.0)).xyz;

    // Tile alpha mask — fades out as the tile zooms past full size.
    float tileAlpha = 1.0 - smoothstep(0.55, 1.0, pTile);

    // Inside-the-tile alpha falloff (so tile edges look softer, not hard rects).
    float tx = abs(tileUV.x - 0.5) * 2.0;
    float ty = abs(tileUV.y - 0.5) * 2.0;
    float edgeSoft = 1.0 - smoothstep(0.85, 1.0, max(tx, ty));
    tileAlpha *= edgeSoft;

    // Background: new preset.
    vec3 newCol = texture(iChannel1, uv).xyz;

    // Drop-shadow under each tile.
    float shadow = 0.25 * smoothstep(0.0, 0.5, pTile) * (1.0 - smoothstep(0.6, 1.0, pTile));
    newCol *= 1.0 - shadow * edgeSoft;

    // Edge highlight on the tile rim while it's flying outward.
    float rim = smoothstep(0.92, 1.0, max(tx, ty)) * smoothstep(0.0, 0.4, pTile)
              * (1.0 - smoothstep(0.7, 1.0, pTile));
    oldCol += vec3(rim) * 0.4;

    vec3 col = mix(newCol, oldCol, tileAlpha);

    fragColor = vec4(clamp(col, 0.0, 1.0), 1.0);
}
