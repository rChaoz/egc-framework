#version 330

// Input
in vec2 texcoord;

// Uniform properties
uniform sampler2D texture_1;
uniform sampler2D texture_2;
uniform int bothTextures;
// TODO(student): Declare various other uniforms

// Output
layout(location = 0) out vec4 out_color;


void main()
{
    vec4 color1 = texture2D(texture_1, texcoord);
    vec4 color2 = texture2D(texture_2, texcoord);

    vec4 color = color1;
    if (bothTextures == 1) color = mix(color1, color2, 0.5f);
    
    if (color.a < 0.5f) discard;
	out_color = color;

}
