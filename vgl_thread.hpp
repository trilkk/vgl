#ifndef VGL_THREAD_HPP
#define VGL_THREAD_HPP

#if defined(VGL_ENABLE_GTK)
#include "vgl_extern_gtk.hpp"
#else
#include "vgl_extern_sdl.hpp"
#endif

namespace vgl
{

/// Thread class.
class Thread
{
public:
    /// Internal thread implementation.
    using thread_type =
#if defined(VGL_ENABLE_GTK)
        GThread
#else
        SDL_Thread
#endif
        ;

    /// Internal thread ID type.
    using id_type =
#if defined(VGL_ENABLE_GTK)
        GThread*
#else
        SDL_threadID
#endif
        ;

    /// Internal thread function return type.
    using return_type =
#if defined(VGL_ENABLE_GTK)
        void*
#else
        int
#endif
        ;

    /// Internal thread function type.
    using func_type = return_type (*)(void*);

private:
    /// Actual thread.
    thread_type* m_thread;

#if !defined(VGL_ENABLE_GTK)
    /// Thread ID.
    SDL_threadID m_id;
#endif

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
    explicit Thread(func_type func, void* data, const char* name = nullptr) :
#if defined(VGL_ENABLE_GTK)
        m_thread(dnload_g_thread_new(name, func, data))
#else
        m_thread(dnload_SDL_CreateThread(func, name, data)),
        m_id(dnload_SDL_GetThreadID(m_thread))
#endif
    {
    }

    /// Move constructor.
    ///
    /// \param op Source.
    constexpr Thread(Thread&& op) noexcept :
        m_thread(op.m_thread)
#if !defined(VGL_ENABLE_GTK)
        , m_id(op.m_id)
#endif
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
#if defined(VGL_ENABLE_GTK)
            dnload_g_thread_join(m_thread);
#else
            dnload_SDL_WaitThread(m_thread, NULL);
#endif
        }
    }

public:
    /// Accessor.
    ///
    /// \return Thread ID.
    constexpr id_type getId() const
    {
#if defined(VGL_ENABLE_GTK)
        return m_thread;
#else
        return m_id;
#endif
    }

public:
    /// Move operator.
    ///
    /// \param op Source.
    /// \return This object.
    constexpr Thread& operator=(Thread&& op) noexcept
    {
        m_thread = op.m_thread;
#if !defined(VGL_ENABLE_GTK)
        m_id = op.m_id;
#endif
        op.m_thread = nullptr;
        return *this;
    }

public:
    /// Gets the current thread ID.
    /// \return ID of the running thread.
    static inline id_type get_current_thread_id()
    {
        return
#if defined(VGL_ENABLE_GTK)
            dnload_g_thread_self()
#else
            dnload_SDL_ThreadID()
#endif
            ;
    }
};

}

#endif
