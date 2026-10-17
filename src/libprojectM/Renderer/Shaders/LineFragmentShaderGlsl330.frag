precision mediump float;

in vec4 fragment_color;
in highp float edge_distance;

uniform highp float half_width;
uniform highp float antialias; // 1: one-pixel anti-aliased edge; 0: hard edge like MilkDrop's GL lines

out vec4 color;

void main()
{
    float coverage;
    if (antialias > 0.5)
    {
        // How much of this pixel a line 2 * half_width wide covers (a box filter across the line).
        coverage = clamp(half_width + 0.5 - abs(edge_distance), 0.0, 1.0);
    }
    else
    {
        // The vertex shader sized the quad to the band, at least 1 px wide: every pixel it covers
        // is in. Lines thinner than 1 px fade by their width instead.
        coverage = min(2.0 * half_width, 1.0);
    }
    color = vec4(fragment_color.rgb, fragment_color.a * coverage);
}
