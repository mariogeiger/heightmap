#version 130
in vec2 vertex;

uniform int perm[256];
uniform float step;

flat out vec2 grad[4];
flat out vec2 pos0;
noperspective out vec2 pos;

void main(void)
{
    gl_Position = vec4(vertex, 0.0, 1.0);

    const float u = 0.707106781;
    const vec2 gradient[8] = vec2[8](vec2(1.0, 0.0), vec2(-u,u), vec2(u,u), vec2(u,-u),
                                     vec2(-u,-u), vec2(-1.0, 0.0), vec2(0.0, 1.0), vec2(0.0, -1.0));

    //Normaliser [-1,1] -> [0,1]
    vec2 v = (vertex.xy + vec2(1.0, 1.0)) / 2.0;

    //On récupère les positions de la grille associée à (x,y)
    vec2 p = floor(v / step + vec2(0.5, 0.5));

    //Masquage
    int ii = int(p.x) & 255;
    int jj = int(p.y) & 255;

    //Pour récupérer les vecteurs
    int gi0 = perm[(ii +     perm[jj              ]) & 255] & 7;
    int gi1 = perm[(ii +     perm[(jj + 255) & 255]) & 255] & 7;
    int gi2 = perm[(ii + 1 + perm[(jj + 255) & 255]) & 255] & 7;
    int gi3 = perm[(ii + 1 + perm[jj              ]) & 255] & 7;

    //Outputs
    grad[0] = gradient[gi0];
    grad[1] = gradient[gi1];
    grad[2] = gradient[gi2];
    grad[3] = gradient[gi3];

    pos0 = v;
    pos = v;
}
