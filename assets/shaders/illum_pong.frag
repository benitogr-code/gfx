#version 410 core

in vec3 vtx_fragpos;
in vec3 vtx_normal;
in vec2 vtx_texcoords;

// Samplers
uniform int       texture_enable;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

// Fallback material
uniform vec3      mat_color;
uniform float     mat_ambient;
uniform float     mat_specular;

// Directional light
uniform vec3      light_pos;
uniform vec3      light_color;

// View
uniform vec3      view_pos;

out vec4 out_color;

void main() {
    vec3 color = mat_color;

    if (texture_enable != 0) {
        color = texture(texture_diffuse1, vtx_texcoords).rgb;
    }

    vec3 normal = normalize(vtx_normal);
    vec3 light_dir = normalize(light_pos - vtx_fragpos);

    vec3 ambient = mat_ambient * light_color;

    float diffuse_factor = max(dot(normal, light_dir), 0.0);
    vec3 diffuse = diffuse_factor * light_color;

    vec3 view_dir = normalize(view_pos - vtx_fragpos);
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
    vec3 specular = mat_specular * spec * light_color;

    vec3 result = (ambient + diffuse + specular) * color;
    out_color = vec4(result, 1.0);
}