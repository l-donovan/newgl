uniform mat4 model;
//uniform mat4 view;
//uniform mat4 projection;

in vec2 vertex;
in vec2 tex_coord;
in vec4 color;

out vec2 v_uv;
out vec4 v_color;

void main() {
    v_uv = tex_coord.xy;
    v_color = color;

    //gl_Position = projection * view * model * vertex;
    gl_Position = model * vec4(vertex, 0.0f, 1.0f);
}
