#version 330

// Input
in vec2 tex_coord;
in vec3 obj_color;
in vec3 world_position;
in vec3 world_normal;

// Texture uniforms
uniform sampler2D u_texture_0;
uniform int bothTextures;
uniform int useTexture;

// Lighting uniforms
uniform int spotlightCount;
uniform vec3 spotlights[100];

uniform int coinCount;
uniform vec3 coinLights[100];

uniform vec3 playerSpotlightPos;
uniform vec3 playerSpotlightDir;
uniform vec3 eye_position;

// Output
layout(location = 0) out vec4 out_color;


void main() {
    // Get color
    vec3 color;
    if (useTexture == 0) color = obj_color;
    else {
        vec4 texColor = texture2D(u_texture_0, tex_coord);
        if (texColor.a < 0.5f) discard;
	    color = texColor.rgb;
    }

    //out_color = vec4(color, 1);
    //return;

    // LIGHTING
    float ambient = .2f, diffuse = 0, specular = 0;
    vec3 N = normalize(world_normal);
    vec3 V = normalize(eye_position - world_position);

    // Directional light (sun)
    vec3 H = normalize(vec3(.5f, 1, 1) + V);
    diffuse += .6f * max(dot(N, normalize(vec3(.3f, 1, 1))), 0);

    // Lightposts
    for (int i = 0; i < spotlightCount; ++i) {
        vec3 light_position = spotlights[i];
        vec3 L = normalize(light_position - world_position);

        float attenuate = 1 / (pow(distance(light_position, world_position), 1) + 1);

        float cut_off = radians(40.0);
        float spot_light_limit = cos(cut_off);

        // Right direction
        float spot_light = dot(-L, normalize(vec3(1, -2, 0)));
        float light_att_factor = pow((spot_light - spot_light_limit) / (1.0f - spot_light_limit), 2);

        if (spot_light > spot_light_limit) {
            diffuse += max(dot(N, L), 0) * attenuate * light_att_factor * 2;
            specular += pow(max(dot(V, reflect (-L, N)), 0), 30) * attenuate * light_att_factor * 2;
        }

        // Left direction
        spot_light = dot(-L, normalize(vec3(-1, -2, 0)));
        light_att_factor = pow((spot_light - spot_light_limit) / (1.0f - spot_light_limit), 2);

        if (spot_light > spot_light_limit) {
            diffuse += max(dot(N, L), 0) * attenuate * light_att_factor * 2;
            specular += pow(max(dot(V, reflect (-L, N)), 0), 30) * attenuate * light_att_factor * 2;
        }
    }

    // Coins lighting
    float coinsLight = 0;
    for (int i = 0; i < coinCount; ++i) {
        vec3 light_position = coinLights[i];
        vec3 L = normalize(light_position - world_position);

        float attenuate = 1 / (pow(distance(light_position, world_position), 2) + 1);

        coinsLight += max(dot(N, L), 0) * attenuate * .8f;
        coinsLight += pow(max(dot(V, reflect (-L, N)), 0), 30) * attenuate * .8f;
    }

    // Player lantern
    float lanternLight = 0;
    vec3 L = normalize(playerSpotlightPos - world_position);

    float attenuate = 1 / (pow(distance(playerSpotlightPos, world_position), .6f) + 1);

    float cut_off = radians(60.0);
    float spot_light = dot(-L, normalize(playerSpotlightDir));
    float spot_light_limit = cos(cut_off);
    float light_att_factor = pow((spot_light - spot_light_limit) / (1.0f - spot_light_limit), 2);

    if (spot_light > spot_light_limit) {
        lanternLight += max(dot(N, L), 0) * attenuate * light_att_factor * 2.5f;
        lanternLight += pow(max(dot(V, reflect (-L, N)), 0), 30) * attenuate * light_att_factor * 2.5f;
    }

    // Calculate sum of lights
    out_color = vec4(color * (ambient + diffuse + specular) + vec3(1, .85f, .21f) * coinsLight + vec3(.8f, .8f, 1.f) * lanternLight, 1);
}
