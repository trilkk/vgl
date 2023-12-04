#ifndef VGL_FENCE_POOL_HPP
#define VGL_FENCE_POOL_HPP

#include "vgl_fence.hpp"
#include "vgl_queue.hpp"

namespace vgl
{

namespace detail
{

/// Pool for holding fence data.
///
/// Hides the internal implementation from task dispatcher.
/// Uses explicit memory management because the fence data pointers are passed naked.
class FencePool
{
private:
    /// Container for fence data.
    queue<FenceData*> m_pool;

public:
    /// Destructor.
    ~FencePool()
    {
#if defined(USE_LD)
        while(!m_pool.empty())
        {
            FenceData* data = m_pool.front();
            VGL_ASSERT(data);
            delete data;
            m_pool.pop();
        }
#endif
    }

public:
    /// Is the fence pool empty?
    ///
    /// \return True if empty, false otherwise.
    constexpr bool empty() noexcept
    {
        return m_pool.empty();
    }

    /// Acquire fence data.
    ///
    /// \return Fence data.
    FenceData* acquire()
    {
        if(m_pool.empty())
        {
            return new FenceData();
        }

        FenceData* ret = m_pool.front();
        m_pool.pop();
        VGL_ASSERT(ret);
        return ret;
    }

    /// Release fence data back to the pool.
    ///
    /// \param op Fence data to release.
    void emplace(FenceData* op)
    {
        VGL_ASSERT(op);
        m_pool.emplace(op);
    }
};

}

}

#endif
