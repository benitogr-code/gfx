#version 410 core

//#common.inc

in vec3 vtx_fragpos;
in vec3 vtx_normal;
in vec2 vtx_texcoords;

// Samplers
uniform int       texture_slots;
uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;

// Fallback material
uniform vec3      mat_color;
uniform float     mat_ambient;
uniform float     mat_specular;
uniform float     mat_shininess;

// Directional light
uniform vec3      light_pos;
uniform vec3      light_color;

out vec4 out_color;

void main() {
    vec3 amb_color = vec3(mat_ambient, mat_ambient, mat_ambient);
    vec3 diff_color = mat_color;
    vec3 spec_color = vec3(mat_specular, mat_specular, mat_specular);

    if (texture_slots != 0) {
        diff_color = texture(texture_diffuse, vtx_texcoords).rgb;
        spec_color = texture(texture_specular, vtx_texcoords).rgb;
    }

    vec3 normal = normalize(vtx_normal);
    vec3 light_dir = normalize(light_pos - vtx_fragpos);

    vec3 ambient = amb_color * light_color;

    float diffuse_factor = max(dot(normal, light_dir), 0.0);
    vec3 diffuse = diffuse_factor * diff_color * light_color;

    vec3 view_dir = normalize(camera.pos - vtx_fragpos);
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec_factor = pow(max(dot(view_dir, reflect_dir), 0.0), mat_shininess);
    vec3 specular = spec_factor * spec_color * light_color;

    vec3 result = ambient + diffuse + specular;
    out_color = vec4(result, 1.0);
}
