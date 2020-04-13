#ifndef VGL_THREAD_HPP
#define VGL_THREAD_HPP

namespace vgl
{

/// Thread class.
class Thread
{
private:
    /// Actual thread.
    SDL_Thread* m_thread;

    /// Thread ID.
    SDL_threadID m_id;

private:
    /// Deleted copy constructor.
    Thread(const Thread&) = delete;
    /// Deleted assignment.
    Thread& operator=(const Thread&) = delete;

public:
    /// Constructor.
    ///
    /// \param func Function pointer to run.
    /// \param data Data for function.
    explicit Thread(int (*func)(void*), void* data) :
        m_thread(dnload_SDL_CreateThread(func, NULL, data)),
        m_id(dnload_SDL_GetThreadID(m_thread))
    {
    }

    /// Move constructor.
    ///
    /// \param op Source.
    Thread(Thread&& op) :
        m_thread(op.m_thread),
        m_id(op.m_id)
    {
        op.m_thread = nullptr;
    }


    /// Destructor.
    ///
    /// Will implicitly join the thread.
    ~Thread()
    {
        if(m_thread)
        {
            dnload_SDL_WaitThread(m_thread, NULL);
        }
    }

public:
    /// Accessor.
    ///
    /// \return Thread ID.
    constexpr SDL_threadID getId() const
    {
        return m_id;
    }

public:
    /// Move operator.
    ///
    /// \param op Source.
    /// \return This object.
    Thread& operator=(Thread&& op)
    {
        m_thread = op.m_thread;
        m_id = op.m_id;
        op.m_thread = nullptr;
        return *this;
    }
};

}

#endif
