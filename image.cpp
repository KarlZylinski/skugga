#include "image.h"

namespace image
{

unsigned pixel_size(PixelFormat pf)
{
    switch (pf)
    {
        case PixelFormat::R8G8B8A8_UINT_NORM:
            return 4;
        case PixelFormat::R32G32B32A32_FLOAT:
            return 16;
        default:
            Error("Unknown pixel format."); return 0;
    }
}

unsigned size(PixelFormat pf, unsigned size_x, unsigned size_y)
{
    return size_x * size_y * pixel_size(pf);
}

} // namespace image
