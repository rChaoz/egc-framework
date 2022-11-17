#version 330

// Input
in vec3 f_color;

// Output
layout(location = 0) out vec4 out_color;


void main()
{
    out_color = vec4(f_color, 1);

}
