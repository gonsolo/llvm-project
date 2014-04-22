// RUN: %clang_asan -O2 %s -o %t
// We need replace_intrin=0 to avoid reporting errors in memcpy.
// RUN: ASAN_OPTIONS=replace_intrin=0:check_printf=1 not %t 2>&1 | FileCheck --check-prefix=CHECK-ON %s
// RUN: ASAN_OPTIONS=replace_intrin=0:check_printf=0 %t 2>&1 | FileCheck --check-prefix=CHECK-OFF %s
// RUN: ASAN_OPTIONS=replace_intrin=0 not %t 2>&1 | FileCheck --check-prefix=CHECK-ON %s

#include <stdio.h>
#include <string.h>
int main() {
  volatile char c = '0';
  volatile int x = 12;
  volatile float f = 1.239;
  volatile char s[] = "34";
  volatile char fmt[2];
  memcpy((char *)fmt, "%c %d %f %s\n", sizeof(fmt));
  printf((char *)fmt, c, x, f, s);
  return 0;
  // Check that format string is sanitized.
  // CHECK-ON: stack-buffer-overflow
  // CHECK-ON-NOT: 0 12 1.239 34
  // CHECK-OFF: 0
}
