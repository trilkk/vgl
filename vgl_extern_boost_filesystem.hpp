#ifndef VGL_EXTERN_BOOST_FILESYSTEM_HPP
#define VGL_EXTERN_BOOST_FILESYSTEM_HPP

/// \file boost::filesystem locating and reading functionality.
/// This file only makes sense when not building size-minimized. It's not header-only.

#if defined(USE_LD)

#include <string_view>

#include <boost/filesystem.hpp>

namespace vgl
{

/// Find a file, try several different default locations.
///
/// \param fname Base name to open.
/// \return File to open or an empty string.
boost::filesystem::path find_file(std::string_view fname);

/// Read a file.
///
/// \param name File to read.
/// \return Contents of file as a string.
std::string read_file(const boost::filesystem::path& name);

/// Read a data file from disk.
///
/// Only basename is given.
/// The actual file is located from source or release folders.
///
/// \param fname File basename to look for.
/// \return File contents.
std::string read_file_locate(std::string_view fname);

/// Write a file.
///
/// \param name File to write.
/// \param contents Contents to write.
void write_file(const boost::filesystem::path& name, std::string_view contents);

}

#endif

#endif
