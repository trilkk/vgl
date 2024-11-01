#ifndef VGL_EXTERN_BOOST_FILESYSTEM_HPP
#define VGL_EXTERN_BOOST_FILESYSTEM_HPP

/// \file boost::filesystem locating and reading functionality.
/// This file only makes sense when not building size-minimized. It's not header-only.

#if defined(USE_LD)

#include "vgl_optional.hpp"
#include "vgl_string_view.hpp"
#include "vgl_vector.hpp"

#include <filesystem>

namespace vgl
{

/// Wrapper for path.
///
/// Intentionally does not inherit as to not confuse external code.
class path
{
public:
    /// Storage type.
    using storage_type = std::filesystem::path;

public:
    /// RAII wrapper for a file descriptor.
    class FileDescriptor
    {
    private:
        /// Contained descriptor.
        FILE* m_fd = nullptr;

    private:
        /// Deleted copy constructor.
        FileDescriptor(const FileDescriptor&);
        /// Deleted copy operator.
        FileDescriptor& operator=(const FileDescriptor&);

    public:
        /// Constructor.
        ///
        /// \param fname Filename.
        /// \param mode Opening mode.
        explicit FileDescriptor(const char* fname, const char* mode);

        /// Destructor.
        ~FileDescriptor();

        /// Move constructor.
        ///
        /// \param other Source object.
        /// \return This object.
        FileDescriptor(FileDescriptor&& other) :
            m_fd(other.m_fd)
        {
            other.m_fd = nullptr;
        }

    public:
        /// Read one character from the file.
        ///
        /// \return Character read or nullopt.
        optional<char> readChar() const;

        /// Read one unsigned byte from the file.
        ///
        /// \return Unsigned byte read or nullopt.
        optional<uint8_t> readUnsigned() const;

        /// Write data to file.
        ///
        /// \param data Data to write.
        /// \param len Data size.
        /// \return Number of bytes written.
        size_t write(const void* data, size_t len);

    public:
        /// Conversion to bool.
        ///
        /// \return True if the file was opened successfully, false otherwise.
        operator bool() const
        {
            return static_cast<bool>(m_fd);
        }

        /// Move operator.
        ///
        /// \param other Source object.
        /// \return This object.
        FileDescriptor& operator=(FileDescriptor&& other)
        {
            m_fd = other.m_fd;
            other.m_fd = nullptr;
            return *this;
        }
    };

private:
    /// Internal path.
    storage_type m_data;

public:
    /// Default constructor.
    explicit path() = default;

    /// Constructor.
    ///
    /// Intentionally not explicit.
    ///
    /// \param op String representation of a path.
    path(const char* op) :
        m_data(op)
    {
    }

    /// Constructor.
    ///
    /// Intentionally not explicit.
    ///
    /// \param op String representation of a path.
    path(const string& op) :
        m_data(op.c_str())
    {
    }

    /// Base class copy constructor.
    ///
    /// \param op Boost path object.
    explicit path(const storage_type& op) :
        m_data(op)
    {
    }

    /// Base class move constructor.
    ///
    /// \param op Boost path object.
    explicit path(storage_type&& op) :
        m_data(move(op))
    {
    }

    /// Copy constructor.
    ///
    /// \param other Source object.
    /// \return This object.
    path(const path& other) :
        m_data(other.m_data)
    {
    }

    /// Move constructor.
    ///
    /// \param other Source object.
    /// \return This object.
    path(path&& other) :
        m_data(move(other.m_data))
    {
    }

public:
    /// Open a file using fopen.
    ///
    /// \param mode Opening mode.
    /// \return FILE handle or nullptr.
    FileDescriptor openFile(const char* mode) const
    {
        return FileDescriptor(getString().c_str(), mode);
    }

    /// Reads the file pointed by the path into a string.
    ///
    /// \return Vector containing file data.
    optional<string> readToString() const;
    /// Reads the file pointed by the path into a vector.
    ///
    /// \param contents File contents to write.
    /// \return Vector containing file data.
    optional<vector<uint8_t>> readToVector() const;

    /// Writes given data into a file.
    ///
    /// \return True if file was completely written.
    bool write(string_view contents) const;
    /// Writes given data into a file.
    ///
    /// \param contents File contents to write.
    /// \return True if file was completely written.
    bool write(const vector<uint8_t>& contents) const
    {
        return write(string_view(reinterpret_cast<const char*>(contents.data()), contents.size()));
    }

    /// Tells if the path is empty.
    ///
    /// \return True if empty, false otherwise.
    bool empty() const
    {
        return m_data.empty();
    }

    /// Tells if the path refers to an existing file.
    ///
    /// \return True if a file corresponding to the path exists, false otherwise.
    bool exists() const
    {
        return std::filesystem::exists(m_data);
    }

    /// Gets the string representation from the path.
    ///
    /// \return String representation.
    string getString() const
    {
        return m_data.string();
    }

    /// Replaces the extension from this path.
    ///
    /// \param op New extension.
    /// \return This object.
    path& replaceExtension(const path& op = path())
    {
        m_data.replace_extension(op.m_data);
        return *this;
    }

public:
    /// Copy operator.
    ///
    /// \param other Source object.
    /// \return This object.
    path& operator=(const path& other)
    {
        m_data = other.m_data;
        return *this;
    }

    /// Move operator.
    ///
    /// \param other Source object.
    /// \return This object.
    path& operator=(path&& other)
    {
        m_data = move(other.m_data);
        return *this;
    }

    /// Append operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Appended path.
    path operator/(const path& rhs) const
    {
        return path(m_data / rhs.m_data);
    }

    /// Append into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This object.
    path& operator/=(const path& rhs)
    {
        m_data /= rhs.m_data;
        return *this;
    }

public:
    /// Output to stream operator.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Output stream.
    friend std::ostream& operator<<(std::ostream& lhs, const path& rhs)
    {
        return lhs << rhs.m_data;
    }
};

/// String representation of a path.
///
/// \param op Path input.
/// \return String representation.
inline string to_string(const path& op)
{
    return op.getString();
}

/// Find a file, try several different default locations.
///
/// \param fname Base name to open.
/// \return File to open or an empty string.
path find_file(const path& fname);
/// Wrapper for finding a file.
///
/// \param fname Base name to open.
/// \return File to open or an empty string.
inline path find_file(const char* fname)
{
    return find_file(path(fname));
}
/// Wrapper for finding a file.
///
/// \param fname Base name to open.
/// \return File to open or an empty string.
inline path find_file(const string& fname)
{
    return find_file(path(fname));
}
/// Wrapper for finding a file.
///
/// \param fname Base name to open.
/// \return File to open or an empty string.
inline path find_file(string_view fname)
{
    return find_file(string(fname));
}

/// Read a file.
///
/// \param name File to read.
/// \return Contents of file as a string.
string read_file(const path& name);

/// Read a data file from disk.
///
/// Only basename is given.
/// The actual file is located from source or release folders.
///
/// \param fname File basename to look for.
/// \return File contents.
string read_file_locate(string_view fname);

}

#endif

#endif
