uniform sampler2D atlas;

in vec2 v_uv;
in vec4 v_color;

out vec4 frag_color;

void main() {
    float a = texture(atlas, v_uv.xy).r;
    frag_color = vec4(v_color.rgb, v_color.a * a);
}
