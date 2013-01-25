// Check that -iwithprefix falls into the "after" search list.
//
// RUN: rm -rf %t.tmps
// RUN: mkdir -p %t.tmps/first %t.tmps/second
// RUN: %clang_cc1 \
// RUN:   -iprefix %t.tmps/ -iwithprefix second \
// RUN:    -isystem %t.tmps/first -v 2> %t.out
// RUN: FileCheck < %t.out %s

// CHECK: #include <...> search starts here:
// CHECK: {{.*}}.tmps/first
// CHECK: /usr/include
// CHECK: {{.*}}.tmps/second
// CHECK-NOT: {{.*}}.tmps


