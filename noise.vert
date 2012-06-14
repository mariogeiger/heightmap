#version 130
in vec2 vertex;

#define PERMUTATION_SIZE 64
//uniform int perm[PERMUTATION_SIZE];
uniform float step;

flat out vec2 grad[4];
flat out vec2 pos0;
noperspective out vec2 pos;

void main(void)
{
    gl_Position = vec4(vertex, 0.0, 1.0);

    const float u = 0.707106781; //1.0/sqrt(2.0);
    const vec2 gradient[8] = vec2[8](vec2(u,u), vec2(-u,u), vec2(u,-u), vec2(-u,-u),
                                     vec2(1.0, 0.0), vec2(-1.0, 0.0), vec2(0.0, 1.0), vec2(0.0, -1.0));

    const int perm[PERMUTATION_SIZE] = int[PERMUTATION_SIZE](48, 21, 60, 14, 51, 31, 20, 25, 35, 62, 26, 4, 40, 11, 43, 49, 10, 27, 54, 12, 46, 32, 24, 28, 16, 7, 1, 6, 42, 30, 34, 37,
                                                             13, 52, 22, 47, 18, 0, 53, 39, 57, 23, 36, 58, 3, 45, 63, 29, 33, 55, 17, 19, 9, 44, 2, 41, 5, 50, 8, 61, 15, 56, 38, 59);

    //Normaliser [-1,1] -> [0,1]
    vec2 v = (vertex.xy + vec2(1.0, 1.0)) / 2.0;

    //On récupère les positions de la grille associée à (x,y)
    vec2 p = floor(v / step + vec2(0.5, 0.5));

    //Masquage
    int ii = int(p.x) % PERMUTATION_SIZE;
    int jj = int(p.y) % PERMUTATION_SIZE;

    //Pour récupérer les vecteurs
    int gi0 = perm[(ii +     perm[jj              ]) % PERMUTATION_SIZE] % 8;
    int gi1 = perm[(ii +     perm[(jj + PERMUTATION_SIZE-1) % PERMUTATION_SIZE]) % PERMUTATION_SIZE] % 8;
    int gi2 = perm[(ii + 1 + perm[(jj + PERMUTATION_SIZE-1) % PERMUTATION_SIZE]) % PERMUTATION_SIZE] % 8;
    int gi3 = perm[(ii + 1 + perm[jj              ]) % PERMUTATION_SIZE] % 8;

    grad[0] = gradient[gi0];
    grad[1] = gradient[gi1];
    grad[2] = gradient[gi2];
    grad[3] = gradient[gi3];

    pos0 = v;
    pos = v;
}
