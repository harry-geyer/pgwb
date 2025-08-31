#include <stdlib.h>
#include <math.h>


#define PGWB_PERMUTATION_SIZE 256


static void _pgwb_init_permutation(void);
static float _pgwb_fade(float t); 
static float _pgwb_lerp(float a, float b, float t); 
static float _pgwb_grad_dot(int hash, float x, float y); 
static float _pgwb_perlin_noise(float x, float y); 


static int _pgwb_p[PGWB_PERMUTATION_SIZE * 2];
static int _pgwb_grad[8][2] =
{
    {1, 1}, 
    {1, -1}, 
    {-1, 1}, 
    {-1, -1},
    {1, 0}, 
    {-1, 0}, 
    {0, 1}, 
    {0, -1}
};


void pgwb_perlin_generate(unsigned seed, float* height_map, int width, int height, float frequency, float amplitude)
{
    srand(seed);
    _pgwb_init_permutation();

    for (int y = 0; y < height; y++) 
    {
        for (int x = 0; x < width; x++) 
        {
            float nx = (float)x / width - 0.5f;
            float ny = (float)y / height - 0.5f;
            float noise_value = _pgwb_perlin_noise(nx * frequency, ny * frequency) * amplitude;
            height_map[y * width + x] = noise_value;
        }
    }
}


static void _pgwb_init_permutation(void) 
{
    for (int i = 0; i < PGWB_PERMUTATION_SIZE; i++) 
    {
        _pgwb_p[i] = i;
    }
    for (int i = 0; i < PGWB_PERMUTATION_SIZE; i++) 
    {
        int j = rand() % PGWB_PERMUTATION_SIZE;
        int temp = _pgwb_p[i];
        _pgwb_p[i] = _pgwb_p[j];
        _pgwb_p[j] = temp;
    }
    for (int i = 0; i < PGWB_PERMUTATION_SIZE; i++) 
    {
        _pgwb_p[PGWB_PERMUTATION_SIZE + i] = _pgwb_p[i];
    }
}


static float _pgwb_fade(float t) 
{
    return t * t * t * (t * (t * 6.f - 15.f) + 10.f);
}


static float _pgwb_lerp(float a, float b, float t) 
{
    return a + t * (b - a);
}


static float _pgwb_grad_dot(int hash, float x, float y) 
{
    int h = hash & 7;
    return _pgwb_grad[h][0] * x + _pgwb_grad[h][1] * y;
}


static float _pgwb_perlin_noise(float x, float y) 
{
    int x0 = (int)floor(x) & 255;
    int y0 = (int)floor(y) & 255;

    float xf = x - floor(x);
    float yf = y - floor(y);

    float u = _pgwb_fade(xf);
    float v = _pgwb_fade(yf);

    int aa = _pgwb_p[_pgwb_p[x0] + y0];
    int ab = _pgwb_p[_pgwb_p[x0] + y0 + 1];
    int ba = _pgwb_p[_pgwb_p[x0 + 1] + y0];
    int bb = _pgwb_p[_pgwb_p[x0 + 1] + y0 + 1];

    float x1 = _pgwb_lerp(_pgwb_grad_dot(aa, xf, yf), _pgwb_grad_dot(ba, xf - 1, yf), u);
    float x2 = _pgwb_lerp(_pgwb_grad_dot(ab, xf, yf - 1), _pgwb_grad_dot(bb, xf - 1, yf - 1), u);
    return (_pgwb_lerp(x1, x2, v) + 1) / 2;
}

