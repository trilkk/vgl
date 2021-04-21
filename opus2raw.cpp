#include <iostream>

#include <boost/exception/diagnostic_information.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
namespace fs = boost::filesystem;
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

#include "vgl_optional.hpp"
#include "vgl_opus.hpp"
#include "vgl_string_view.hpp"
#include "vgl_vector.hpp"

static const char *usage = ""
"Usage: opus2raw <options> [input-file]\n"
"Reads an ogg opus file and writes a raw opus file without the ogg container.\n"
"Decoder opus settings are printed to stdout as opposed to being saved.\n";

/// Read file contents.
///
/// \param op Filename.
/// \return File contents.
vgl::vector<uint8_t> read_file(const fs::path& op)
{
    vgl::vector<uint8_t> ret;

    {
        FILE* fd = fopen(op.string().c_str(), "rb");
        if(!fd)
        {
            boost::throw_exception(std::runtime_error("could not open '" + op.string() + "' for reading"));
        }

        while(!feof(fd))
        {
            ret.push_back(static_cast<uint8_t>(fgetc(fd)));
        }

        fclose(fd);
    }

    return ret;
}

/// Write binary file.
///
/// \param data Data to write.
/// \param fname Filename to write.
void write_file(const vgl::vector<uint8_t>& data, const fs::path& op)
{
    FILE* fd = fopen(op.string().c_str(), "wb");
    if(!fd)
    {
        boost::throw_exception(std::runtime_error("could not open '" + op.string() + "' for writing"));
    }

    for(uint8_t vv : data)
    {
        fputc(static_cast<int>(vv), fd);
    }

    fclose(fd);
}

/// Read ogg opus file and store raw opus file.
///
/// \param infile Input file.
/// \param outfile Output file.
void opus2raw(const fs::path& infile, const fs::path& outfile)
{
    vgl::vector<uint8_t> input_data = read_file(infile);
    vgl::vector<uint8_t> output_data;
    vgl::detail::OggStream stream(input_data.data(), input_data.size());

    ogg_packet packet;
    stream.readPacket(packet);

    // First packet - read header.
    if((packet.bytes < 19) ||
            (vgl::string_view(reinterpret_cast<const char*>(packet.packet), 8) != vgl::string_view("OpusHead")) ||
            (packet.packet[8] != 1))
    {
        BOOST_THROW_EXCEPTION(std::runtime_error("first packet is not opus header"));
    }
    unsigned channels = static_cast<unsigned>(packet.packet[9]);
    unsigned skip_bytes = static_cast<unsigned>(packet.packet[10]) + (static_cast<unsigned>(packet.packet[11]) << 8);

    // Discard comment packet.
    stream.readPacket(packet);

    // Read until done.
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

    write_file(output_data, outfile);

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
        vgl::optional<fs::path> input_file;
        vgl::optional<fs::path> output_file;

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
                input_file = fs::path(vmap["input-file"].as<std::string>());
            }
            if(vmap.count("output-file"))
            {
                output_file = fs::path(vmap["output-file"].as<std::string>());
            }
        }
        else
        {
            std::cout << usage << desc << std::endl;
            return 0;
        }

        if(!input_file)
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("input file not specified"));
        }
        if(!output_file)
        {
            output_file = *input_file;
            output_file->replace_extension(".opus.raw");
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

