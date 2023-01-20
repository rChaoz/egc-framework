#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;
layout(location = 3) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

// Output
out vec3 f_color;
out vec2 f_tex_coord;
out vec3 world_position;
out vec3 world_normal;

void main() {
    f_color = v_color;
    f_tex_coord = v_texture_coord;

    vec4 pos = vec4(v_position, 1);
    world_position = (Model * pos).xyz;
    world_normal = (Model * vec4(v_normal, 0)).xyz;
    gl_Position = Projection * View * Model * pos;
}
