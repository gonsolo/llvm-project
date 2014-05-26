// RUN: %clangxx_asan -O0 %s -Fe%t
// RUN: not %run %t 2>&1 | FileCheck %s

#include <windows.h>
#include <stdio.h>

extern "C" const char *foo = "foobarspam";

int main(void) {
  if (foo[16])
    printf("Boo\n");
// CHECK-NOT: Boo
// CHECK: AddressSanitizer: global-buffer-overflow on address [[ADDR:0x[0-9a-f]+]]
// CHECK: READ of size 1 at [[ADDR]] thread T0
// CHECK-NEXT:   {{#0 .* main .*global_const_string_oob.cc:}}[[@LINE-5]]
// CHECK: [[ADDR]] is located 5 bytes to the right of global variable [[STR:.*]] from {{'.*global_const_string_oob.cc' .*}} of size 11
// CHECK:   [[STR]] is ascii string 'foobarspam'
  return 0;
}

