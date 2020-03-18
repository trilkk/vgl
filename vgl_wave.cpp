#include "vgl_wave.hpp"

#include <utility>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/wave/cpp_context.hpp>
#include <boost/wave/cpplexer/cpp_lex_iterator.hpp>

namespace fs = boost::filesystem;
using wave_token = boost::wave::cpplexer::lex_token<>;
using wave_cpplex_iterator = boost::wave::cpplexer::lex_iterator<wave_token>;
using wave_context = boost::wave::context<std::string::const_iterator, wave_cpplex_iterator>;

//######################################
// Local ###############################
//######################################

namespace
{

/// Find a file, try several different default locations.
///
/// \param fname Base name to open.
/// \return File to open or an empty string.
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

/// Read a file.
///
/// \param name File to open.
/// \return Contents of file as a string.
std::string read_file(const fs::path &name)
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
}

/// Split GLSL source to onward-passed preprocessor definitions and the rest.
///
/// \param source Source input.
/// \return Pair of strings, GLSL shader compiler preprocessor input and rest of the source.
std::pair<std::string, std::string> glsl_split(const std::string& source)
{
    std::vector<std::string> lines;

    boost::split(lines, source, boost::is_any_of("\n"));

    std::vector<std::string> glsl_list;
    std::vector<std::string> cpp_list;

    for(const auto& vv : lines)
    {
        std::string ii = boost::trim_copy(vv);

        if(boost::starts_with(ii, "#"))
        {
            ii = boost::trim_copy(ii.substr(1));

            if(boost::starts_with(ii, "define") ||
                    boost::starts_with(ii, "version"))
            {
                glsl_list.push_back(vv);
                continue;
            }
        }

        cpp_list.push_back(vv);
    }

    std::string glsl_ret = boost::algorithm::join(glsl_list, "\n");
    std::string cpp_ret = boost::algorithm::join(cpp_list, "\n");

    if(!glsl_ret.empty())
    {
        glsl_ret += "\n";
    }

    return std::make_pair(glsl_ret, cpp_ret);
}

/// Tidy up given source, remove all preprocess lines.
///
/// \prarm source Source input.
/// \return Source with preprocessor lines removed.
std::string glsl_tidy(const std::string& source)
{
    std::vector<std::string> lines;

    boost::split(lines, source, boost::is_any_of("\n"));

    std::vector<std::string> accepted;

    for(const auto& vv : lines)
    {
        std::string ii = boost::trim_copy(vv);

        if(!boost::starts_with(ii, "#"))
        {
            accepted.push_back(ii);
        }
    }

    std::string ret = boost::algorithm::join(accepted, "\n");

    return ret;
}

#if !defined(DNLOAD_GLESV2)

/// Line comment regex on a string.
///
/// \param bb String iterator.
/// \param ee String endpoint.
/// \return Iterator to end of match or original iterator if no match.
static std::string::const_iterator regex_line_comment(std::string::const_iterator bb,
        const std::string::const_iterator &ee)
{
    std::string::const_iterator ii = bb;

    if((ii == ee) || ('/' != *ii))
    {
        return bb;
    }
    ++ii;
    if((ii == ee) || ('/' != *ii))
    {
        return bb;
    }

    for(;;)
    {
        ++ii;
        if(ii == ee)
        {
            return ii;
        }
        if('\n' == *ii)
        {
            ++ii;
            return ii;
        }
    }
}

/// Block comment regex on a string.
///
/// \param bb String iterator.
/// \param ee String endpoint.
/// \return Iterator to end of match or original iterator if no match.
static std::string::const_iterator regex_block_comment(std::string::const_iterator bb,
        const std::string::const_iterator &ee)
{
    std::string::const_iterator ii = bb;
    bool allow_return = false;

    if((ii == ee) || ('/' != *ii))
    {
        return bb;
    }
    ++ii;
    if((ii == ee) || ('*' != *ii))
    {
        return bb;
    }

    for(;;)
    {
        ++ii;
        if(ii == ee)
        {
            return bb;
        }
        if(allow_return)
        {
            if('/' == *ii)
            {
                ++ii;
                return ii;
            }
            allow_return = false;
        }
        else if('*' == *ii)
        {
            allow_return = true;
        }
    }
}

/// Comment regex on a string.
///
/// \param bb String iterator.
/// \param ee String endpoint.
/// \return Iterator to end of match or original iterator if no match.
static std::string::const_iterator regex_comment(std::string::const_iterator bb,
        const std::string::const_iterator &ee)
{
    std::string::const_iterator ii = regex_line_comment(bb, ee);
    if(ii != bb)
    {
        return ii;
    }
    return regex_block_comment(bb, ee);
}

