#define PI  3.14159265359
#define TAU 6.28318530718

uniform sampler2D texture_0;

smooth in vec3 eye_direction;

out vec4 frag_color;

void main() {
    vec3 eye = normalize(eye_direction);

    float inclination = acos(eye.y);
    float azimuth = atan(eye.z, eye.x);

    frag_color = texture(texture_0, vec2(azimuth / TAU, inclination / PI));
}