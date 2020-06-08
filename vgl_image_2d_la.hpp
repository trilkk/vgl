#ifndef VGL_IMAGE_2D_LA_HPP
#define VGL_IMAGE_2D_LA_HPP

#include "vgl_image_2d.hpp"
#include "vgl_unique_ptr.hpp"
#include "vgl_vec2.hpp"

namespace vgl
{

/// 2-dimensional luminance-alpha image.
class Image2DLA : public Image2D
{
public:
    /// Constructor.
    ///
    /// \param width Image width.
    /// \param height Image height.
    explicit Image2DLA(unsigned width, unsigned height) :
        Image2D(width, height, 2)
    {
    }

public:
    /// Set pixel value wrapper.
    ///
    /// \param px X coordinate.
    /// \param py Y coordinate.
    /// \param luminance Luminance component.
    /// \param alpha Alpha component.
    constexpr void setPixel(unsigned px, unsigned py, float luminance, float alpha)
    {
        setValue(px, py, 0, luminance);
        setValue(px, py, 1, alpha);
    }
    /// Set pixel value wrapper.
    ///
    /// \param px X coordinate.
    /// \param py Y coordinate.
    /// \param col 2-component vector as color.
    constexpr void setPixel(unsigned px, unsigned py, const vec2& col)
    {
        setPixel(px, py, col.x(), col.y());
    }
};

/// Image2DLA unique pointer type.
using Image2DLAUptr = unique_ptr<Image2DLA>;

}

#endif
