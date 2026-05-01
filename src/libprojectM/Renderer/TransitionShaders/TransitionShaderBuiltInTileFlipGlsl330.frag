// Tile flip transition — grid of tiles that flip like cards to reveal the new preset.

float hash(vec2 p)
{
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    vec2 uv = fragCoord / iResolution.xy;
    float progress = iProgressCosine;

    // Randomize grid size and flip direction
    float gridX = floor(mod(float(iRandStatic.x) * 0.01, 6.0)) + 4.0;
    float gridY = floor(mod(float(iRandStatic.y) * 0.01, 6.0)) + 4.0;
    float flipDir = (mod(float(iRandStatic.z) * 0.01, 2.0) < 1.0) ? 1.0 : -1.0;

    // Tile coordinates
    vec2 tileSize = vec2(1.0 / gridX, 1.0 / gridY);
    vec2 tileId = floor(uv / tileSize);
    vec2 tileUV = fract(uv / tileSize);

    // Per-tile random delay
    float tileHash = hash(tileId + vec2(float(iRandStatic.w)));
    float tileProgress = smoothstep(tileHash * 0.5, tileHash * 0.5 + 0.5, progress);

    // Simulate card flip with perspective compression
    float flipAngle = tileProgress * 3.14159265 * flipDir;
    float perspective = cos(flipAngle);
    float edgeDarken = smoothstep(0.0, 0.3, abs(perspective));

    // Adjust tile UV for perspective
    vec2 sampleUV = uv;
    sampleUV.x = (tileId.x + (tileUV.x - 0.5) / max(perspective, 0.01) + 0.5) * tileSize.x;
    sampleUV.y = (tileId.y + (tileUV.y - 0.5) / max(perspective, 0.01) + 0.5) * tileSize.y;

    // Which side of the card are we seeing?
    vec3 color;
    if (perspective > 0.0)
    {
        color = texture(iChannel0, clamp(sampleUV, tileId * tileSize, (tileId + 1.0) * tileSize)).xyz;
    }
    else
    {
        color = texture(iChannel1, clamp(sampleUV, tileId * tileSize, (tileId + 1.0) * tileSize)).xyz;
    }

    color *= edgeDarken;

    fragColor = vec4(color, 1.0);
}
