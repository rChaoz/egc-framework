#version 330

// Input
in vec2 tex_coord;
in vec3 obj_color;

// Uniform properties
uniform sampler2D u_texture_0;
uniform int bothTextures;
uniform int useTexture;

// Output
layout(location = 0) out vec4 out_color;


void main() {
    if (useTexture == 0) out_color = vec4(obj_color, 1);
    else {
        vec4 color = texture2D(u_texture_0, tex_coord);
        if (color.a < 0.5f) discard;
	    out_color = color;
    }
}
