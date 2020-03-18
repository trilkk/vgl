#ifndef VGL_WAVE_HPP
#define VGL_WAVE_HPP

/// \file boost::wave preprocessing functionality.
/// This file only makes sense when not building size-minimized. It's not header-only.

#if defined(USE_LD)

#include <string_view>

namespace vgl
{

/// Read a data file from disk.
///
/// Only basename is given.
/// The actual file is located from source or release folders.
///
/// \param fname File basename to look for.
/// \return File contents.
std::string read_file_locate(std::string_view fname);

/// Preprocess GLSL with wave.
///
/// \parmam op Source input.
/// \ŗeturn Preprocessed source.
std::string wave_preprocess_glsl(std::string_view op);

}

#endif

#endif
