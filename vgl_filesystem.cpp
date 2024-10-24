#include "vgl_filesystem.hpp"

#include <iostream>

namespace vgl
{

FILE* path::fopen(const char* mode) const
{
    FILE* ret = ::fopen(getString().c_str(), mode);
    if(!ret)
    {
        std::cerr << "path::fopen(): could not open '" << to_string(*this) << "' using mode '" << mode << "'\n";
    }
    return ret;
}

optional<string> path::readToString() const
{
    std::ostringstream ret;

    {
        FILE* fd = fopen("rb");
        if(!fd)
        {
            return nullopt;
        }

        while(!feof(fd))
        {
            char cc = static_cast<char>(fgetc(fd));
            ret << cc;
        }

        fclose(fd);
    }

    return optional<string>(ret.str());
}

optional<vector<uint8_t>> path::readToVector() const
{
    vector<uint8_t> ret;

    {
        FILE* fd = fopen("rb");
        if(!fd)
        {
            return nullopt;
        }

        while(!feof(fd))
        {
            ret.push_back(static_cast<uint8_t>(fgetc(fd)));
        }

        fclose(fd);
    }

    return ret;
}

bool path::write(string_view contents) const
{
    FILE* fd = fopen("wb");
    if(!fd)
    {
        return false;
    }

    size_t bytes_written = fwrite(contents.data(), 1u, contents.length(), fd);
    bool ret = (bytes_written == contents.length());
    if(!ret)
    {
        std::cerr << "path::write(): could only write " << bytes_written << " out of " << contents.length() <<
            "bytes requested to '" << to_string(*this) << "'\n";
    }
    fclose(fd);
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

