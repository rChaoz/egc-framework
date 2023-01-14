#version 330

// Input
in vec2 texcoord;

// Uniform properties
uniform sampler2D texture;
uniform int bothTextures;

// Output
layout(location = 0) out vec4 out_color;


void main() {
    vec4 color = texture2D(texture, texcoord);
    
    if (color.a < 0.05f) discard;
	out_color = color;
}
