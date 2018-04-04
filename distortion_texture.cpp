#include "distortion_texture.h"
#include "math.h"
#include "memory.h"

static void create_distortion_compensation_texture(float* pixels, unsigned size)
{
    const unsigned half_size = size/2;
    const unsigned fourth_size = size/4;
    const Vector3 observation_dir = vector3_up;
    const Vector3 observation_pos = {(float)half_size, 0, (float)half_size};
    const unsigned plane_y = half_size;

    for (unsigned z = fourth_size; z < size - fourth_size; ++z)
    {
        for (unsigned x = fourth_size; x < size - fourth_size; ++x)
        {
            const Vector3 p = {(float)x * 2 - (float)half_size, (float)plane_y, (float)z * 2 - (float)half_size};
            pixels[z * size + x] = vector3_dot(observation_dir, vector3_normalize(p - observation_pos));
        }
    }

    for (unsigned z = fourth_size; z <= size - fourth_size; ++z)
        memcpy(pixels + z * size, pixels + z * size + fourth_size * 2, fourth_size*sizeof(float));

    for (unsigned z = fourth_size; z <= size - fourth_size; ++z)
        memcpy(pixels + z * size + fourth_size * 3, pixels + z * size + fourth_size, fourth_size*sizeof(float));

    for (unsigned z = 0; z < fourth_size; ++z)
        memcpy(pixels + z * size + fourth_size + 1, pixels + size * (size - z - fourth_size * 2) + fourth_size + 1, half_size*sizeof(float));

    for (unsigned z = fourth_size * 3; z < size; ++z)
        memcpy(pixels + z * size + fourth_size + 1, pixels + size * (z - fourth_size * 2) + fourth_size + 1, half_size*sizeof(float));
}

static float apply_cosine_law_texture(float* pixels, unsigned size)
{
    const float half_size = (float)(size/2);
    const unsigned num_pixels = size*size;
    const Vector3 light_dir = {0, -1, 0};
    const float dome_radius = half_size;
    const Vector3 pos = vector3_zero;
    float total_intensity = 0;
    for (unsigned i = 0; i < num_pixels; ++i)
    {
        const float x = (float)(i % size) - half_size;
        const float z = (float)(i / size) - half_size;
        const float ysq = dome_radius*dome_radius - x*x - z*z;

        if (ysq < 0) {
            pixels[i] = 0;
            continue;
        }

        const float y = sqrtf(ysq);
        const Vector3 normal = {x, y, z};
        const float color = vector3_dot(vector3_normalize(normal), -light_dir);
        pixels[i] *= color;
        total_intensity += pixels[i];
    }
    return total_intensity;
}

static void create_multiplier_map_texture(float* pixels, unsigned size)
{
    create_distortion_compensation_texture(pixels, size);
    float total_intensity = apply_cosine_law_texture(pixels, size);
    float total_intensity_inv = 1.0f/total_intensity;
    Unused(total_intensity_inv);
    const unsigned num_pixels = size*size;
    for (unsigned i = 0; i < num_pixels; ++i)
    {
        pixels[i] *= total_intensity_inv;
    }
}

float* create_distortion_texture(Allocator* alloc, unsigned width)
{
    unsigned num_pixels = width*width;
    unsigned size = num_pixels*sizeof(float);
    float* pixels = (float*)alloc->alloc(size);
    memset(pixels, 0, size);
    create_multiplier_map_texture(pixels, width);
    return pixels;
}