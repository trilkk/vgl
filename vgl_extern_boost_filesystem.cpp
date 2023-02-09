#include "vgl_extern_boost_filesystem.hpp"

#include <fstream>
#include <sstream>

#include <boost/throw_exception.hpp>

namespace fs = boost::filesystem;

namespace vgl
{

fs::path find_file(std::string_view fname)
{
    fs::path attempt = fname.data();
    if(fs::exists(attempt))
    {
        return attempt;
    }

    attempt = fs::path("..") / fs::path(fname.data());
    if(fs::exists(attempt))
    {
        return attempt;
    }

    const fs::path SRC_PATH("src");
    attempt = SRC_PATH / fs::path(fname.data());
    if(fs::exists(attempt))
    {
        return attempt;
    }

    attempt = fs::path("..") / SRC_PATH / fs::path(fname.data());
    if(fs::exists(attempt))
    {
        return attempt;
    }

    const fs::path REL_PATH("rel");
    attempt = REL_PATH / fs::path(fname.data());
    if(fs::exists(attempt))
    {
        return attempt;
    }

    attempt = fs::path("..") / REL_PATH / fs::path(fname.data());
    if(fs::exists(attempt))
    {
        return attempt;
    }

    return fs::path();
}

std::string read_file(const fs::path& name)
{
    std::ostringstream ret;
    std::ifstream fd(name.string());

    for(;;)
    {
        char cc;
        fd.get(cc);

        if(fd.eof())
        {
            return ret.str();
        }

        ret << cc;
    }

    return ret.str();
}

std::string read_file_locate(std::string_view fname)
{
    fs::path real_path = find_file(fname);
    if(real_path.empty())
    {
        BOOST_THROW_EXCEPTION(std::runtime_error("read_file_locate(): '" + std::string(fname) + "' not found"));
    }
    return read_file(real_path).data();
}

void write_file(const boost::filesystem::path& name, std::string_view contents)
{
    std::ofstream fd(name.string());
    fd << contents;
}

}

