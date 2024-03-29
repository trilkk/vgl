#ifndef VGL_CSG_FILE
#define VGL_CSG_FILE

#if defined(USE_LD)

#include "vgl_extern_boost_filesystem.hpp"

namespace vgl
{

/// CSG header file generator and loader.
class CsgFile
{
private:
    /// Base filename.
    std::string m_basename;

    /// Full filename path.
    boost::filesystem::path m_filename;

    /// Contents.
    std::string m_contents;

public:
    /// Constructor.
    ///
    /// \param filename Basename of the file to read.
    explicit CsgFile(std::string_view filename);

    /// Update the file if it's different from given input.
    ///
    /// \param data Input data to compare to file contents.
    /// \param count Number of data elements.
    /// \return Size of data array written to the header. Zero if header not updated.
    unsigned update(const int16_t* data, unsigned count);

    /// Accessor.
    ///
    /// \return Filename.
    const boost::filesystem::path& getFilename() const
    {
        return m_filename;
    }

    /// Accessor.
    ///
    /// \return Truth value corresponding to the CSG file being valid.
    bool isValid() const
    {
        return !m_filename.empty();
    }

public:
    /// Conversion to bool.
    ///
    /// \return Truth value corresponding to the CSG file being valid.
    operator bool() const
    {
        return isValid();
    }

private:
    /// Generate contents corresponding to a data block.
    ///
    /// \param filename Base filename.
    /// \param data Input data.
    /// \param count Number of data elements.
    /// \return Header data generated from the data.
    static std::string generate_contents(std::string_view filename, const int16_t* data, unsigned count);
};

}

#endif

#endif
