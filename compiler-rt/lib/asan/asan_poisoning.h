//===-- asan_poisoning.h ----------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file is a part of AddressSanitizer, an address sanity checker.
//
// Shadow memory poisoning by ASan RTL and by user application.
//===----------------------------------------------------------------------===//

#include "asan_interceptors.h"
#include "asan_internal.h"
#include "asan_mapping.h"
#include "sanitizer_common/sanitizer_flags.h"

namespace __asan {

// Poisons the shadow memory for "size" bytes starting from "addr".
void PoisonShadow(uptr addr, uptr size, u8 value);

// Poisons the shadow memory for "redzone_size" bytes starting from
// "addr + size".
void PoisonShadowPartialRightRedzone(uptr addr,
                                     uptr size,
                                     uptr redzone_size,
                                     u8 value);

// Fast versions of PoisonShadow and PoisonShadowPartialRightRedzone that
// assume that memory addresses are properly aligned. Use in
// performance-critical code with care.
ALWAYS_INLINE void FastPoisonShadow(uptr aligned_beg, uptr aligned_size,
                                    u8 value) {
  DCHECK(flags()->poison_heap);
  uptr PageSize = GetPageSizeCached();
  uptr shadow_beg = MEM_TO_SHADOW(aligned_beg);
  uptr shadow_end = MEM_TO_SHADOW(
      aligned_beg + aligned_size - SHADOW_GRANULARITY) + 1;
  if (value ||
      shadow_end - shadow_beg < common_flags()->clear_shadow_mmap_threshold) {
    REAL(memset)((void*)shadow_beg, value, shadow_end - shadow_beg);
  } else {
    uptr page_beg = RoundUpTo(shadow_beg, PageSize);
    uptr page_end = RoundDownTo(shadow_end, PageSize);

    if (page_beg >= page_end) {
      REAL(memset)((void *)shadow_beg, 0, shadow_end - shadow_beg);
    } else {
      if (page_beg != shadow_beg) {
        REAL(memset)((void *)shadow_beg, 0, page_beg - shadow_beg);
      }
      if (page_end != shadow_end) {
        REAL(memset)((void *)page_end, 0, shadow_end - page_end);
      }
      void *res = MmapFixedNoReserve(page_beg, page_end - page_beg);
      CHECK_EQ(page_beg, res);
    }
  }
}

ALWAYS_INLINE void FastPoisonShadowPartialRightRedzone(
    uptr aligned_addr, uptr size, uptr redzone_size, u8 value) {
  DCHECK(flags()->poison_heap);
  bool poison_partial = flags()->poison_partial;
  u8 *shadow = (u8*)MEM_TO_SHADOW(aligned_addr);
  for (uptr i = 0; i < redzone_size; i += SHADOW_GRANULARITY, shadow++) {
    if (i + SHADOW_GRANULARITY <= size) {
      *shadow = 0;  // fully addressable
    } else if (i >= size) {
      *shadow = (SHADOW_GRANULARITY == 128) ? 0xff : value;  // unaddressable
    } else {
      // first size-i bytes are addressable
      *shadow = poison_partial ? static_cast<u8>(size - i) : 0;
    }
  }
}

// Calls __sanitizer::FlushUnneededShadowMemory() on
// [MemToShadow(p), MemToShadow(p+size)] with proper rounding.
void FlushUnneededASanShadowMemory(uptr p, uptr size);

}  // namespace __asan
