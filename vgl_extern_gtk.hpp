#ifndef VGL_EXTERN_GTK_HPP
#define VGL_EXTERN_GTK_HPP

/// \file
/// \brief External include: GTK

#if defined(USE_LD)

#include "gtk/gtk.h"

/// \cond
#if !defined(dnload_g_cond_broadcast)
#define dnload_g_cond_broadcast g_cond_broadcast
#endif
#if !defined(dnload_g_cond_clear)
#define dnload_g_cond_clear g_cond_clear
#endif
#if !defined(dnload_g_cond_init)
#define dnload_g_cond_init g_cond_init
#endif
#if !defined(dnload_g_cond_signal)
#define dnload_g_cond_signal g_cond_signal
#endif
#if !defined(dnload_g_cond_wait)
#define dnload_g_cond_wait g_cond_wait
#endif
#if !defined(dnload_g_mutex_clear)
#define dnload_g_mutex_clear g_mutex_clear
#endif
#if !defined(dnload_g_mutex_init)
#define dnload_g_mutex_init g_mutex_init
#endif
#if !defined(dnload_g_mutex_lock)
#define dnload_g_mutex_lock g_mutex_lock
#endif
#if !defined(dnload_g_mutex_unlock)
#define dnload_g_mutex_unlock g_mutex_unlock
#endif
#if !defined(dnload_g_thread_join)
#define dnload_g_thread_join g_thread_join
#endif
#if !defined(dnload_g_thread_new)
#define dnload_g_thread_new g_thread_new
#endif
#if !defined(dnload_g_thread_self)
#define dnload_g_thread_self g_thread_self
#endif
/// \endcond

#endif

#endif
