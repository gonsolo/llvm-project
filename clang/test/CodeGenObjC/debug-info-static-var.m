// RUN: %clang_cc1 -g -emit-llvm -o - %s | FileCheck %s
// Radar 8801045
// Do not emit AT_MIPS_linkage_name for static variable i

// CHECK: metadata !"i", metadata !"i", metadata !""

@interface A {
}
-(void) foo;
@end

@implementation A 
-(void)foo {
  static int i = 1;
}
@end

