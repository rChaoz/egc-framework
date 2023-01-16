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
uniform vec2 textureCoordinatesDelta;

// Output
out vec2 tex_coord;
out vec3 obj_color;

void main() {
    tex_coord = v_texture_coord + textureCoordinatesDelta;
    if (overrideColor.x > 1 || overrideColor.y > 1 || overrideColor.z > 1) obj_color = v_color;
    else obj_color = overrideColor;
    gl_Position = Projection * View * Model * vec4(v_position, 1.0);
}
