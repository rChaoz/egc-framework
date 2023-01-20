#version 330

// Input
in vec3 f_color;
in vec2 f_tex_coord;
in vec3 world_position;
in vec3 world_normal;

// Output
layout(location = 0) out vec4 out_color;

// Uniforms
uniform int useTexture;
uniform int shinyness;
uniform sampler2D u_texture_0;
uniform vec3 overrideColor;
uniform vec3 eye_position;

void main() {
    vec3 color;
    if (useTexture != 0) color = texture2D(u_texture_0, f_tex_coord).rgb;
    else if (overrideColor.r > 1 || overrideColor.g > 1 || overrideColor.b > 1) color = f_color;
    else color = overrideColor;

    // LIGHTING
    float ambient = .2f, diffuse = 0, specular = 0;
    vec3 N = normalize(world_normal);
    vec3 V = normalize(eye_position - world_position);

    // Directional light (sun)
    vec3 L = vec3(.5f, 3, 1);
    diffuse += .9f * max(dot(N, normalize(L)), 0);
    if (dot(N, L) > 0) specular += .15f * pow(max(dot(V, reflect (-L, N)), 0), shinyness);

    out_color = vec4(color * (ambient + diffuse + specular), 1);
}
