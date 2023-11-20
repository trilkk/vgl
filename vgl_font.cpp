#include "vgl_font.hpp"

namespace vgl
{

FT_Library Font::g_freetype_library = nullptr;

#if defined(USE_LD)
unsigned Font::g_freetype_count = 0;
#endif

}

