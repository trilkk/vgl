#ifndef VGL_PACKED_DATA_HPP
#define VGL_PACKED_DATA_HPP

#include "vgl_vector.hpp"

namespace vgl
{

/// Packed data storage.
///
/// Stores arbitrary data in sequence.
/// The data must not have constructors or destructors.
class PackedData
{
private:
    /// Contained data.
    vector<uint8_t> m_data;

public:
    /// Constructor.
    constexpr explicit PackedData() noexcept = default;

    /// Copy constructor.
    ///
    /// \param op Source packed data.
    explicit PackedData(const PackedData& op)
    {
        for(const auto& vv : op.m_data)
        {
            m_data.push_back(vv);
        }
    }

    /// Move constructor.
    ///
    /// \param op Source packed data.
    explicit PackedData(PackedData&& op) :
        m_data(move(op.m_data))
    {
    }

private:
    /// Add data to the rendering queue.
    ///
    /// \param value Pointer to value to add.
    /// \param cound Value size in bytes.
    void addData(const void* value, unsigned count)
    {
        const uint8_t* src = reinterpret_cast<const uint8_t*>(value);
        for(unsigned ii = 0; (ii < count); ++ii)
        {
            m_data.push_back(src[ii]);
        }
    }

public:
    /// Copy operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This object.
    PackedData& operator=(const PackedData& rhs)
    {
        m_data.clear();
        for(const auto& vv : rhs.m_data)
        {
            m_data.push_back(vv);
        }
        return *this;
    }

    /// Move operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This object.
    PackedData& operator=(PackedData&& rhs)
    {
        m_data = move(rhs.m_data);
        return *this;
    }

public:
    /// Accessor.
    ///
    /// \return Pointer to internal data.
    constexpr const void* data() const noexcept
    {
        return m_data.data();
    }

    /// Accessor.
    ///
    /// \return Packed data size in bytes.
    constexpr unsigned size() const noexcept
    {
        return m_data.size();
    }

    /// Append another set of packed data.
    ///
    /// \param op Source to append.
    void append(const PackedData& op)
    {
        for(const auto& vv : op.m_data)
        {
            m_data.push_back(vv);
        }
    }

    /// Push an element of data to the rendering queue.
    ///
    /// \param op Data to add.
    template<typename T> void push(const T& op)
    {
        addData(reinterpret_cast<const void*>(&op), sizeof(T));
    }
};

}

#endif
