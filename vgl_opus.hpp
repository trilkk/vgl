#ifndef VGL_OPUS_HPP
#define VGL_OPUS_HPP

#include "vgl_algorithm.hpp"
#include "vgl_assert.hpp"
#include "vgl_realloc.hpp"

#if !defined(VGL_DISABLE_OGG)
#include "ogg/ogg.h"
#endif

#if !defined(VGL_DISABLE_OPUS)
#include "vgl_vector.hpp"
#include "opus.h"
#endif

#if defined(VGL_USE_LD)
#include "vgl_throw_exception.hpp"
#endif

namespace vgl
{

namespace detail
{

/// Opus maximum packet size in samples for 48kHz.
constexpr int OPUS_MAX_PACKET_SIZE_48000 = 5760;

#if !defined(VGL_DISABLE_OGG)

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
        dnload_ogg_sync_init(&m_sync);
    }

    /// Destructor.
    ~OggReader()
    {
#if defined(VGL_USE_LD)
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
            if(dnload_ogg_sync_pageout(&m_sync, &page) == 1)
            {
                return true;
            }

            // Abort at end of input.
            if(m_pos >= m_size)
            {
                return false;
            }

            unsigned increment = min(m_size - m_pos, BLOCK_SIZE);

            char* buffer = dnload_ogg_sync_buffer(&m_sync, static_cast<long>(increment));
            detail::internal_memcpy(buffer, m_input + m_pos, increment);
            m_pos += increment;

            int err = dnload_ogg_sync_wrote(&m_sync, static_cast<long>(increment));
#if defined(VGL_USE_LD)
            if(err)
            {
                VGL_THROW_RUNTIME_ERROR("ogg_sync_wrote() failed: " + to_string(err));
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
    explicit OggStream(const void* input, unsigned size) :
        m_reader(input, size)
    {
        ogg_page page;
        {
            bool err = m_reader.read(page);
#if defined(VGL_USE_LD)
            if(!err)
            {
                VGL_THROW_RUNTIME_ERROR("OggStream input did not contain even one page");
            }
#else
            (void)err;
#endif
        }

        // First page must correspond to the opus stream.
        int serial = dnload_ogg_page_serialno(&page);
#if defined(VGL_USE_LD)
        {
            int err = ogg_page_bos(&page);
            if(!err)
            {
                VGL_THROW_RUNTIME_ERROR("ogg_page_bos(): first page was not beginning of stream");
            }
        }
#endif

        {
            int err = dnload_ogg_stream_init(&m_stream_state, serial);
#if defined(VGL_USE_LD)
            if(err)
            {
                VGL_THROW_RUNTIME_ERROR("ogg_stream_init() failed: " + to_string(err));
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
#if defined(VGL_USE_LD)
        int err = ogg_stream_clear(&m_stream_state);
        if(err)
        {
            VGL_THROW_RUNTIME_ERROR("ogg_stream_clear() returned nonzero value: " + to_string(err));
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
            int err = dnload_ogg_stream_packetout(&m_stream_state, &op);
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
#if defined(VGL_USE_LD)
            if(err != 1)
            {
                VGL_THROW_RUNTIME_ERROR("ogg_stream_packetout() error: " + to_string(err));
            }
#endif
            return true;
        }
    }

private:
    /// Submit a page to the stream.
    /// \param page Page to submit.
    void submitPage(ogg_page& page)
    {
        int err = dnload_ogg_stream_pagein(&m_stream_state, &page);
#if defined(VGL_USE_LD)
        if(err)
        {
            VGL_THROW_RUNTIME_ERROR("ogg_stream_pagein() packet submission error: " + to_string(err));
        }
#else
        (void)err;
#endif
    }
};

/// Read the opus header from ogg stream.
/// See: https://tools.ietf.org/html/rfc7845#section-5.1
/// \param stream Stream to read from.
/// \return Audio bytes to skip.
unsigned opus_read_ogg_header(OggStream& stream)
{
    ogg_packet packet;
    bool err = stream.readPacket(packet);
#if defined(VGL_USE_LD)
    if(!err)
    {
        VGL_THROW_RUNTIME_ERROR("could not read opus header");
    }
#else
    (void)err;
#endif
    VGL_ASSERT(packet.bytes >= 19);
    unsigned ret = static_cast<unsigned>(packet.packet[10]) + (static_cast<unsigned>(packet.packet[11]) << 8);

    // Comment packet is just discarded.
    err = stream.readPacket(packet);
#if defined(VGL_USE_LD)
    if(!err)
    {
        VGL_THROW_RUNTIME_ERROR("could not read opus comment");
    }
#endif

    return ret;
}

#endif

}

#if !defined(VGL_DISABLE_OPUS)

#if !defined(VGL_DISABLE_OGG)

/// Read ogg opus data from memory.
///
/// \param input Input data.
/// \param size Input data size.
/// \return Data read as samples.
vector<float> opus_read_ogg_memory(const void* input, unsigned size, int channels)
{
    // Read header and comment.
    detail::OggStream stream(input, size);
    unsigned skip_samples = detail::opus_read_ogg_header(stream);

    int err;
    OpusDecoder* decoder = dnload_opus_decoder_create(48000, channels, &err);
#if defined(VGL_USE_LD)
    if(err != OPUS_OK)
    {
        VGL_THROW_RUNTIME_ERROR("opus_decoder_create() failed: " + to_string(err));
    }
#endif

    vector<float> ret;
    for(;;)
    {
        float output[detail::OPUS_MAX_PACKET_SIZE_48000];
        ogg_packet packet;
        if(!stream.readPacket(packet))
        {
            break;
        }

        err = dnload_opus_decode_float(decoder, packet.packet, static_cast<opus_int32>(packet.bytes), output,
                detail::OPUS_MAX_PACKET_SIZE_48000, 0);
#if defined(VGL_USE_LD)
        if(err <= 0)
        {
            VGL_THROW_RUNTIME_ERROR("opus_decode_float() error: " + to_string(err));
        }
        else
#endif
        {
            for(int ii = 0; (ii < err); ++ii)
            {
                if(skip_samples > 0)
                {
                    --skip_samples;
                }
                else
                {
                    ret.push_back(output[ii]);
                }
            }
        }
    }

#if defined(VGL_USE_LD)
    opus_decoder_destroy(decoder);
#endif
    return ret;
}

#else

/// Read ogg opus data from memory.
///
/// \param input Input data.
/// \param size Input data size.
/// \param channels Channel count.
/// \param skip_samples Samples to skip from beginning.
/// \return Data read as samples.
vector<float> opus_read_raw_memory(const void* input, unsigned size, int channels, unsigned skip_samples)
{
    vector<float> ret;

    int err;
    OpusDecoder* decoder = dnload_opus_decoder_create(48000, channels, &err);
#if defined(VGL_USE_LD)
    if(err != OPUS_OK)
    {
        VGL_THROW_RUNTIME_ERROR("opus_decoder_create() failed: " + to_string(err));
    }
#endif

    const uint8_t* input_iter = reinterpret_cast<const uint8_t*>(input);
    const uint8_t* iter_end = input_iter + size;
    do
    {
        float output[detail::OPUS_MAX_PACKET_SIZE_48000];

        // Read packet size.
        opus_int32 bytes = static_cast<opus_int32>(input_iter[0] + (input_iter[1] << 8));
        input_iter += 2;

        err = dnload_opus_decode_float(decoder, input_iter, bytes, output, detail::OPUS_MAX_PACKET_SIZE_48000, 0);
#if defined(VGL_USE_LD)
        if(err <= 0)
        {
            VGL_THROW_RUNTIME_ERROR("opus_decode_float() error: " + to_string(err));
        }
        else
#endif
        {
            for(int ii = 0; (ii < err); ++ii)
            {
                if(skip_samples > 0)
                {
                    --skip_samples;
                }
                else
                {
                    ret.push_back(output[ii]);
                }
            }
        }

        // Advance position.
        input_iter += bytes;
        VGL_ASSERT(input_iter <= iter_end);
    } while(input_iter != iter_end);

#if defined(VGL_USE_LD)
    opus_decoder_destroy(decoder);
#endif
    return ret;
}

#endif

#endif

}

#endif
