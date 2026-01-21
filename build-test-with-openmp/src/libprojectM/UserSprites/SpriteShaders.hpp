#pragma once

#include <string>

static std::string kMilkdropSpriteFragmentGlsl330 = R"(
precision mediump float;

in vec4 fragment_color;
in vec2 fragment_texture;

uniform sampler2D texture_sampler;

uniform int blend_mode;

out vec4 color;

void main() {
    // Comments contain the original code equivalents used in Milkdrop and a short explanation
    // on what each mode does.
    switch (blend_mode)
    {
        case 0:
        // lpDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        // lpDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
        // lpDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
        // lpDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
        // lpDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
        // for (k=0; k<4; k++) v3[k].Diffuse = D3DCOLOR_RGBA_01(r,g,b,a);

        // Vertex diffuse color is modulated/multiplied with texture diffuse color.
        // Texture stage state in D3D is equivalent to only use vertex diffuse alpha and ignoring texture alpha
        color = vec4(fragment_color.rgb * texture(texture_sampler, fragment_texture.st).rgb, fragment_color.a);
        break;

        case 1:
        // for (k=0; k<4; k++) v3[k].Diffuse = D3DCOLOR_RGBA_01(r*a,g*a,b*a,1);

        // Vertex and texture diffuse colors are multiplied/modulated and then scaled by vertex alpha.
        // Texture alpha is ignored (full opacity).
        color = vec4((fragment_color.rgb * texture(texture_sampler, fragment_texture.st).rgb) * fragment_color.a, 1.0);
        break;

        case 2:
        // for (k=0; k<4; k++) v3[k].Diffuse = D3DCOLOR_RGBA_01(r*a,g*a,b*a,1);

        // Vertex and texture diffuse colors are multiplied/modulated and then scaled by vertex alpha.
        // Texture alpha is ignored (full opacity).
        color = vec4((fragment_color.rgb * texture(texture_sampler, fragment_texture.st).rgb) * fragment_color.a, 1.0);
        break;

        case 3:
        // for (k=0; k<4; k++) v3[k].Diffuse = D3DCOLOR_RGBA_01(1,1,1,1);

        // Vertex color is ignored, only use texture diffuse color.
        // Vertex and texture alpha is also ignored in this mode.
        color = vec4(texture(texture_sampler, fragment_texture.st).rgb, 1.0);
        break;

        case 4:
        // lpDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        // lpDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
        // lpDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
        // lpDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
        // lpDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
        // lpDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
        // for (k=0; k<4; k++) v3[k].Diffuse = D3DCOLOR_RGBA_01(r,g,b,a);

        // Vertex diffuse color is modulated/multiplied with texture diffuse color.
        // Texture stage state in D3D is equivalent to multiply (AKA modulate) vertex diffuse alpha with texture alpha.
        color = fragment_color * texture(texture_sampler, fragment_texture.st);
        break;
    }
}
)";

static std::string kMilkdropSpriteVertexGlsl330 = R"(
precision mediump float;

layout(location = 0) in vec2 vertex_position;
layout(location = 1) in vec4 vertex_color;
layout(location = 2) in vec2 vertex_texture;

out vec4 fragment_color;
out vec2 fragment_texture;

void main(){
    gl_Position = vec4(vertex_position, 0.0, 1.0);
    fragment_color = vertex_color;
    fragment_texture = vertex_texture;
}
)";


