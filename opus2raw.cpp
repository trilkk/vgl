#include <iostream>

#include <boost/exception/diagnostic_information.hpp>
#include <boost/program_options.hpp>
namespace po = boost::program_options;

// Do not include dnload.h, since this is not a minifiable binary.
// Instead, just rename the relevant calls.
#define dnload_free free
#define dnload_ogg_page_serialno ogg_page_serialno
#define dnload_ogg_stream_init ogg_stream_init
#define dnload_ogg_stream_packetout ogg_stream_packetout
#define dnload_ogg_stream_pagein ogg_stream_pagein
#define dnload_ogg_sync_buffer ogg_sync_buffer
#define dnload_ogg_sync_init ogg_sync_init
#define dnload_ogg_sync_pageout ogg_sync_pageout
#define dnload_ogg_sync_wrote ogg_sync_wrote
#define dnload_realloc realloc

// Disable some features.
#define VGL_DISABLE_OPUS

#include "vgl_filesystem.hpp"
#include "vgl_opus.hpp"

using vgl::optional;
using vgl::path;
using vgl::runtime_error;
using vgl::string;
using vgl::string_view;
using vgl::to_string;
using vgl::vector;

static const char *usage = ""
"Usage: opus2raw <options> [input-file]\n"
"Reads an ogg opus file and writes a raw opus file without the ogg container.\n"
"Decoder opus settings are printed to stdout as opposed to being saved.\n";

/// Read ogg opus file and store raw opus file.
///
/// \param infile Input file.
/// \param outfile Output file.
void opus2raw(const path& infile, const path& outfile)
{
    optional<vector<uint8_t>> input_data = infile.readToVector();
    if(!input_data)
    {
        VGL_THROW_RUNTIME_ERROR("opus2raw(): failure reading '" + to_string(infile) + "'");
    }
    if(input_data->empty())
    {
        VGL_THROW_RUNTIME_ERROR("opus2raw(): input file '" + to_string(infile) + "' contains no data");
    }

    vgl::detail::OggStream stream(input_data->data(), input_data->size());

    ogg_packet packet;
    stream.readPacket(packet);

    // First packet - read header.
    if((packet.bytes < 19) ||
            (string_view(reinterpret_cast<const char*>(packet.packet), 8) != vgl::string_view("OpusHead")) ||
            (packet.packet[8] != 1))
    {
        VGL_THROW_RUNTIME_ERROR("opus2raw(): first packet in '" + to_string(infile) + "' is not an opus header");
    }
    unsigned channels = static_cast<unsigned>(packet.packet[9]);
    unsigned skip_bytes = static_cast<unsigned>(packet.packet[10]) + (static_cast<unsigned>(packet.packet[11]) << 8);

    // Discard comment packet.
    stream.readPacket(packet);

    // Read until done.
    vector<uint8_t> output_data;
    while(stream.readPacket(packet))
    {
        uint16_t bytes = static_cast<int16_t>(packet.bytes);
        uint8_t bh = static_cast<uint8_t>(bytes >> 8);
        uint8_t bl = static_cast<uint8_t>(bytes & 0xFF);
        output_data.push_back(bl);
        output_data.push_back(bh);

        for(uint16_t ii = 0; (ii < bytes); ++ii)
        {
            output_data.push_back(packet.packet[ii]);
        }
    }
    outfile.write(output_data);

    std::cout << "   Channels: " << channels << "\n Skip bytes: " << skip_bytes << std::endl;
}

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
            ("input-file", po::value<std::string>(), "Input file.")
            ("output-file,o", po::value<std::string>(), "Name of output file to write.\n(default: generated from input file name)");

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
                input_file = path(vmap["input-file"].as<string>());
            }
            if(vmap.count("output-file"))
            {
                output_file = path(vmap["output-file"].as<string>());
            }
        }
        else
        {
            std::cout << usage << desc << std::endl;
            return 0;
        }

        if(!input_file)
        {
            VGL_THROW_RUNTIME_ERROR("input file not specified");
        }
        if(!output_file)
        {
            output_file = *input_file;
            output_file->replaceExtension(".opus.raw");
        }

        opus2raw(*input_file, *output_file);
    }
    catch(const boost::exception &err)
    {
        std::cerr << boost::diagnostic_information(err);
        return 1;
    }

    return 0;
}

