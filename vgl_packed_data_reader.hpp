#ifndef PACKED_DATA_READER_HPP
#define PACKED_DATA_READER_HPP

#include "vgl_packed_data.hpp"

namespace vgl
{

/// Reader class for reading packed memory streams.
///
/// Used for example for reading packed data.
class PackedDataReader
{
private:
    /// Referenced packed data memory.
    const uint8_t* m_data;

    /// Index into the packed data.
    unsigned m_idx = 0;

    /// Size of the packed data.
    unsigned m_size;

public:
    /// Constructor.
    ///
    /// \param src Memory to read.
    /// \param count Number of bytes that can be read.
    explicit PackedDataReader(const void* src, unsigned count) noexcept :
        m_data(reinterpret_cast<const uint8_t*>(src)),
        m_size(count)
    {
    }

    /// Constructor.
    ///
    /// \param op Packed data structure to read.
    explicit PackedDataReader(const PackedData& op) noexcept :
        m_data(reinterpret_cast<const uint8_t*>(op.data())),
        m_size(op.size())
    {
    }

    /// Default copy constructor.
    PackedDataReader(const PackedDataReader&) noexcept = default;
    /// Default move constructor.
    PackedDataReader(PackedDataReader&&) noexcept = default;

public:
    /// Default copy operator.
    constexpr PackedDataReader& operator=(const PackedDataReader&) noexcept = default;
    /// Default move operator.
    constexpr PackedDataReader& operator=(PackedDataReader&&) noexcept = default;

private:
    /// Advance pointer.
    ///
    /// \param op Number of bytes to advance.
    /// \return Pointer before advance.
    const void* advance(unsigned op)
    {
        const void* ret = reinterpret_cast<const void*>(m_data + m_idx);
        m_idx += op;
        return ret;
    }

public:
    /// Gets the number of remaining bytes.
    ///
    /// \return Number of remaining bytes in memory stream.
    constexpr unsigned remaining() const noexcept
    {
        return m_size - m_idx;
    }

    /// Read a value and advance.
    template<typename T> constexpr const T& read() noexcept
    {
#if defined(USE_LD)
        if(remaining() < sizeof(T))
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("cannot read value of size " + std::to_string(sizeof(T)) + ": " +
                        std::to_string(remaining()) + " bytes remaining"));
        }
#endif
        return *reinterpret_cast<const T*>(advance(static_cast<unsigned>(sizeof(T))));
    }
};

}

#endif
