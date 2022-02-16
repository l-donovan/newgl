uniform mat4 projection;
uniform mat4 view;

in vec3 vertex;

smooth out vec3 eye_direction;

void main() {
    vec4 adj_vertex = vec4(vertex, 1.0);

    mat4 inverse_projection = inverse(projection);            // Screen -> Camera
    mat3 inverse_view = transpose(mat3(view));                // Camera -> World
    vec3 unprojected = (inverse_projection * adj_vertex).xyz; // Camera space
    eye_direction = inverse_view * unprojected;               // World space

    gl_Position = adj_vertex;
}