#pragma once

struct Allocator;

enum struct PixelFormat
{
    R8G8B8A8_UINT_NORM,
    R32G32B32A32_FLOAT
};

struct Image
{
    unsigned width;
    unsigned height;
    PixelFormat pixel_format;
    unsigned char* data;
};

namespace image
{

void init_data(Image* i, Allocator* alloc);
unsigned pixel_size(PixelFormat pf);
unsigned size(PixelFormat pf, unsigned size_x, unsigned size_y);
unsigned size(const Image& image);

} // namespace image
