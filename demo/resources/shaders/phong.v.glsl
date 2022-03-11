in vec4 vertex;
in vec3 normal;
in vec2 uv;

out vec2 v_uv;
out vec3 v_norm;
out vec3 v_frag;
out vec3 v_camera;

uniform mat4 model, view, projection;
uniform vec3 camera;

void main() {
    v_uv = vec2(uv.x, 1 - uv.y);
    v_norm = mat3(transpose(inverse(model))) * normal;
    v_frag = vec3(model * vertex);
    v_camera = camera;

    gl_Position = projection * view * model * vertex;
}
