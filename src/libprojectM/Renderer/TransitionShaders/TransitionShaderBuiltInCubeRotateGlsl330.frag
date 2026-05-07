// CubeRotate — perspective rotation of a cube face. The old preset is the front
// face rotating away; the new preset rolls in on the adjacent face. Includes
// vanishing-point perspective foreshortening and edge shading.

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    vec2 uv = fragCoord / iResolution.xy;
    float aspect = iAspectX / iAspectY;

    // Random axis: 0 = horizontal (Y axis), 1 = vertical (X axis), random sign.
    bool horizontal = mod(float(iRandStatic.x) * 0.01, 2.0) < 1.0;
    float sgn = (mod(float(iRandStatic.y) * 0.01, 2.0) < 1.0) ? 1.0 : -1.0;

    float p = iProgressEased;
    float angle = p * 1.57079632 * sgn;        // 0 .. ±90°
    float ca = cos(angle);
    float sa = sin(angle);

    // Centered coords in screen space.
    vec2 c = (uv - 0.5) * 2.0;                  // -1 .. 1

    // Distance from camera to cube center (controls perspective strength).
    const float dist = 2.5;

    // Build the cube-face plane normals and origins for old/new faces.
    // Old face: at +Z (front), rotated by `angle` about the chosen axis.
    // New face: at +X (or +Y) edge, also part of the rotated cube — visible as
    // angle grows.
    //
    // We use a simple approach: construct a rotated plane parameter for each
    // face and pick whichever face the screen ray hits first that's facing camera.

    // Map screen coord to a ray in cube space. For aspect correctness we
    // scale x by aspect on the input side and unwind on the sample side.
    vec2 cAr = c;
    cAr.x *= aspect;

    vec3 col;
    float shade = 1.0;

    if (horizontal)
    {
        // Rotate about Y. Old face center on +Z, new face center on +X*sgn.
        // For a point on the rotated old face at local (lx, ly, +1):
        //   world: (lx*ca + sgn*1.0*sa,  ly,  -lx*sa + sgn*1.0*0.0 + 1.0*ca)... simplified
        // Approach: solve which face is hit by treating (cAr.x, cAr.y) as a
        // proxy for the rotated face's screen footprint.
        //
        // Simpler & more robust: pick face based on rotated x-coordinate sign.
        // Old face screen x-extent shrinks as |sa| grows; new face appears on
        // the trailing side.
        float boundary = sgn * sa;                        // moves from 0 to ±1
        // If we flip the screen x by boundary, samples on one side go to old, other to new.
        float worldX = cAr.x;
        // Old face: screen-space inverse of perspective. Approx: oldU.x = (worldX - boundary*0.5*ca) / (ca + small)
        if ((sgn > 0.0 && worldX < boundary) || (sgn < 0.0 && worldX > boundary))
        {
            // Sample old face with foreshortening.
            float lx = (worldX) / max(ca, 0.01);
            vec2 oldUV = vec2(lx / aspect, cAr.y) * 0.5 + 0.5;
            oldUV = clamp(oldUV, 0.0, 1.0);
            col = texture(iChannel0, oldUV).xyz;
            shade = 0.6 + 0.4 * abs(ca);
        }
        else
        {
            // Sample new face — its surface is rotated by (90° - angle).
            float lx = (worldX - boundary) / max(abs(sa), 0.01) * sgn;
            vec2 newUV = vec2(lx / aspect, cAr.y) * 0.5 + 0.5;
            newUV = clamp(newUV, 0.0, 1.0);
            col = texture(iChannel1, newUV).xyz;
            shade = 0.6 + 0.4 * abs(sa);
        }
    }
    else
    {
        // Rotate about X. Same logic but on Y.
        float boundary = sgn * sa;
        float worldY = cAr.y;
        if ((sgn > 0.0 && worldY < boundary) || (sgn < 0.0 && worldY > boundary))
        {
            float ly = (worldY) / max(ca, 0.01);
            vec2 oldUV = vec2(cAr.x / aspect, ly) * 0.5 + 0.5;
            oldUV = clamp(oldUV, 0.0, 1.0);
            col = texture(iChannel0, oldUV).xyz;
            shade = 0.6 + 0.4 * abs(ca);
        }
        else
        {
            float ly = (worldY - boundary) / max(abs(sa), 0.01) * sgn;
            vec2 newUV = vec2(cAr.x / aspect, ly) * 0.5 + 0.5;
            newUV = clamp(newUV, 0.0, 1.0);
            col = texture(iChannel1, newUV).xyz;
            shade = 0.6 + 0.4 * abs(sa);
        }
    }

    // Subtle vignette around the cube silhouette.
    float vig = 1.0 - smoothstep(0.95, 1.05, length(cAr) * 0.7);
    col *= shade * vig;

    fragColor = vec4(clamp(col, 0.0, 1.0), 1.0);
}
