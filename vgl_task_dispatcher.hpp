#ifndef VGL_TASK_DISPATCHER_HPP
#define VGL_TASK_DISPATCHER_HPP

#include "vgl_task_queue.hpp"
#include "vgl_thread.hpp"
#include "vgl_vector.hpp"

namespace vgl
{

namespace detail
{

/// Task queue class.
class TaskDispatcher
{
private:
    /// Threads created for this task queue.
    vector<Thread> m_threads;

    /// Queue for tasks (any thread).
    detail::TaskQueue m_tasks_any;

    /// Queue for tasks (main thread).
    detail::TaskQueue m_tasks_main;

    /// Pool of free data structures for fences.
    queue<detail::FenceDataUptr> m_fence_pool;

    /// Guard mutex.
    unique_ptr<Mutex> m_mutex;

    /// Main thread ID.
    SDL_threadID m_main_thread_id = 0;

    /// Target concurrency level.
    unsigned m_concurrency = 0;

    /// Number of threads active currently.
    unsigned m_threads_active = 0;

    /// Flag signifying the task queue is being destroyed.
    bool m_quitting = false;

private:
    /// Deleted copy constructor.
    TaskDispatcher(const TaskDispatcher&) = delete;
    /// Deleted assignment.
    TaskDispatcher& operator=(const TaskDispatcher&) = delete;

public:
    /// Default constructor.
    constexpr explicit TaskDispatcher() = default;

    /// Destructor.
    ~TaskDispatcher()
    {
#if defined(USE_LD) && defined(DEBUG)
        if(!m_mutex)
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("task queue never initialized"));
        }
#endif
        {
            ScopedLock sl(*m_mutex);
            m_quitting = true;
            m_tasks_any.uninitialize();
            m_tasks_main.uninitialize();
        }

        // Threads must be joined before destroying anything else.
        m_threads.clear();
    }

private:
    /// Acquire or reuse fence data (unlocked).
    ///
    /// \return Fence data structure to use.
    detail::FenceData* acquireFenceData()
    {
        if(m_fence_pool.empty())
        {
            return new detail::FenceData();
        }

#if defined(USE_LD) && defined(DEBUG)
        BOOST_ASSERT(m_fence_pool.front());
#endif
        detail::FenceData* ret = m_fence_pool.front().release();
#if defined(USE_LD) && defined(DEBUG)
        BOOST_ASSERT(ret);
#endif
        m_fence_pool.pop();
        return ret;
    }
    /// Acquire or reuse fence data (locked).
    ///
    /// \return Fence data structure to use.
    detail::FenceData* acquireFenceDataSafe()
    {
        ScopedLock sl(*m_mutex);
        return acquireFenceData();
    }

    /// Immediately execute given task function.
    ///
    /// Return an inactive fence containing the return value.
    /// \param func Function to dispatch.
    /// \param params Function parameters.
    Fence immediateDispatch(TaskFunc func, void* params)
    {
        detail::FenceData* ret = acquireFenceDataSafe();
        ret->setActive(false);
        ret->setReturnValue(func(params));
        return Fence(ret);
    }

    /// Internally wait (create a fence) and dispatch.
    ///
    /// \param queue Internal task queue.
    /// \param func Function to dispatch.
    /// \param params Function parameters.
    detail::FenceData* internalDispatch(detail::TaskQueue& task_queue, TaskFunc func, void* params)
    {
        ScopedLock sl(*m_mutex);
        detail::FenceData* ret = acquireFenceData();
        ret->setActive(true);
        ret->setReturnValue(nullptr);
        task_queue.emplace(ret, func, params);
        return ret;
    }

    /// Is the calling thread the main thread.
    ///
    /// \return True if yes, false if no.
    bool isMainThread() const
    {
        return (m_main_thread_id == dnload_SDL_ThreadID());
    }

    /// Is this a spawned thread?
    bool isSpawnedThread()
    {
        SDL_threadID current_thread_id = dnload_SDL_ThreadID();
        for(const auto& vv : m_threads)
        {
            if(vv.getId() == current_thread_id)
            {
                return true;
            }
        }
        return false;
    }

    /// Spawn a new thread.
    void spawnThread()
    {
        m_threads.emplace_back(task_thread_func, this);
    }

    /// Thread function.
    void threadFunc()
    {
        ScopedLock sl(*m_mutex);

        while(!m_quitting)
        {
            if((m_threads_active < m_concurrency) && !m_tasks_any.empty())
            {
                ++m_threads_active;

                // Release lock for the duration of executing the task.
                {
                    Task task = m_tasks_any.acquire();
                    sl.release();
                    task();
                    sl.acquire();
                }

                --m_threads_active;
            }
            else
            {
                m_tasks_any.wait(sl);
            }
        }
    }

public:
    /// Initialize the task queue.
    ///
    /// \param op Number of threads to initialize.
    void initialize(unsigned op)
    {
        m_concurrency = op;
        m_main_thread_id = dnload_SDL_ThreadID();

        m_tasks_any.initialize();
        m_tasks_main.initialize();
        m_mutex.reset(new Mutex());

        while(op)
        {
            spawnThread();
            --op;
        }
    }