/// Regex for whitespace (any amount).
///
/// \param bb String iterator.
/// \param ee String endpoint.
/// \return Iterator at the end of whitespace.
static std::string::const_iterator regex_whitespace(std::string::const_iterator bb,
        const std::string::const_iterator ee)
{
    for(std::string::const_iterator ii = bb; (ii != ee); ++ii)
    {
        if(!isspace(static_cast<int>(*ii)))
        {
            return ii;
        }
    }
    return ee;
}

/// Regex for a word and any amount of following whitespace.
///
/// \param bb String iterator.
/// \param ee String endpoint.
/// \param word Word to match.
/// \return Iterator to end of match or original iterator if no match.
static std::string::const_iterator regex_word_whitespace(std::string::const_iterator bb,
        const std::string::const_iterator ee, std::string_view word)
{
    std::string::const_iterator ii = bb;
    unsigned jj = 0;

    while(word.length() > jj)
    {
        if(ii == ee)
        {
            return bb;
        }
        int lhs = tolower(static_cast<int>(*ii));
        int rhs = tolower(static_cast<int>(word[jj]));
        if(lhs != rhs)
        {
            return bb;
        }
        ++ii;
        ++jj;
    }
    return regex_whitespace(ii, ee);
}

/// Regex for precision words and any amount of following whitespace.
///
/// \param bb String iterator.
/// \param ee String endpoint.
/// \return Iterator to end of match or original iterator if no match.
static std::string::const_iterator regex_precision_whitespace(std::string::const_iterator bb,
        const std::string::const_iterator ee)
{
    std::string::const_iterator ii = regex_word_whitespace(bb, ee, "lowp");
    if(ii != bb)
    {
        return regex_whitespace(ii, ee);
    }
    ii = regex_word_whitespace(bb, ee, "mediump");
    if(ii != bb)
    {
        return regex_whitespace(ii, ee);
    }
    ii = regex_word_whitespace(bb, ee, "highp");
    if(ii != bb)
    {
        return regex_whitespace(ii, ee);
    }

    return bb;
}

/// Perform glsl2 regex on a string.
///
/// \param bb String iterator.
/// \param ee String endpoint.
/// \return Iterator to end of match or original iterator if no match.
static std::string::const_iterator regex_glesv2(std::string::const_iterator bb,
        const std::string::const_iterator ee)
{
    std::string::const_iterator ii = bb;

    // Try "precision" statement."
    std::string::const_iterator jj = regex_word_whitespace(ii, ee, "precision");
    if(jj != ii)
    {
        std::string::const_iterator kk = jj;
        jj = regex_precision_whitespace(kk, ee);
        if(jj != kk)
        {
            kk = jj;
            jj = regex_word_whitespace(kk, ee, "float");
            if(jj != kk)
            {
                kk = jj;
                jj = regex_word_whitespace(kk, ee, ";");
                if(jj != kk)
                {
                    return jj;
                }
            }
        }
    }

    // Try precision statement otherwise.
    jj = regex_precision_whitespace(ii, ee);
    if(jj != ii)
    {
        return jj;
    }

    return bb;
}

std::string convert_glesv2_gl(std::string_view op)
{
    std::string ret(op);
    std::string::const_iterator ii = ret.begin();
    std::string::const_iterator ee = ret.end();

    while(ii != ee)
    {
        std::string::const_iterator jj = regex_glesv2(ii, ee);
        if(jj != ii)
        {
            // TODO: Erase with iterators when C++11.
            std::string::difference_type erase_start = ii - ret.begin();
            std::string::difference_type erase_count = jj - ret.begin() - erase_start;
            ret = ret.erase(static_cast<size_t>(erase_start), static_cast<size_t>(erase_count));
            ii = ret.begin() + erase_start;
            ee = ret.end();
            continue;
        }
        jj = regex_comment(ii, ee);
        if(jj != ii)
        {
            ii = jj;
            continue;
        }
        ++ii;
    }

    return ret;
}

#endif

}

namespace vgl
{

std::string read_file_locate(std::string_view fname)
{
    fs::path real_path = find_file(fname);
    return read_file(real_path).data();
}

std::string wave_preprocess_glsl(std::string_view op)
{
    std::string input_source = read_file_locate(op);
#if !defined(DNLOAD_GLESV2)
    input_source = convert_glesv2_gl(input_source);
#endif

    // Split into GLSL preprocess code and the rest.
    std::pair<std::string, std::string> source = glsl_split(input_source);

    // Preprocess with wave.
    std::ostringstream preprocessed;
    wave_context ctx(cbegin(source.second), cend(source.second), "Boost::Wave GLSL;");
    ctx.add_macro_definition("USE_LD");
    for(const auto& vv : ctx)
    {
        preprocessed << vv.get_value();
    }

    return source.first + glsl_tidy(preprocessed.str());
}

}

