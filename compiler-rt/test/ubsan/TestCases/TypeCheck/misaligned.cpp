// RUN: %clangxx -fsanitize=alignment -g %s -O3 -o %t
// RUN: %run %t l0 && %run %t s0 && %run %t r0 && %run %t m0 && %run %t f0 && %run %t n0
// RUN: %run %t l1 2>&1 | FileCheck %s --check-prefix=CHECK-LOAD --strict-whitespace
// RUN: %run %t s1 2>&1 | FileCheck %s --check-prefix=CHECK-STORE
// RUN: %run %t r1 2>&1 | FileCheck %s --check-prefix=CHECK-REFERENCE
// RUN: %run %t m1 2>&1 | FileCheck %s --check-prefix=CHECK-MEMBER
// RUN: %run %t f1 2>&1 | FileCheck %s --check-prefix=CHECK-MEMFUN
// RUN: %run %t n1 2>&1 | FileCheck %s --check-prefix=CHECK-NEW
// RUN: UBSAN_OPTIONS=print_stacktrace=1 %run %t l1 2>&1 | FileCheck %s --check-prefix=CHECK-LOAD --check-prefix=CHECK-STACK-LOAD

#include <new>

struct S {
  S() {}
  int f() { return 0; }
  int k;
};

int main(int, char **argv) {
  char c[] __attribute__((aligned(8))) = { 0, 0, 0, 0, 1, 2, 3, 4, 5 };

  // Pointer value may be unspecified here, but behavior is not undefined.
  int *p = (int*)&c[4 + argv[1][1] - '0'];
  S *s = (S*)p;

  (void)*p; // ok!

  switch (argv[1][0]) {
  case 'l':
    // CHECK-LOAD: misaligned.cpp:[[@LINE+4]]:12: runtime error: load of misaligned address [[PTR:0x[0-9a-f]*]] for type 'int', which requires 4 byte alignment
    // CHECK-LOAD-NEXT: [[PTR]]: note: pointer points here
    // CHECK-LOAD-NEXT: {{^ 00 00 00 01 02 03 04  05}}
    // CHECK-LOAD-NEXT: {{^             \^}}
    return *p && 0;
    // CHECK-STACK-LOAD: #0 {{.*}} in main{{.*}}misaligned.cpp

  case 's':
    // CHECK-STORE: misaligned.cpp:[[@LINE+4]]:5: runtime error: store to misaligned address [[PTR:0x[0-9a-f]*]] for type 'int', which requires 4 byte alignment
    // CHECK-STORE-NEXT: [[PTR]]: note: pointer points here
    // CHECK-STORE-NEXT: {{^ 00 00 00 01 02 03 04  05}}
    // CHECK-STORE-NEXT: {{^             \^}}
    *p = 1;
    break;

  case 'r':
    // CHECK-REFERENCE: misaligned.cpp:[[@LINE+4]]:15: runtime error: reference binding to misaligned address [[PTR:0x[0-9a-f]*]] for type 'int', which requires 4 byte alignment
    // CHECK-REFERENCE-NEXT: [[PTR]]: note: pointer points here
    // CHECK-REFERENCE-NEXT: {{^ 00 00 00 01 02 03 04  05}}
    // CHECK-REFERENCE-NEXT: {{^             \^}}
    {int &r = *p;}
    break;

  case 'm':
    // CHECK-MEMBER: misaligned.cpp:[[@LINE+4]]:15: runtime error: member access within misaligned address [[PTR:0x[0-9a-f]*]] for type 'S', which requires 4 byte alignment
    // CHECK-MEMBER-NEXT: [[PTR]]: note: pointer points here
    // CHECK-MEMBER-NEXT: {{^ 00 00 00 01 02 03 04  05}}
    // CHECK-MEMBER-NEXT: {{^             \^}}
    return s->k && 0;

  case 'f':
    // CHECK-MEMFUN: misaligned.cpp:[[@LINE+4]]:12: runtime error: member call on misaligned address [[PTR:0x[0-9a-f]*]] for type 'S', which requires 4 byte alignment
    // CHECK-MEMFUN-NEXT: [[PTR]]: note: pointer points here
    // CHECK-MEMFUN-NEXT: {{^ 00 00 00 01 02 03 04  05}}
    // CHECK-MEMFUN-NEXT: {{^             \^}}
    return s->f() && 0;

  case 'n':
    // CHECK-NEW: misaligned.cpp:[[@LINE+4]]:5: runtime error: constructor call on misaligned address [[PTR:0x[0-9a-f]*]] for type 'S', which requires 4 byte alignment
    // CHECK-NEW-NEXT: [[PTR]]: note: pointer points here
    // CHECK-NEW-NEXT: {{^ 00 00 00 01 02 03 04  05}}
    // CHECK-NEW-NEXT: {{^             \^}}
    return (new (s) S)->k && 0;
  }
}
