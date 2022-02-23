in vec4 vertex;
in vec3 normal;
in vec2 uv;

out vec2 v_uv;
out vec3 v_norm;
out vec3 v_reflection;
out vec3 v_refraction;
out float v_fresnel;

uniform mat4 model, view, projection;
uniform vec3 camera;

// Indices of refraction
const float ior_medium = 1.0;
const float ior_material = 1.51714;

// ior_medium to glass ratio of the indices of refraction (Eta)
const float Eta = ior_medium / ior_material;

// see http://en.wikipedia.org/wiki/Refractive_index Reflectivity
const float R0 = ((ior_medium - ior_material) * (ior_medium - ior_material)) / ((ior_medium + ior_material) * (ior_medium + ior_material));

void main() {
    v_uv = vec2(uv.x, 1 - uv.y);

    vec3 obj_pos_world = (model * vertex).xyz;
    vec3 incident_vector = normalize(camera - obj_pos_world);
    vec3 normal_vector = normalize((model * vec4(normalize(normal), 1.0)).xyz);

    v_fresnel = R0 + (1.0 - R0) * pow(1.0 + dot(normal_vector, incident_vector), 0.3);
    v_fresnel = dot(normal_vector, incident_vector);
    v_reflection = refract(incident_vector, normal_vector, Eta);
    v_refraction = reflect(incident_vector, normal_vector);
    v_norm = normal_vector;

    gl_Position = projection * view * model * vertex;
}