    /// Gets a main context task.
    ///
    /// \return Main context task.
    Task acquireMainTask()
    {
        ScopedLock sl(*m_mutex);

        while(m_tasks_main.empty())
        {
            m_tasks_main.wait(sl);
        }

        return m_tasks_main.acquire();
    }

    /// Dispatch a task (any thread).
    ///
    /// \param func Function to dispatch.
    /// \param params Function parameters.
    void dispatch(TaskFunc func, void* params)
    {
        ScopedLock sl(*m_mutex);
        m_tasks_any.emplace(func, params);
    }
    /// Dispatch a task (main thread).
    ///
    /// \param func Function to dispatch.
    /// \param params Function parameters.
    void dispatchMain(TaskFunc func, void* params)
    {
        ScopedLock sl(*m_mutex);
        m_tasks_main.emplace(func, params);
    }

    /// Dispatch a task and wait for it to complete (any thread).
    ///
    /// \param func Function to dispatch.
    /// \param params Function parameters.
    Fence wait(TaskFunc func, void* params)
    {
        // Prevent deadlock - main thread cannot wait.
        if(isMainThread())
        {
            return immediateDispatch(func, params);
        }

        detail::FenceData* data = internalDispatch(m_tasks_any, func, params);
        return Fence(data);
    }
    /// Dispatch a task and wait for it to complete (main thread).
    ///
    /// \param func Function to dispatch.
    /// \param params Function parameters.
    Fence waitMain(TaskFunc func, void* params)
    {
        // Prevent deadlock - main thread cannot wait.
        if(isMainThread())
        {
            return immediateDispatch(func, params);
        }

        detail::FenceData* data = internalDispatch(m_tasks_main, func, params);
        return Fence(data);
    }

    /// Wait on a fence internal state.
    ///
    /// \param op Fence data.
    /// \return Stored return value from the fence.
    void* wait(detail::FenceData* op)
    {
        ScopedLock sl(*m_mutex);

#if defined(USE_LD) && defined(DEBUG)
        if(isMainThread())
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("cannot wait on main thread"));
        }
#endif

        // Fence may have turned inactive before the wait point is reached.
        if(op->isActive())
        {
            bool is_spawned = isSpawnedThread();

            // If waiting would lock the last concurrent thread, spawn a new one.
            if(is_spawned)
            {
                if(m_threads_active >= m_concurrency)
                {
                    spawnThread();
                }
                --m_threads_active;
            }

            m_tasks_any.signal();
            op->wait(sl);

            if(is_spawned)
            {
                ++m_threads_active;
            }
        }

        m_fence_pool.emplace(op);
        return op->getReturnValue();
    }

private:
    /// Task dispatcher thread.
    ///
    /// \param op Pointer to task queue.
    static int task_thread_func(void* op)
    {
        TaskDispatcher* task_queue = static_cast<TaskDispatcher*>(op);
        task_queue->threadFunc();
        return 0;
    }
};

/// Queue for tasks.
TaskDispatcher g_task_dispatcher;

/// Internal wait for fence data on the task dispatcher.
///
/// \param op Fence data.
void* internal_fence_data_wait(detail::FenceData* op)
{
    return g_task_dispatcher.wait(op);
}

}

/// Initialize task system.
///
/// \param op Concurrency level.
void tasks_initialize(unsigned op)
{
    detail::g_task_dispatcher.initialize(op);
}

/// Get a main loop task.
///
/// This function blocks if main loop tasks are not available.
///
/// \return New main loop task.
Task task_acquire_main()
{
    return detail::g_task_dispatcher.acquireMainTask();
}

/// Dispatch task (any thread).
///
/// \param func Function to dispatch.
/// \param params Function parameters.
void task_dispatch(TaskFunc func, void* params)
{
    detail::g_task_dispatcher.dispatch(func, params);
}
/// Dispatch task (main thread).
///
/// \param func Function to dispatch.
/// \param params Function parameters.
void task_dispatch_main(TaskFunc func, void* params)
{
    detail::g_task_dispatcher.dispatchMain(func, params);
}

/// Wait on a task (any thread).
///
/// \param func Function to dispatch.
/// \param params Function parameters.
/// \return Fence.
Fence task_wait(TaskFunc func, void* params)
{
    return detail::g_task_dispatcher.wait(func, params);
}
/// Wait on a task (main thread).
///
/// \param func Function to dispatch.
/// \param params Function parameters.
/// \return Fence.
Fence task_wait_main(TaskFunc func, void* params)
{
    return detail::g_task_dispatcher.waitMain(func, params);
}

}

#endif
