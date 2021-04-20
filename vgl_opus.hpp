#ifndef VGL_OPUS_HPP
#define VGL_OPUS_HPP

#include "vgl_realloc.hpp"

#include "ogg/ogg.h"
#include "opus.h"

namespace vgl
{

/// Abstraction for ogg reading.
class OggReader
{
private:
    /// Input data.
    const uint8_t* m_input;

    /// Input data size.
    unsigned m_size;

    /// Input data position.
    unsigned m_pos = 0;

    /// Ogg synchronization state.
    ogg_sync_state m_sync;

public:
    /// Constructor.
    ///
    /// \param input Input data.
    /// \param size Input data size.
    explicit OggReader(const void* input, unsigned size) :
        m_input(reinterpret_cast<const uint8_t*>(input)),
        m_size(size)
    {
        ogg_sync_init(&m_sync);
    }

    /// Destructor.
    ~OggReader()
    {
#if defined(USE_LD)
        ogg_sync_clear(&m_sync);
#endif
    }

public:
    /// Read data into a page.
    ///
    /// \return If there was no data to read, false.
    bool read(ogg_page& page)
    {
        static const unsigned BLOCK_SIZE = 8192;

        for(;;)
        {
            if (ogg_sync_pageout(&m_sync, &page) == 1)
            {
                std::cout << "got page: " << page.header_len << " ;; " << page.body_len << " ;; " <<
                    ogg_page_serialno(&page) << std::endl;
                return true;
            }

            // Abort at end of input.
            if(m_pos >= m_size)
            {
                return false;
            }

            unsigned increment = min(m_size - m_pos, BLOCK_SIZE);

            std::cout << "ogg_sync_buffer with " << increment << std::endl;

            char* buffer = ogg_sync_buffer(&m_sync, static_cast<long>(increment));
            detail::internal_memcpy(buffer, m_input + m_pos, increment);
            m_pos += increment;

            std::cout << "ogg_sync_wrote with " << increment << std::endl;

            int err = ogg_sync_wrote(&m_sync, static_cast<long>(increment));
#if defined(USE_LD)
            if(err)
            {
                BOOST_THROW_EXCEPTION(std::runtime_error("ogg_sync_wrote() failed: " + std::to_string(err)));
            }
#else
            (void)err;
#endif
        }
    }
};

/// Ogg stream abstraction.
class OggStream
{
private:
    /// Reader.
    OggReader m_reader;

    /// Stream state.
    ogg_stream_state m_stream_state;

public:
    /// Constructor.
    /// \param input Input data.
    /// \param size Input data size.
    explicit OggStream(void* input, unsigned size) :
        m_reader(input, size)
    {
        ogg_page page;
        {
            bool err = m_reader.read(page);
#if defined(USE_LD)
            if(!err)
            {
                BOOST_THROW_EXCEPTION(std::runtime_error("OggStream input did not contain even one page"));
            }
#else
            (void)err;
#endif
        }

        // First page must correspond to the opus stream.
        int serial = ogg_page_serialno(&page);
#if defined(USE_LD)
        {
            int err = ogg_page_bos(&page);
            if(!err)
            {
                BOOST_THROW_EXCEPTION(std::runtime_error("ogg_page_bos(): first page was not beginning of stream"));
            }
        }
#endif

        std::cout << "ogg page serial was " << serial << std::endl;

        {
            int err = ogg_stream_init(&m_stream_state, serial);
#if defined(USE_LD)
            if(err)
            {
                BOOST_THROW_EXCEPTION(std::runtime_error("ogg_stream_init() failed: " + std::to_string(err)));
            }
#else
            (void)err;
#endif
        }

        submitPage(page);
    }

    /// Destructor.
    ~OggStream()
    {
#if defined(USE_LD)
        int err = ogg_stream_clear(&m_stream_state);
        if(err)
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("ogg_stream_clear() returned nonzero value: " + std::to_string(err)));
        }
#endif
    }

public:
    /// Read a packet from the stream.
    /// \param op Output packet.
    /// \return True if there still was a packet to read, false otherwise.
    bool readPacket(ogg_packet& op)
    {
        for(;;)
        {
            int err = ogg_stream_packetout(&m_stream_state, &op);
            // Not enough data -> try to submit more packets.
            if(!err)
            {
                ogg_page page;
                if(!m_reader.read(page))
                {
                    return false;
                }
                submitPage(page);
                continue;
            }
#if defined(USE_LD)
            if(err < 0)
            {
                BOOST_THROW_EXCEPTION(std::runtime_error("ogg_stream_packetout() error: " + std::to_string(err)));
            }
#endif
            VGL_ASSERT(err == 1);
            return true;
        }
    }

private:
    /// Submit a page to the stream.
    /// \param page Page to submit.
    void submitPage(ogg_page& page)
    {
        int err = ogg_stream_pagein(&m_stream_state, &page);
#if defined(USE_LD)
        if(err)
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("ogg_stream_pagein() packet submission error: " + std::to_string(err)));
        }
#else
        (void)err;
#endif
    }
};

/// Read opus data from memory.
///
/// \param input Input data.
/// \param size Input data size.
/// \return Data read as samples.
vector<float> opus_read_memory(void* input, unsigned size, int channels)
{
    static const unsigned OPUS_MAX_PACKET_SIZE_48000 = 5760;

    int err;
    OpusDecoder* decoder = opus_decoder_create(48000, channels, &err);
#if defined(USE_LD)
    if(err != OPUS_OK)
    {
        BOOST_THROW_EXCEPTION(std::runtime_error("opus_decoder_create() failed: " + std::to_string(err)));
    }
#endif

    OggStream stream(input, size);
    vector<float> ret;
    {
        unsigned outpos = 0;
        for(;;)
        {
            ogg_packet packet;
            if(!stream.readPacket(packet))
            {
                break;
            }

            // Must have size for at least one opus packet decoded.
            ret.resize(outpos + OPUS_MAX_PACKET_SIZE_48000);

            std::cout << "decoding packet: " << packet.bytes << " ;; " << packet.packetno << " ;; " << packet.b_o_s <<
                " ;; " << packet.e_o_s << std::endl;
            //sleep(1);

            err = opus_decode_float(decoder, packet.packet, static_cast<opus_int32>(packet.bytes), ret.data() + outpos,
                    static_cast<int>(OPUS_MAX_PACKET_SIZE_48000), 0);
#if defined(USE_LD)
            if(err == OPUS_INVALID_PACKET)
            {
                std::cout << "invalid packet\n";
            }
            else if(err <= 0)
            {
                BOOST_THROW_EXCEPTION(std::runtime_error("opus_decode_float() error: " + std::to_string(err)));
            }
            else
#endif
            {
                std::cout << "decoded " << err << " samples" << std::endl;
                outpos += err;
            }
        }
        ret.resize(outpos);
    }

#if defined(USE_LD)
    opus_decoder_destroy(decoder);
#endif
    return ret;
}

}

#endif
