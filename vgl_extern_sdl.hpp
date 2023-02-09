#ifndef VGL_EXTERN_SDL_HPP
#define VGL_EXTERN_SDL_HPP

/// \file
/// \brief External include: SDL

#if defined(USE_LD)

#include "SDL.h"

/// \cond
#if !defined(dnload_SDL_CondBroadcast)
#define dnload_SDL_CondBroadcast SDL_CondBroadcast
#endif
#if !defined(dnload_SDL_CondSignal)
#define dnload_SDL_CondSignal SDL_CondSignal
#endif
#if !defined(dnload_SDL_CondWait)
#define dnload_SDL_CondWait SDL_CondWait
#endif
#if !defined(dnload_SDL_CreateCond)
#define dnload_SDL_CreateCond SDL_CreateCond
#endif
#if !defined(dnload_SDL_CreateMutex)
#define dnload_SDL_CreateMutex SDL_CreateMutex
#endif
#if !defined(dnload_SDL_CreateThread)
#define dnload_SDL_CreateThread SDL_CreateThread
#endif
#if !defined(dnload_SDL_DestroyCond)
#define dnload_SDL_DestroyCond SDL_DestroyCond
#endif
#if !defined(dnload_SDL_DestroyMutex)
#define dnload_SDL_DestroyMutex SDL_DestroyMutex
#endif
#if !defined(dnload_SDL_GetThreadID)
#define dnload_SDL_GetThreadID SDL_GetThreadID
#endif
#if !defined(dnload_SDL_LockMutex)
#define dnload_SDL_LockMutex SDL_LockMutex
#endif
#if !defined(dnload_SDL_ThreadID)
#define dnload_SDL_ThreadID SDL_ThreadID
#endif
#if !defined(dnload_SDL_UnlockMutex)
#define dnload_SDL_UnlockMutex SDL_UnlockMutex
#endif
#if !defined(dnload_SDL_WaitThread)
#define dnload_SDL_WaitThread SDL_WaitThread
#endif
/// \endcond

#endif

#endif
