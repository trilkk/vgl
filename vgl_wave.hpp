#ifndef VGL_WAVE_HPP
#define VGL_WAVE_HPP

/// \file boost::wave preprocessing functionality.
/// This file only makes sense when not building size-minimized. It's not header-only.

#include "vgl_config.hpp"

#if defined(VGL_USE_LD)

#include "vgl_string_view.hpp"

namespace vgl
{

/// Preprocess GLSL with wave.
///
/// \parmam op Source input.
/// \ŗeturn Preprocessed source.
string wave_preprocess_glsl(string_view op);

}

#endif

#endif
