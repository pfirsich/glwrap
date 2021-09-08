#include "glw/imageformat.hpp"

namespace glw {
bool hasDepth(ImageFormat format)
{
    return format == ImageFormat::Depth || format == ImageFormat::Depth16
        || format == ImageFormat::Depth24 || format == ImageFormat::Depth32
        || format == ImageFormat::Depth32F || format == ImageFormat::DepthStencil
        || format == ImageFormat::Depth24Stencil8 || format == ImageFormat::Depth32FStencil8;
}

bool hasStencil(ImageFormat format)
{
    return format == ImageFormat::Stencil || format == ImageFormat::Stencil1
        || format == ImageFormat::Stencil4 || format == ImageFormat::Stencil8
        || format == ImageFormat::Stencil16 || format == ImageFormat::DepthStencil
        || format == ImageFormat::Depth24Stencil8 || format == ImageFormat::Depth32FStencil8;
}
}
