//===-- sanitizer_platform_interceptors.h -----------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines macro telling whether sanitizer tools can/should intercept
// given library functions on a given platform.
//
//===----------------------------------------------------------------------===//
#ifndef SANITIZER_PLATFORM_INTERCEPTORS_H
#define SANITIZER_PLATFORM_INTERCEPTORS_H

#include "sanitizer_internal_defs.h"

#if !SANITIZER_WINDOWS
# define SI_NOT_WINDOWS 1
# include "sanitizer_platform_limits_posix.h"
#else
# define SI_NOT_WINDOWS 0
#endif

#if SANITIZER_LINUX && !SANITIZER_ANDROID
# define SI_LINUX_NOT_ANDROID 1
#else
# define SI_LINUX_NOT_ANDROID 0
#endif

#if SANITIZER_LINUX
# define SI_LINUX 1
#else
# define SI_LINUX 0
#endif

#if SANITIZER_MAC
# define SI_MAC 1
#else
# define SI_MAC 0
#endif

# define SANITIZER_INTERCEPT_STRCASECMP SI_NOT_WINDOWS

# define SANITIZER_INTERCEPT_READ   SI_NOT_WINDOWS
# define SANITIZER_INTERCEPT_PREAD  SI_NOT_WINDOWS
# define SANITIZER_INTERCEPT_WRITE  SI_NOT_WINDOWS
# define SANITIZER_INTERCEPT_PWRITE SI_NOT_WINDOWS

#define SANITIZER_INTERCEPT_PREAD64 SI_LINUX_NOT_ANDROID
#define SANITIZER_INTERCEPT_PWRITE64 SI_LINUX_NOT_ANDROID

#define SANITIZER_INTERCEPT_READV SI_NOT_WINDOWS
#define SANITIZER_INTERCEPT_WRITEV SI_NOT_WINDOWS

#define SANITIZER_INTERCEPT_PREADV SI_LINUX_NOT_ANDROID
#define SANITIZER_INTERCEPT_PWRITEV SI_LINUX_NOT_ANDROID
#define SANITIZER_INTERCEPT_PREADV64 SI_LINUX_NOT_ANDROID
#define SANITIZER_INTERCEPT_PWRITEV64 SI_LINUX_NOT_ANDROID

# define SANITIZER_INTERCEPT_PRCTL   SI_LINUX

# define SANITIZER_INTERCEPT_LOCALTIME_AND_FRIENDS SI_NOT_WINDOWS

# define SANITIZER_INTERCEPT_SCANF SI_NOT_WINDOWS
# define SANITIZER_INTERCEPT_ISOC99_SCANF SI_LINUX

# define SANITIZER_INTERCEPT_FREXP 1
# define SANITIZER_INTERCEPT_FREXPF_FREXPL SI_NOT_WINDOWS

# define SANITIZER_INTERCEPT_GETPWNAM_AND_FRIENDS SI_NOT_WINDOWS
# define SANITIZER_INTERCEPT_GETPWNAM_R_AND_FRIENDS \
    SI_MAC || SI_LINUX_NOT_ANDROID
# define SANITIZER_INTERCEPT_CLOCK_GETTIME SI_LINUX
# define SANITIZER_INTERCEPT_GETITIMER SI_NOT_WINDOWS
# define SANITIZER_INTERCEPT_TIME SI_NOT_WINDOWS
# define SANITIZER_INTERCEPT_GLOB SI_LINUX_NOT_ANDROID
# define SANITIZER_INTERCEPT_WAIT SI_NOT_WINDOWS
# define SANITIZER_INTERCEPT_INET SI_NOT_WINDOWS
# define SANITIZER_INTERCEPT_PTHREAD_GETSCHEDPARAM SI_NOT_WINDOWS
# define SANITIZER_INTERCEPT_GETADDRINFO SI_NOT_WINDOWS
# define SANITIZER_INTERCEPT_GETSOCKNAME SI_NOT_WINDOWS
# define SANITIZER_INTERCEPT_GETHOSTBYNAME SI_NOT_WINDOWS
# define SANITIZER_INTERCEPT_GETHOSTBYNAME_R SI_LINUX
# define SANITIZER_INTERCEPT_GETSOCKOPT SI_NOT_WINDOWS
# define SANITIZER_INTERCEPT_ACCEPT SI_NOT_WINDOWS
# define SANITIZER_INTERCEPT_ACCEPT4 SI_LINUX
# define SANITIZER_INTERCEPT_MODF SI_NOT_WINDOWS
# define SANITIZER_INTERCEPT_RECVMSG SI_NOT_WINDOWS
# define SANITIZER_INTERCEPT_GETPEERNAME SI_NOT_WINDOWS
# define SANITIZER_INTERCEPT_IOCTL SI_NOT_WINDOWS
# define SANITIZER_INTERCEPT_INET_ATON SI_NOT_WINDOWS
# define SANITIZER_INTERCEPT_SYSINFO SI_LINUX
# define SANITIZER_INTERCEPT_READDIR SI_NOT_WINDOWS
# define SANITIZER_INTERCEPT_READDIR64 SI_LINUX_NOT_ANDROID
# define SANITIZER_INTERCEPT_PTRACE SI_LINUX

#endif  // #ifndef SANITIZER_PLATFORM_INTERCEPTORS_H
