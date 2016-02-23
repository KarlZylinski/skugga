enum struct PixelFormat
{
    R8G8B8A8_UINT_NORM
};

struct Image
{
    uint8* data;
    uint32 width;
    uint32 height;
    PixelFormat pixel_format;
};

namespace image
{

uint32 pixel_size(PixelFormat pf)
{
    switch (pf)
    {
        case PixelFormat::R8G8B8A8_UINT_NORM:
            return 4;
        default:
            return 0;
    }
}

uint32 calc_size(PixelFormat pf, uint32 size_x, uint32 size_y)
{
    return size_x * size_y * pixel_size(pf);
}

} // namespace image
