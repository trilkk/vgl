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
string headerify(string_view op)
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

CsgFile::CsgFile(string_view filename) :
    m_basename(filename),
    m_filename(find_file(filename))
{
    if(!isValid())
    {
        std::cerr << "CsgFile::CsgFile(): '" << m_basename << "' not found" << std::endl;
    }
    m_contents = m_filename.readToString().value_or(string());
}

unsigned CsgFile::update(const int16_t* data, unsigned count)
{
    if(!data || (count <= 0))
    {
        std::cerr << "CsgFile::update(): illegal input data: " << data << " ; " << count << std::endl;
        return 0;
    }
    string cmp_contents = generate_contents(m_basename, data, count);
    if(cmp_contents == m_contents)
    {
        return 0;
    }
    m_filename.write(cmp_contents);
    return static_cast<unsigned>(sizeof(*data) * count);
}

string CsgFile::generate_contents(string_view filename, const int16_t* data, unsigned count)
{
    const unsigned LINE_LEN = 78;

    std::ostringstream sstr;
    string fname = headerify(filename);
    string_view indent = "    ";

    sstr << "#ifndef __" << fname << "__\n#define __" << fname << "__\n\nconst unsigned g_" << fname <<
        "_size = " << count << ";\n\nconst int16_t g_" << fname << "[] =\n{\n";

    std::string cumulative;
    for(unsigned ii = 0; (ii < count); ++ii)
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

    // Some platforms require the array size to be aligned to 4 bytes.
    if(count & 1)
    {
        sstr << "\n#if !defined(__x86_64__) && !defined(__i386__)\n" << indent << "0,\n#endif";
    }

    sstr << "\n};\n\n#endif";
    return sstr.str();
}

}

