#version 330

// Input
in vec3 world_position;
in vec3 world_normal;

// Uniforms for light properties
uniform vec3 light_direction;
uniform vec3 light_positions[2];
uniform vec3 eye_position;

uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

// TODO(student): Declare any other uniforms
uniform int use_spotlight;

uniform vec3 object_color;

// Output
layout(location = 0) out vec4 out_color;


void main()
{
    vec3 ambient = material_kd * 0.25 * vec3(1);    
    vec3 diffuse = vec3(0);
    vec3 specular = vec3(0);
    vec3 N = normalize(world_normal);

    for (int i = 0; i < 2; i++) {
        vec3 light_position = light_positions[i];
        vec3 L = normalize(light_position - world_position);
        vec3 V = normalize(eye_position - world_position);
        vec3 H = normalize(L + V);

        float diffuse_light = material_kd * max(dot(N,L), 0);
        float atenuare = 1 / (distance(light_position, world_position) * distance(light_position, world_position) + 1);

        // It's important to distinguish between "reflection model" and
        // "shading method". In this shader, we are experimenting with the Phong
        // (1975) and Blinn-Phong (1977) reflection models, and we are using the
        // Gouraud (1971) shading method. There is also the Phong (1975) shading
        // method, which we'll use in the future. Don't mix them up!
        if (use_spotlight == 0 && diffuse_light > 0) {
            diffuse += diffuse_light * object_color * atenuare;
            specular += material_ks * object_color * pow(max(dot(V, reflect (-L, N)), 0), material_shininess) * atenuare;
        } else if (use_spotlight == 1) {
            float cut_off = radians(30.0);
            float spot_light = dot(-L, light_direction);
            float spot_light_limit = cos(cut_off);
            // Quadratic attenuation
            float linear_att = (spot_light - spot_light_limit) / (1.0f - spot_light_limit);
            float light_att_factor = pow(linear_att, 2);

            if (spot_light > cos(cut_off))
            {
	            // fragmentul este iluminat de spot, deci se calculeaza valoarea luminii conform  modelului Phong
	            // se calculeaza atenuarea luminii
                diffuse += diffuse_light * object_color * atenuare * light_att_factor;
                specular += material_ks * object_color * pow(max(dot(V, reflect (-L, N)), 0), material_shininess) * atenuare * light_att_factor;
            }
        }

    }
    

    out_color = vec4(ambient + diffuse * 3 + specular * 3, 1.0);
}
