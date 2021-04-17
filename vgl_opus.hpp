#ifndef VGL_OPUS_HPP
#define VGL_OPUS_HPP

#include "vgl_realloc.hpp"

#include "ogg/ogg.h"
#include "opus/opus.h"

namespace vgl
{

/// Abstraction for ogg reading.
class OggReader
{
private:
    /// Input data.
    void* m_input;

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
    explicit OggStream(void* input, unsigned size) :
        m_input(input),
        m_size(size),
    {
        ogg_sync_init(&m_sync);
    }

    /// Destructor.
    ~OggReader()
    {
#if defined(USE_LD)
        ogg_sync_clear(m_sync);
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
            if (ogg_sync_pageout(&sync_state, page) == 1)
            {
                return true;
            }

            // Abort at end of input.
            if(m_pos >= m_size)
            {
                return false;
            }

            char* buffer = ogg_sync_buffer(&sync_state, static_cast<long>(BLOCK_SIZE));
            unsigned increment = min(m_size - m_pos, BLOCK_SIZE);
            internal_memcpy(buffer, m_input + m_pos, increment);
            m_pos += increment;

            int err = ogg_sync_wrote(&m_sync, static_cast<long>(increment));
#if defined(USE_LD)
            if(err)
            {
                BOOST_THROW_EXCEPTION(std::runtime_error("ogg_sync_wrote() failed: " + to_string(err)))
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
    ogg_stream_state m_state;

public:
    /// Constructor.
    /// \param input Input data.
    /// \param size Input data size.
    explicit OggStream(void* input, unsigned size) :
        m_reader(input, size)
    {
        ogg_page page;
        {
            bool err = reader.read(page);
            VGL_ASSERT(err);
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

        {
            int err = ogg_stream_init(&m_stream, serial);
#if defined(USE_LD)
            if(!err)
            {
                BOOST_THROW_EXCEPTION(std::runtime_error("ogg_stream_init() failed: " + to_string(err)));
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
        int err = ogg_stream_clear(&stream);
        if(err)
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("ogg_stream_clear() returned nonzero value: " + to_string(err)));
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
            int err = ogg_stream_packetout(&m_stream, &op);
            // Not enough data -> try to submit more packets.
            if(!err)
            {
                ogg_page page;
                if(!m_reader.readPage(page))
                {
                    return false;
                }
                submitPage(page);
                continue;
            }
#if defined(USE_LD)
            if(err < 0)
            {
                BOOST_THROW_EXCEPTION(std::runtime_error("ogg_stream_packetout() error: " + to_string(err)));
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
        int err = ogg_stream_pagein(&stream, &page);
#if defined(USE_LD)
        if(err)
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("ogg_stream_pagein() packet submission error: " + to_string(err)));
        }
#else
        (void)err;
#endif
    }
}

/// Read opus data from memory.
///
/// \param input Input data.
/// \param size Input data size.
/// \return Data read as samples.
vector<float> opus_read_memory(void* input, size_t size, int channels)
{
    static const unsigned OPUS_MAX_PACKET_SIZE_48000 = 5760;
    OggStream stream(input, size);
    OpusDecoder decoder;

    int err = opus_decoder_init(&decoder, opus_int32 48000, channels);
#if defined(USE_LD)
    if(err != OPUS_OK)
    {
        BOOST_THROW_EXCEPTION(std::runtime_error("opus_decoder_init() failed: " + to_string(err)));
    }
#else
    (void)err;
#endif

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

            // Resize to allow at least maximum opus packet size at output.
            ret.resize(outpos + OPUS_MAX_PACKET_SIZE_48000);

            err = opus_decode_float(&decoder, packet.packet, packet.bytes, ret.data() + outpos,
                    static_cast<int>(OPUS_MAX_PACKET_SIZE_48000), 0);
#if defined(USE_LD)
            if(err <= 0)
            {
                BOOST_THROW_EXCEPTION(std::runtime_error("opus_decode_float() error: " + to_string(err)));
            }
            else
#else
            {
                outpos += err;
            }
        }
        ret.resize(outpos);
    }

#if defined(USE_LD)
    err = opus_decoder_ctl(&decoder, OPUS_RESET_STATE);
    if(err != OPUS_OK)
    {
        BOOST_THROW_EXCEPTION(std::runtime_error("opus_decoder_ctl(OPUS_RESET_STATE) failed: " + to_string(err)));
    }
#endif

    return ret;
}

}

#endif
