#include "vgl_filesystem.hpp"

#include "vgl_assert.hpp"

#include <iostream>

namespace vgl
{

path::FileDescriptor::FileDescriptor(const char* fname, const char* mode) :
    m_fd(fopen(fname, mode))
{
    if(!m_fd)
    {
        std::cerr << "path::fopen(): could not open '" << to_string(*this) << "' using mode '" << mode << "'\n";
    }
}

path::FileDescriptor::~FileDescriptor()
{
    if(m_fd)
    {
        fclose(m_fd);
    }
}

optional<char> path::FileDescriptor::readChar() const
{
    VGL_ASSERT(m_fd);
    char cc = static_cast<char>(fgetc(m_fd));
    if(feof(m_fd))
    {
        return nullopt;
    }
    return cc;
}

optional<uint8_t> path::FileDescriptor::readUnsigned() const
{
    VGL_ASSERT(m_fd);
    uint8_t cc = static_cast<uint8_t>(fgetc(m_fd));
    if(feof(m_fd))
    {
        return nullopt;
    }
    return cc;
}

size_t path::FileDescriptor::write(const void* data, size_t len)
{
    VGL_ASSERT(m_fd);
    return fwrite(data, 1u, len, m_fd);
}

optional<string> path::readToString() const
{
    auto fd = openFile("rb");
    if(!fd)
    {
        return nullopt;
    }

    vector<char> ret;
    for(;;)
    {
        optional<char> cc = fd.readChar();
        if(!cc)
        {
            break;
        }
        ret.push_back(*cc);
    }

    return string(ret.data(), ret.size());
}

optional<vector<uint8_t>> path::readToVector() const
{
    auto fd = openFile("rb");
    if(!fd)
    {
        return nullopt;
    }

    vector<uint8_t> ret;
    for(;;)
    {
        optional<uint8_t> cc = fd.readUnsigned();
        if(!cc)
        {
            break;
        }
        ret.push_back(*cc);
    }

    return ret;
}

bool path::write(string_view contents) const
{
    auto fd = openFile("wb");
    if(!fd)
    {
        return false;
    }

    size_t bytes_written = fd.write(contents.data(), contents.length());
    bool ret = (bytes_written == contents.length());
    if(!ret)
    {
        std::cerr << "path::write(): could only write " << bytes_written << " out of " << contents.length() <<
            "bytes requested to '" << to_string(*this) << "'\n";
    }
    return ret;
}

path find_file(const path& fname)
{
    if(fname.exists())
    {
        return fname;
    }

    {
        path attempt = path("..") / fname;
        if(attempt.exists())
        {
            return attempt;
        }
    }

    {
        path attempt = path("src") / fname;
        if(attempt.exists())
        {
            return attempt;
        }

        attempt = path("..") / attempt;
        if(attempt.exists())
        {
            return attempt;
        }
    }

    {
        path attempt = path("rel") / fname;
        if(attempt.exists())
        {
            return attempt;
        }

        attempt = path("..") / attempt;
        if(attempt.exists())
        {
            return attempt;
        }
    }

    return path();
}

string read_file_locate(string_view fname)
{
    path real_path = find_file(fname);
    if(real_path.empty())
    {
        VGL_THROW_RUNTIME_ERROR("read_file_locate(): '" + string(fname) + "' not found");
    }
    optional<string> contents = real_path.readToString();
    if(!contents)
    {
        VGL_THROW_RUNTIME_ERROR("read_file_locate(): failure reading '" + string(fname) + "'");
    }
    return *contents;
}

}

