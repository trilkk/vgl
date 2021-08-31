#include "vgl_csg_file.hpp"

#include <iostream>
#include <sstream>

#include <boost/throw_exception.hpp>

namespace vgl
{

namespace
{

/// Convert a string to a header-friendly one.
///
/// \param op Input string.
/// \return Header-friendly string.
std::string headerify(std::string_view op)
{
    std::ostringstream sstr;
    for(const auto cc : op)
    {
        switch(cc)
        {
        case ' ':
        case '.':
        case '-':
            sstr << static_cast<char>('_');
            break;

        default:
            sstr << cc;
            break;
        }
    }
    return sstr.str();
}

}

CsgFile::CsgFile(std::string_view filename) :
    m_basename(filename),
    m_filename(find_file(m_basename))
{
    if(!isValid())
    {
        std::cerr << "CsgFile(): '" << m_basename << "' not found" << std::endl;
    }
    m_contents = read_file(m_filename);
}

bool CsgFile::update(const int16_t* data, size_t count)
{
    std::string cmp_contents = generate_contents(m_basename, data, count);
    if(cmp_contents == m_contents)
    {
        return false;
    }
    write_file(m_filename, cmp_contents);
    return true;
}

std::string CsgFile::generate_contents(std::string_view filename, const int16_t* data, size_t count)
{
    const size_t LINE_LEN = 78;

    std::ostringstream sstr;
    std::string fname = headerify(filename);
    std::string_view indent = "    ";

    sstr << "#ifndef __" << fname << "__\n#define __" << fname << "__\n\nconst int16_t g_" << fname << "[] =\n{\n";

    std::string cumulative;
    for(size_t ii = 0; (ii < count); ++ii)
    {
    std::string increment = std::to_string(data[ii]) + ",";
        std::string prospective = cumulative;
        if(prospective.length() > 0)
        {
            prospective += " ";
        }
        prospective += increment;

        // Keep updating the cumulative line unless it's too long.
        if((prospective.length() + indent.length()) <= LINE_LEN)
        {
            cumulative = prospective;
            continue;
        }

        sstr << indent << cumulative << "\n";
        cumulative = increment;
    }
    if(cumulative.length() > 0)
    {
        sstr << indent << cumulative;
    }

    sstr << "\n};\n\n#endif";
    return sstr.str();
}

}

