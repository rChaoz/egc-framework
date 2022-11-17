#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 3) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform float Time;

// Output
out vec3 f_color;

void main()
{
    f_color = vec3(v_color.x, abs(sin(Time)), v_color.z);

    gl_Position = Projection * View * Model * vec4(v_position.x, v_position.y + abs(cos(Time) * 10), v_position.z, 1);
}
