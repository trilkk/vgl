#include "vgl_texture.hpp"

namespace vgl
{

const Texture* Texture::g_current_texture[Texture::MAX_TEXTURE_UNITS] =
{
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

unsigned Texture::g_active_texture_unit = 0u;

}

