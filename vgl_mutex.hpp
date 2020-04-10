#ifndef VGL_MUTEX_HPP
#define VGL_MUTEX_HPP

namespace vgl
{

/// Mutex class.
class Mutex
{
private:
    /// Actual mutex.
    SDL_mutex *m_mutex;

private:
    /// Deleted copy constructor.
    Mutex(const Mutex&) = delete;
    /// Deleted assignment.
    Mutex& operator=(const Mutex&) = delete;

public:
    /// Constructor.
    explicit Mutex() :
        m_mutex(dnload_SDL_CreateMutex())
    {
#if defined(USE_LD) && defined(DEBUG)
        if(!m_mutex)
        {
            BOOST_THROW_EXCEPTION(std::runtime_error(std::string("Mutex::Mutex(): ") + SDL_GetError()));
        }
#endif
    }

    /// Move constructor.
    ///
    /// \param op Source.
    Mutex(Mutex&& op) :
        m_mutex(op.m_mutex)
    {
        op.m_mutex = nullptr;
    }

    /// Destructor.
    ~Mutex()
    {
        if(m_mutex)
        {
            dnload_SDL_DestroyMutex(m_mutex);
        }
    }

public:
    /// Accessor.
    ///
    /// \return Inner mutex.
    constexpr SDL_mutex* getInnerMutex() const
    {
        return m_mutex;
    }

    /// Lock.
    void acquire()
    {
        int err = dnload_SDL_LockMutex(m_mutex);
#if defined(USE_LD) && defined(DEBUG)
        if(err)
        {
            BOOST_THROW_EXCEPTION(std::runtime_error(std::string("Mutex::acquire(): ") + SDL_GetError()));
        }
#else
        (void)err;
#endif
    }

    /// Unlock.
    void release()
    {
        int err = dnload_SDL_UnlockMutex(m_mutex);
#if defined(USE_LD) && defined(DEBUG)
        if(err)
        {
            BOOST_THROW_EXCEPTION(std::runtime_error(std::string("Mutex::release(): ") + SDL_GetError()));
        }
#else
        (void)err;
#endif
    }

public:
    /// Move operator.
    ///
    /// \param op Source.
    /// \return This object.
    Mutex& operator=(Mutex&& op)
    {
        m_mutex = op.m_mutex;
        op.m_mutex = nullptr;
        return *this;
    }
};

}

#endif
