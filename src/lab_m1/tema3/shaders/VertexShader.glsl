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
uniform vec3 overrideColor;

// Output
out vec2 tex_coord;
out vec3 obj_color;
out vec3 world_position;
out vec3 world_normal;

void main() {
    tex_coord = v_texture_coord;
    world_position = (Model * vec4(v_position, 0)).xyz;
    world_normal = normalize((Model * vec4(v_normal, 0)).xyz);

    if (overrideColor.x > 1 || overrideColor.y > 1 || overrideColor.z > 1) obj_color = v_color;
    else obj_color = overrideColor;
    gl_Position = Projection * View * Model * vec4(v_position, 1.0);
}
