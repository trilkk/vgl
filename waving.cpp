#include "vgl_filesystem.hpp"
#include "vgl_wave.hpp"

using vgl::optional;
using vgl::path;
using vgl::runtime_error;
using vgl::string;
using vgl::string_view;
using vgl::to_string;
using vgl::vector;

#include <iostream>

#include <boost/exception/diagnostic_information.hpp>
#include <boost/program_options.hpp>
namespace po = boost::program_options;

static const char *usage = ""
"Usage: waving [input-file]\n"
"Reads a glsl file and preprocesses it to standard output.\n";

/// Main function.
///
/// \param argc Argument count.
/// \param argv Arguments.
/// \return Program return code.
int main(int argc, char **argv)
{
    try
    {
        optional<path> input_file;
        optional<path> output_file;

        po::options_description desc("Options");
        desc.add_options()
            ("help,h", "Print help text.")
            ("input-file", po::value<std::string>(), "Input file.");

        if(argc > 0)
        {
            po::positional_options_description pdesc;
            pdesc.add("input-file", -1);

            po::variables_map vmap;
            po::store(po::command_line_parser(argc, argv).options(desc).positional(pdesc).run(), vmap);
            po::notify(vmap);

            if(vmap.count("help"))
            {
                std::cout << usage << desc << std::endl;
                return 0;
            }
            if(vmap.count("input-file"))
            {
                input_file = path(vmap["input-file"].as<std::string>().c_str());
            }
        }
        else
        {
            std::cout << usage << desc << std::endl;
            return 0;
        }

        if(input_file)
        {
            vector<string> glsl_vector;
            glsl_vector.push_back("---- Preprocessed GLSL begin ----\n");
            glsl_vector.push_back(vgl::wave_preprocess_glsl(input_file->getString()));
            glsl_vector.push_back("---- Preprocessed GLSL end ----\n");
            for(const auto& vv : glsl_vector)
            {
                std::cout << vv;
            }
        }
        else
        {
            vector<string> number_vector;
            number_vector.emplace_back("Input file not specified, testing to_string():\nint8_t number: ");
            number_vector.emplace_back(to_string(static_cast<int8_t>(0x80)));
            number_vector.emplace_back("\nuint8_t number: ");
            number_vector.emplace_back(to_string(static_cast<uint8_t>(0xFF)));
            number_vector.emplace_back("\nint16_t number: ");
            number_vector.emplace_back(to_string(static_cast<int16_t>(0x8000)));
            number_vector.emplace_back("\nuint16_t number: ");
            number_vector.emplace_back(to_string(static_cast<uint16_t>(0xFFFF)));
            number_vector.emplace_back("\nint32_t number: ");
            number_vector.emplace_back(to_string(static_cast<int32_t>(0x80000000)));
            number_vector.emplace_back("\nuint32_t number: ");
            number_vector.emplace_back(to_string(static_cast<uint32_t>(0xFFFFFFFF)));
            number_vector.emplace_back("\nint64_t number: ");
            number_vector.emplace_back(to_string(static_cast<int64_t>(0x8000000000000000)));
            number_vector.emplace_back("\nuint64_t number: ");
            number_vector.emplace_back(to_string(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF)));
            number_vector.emplace_back("\npointer: ");
            number_vector.emplace_back(to_string(reinterpret_cast<void*>(0xDEADBEEF)));
            for(const auto& vv : number_vector)
            {
                std::cout << vv;
            }
            std::cout << std::endl;
        }
    }
    catch(const boost::exception &err)
    {
        std::cerr << boost::diagnostic_information(err);
        return 1;
    }

    return 0;
}

