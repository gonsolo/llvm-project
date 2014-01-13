// Test this without pch.
// RUN: %clang_cc1 -include %S/objcxx-ivar-class.h -cxx-abi itanium %s -emit-llvm -o - | FileCheck %s

// Test with pch.
// RUN: %clang_cc1 -x objective-c++-header -cxx-abi itanium -emit-pch -o %t %S/objcxx-ivar-class.h
// RUN: %clang_cc1 -include-pch %t -cxx-abi itanium %s -emit-llvm -o - | FileCheck %s

// CHECK: [C position]
// CHECK: call {{.*}} @_ZN1SC1ERKS_

// CHECK: [C setPosition:]
// CHECK: = call {{.*}}%struct.S* @_ZN1SaSERKS_

// CHECK: [C .cxx_destruct]
// CHECK: [C .cxx_construct]
