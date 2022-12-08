#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

// Uniforms for light properties
uniform vec3 light_position;
uniform vec3 eye_position;
uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

uniform vec3 object_color;

// Output value to fragment shader
out vec3 color;


void main()
{
    vec3 world_pos = (Model * vec4(v_position,1)).xyz;
    vec3 N = normalize(mat3(Model) * v_normal);
    vec3 L = normalize(light_position - world_pos);
    vec3 V = normalize(eye_position - world_pos);
    vec3 H = normalize(L + V);

    vec3 ambient = material_kd * 0.25 * vec3(1);

    float diffuse_light = material_kd * max(dot(N,L), 0);

    vec3 diffuse = vec3(0);

    vec3 specular = vec3(0);

    // It's important to distinguish between "reflection model" and
    // "shading method". In this shader, we are experimenting with the Phong
    // (1975) and Blinn-Phong (1977) reflection models, and we are using the
    // Gouraud (1971) shading method. There is also the Phong (1975) shading
    // method, which we'll use in the future. Don't mix them up!
    if (diffuse_light > 0)
    {
        diffuse = diffuse_light * object_color;
        specular = material_ks * object_color * pow(max(dot(V, reflect (-L, N)), 0), material_shininess);
    }

    float atenuare = 1 / distance(light_position, world_pos);

    color = ambient + atenuare * (diffuse + specular);

    gl_Position = Projection * View * Model * vec4(v_position, 1.0);
}
