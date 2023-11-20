#ifndef VGL_EXTERN_FREEETYPE_HPP
#define VGL_EXTERN_FREEETYPE_HPP

/// \file
/// \brief External include: Freetype

#if defined(USE_LD)

#include "ft2build.h"
#include FT_FREETYPE_H

/// \cond
#if !defined(dnload_FT_Get_Char_Index)
#define dnload_FT_Get_Char_Index FT_Get_Char_Index
#endif
#if !defined(dnload_FT_Init_FreeType)
#define dnload_FT_Init_FreeType FT_Init_FreeType
#endif
#if !defined(dnload_FT_Load_Glyph)
#define dnload_FT_Load_Glyph FT_Load_Glyph
#endif
#if !defined(dnload_FT_New_Face)
#define dnload_FT_New_Face FT_New_Face
#endif
#if !defined(dnload_FT_Render_Glyph)
#define dnload_FT_Render_Glyph FT_Render_Glyph
#endif
#if !defined(dnload_FT_Set_Pixel_Sizes)
#define dnload_FT_Set_Pixel_Sizes FT_Set_Pixel_Sizes
#endif
/// \endcond

#endif

#endif
