#version 130
flat in vec2 grad[4];
flat in vec2 pos0;
noperspective in vec2 pos;

uniform float step;
uniform float amplitude;

out vec4 color;

void main(void)
{
    vec2 d = (pos - pos0) / step;
    float a = clamp(length(d), 0.0, 1.0);
    color = vec4(a, a, a, 1.0);
//    return;

    //on récupère les vecteurs et on pondère
    float s = dot(grad[0], d);
    float t = dot(grad[1], d + vec2(0.0, 1.0));
    float u = dot(grad[2], d + vec2(-1.0, 1.0));
    float v = dot(grad[3], d + vec2(-1.0, 0.0));

    //Lissage
    float Cx = 3.0 * d.x * d.x - 2.0 * abs(d.x * d.x * d.x);

    float Li1 = s + Cx*(v-s);
    float Li2 = t + Cx*(u-t);

    float Cy = 3.0 * d.y * d.y - 2.0 * abs(d.y * d.y * d.y);

    float noise = Li1 + Cy*(Li2-Li1);
    float z = amplitude * (noise + 1.0) / 2.0;
    color = vec4(z, z, z, 1.0);
}
