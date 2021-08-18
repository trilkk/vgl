#ifndef VGL_WAVE_HPP
#define VGL_WAVE_HPP

/// \file boost::wave preprocessing functionality.
/// This file only makes sense when not building size-minimized. It's not header-only.

#if defined(USE_LD)

#include "vgl_path.hpp"

namespace vgl
{

/// Preprocess GLSL with wave.
///
/// \parmam op Source input.
/// \ŗeturn Preprocessed source.
std::string wave_preprocess_glsl(std::string_view op);

}

#endif

#endif
