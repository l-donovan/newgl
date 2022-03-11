uniform sampler2D texture_0;

uniform vec3 mat_color;
uniform vec3 mat_light_pos;
uniform vec3 mat_light_color;

uniform float mat_ambient_strength;
uniform float mat_specular_strength;
uniform vec3 mat_ambient_color;

uniform bool mat_use_texture;

in vec2 v_uv;
in vec3 v_norm;
in vec3 v_frag;
in vec3 v_camera;

out vec4 frag_color;

void main(void) {
    vec3 ambient = mat_ambient_strength * mat_ambient_color;

    vec3 norm = normalize(v_norm);
    vec3 light_dir = normalize(mat_light_pos - v_frag);  

    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = diff * mat_light_color;

    vec3 view_dir = normalize(v_camera - v_frag);
    vec3 reflect_dir = reflect(-light_dir, norm);

    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
    vec3 specular = mat_specular_strength * spec * mat_light_color;

    vec3 result = ambient + diffuse + specular;

    if (mat_use_texture) {
        result *= texture(texture_0, v_uv).rgb;
    } else {
        result *= mat_color;
    }

    frag_color = vec4(result, 1.0);
}
