// Check that we can store lots of stack frames if asked to.

// RUN: %clangxx_asan -m64 -O0 %s -o %t 2>&1
// RUN: ASAN_OPTIONS=malloc_context_size=120:redzone=512 %t 2>&1 | \
// RUN: %symbolize | FileCheck %s

// RUN: %clangxx_asan -m32 -O0 %s -o %t 2>&1
// RUN: ASAN_OPTIONS=malloc_context_size=120:redzone=512 %t 2>&1 | \
// RUN: %symbolize | FileCheck %s
#include <stdlib.h>
#include <stdio.h>

template <int depth>
struct DeepFree {
  static void free(char *x) {
    DeepFree<depth - 1>::free(x);
  }
};

template<>
struct DeepFree<0> {
  static void free(char *x) {
    ::free(x);
  }
};

int main() {
  char *x = new char[10];
  // deep_free(x);
  DeepFree<200>::free(x);
  return x[5];
  // CHECK: {{.*ERROR: AddressSanitizer: heap-use-after-free on address}}
  // CHECK: DeepFree<36>
  // CHECK: DeepFree<98>
  // CHECK: DeepFree<115>
}
