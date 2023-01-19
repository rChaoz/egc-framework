#version 330

// Input
in vec3 f_color;
in vec2 f_tex_coord;

// Output
layout(location = 0) out vec4 out_color;

// Uniforms
uniform int useTexture;
uniform sampler2D u_texture_0;
uniform vec3 overrideColor;

void main() {
    if (useTexture != 0) out_color = texture2D(u_texture_0, f_tex_coord);
    else if (overrideColor.r > 1 || overrideColor.g > 1 || overrideColor.b > 1) out_color = vec4(f_color, 1);
    else out_color = vec4(overrideColor, 1);
}
