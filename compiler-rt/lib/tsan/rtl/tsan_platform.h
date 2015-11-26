//===-- tsan_platform.h -----------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file is a part of ThreadSanitizer (TSan), a race detector.
//
// Platform-specific code.
//===----------------------------------------------------------------------===//

#ifndef TSAN_PLATFORM_H
#define TSAN_PLATFORM_H

#if !defined(__LP64__) && !defined(_WIN64)
# error "Only 64-bit is supported"
#endif

#include "tsan_defs.h"
#include "tsan_trace.h"

namespace __tsan {

#if !defined(SANITIZER_GO)

#if defined(__x86_64__)
/*
C/C++ on linux/x86_64 and freebsd/x86_64
0000 0000 1000 - 0100 0000 0000: main binary and/or MAP_32BIT mappings
0100 0000 0000 - 0200 0000 0000: -
0200 0000 0000 - 1000 0000 0000: shadow
1000 0000 0000 - 3000 0000 0000: -
3000 0000 0000 - 4000 0000 0000: metainfo (memory blocks and sync objects)
4000 0000 0000 - 6000 0000 0000: -
6000 0000 0000 - 6200 0000 0000: traces
6200 0000 0000 - 7d00 0000 0000: -
7d00 0000 0000 - 7e00 0000 0000: heap
7e00 0000 0000 - 7e80 0000 0000: -
7e80 0000 0000 - 8000 0000 0000: modules and main thread stack
*/
struct Mapping {
  static const uptr kMetaShadowBeg = 0x300000000000ull;
  static const uptr kMetaShadowEnd = 0x400000000000ull;
  static const uptr kTraceMemBeg   = 0x600000000000ull;
  static const uptr kTraceMemEnd   = 0x620000000000ull;
  static const uptr kShadowBeg     = 0x020000000000ull;
  static const uptr kShadowEnd     = 0x100000000000ull;
  static const uptr kHeapMemBeg    = 0x7d0000000000ull;
  static const uptr kHeapMemEnd    = 0x7e0000000000ull;
  static const uptr kLoAppMemBeg   = 0x000000001000ull;
  static const uptr kLoAppMemEnd   = 0x010000000000ull;
  static const uptr kHiAppMemBeg   = 0x7e8000000000ull;
  static const uptr kHiAppMemEnd   = 0x800000000000ull;
  static const uptr kAppMemMsk     = 0x7c0000000000ull;
  static const uptr kAppMemXor     = 0x020000000000ull;
  static const uptr kVdsoBeg       = 0xf000000000000000ull;
};
#elif defined(__mips64)
/*
C/C++ on linux/mips64
0100 0000 00 - 0200 0000 00: main binary
0200 0000 00 - 1400 0000 00: -
1400 0000 00 - 2400 0000 00: shadow
2400 0000 00 - 3000 0000 00: -
3000 0000 00 - 4000 0000 00: metainfo (memory blocks and sync objects)
4000 0000 00 - 6000 0000 00: -
6000 0000 00 - 6200 0000 00: traces
6200 0000 00 - fe00 0000 00: -
fe00 0000 00 - ff00 0000 00: heap
ff00 0000 00 - ff80 0000 00: -
ff80 0000 00 - ffff ffff ff: modules and main thread stack
*/
struct Mapping {
  static const uptr kMetaShadowBeg = 0x3000000000ull;
  static const uptr kMetaShadowEnd = 0x4000000000ull;
  static const uptr kTraceMemBeg   = 0x6000000000ull;
  static const uptr kTraceMemEnd   = 0x6200000000ull;
  static const uptr kShadowBeg     = 0x1400000000ull;
  static const uptr kShadowEnd     = 0x2400000000ull;
  static const uptr kHeapMemBeg    = 0xfe00000000ull;
  static const uptr kHeapMemEnd    = 0xff00000000ull;
  static const uptr kLoAppMemBeg   = 0x0100000000ull;
  static const uptr kLoAppMemEnd   = 0x0200000000ull;
  static const uptr kHiAppMemBeg   = 0xff80000000ull;
  static const uptr kHiAppMemEnd   = 0xffffffffffull;
  static const uptr kAppMemMsk     = 0xfc00000000ull;
  static const uptr kAppMemXor     = 0x0400000000ull;
  static const uptr kVdsoBeg       = 0xfffff00000ull;
};
#elif defined(__aarch64__)
// AArch64 supports multiple VMA which leads to multiple address transformation
// functions.  To support these multiple VMAS transformations and mappings TSAN
// runtime for AArch64 uses an external memory read (vmaSize) to select which
// mapping to use.  Although slower, it make a same instrumented binary run on
// multiple kernels.

/*
C/C++ on linux/aarch64 (39-bit VMA)
0000 0010 00 - 0100 0000 00: main binary
0100 0000 00 - 0800 0000 00: -
0800 0000 00 - 1F00 0000 00: shadow memory
1C00 0000 00 - 3100 0000 00: -
3100 0000 00 - 3400 0000 00: metainfo
3400 0000 00 - 6000 0000 00: -
6000 0000 00 - 6200 0000 00: traces
6200 0000 00 - 7d00 0000 00: -
7c00 0000 00 - 7d00 0000 00: heap
7d00 0000 00 - 7fff ffff ff: modules and main thread stack
*/
struct Mapping39 {
  static const uptr kLoAppMemBeg   = 0x0000001000ull;
  static const uptr kLoAppMemEnd   = 0x0100000000ull;
  static const uptr kShadowBeg     = 0x0800000000ull;
  static const uptr kShadowEnd     = 0x1F00000000ull;
  static const uptr kMetaShadowBeg = 0x3100000000ull;
  static const uptr kMetaShadowEnd = 0x3400000000ull;
  static const uptr kTraceMemBeg   = 0x6000000000ull;
  static const uptr kTraceMemEnd   = 0x6200000000ull;
  static const uptr kHeapMemBeg    = 0x7c00000000ull;
  static const uptr kHeapMemEnd    = 0x7d00000000ull;
  static const uptr kHiAppMemBeg   = 0x7d00000000ull;
  static const uptr kHiAppMemEnd   = 0x7fffffffffull;
  static const uptr kAppMemMsk     = 0x7800000000ull;
  static const uptr kAppMemXor     = 0x0200000000ull;
  static const uptr kVdsoBeg       = 0x7f00000000ull;
};

/*
C/C++ on linux/aarch64 (42-bit VMA)
00000 0010 00 - 01000 0000 00: main binary
01000 0000 00 - 10000 0000 00: -
10000 0000 00 - 20000 0000 00: shadow memory
20000 0000 00 - 26000 0000 00: -
26000 0000 00 - 28000 0000 00: metainfo
28000 0000 00 - 36200 0000 00: -
36200 0000 00 - 36240 0000 00: traces
36240 0000 00 - 3e000 0000 00: -
3e000 0000 00 - 3f000 0000 00: heap
3f000 0000 00 - 3ffff ffff ff: modules and main thread stack
*/
struct Mapping42 {
  static const uptr kLoAppMemBeg   = 0x00000001000ull;
  static const uptr kLoAppMemEnd   = 0x01000000000ull;
  static const uptr kShadowBeg     = 0x10000000000ull;
  static const uptr kShadowEnd     = 0x20000000000ull;
  static const uptr kMetaShadowBeg = 0x26000000000ull;
  static const uptr kMetaShadowEnd = 0x28000000000ull;
  static const uptr kTraceMemBeg   = 0x36200000000ull;
  static const uptr kTraceMemEnd   = 0x36400000000ull;
  static const uptr kHeapMemBeg    = 0x3e000000000ull;
  static const uptr kHeapMemEnd    = 0x3f000000000ull;
  static const uptr kHiAppMemBeg   = 0x3f000000000ull;
  static const uptr kHiAppMemEnd   = 0x3ffffffffffull;
  static const uptr kAppMemMsk     = 0x3c000000000ull;
  static const uptr kAppMemXor     = 0x04000000000ull;
  static const uptr kVdsoBeg       = 0x37f00000000ull;
};

// Indicates the runtime will define the memory regions at runtime.
#define TSAN_RUNTIME_VMA 1
#endif

#elif defined(SANITIZER_GO) && !SANITIZER_WINDOWS

/* Go on linux, darwin and freebsd
0000 0000 1000 - 0000 1000 0000: executable
0000 1000 0000 - 00c0 0000 0000: -
00c0 0000 0000 - 00e0 0000 0000: heap
00e0 0000 0000 - 2000 0000 0000: -
2000 0000 0000 - 2380 0000 0000: shadow
2380 0000 0000 - 3000 0000 0000: -
3000 0000 0000 - 4000 0000 0000: metainfo (memory blocks and sync objects)
4000 0000 0000 - 6000 0000 0000: -
6000 0000 0000 - 6200 0000 0000: traces
6200 0000 0000 - 8000 0000 0000: -
*/

struct Mapping {
  static const uptr kMetaShadowBeg = 0x300000000000ull;
  static const uptr kMetaShadowEnd = 0x400000000000ull;
  static const uptr kTraceMemBeg   = 0x600000000000ull;
  static const uptr kTraceMemEnd   = 0x620000000000ull;
  static const uptr kShadowBeg     = 0x200000000000ull;
  static const uptr kShadowEnd     = 0x238000000000ull;
  static const uptr kAppMemBeg     = 0x000000001000ull;
  static const uptr kAppMemEnd     = 0x00e000000000ull;
};

#elif defined(SANITIZER_GO) && SANITIZER_WINDOWS

/* Go on windows
0000 0000 1000 - 0000 1000 0000: executable
0000 1000 0000 - 00f8 0000 0000: -
00c0 0000 0000 - 00e0 0000 0000: heap
00e0 0000 0000 - 0100 0000 0000: -
0100 0000 0000 - 0500 0000 0000: shadow
0500 0000 0000 - 0560 0000 0000: -
0560 0000 0000 - 0760 0000 0000: traces
0760 0000 0000 - 07d0 0000 0000: metainfo (memory blocks and sync objects)
07d0 0000 0000 - 8000 0000 0000: -
*/

struct Mapping {
  static const uptr kMetaShadowBeg = 0x076000000000ull;
  static const uptr kMetaShadowEnd = 0x07d000000000ull;
  static const uptr kTraceMemBeg   = 0x056000000000ull;
  static const uptr kTraceMemEnd   = 0x076000000000ull;
  static const uptr kShadowBeg     = 0x010000000000ull;
  static const uptr kShadowEnd     = 0x050000000000ull;
  static const uptr kAppMemBeg     = 0x000000001000ull;
  static const uptr kAppMemEnd     = 0x00e000000000ull;
}

#else
# error "Unknown platform"
#endif


#ifdef TSAN_RUNTIME_VMA
extern uptr vmaSize;
#endif


enum MappingType {
  MAPPING_LO_APP_BEG,
  MAPPING_LO_APP_END,
  MAPPING_HI_APP_BEG,
  MAPPING_HI_APP_END,
  MAPPING_HEAP_BEG,
  MAPPING_HEAP_END,
  MAPPING_APP_BEG,
  MAPPING_APP_END,
  MAPPING_SHADOW_BEG,
  MAPPING_SHADOW_END,
  MAPPING_META_SHADOW_BEG,
  MAPPING_META_SHADOW_END,
  MAPPING_TRACE_BEG,
  MAPPING_TRACE_END,
  MAPPING_VDSO_BEG,
};

template<typename Mapping, int Type>
uptr MappingImpl(void) {
  switch (Type) {
#ifndef SANITIZER_GO
    case MAPPING_LO_APP_BEG: return Mapping::kLoAppMemBeg;
    case MAPPING_LO_APP_END: return Mapping::kLoAppMemEnd;
    case MAPPING_HI_APP_BEG: return Mapping::kHiAppMemBeg;
    case MAPPING_HI_APP_END: return Mapping::kHiAppMemEnd;
    case MAPPING_HEAP_BEG: return Mapping::kHeapMemBeg;
    case MAPPING_HEAP_END: return Mapping::kHeapMemEnd;
    case MAPPING_VDSO_BEG: return Mapping::kVdsoBeg;
#else
    case MAPPING_APP_BEG: return Mapping::kAppMemBeg;
    case MAPPING_APP_END: return Mapping::kAppMemEnd;
#endif
    case MAPPING_SHADOW_BEG: return Mapping::kShadowBeg;
    case MAPPING_SHADOW_END: return Mapping::kShadowEnd;
    case MAPPING_META_SHADOW_BEG: return Mapping::kMetaShadowBeg;
    case MAPPING_META_SHADOW_END: return Mapping::kMetaShadowEnd;
    case MAPPING_TRACE_BEG: return Mapping::kTraceMemBeg;
    case MAPPING_TRACE_END: return Mapping::kTraceMemEnd;
  }
}

template<int Type>
uptr MappingArchImpl(void) {
#ifdef __aarch64__
  if (vmaSize == 39)
    return MappingImpl<Mapping39, Type>();
  else
    return MappingImpl<Mapping42, Type>();
  DCHECK(0);
#else
  return MappingImpl<Mapping, Type>();
#endif
}

#ifndef SANITIZER_GO
ALWAYS_INLINE
uptr LoAppMemBeg(void) {
  return MappingArchImpl<MAPPING_LO_APP_BEG>();
}
ALWAYS_INLINE
uptr LoAppMemEnd(void) {
  return MappingArchImpl<MAPPING_LO_APP_END>();
}

ALWAYS_INLINE
uptr HeapMemBeg(void) {
  return MappingArchImpl<MAPPING_HEAP_BEG>();
}
ALWAYS_INLINE
uptr HeapMemEnd(void) {
  return MappingArchImpl<MAPPING_HEAP_END>();
}

ALWAYS_INLINE
uptr HiAppMemBeg(void) {
  return MappingArchImpl<MAPPING_HI_APP_BEG>();
}
ALWAYS_INLINE
uptr HiAppMemEnd(void) {
  return MappingArchImpl<MAPPING_HI_APP_END>();
}

ALWAYS_INLINE
uptr VdsoBeg(void) {
  return MappingArchImpl<MAPPING_VDSO_BEG>();
}

#else

ALWAYS_INLINE
uptr AppMemBeg(void) {
  return MappingArchImpl<MAPPING_APP_BEG>();
}
ALWAYS_INLINE
uptr AppMemEnd(void) {
  return MappingArchImpl<MAPPING_APP_END>();
}

#endif

static inline
bool GetUserRegion(int i, uptr *start, uptr *end) {
  switch (i) {
  default:
    return false;
#ifndef SANITIZER_GO
  case 0:
    *start = LoAppMemBeg();
    *end = LoAppMemEnd();
    return true;
  case 1:
    *start = HiAppMemBeg();
    *end = HiAppMemEnd();
    return true;
  case 2:
    *start = HeapMemBeg();
    *end = HeapMemEnd();
    return true;
#else
  case 0:
    *start = AppMemBeg();
    *end = AppMemEnd();
    return true;
#endif
  }
}

ALWAYS_INLINE
uptr ShadowBeg(void) {
  return MappingArchImpl<MAPPING_SHADOW_BEG>();
}
ALWAYS_INLINE
uptr ShadowEnd(void) {
  return MappingArchImpl<MAPPING_SHADOW_END>();
}

ALWAYS_INLINE
uptr MetaShadowBeg(void) {
  return MappingArchImpl<MAPPING_META_SHADOW_BEG>();
}
ALWAYS_INLINE
uptr MetaShadowEnd(void) {
  return MappingArchImpl<MAPPING_META_SHADOW_END>();
}

ALWAYS_INLINE
uptr TraceMemBeg(void) {
  return MappingArchImpl<MAPPING_TRACE_BEG>();
}
ALWAYS_INLINE
uptr TraceMemEnd(void) {
  return MappingArchImpl<MAPPING_TRACE_END>();
}


template<typename Mapping>
bool IsAppMemImpl(uptr mem) {
#ifndef SANITIZER_GO
  return (mem >= Mapping::kHeapMemBeg && mem < Mapping::kHeapMemEnd) ||
         (mem >= Mapping::kLoAppMemBeg && mem < Mapping::kLoAppMemEnd) ||
         (mem >= Mapping::kHiAppMemBeg && mem < Mapping::kHiAppMemEnd);
#else
  return mem >= Mapping::kAppMemBeg && mem < Mapping::kAppMemEnd;
#endif
}

ALWAYS_INLINE
bool IsAppMem(uptr mem) {
#ifdef __aarch64__
  if (vmaSize == 39)
    return IsAppMemImpl<Mapping39>(mem);
  else
    return IsAppMemImpl<Mapping42>(mem);
  DCHECK(0);
#else
  return IsAppMemImpl<Mapping>(mem);
#endif
}


template<typename Mapping>
bool IsShadowMemImpl(uptr mem) {
  return mem >= Mapping::kShadowBeg && mem <= Mapping::kShadowEnd;
}

ALWAYS_INLINE
bool IsShadowMem(uptr mem) {
#ifdef __aarch64__
  if (vmaSize == 39)
    return IsShadowMemImpl<Mapping39>(mem);
  else
    return IsShadowMemImpl<Mapping42>(mem);
  DCHECK(0);
#else
  return IsShadowMemImpl<Mapping>(mem);
#endif
}


template<typename Mapping>
bool IsMetaMemImpl(uptr mem) {
  return mem >= Mapping::kMetaShadowBeg && mem <= Mapping::kMetaShadowEnd;
}

ALWAYS_INLINE
bool IsMetaMem(uptr mem) {
#ifdef __aarch64__
  if (vmaSize == 39)
    return IsMetaMemImpl<Mapping39>(mem);
  else
    return IsMetaMemImpl<Mapping42>(mem);
  DCHECK(0);
#else
  return IsMetaMemImpl<Mapping>(mem);
#endif
}


template<typename Mapping>
uptr MemToShadowImpl(uptr x) {
  DCHECK(IsAppMem(x));
#ifndef SANITIZER_GO
  return (((x) & ~(Mapping::kAppMemMsk | (kShadowCell - 1)))
      ^ Mapping::kAppMemXor) * kShadowCnt;
#else
  return ((x & ~(kShadowCell - 1)) * kShadowCnt) | Mapping::kShadowBeg;
#endif
}

ALWAYS_INLINE
uptr MemToShadow(uptr x) {
#ifdef __aarch64__
  if (vmaSize == 39)
    return MemToShadowImpl<Mapping39>(x);
  else
    return MemToShadowImpl<Mapping42>(x);
  DCHECK(0);
#else
  return MemToShadowImpl<Mapping>(x);
#endif
}


template<typename Mapping>
u32 *MemToMetaImpl(uptr x) {
  DCHECK(IsAppMem(x));
#ifndef SANITIZER_GO
  return (u32*)(((((x) & ~(Mapping::kAppMemMsk | (kMetaShadowCell - 1)))
        ^ Mapping::kAppMemXor) / kMetaShadowCell * kMetaShadowSize)
          | Mapping::kMetaShadowBeg);
#else
  return (u32*)(((x & ~(kMetaShadowCell - 1)) / \
      kMetaShadowCell * kMetaShadowSize) | Mapping::kMetaShadowBeg);
#endif
}

ALWAYS_INLINE
u32 *MemToMeta(uptr x) {
#ifdef __aarch64__
  if (vmaSize == 39)
    return MemToMetaImpl<Mapping39>(x);
  else
    return MemToMetaImpl<Mapping42>(x);
  DCHECK(0);
#else
  return MemToMetaImpl<Mapping>(x);
#endif
}


template<typename Mapping>
uptr ShadowToMemImpl(uptr s) {
  DCHECK(IsShadowMem(s));
#ifndef SANITIZER_GO
  if (s >= MemToShadow(Mapping::kLoAppMemBeg)
      && s <= MemToShadow(Mapping::kLoAppMemEnd - 1))
    return (s / kShadowCnt) ^ Mapping::kAppMemXor;
  else
    return ((s / kShadowCnt) ^ Mapping::kAppMemXor) | Mapping::kAppMemMsk;
#else
# ifndef SANITIZER_WINDOWS
  return (s & ~Mapping::kShadowBeg) / kShadowCnt;
# else
  // FIXME(dvyukov): this is most likely wrong as the mapping is not bijection.
  return (s - Mapping::kShadowBeg) / kShadowCnt;
# endif // SANITIZER_WINDOWS
#endif
}

ALWAYS_INLINE
uptr ShadowToMem(uptr s) {
#ifdef __aarch64__
  if (vmaSize == 39)
    return ShadowToMemImpl<Mapping39>(s);
  else
    return ShadowToMemImpl<Mapping42>(s);
  DCHECK(0);
#else
  return ShadowToMemImpl<Mapping>(s);
#endif
}



// The additional page is to catch shadow stack overflow as paging fault.
// Windows wants 64K alignment for mmaps.
const uptr kTotalTraceSize = (kTraceSize * sizeof(Event) + sizeof(Trace)
    + (64 << 10) + (64 << 10) - 1) & ~((64 << 10) - 1);

template<typename Mapping>
uptr GetThreadTraceImpl(int tid) {
  uptr p = Mapping::kTraceMemBeg + (uptr)tid * kTotalTraceSize;
  DCHECK_LT(p, Mapping::kTraceMemEnd);
  return p;
}

ALWAYS_INLINE
uptr GetThreadTrace(int tid) {
#ifdef __aarch64__
  if (vmaSize == 39)
    return GetThreadTraceImpl<Mapping39>(tid);
  else
    return GetThreadTraceImpl<Mapping42>(tid);
  DCHECK(0);
#else
  return GetThreadTraceImpl<Mapping>(tid);
#endif
}


template<typename Mapping>
uptr GetThreadTraceHeaderImpl(int tid) {
  uptr p = Mapping::kTraceMemBeg + (uptr)tid * kTotalTraceSize
      + kTraceSize * sizeof(Event);
  DCHECK_LT(p, Mapping::kTraceMemEnd);
  return p;
}

ALWAYS_INLINE
uptr GetThreadTraceHeader(int tid) {
#ifdef __aarch64__
  if (vmaSize == 39)
    return GetThreadTraceHeaderImpl<Mapping39>(tid);
  else
    return GetThreadTraceHeaderImpl<Mapping42>(tid);
  DCHECK(0);
#else
  return GetThreadTraceHeaderImpl<Mapping>(tid);
#endif
}

void InitializePlatform();
void InitializePlatformEarly();
void CheckAndProtect();
void InitializeShadowMemoryPlatform();
void FlushShadowMemory();
void WriteMemoryProfile(char *buf, uptr buf_size, uptr nthread, uptr nlive);

// Says whether the addr relates to a global var.
// Guesses with high probability, may yield both false positives and negatives.
bool IsGlobalVar(uptr addr);
int ExtractResolvFDs(void *state, int *fds, int nfd);
int ExtractRecvmsgFDs(void *msg, int *fds, int nfd);

int call_pthread_cancel_with_cleanup(int(*fn)(void *c, void *m,
    void *abstime), void *c, void *m, void *abstime,
    void(*cleanup)(void *arg), void *arg);

void DestroyThreadState();

}  // namespace __tsan

#endif  // TSAN_PLATFORM_H
