#pragma once

enum struct PixelFormat
{
    R8G8B8A8_UINT_NORM,
    R32G32B32A32_FLOAT
};

struct Image
{
    unsigned char* data;
    unsigned width;
    unsigned height;
    PixelFormat pixel_format;
};

namespace image
{

unsigned pixel_size(PixelFormat pf);
unsigned size(PixelFormat pf, unsigned size_x, unsigned size_y);

} // namespace image
