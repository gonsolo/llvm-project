//===-- sanitizer_allocator64_test.cc -------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
// Tests for sanitizer_allocator64.h.
//===----------------------------------------------------------------------===//
#include "sanitizer_common/sanitizer_allocator64.h"
#include "gtest/gtest.h"

#include <algorithm>
#include <vector>

TEST(SanitizerCommon, DefaultSizeClassMap) {
  typedef DefaultSizeClassMap SCMap;

#if 0
  for (uptr i = 0; i < SCMap::kNumClasses; i++) {
    // printf("% 3ld: % 5ld (%4lx);   ", i, SCMap::Size(i), SCMap::Size(i));
    printf("c%ld => %ld  ", i, SCMap::Size(i));
    if ((i % 8) == 7)
      printf("\n");
  }
  printf("\n");
#endif

  for (uptr c = 0; c < SCMap::kNumClasses; c++) {
    uptr s = SCMap::Size(c);
    CHECK_EQ(SCMap::ClassID(s), c);
    if (c != SCMap::kNumClasses - 1)
      CHECK_EQ(SCMap::ClassID(s + 1), c + 1);
    CHECK_EQ(SCMap::ClassID(s - 1), c);
    if (c)
      CHECK_GT(SCMap::Size(c), SCMap::Size(c-1));
  }
  CHECK_EQ(SCMap::ClassID(SCMap::kMaxSize + 1), 0);

  for (uptr s = 1; s <= SCMap::kMaxSize; s++) {
    uptr c = SCMap::ClassID(s);
    CHECK_LT(c, SCMap::kNumClasses);
    CHECK_GE(SCMap::Size(c), s);
    if (c > 0)
      CHECK_LT(SCMap::Size(c-1), s);
  }
}

TEST(SanitizerCommon, SizeClassAllocator64) {
  const uptr space_beg  = 0x600000000000ULL;
  const uptr space_size = 0x10000000000;  // 1T
  const uptr metadata_size = 16;
  typedef DefaultSizeClassMap SCMap;
  typedef SizeClassAllocator64<space_beg, space_size,
                               metadata_size, SCMap> Allocator;

  Allocator a;
  a.Init();

  static const uptr sizes[] = {1, 16, 30, 40, 100, 1000, 10000,
    50000, 60000, 100000, 300000, 500000, 1000000, 2000000};

  std::vector<void *> allocated;

  uptr last_total_allocated = 0;
  for (int i = 0; i < 5; i++) {
    // Allocate a bunch of chunks.
    for (uptr s = 0; s < sizeof(sizes) /sizeof(sizes[0]); s++) {
      uptr size = sizes[s];
      // printf("s = %ld\n", size);
      uptr n_iter = std::max((uptr)2, 1000000 / size);
      for (uptr i = 0; i < n_iter; i++) {
        void *x = a.Allocate(size);
        allocated.push_back(x);
        CHECK(a.PointerIsMine(x));
        uptr class_id = a.GetSizeClass(x);
        CHECK_EQ(class_id, SCMap::ClassID(size));
      }
    }
    // Deallocate all.
    for (uptr i = 0; i < allocated.size(); i++) {
      a.Deallocate(allocated[i]);
    }
    allocated.clear();
    uptr total_allocated = a.TotalMemoryUsedIncludingFreeLists();
    if (last_total_allocated == 0)
      last_total_allocated = total_allocated;
    CHECK_EQ(last_total_allocated, total_allocated);
  }

  a.TestOnlyUnmap();
}
