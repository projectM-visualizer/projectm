// PageCurl — fakes a 3D page-turn by rolling a diagonal seam across the screen,
// with a curl shadow on the back face and a bright highlight on the leading edge.

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    vec2 uv = fragCoord / iResolution.xy;
    float aspect = iResolution.x / max(iResolution.y, 1.0);

    // Random seam angle from a small set so it always feels deliberate.
    int dirPick = int(mod(float(iRandStatic.x), 4.0));
    vec2 dir;
    if (dirPick == 0)      dir = normalize(vec2( 1.0,  0.6));
    else if (dirPick == 1) dir = normalize(vec2(-1.0,  0.6));
    else if (dirPick == 2) dir = normalize(vec2( 1.0, -0.6));
    else                   dir = normalize(vec2(-1.0, -0.6));

    float p = iProgressBicubic;

    // Coord along the curl axis, normalized so the seam sweeps from -1..+1.
    vec2 c = uv - 0.5;
    c.x *= aspect;
    float along = dot(c, dir);
    float seam  = mix(-1.2 * 0.5 * (aspect + 1.0), 1.2 * 0.5 * (aspect + 1.0), p);
    float d     = along - seam;

    // Curl radius — the curl is the ribbon between [-curlRad, 0] in `d`.
    float curlRad = 0.18 + 0.05 * clamp(iMidAtt, 0.0, 2.0);

    if (d > 0.0)
    {
        // Untouched old page.
        fragColor = vec4(texture(iChannel0, uv).xyz, 1.0);
        return;
    }
    else if (d > -curlRad)
    {
        // The curl: parameterize by angle = pi*(1 - t), where t = -d/curlRad in [0,1].
        float t = -d / curlRad;
        float angle = 3.14159265 * t;

        // Project the curl back along the seam direction.
        // The visible point on the curled paper at parameter t is at:
        //   along' = seam - sin(angle) * curlRad     (foreshortened forward)
        // Sample the OLD page at that point.
        float alongCurl = seam - sin(angle) * curlRad;
        vec2 cCurl = c + dir * (alongCurl - along);
        cCurl.x /= aspect;
        vec2 oldUV = clamp(cCurl + 0.5, 0.0, 1.0);
        vec3 oldCol = texture(iChannel0, oldUV).xyz;

        // Backface shading: the back of the curled page is a darker, desaturated
        // version of the old page (paper-back tint).
        vec3 backTint = vec3(dot(oldCol, vec3(0.299, 0.587, 0.114))) * 0.55
                      + vec3(0.10, 0.08, 0.06);

        // Lighting: use cos(angle) as Lambert-ish term (front faces camera at t=0,
        // grazing in middle, back at t=1).
        float lambert = abs(cos(angle));
        float frontFace = step(0.5, 1.0 - t);    // first half is front-of-page
        vec3 paper = mix(backTint, oldCol, frontFace);
        paper *= 0.55 + 0.45 * lambert;

        // Bright highlight along the leading edge.
        float edgeHi = smoothstep(0.06, 0.0, abs(d)) * 0.6;
        paper += vec3(1.0) * edgeHi;

        // Drop shadow on the new page just below the curl.
        // (Not visible here — handled in the d < -curlRad branch.)
        fragColor = vec4(clamp(paper, 0.0, 1.0), 1.0);
        return;
    }
    else
    {
        // Revealed new page, with a soft shadow cast by the curl.
        vec3 newCol = texture(iChannel1, uv).xyz;
        float shadow = smoothstep(curlRad + 0.12, curlRad, -d) * 0.45;
        newCol *= (1.0 - shadow);
        fragColor = vec4(clamp(newCol, 0.0, 1.0), 1.0);
        return;
    }
}
