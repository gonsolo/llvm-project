// RUN: llvm-mc -filetype=obj -triple=x86_64-pc-linux %s -o %t
// RUN: llvm-mc -filetype=obj -triple=x86_64-pc-linux %p/Inputs/visibility.s -o %t2
// RUN: lld -flavor gnu2 %t %t2 -o %t3
// RUN: llvm-readobj -t %t3 | FileCheck %s
// REQUIRES: x86

// CHECK:      Symbols [
// CHECK-NEXT:   Symbol {
// CHECK-NEXT:     Name:
// CHECK-NEXT:     Value: 0x0
// CHECK-NEXT:     Size: 0
// CHECK-NEXT:     Binding: Local
// CHECK-NEXT:     Type: None
// CHECK-NEXT:     Other: 0
// CHECK-NEXT:     Section: Undefined
// CHECK-NEXT:   }
// CHECK-NEXT:   Symbol {
// CHECK-NEXT:     Name: _start
// CHECK-NEXT:     Value: 0x11000
// CHECK-NEXT:     Size: 0
// CHECK-NEXT:     Binding: Global
// CHECK-NEXT:     Type: None
// CHECK-NEXT:     Other: 0
// CHECK-NEXT:     Section: .text
// CHECK-NEXT:   }
// CHECK-NEXT:   Symbol {
// CHECK-NEXT:     Name: default
// CHECK-NEXT:     Value: 0x11000
// CHECK-NEXT:     Size: 0
// CHECK-NEXT:     Binding: Global
// CHECK-NEXT:     Type: None
// CHECK-NEXT:     Other: 0
// CHECK-NEXT:     Section: .text
// CHECK-NEXT:   }
// CHECK-NEXT:   Symbol {
// CHECK-NEXT:     Name: protected
// CHECK-NEXT:     Value: 0x11000
// CHECK-NEXT:     Size: 0
// CHECK-NEXT:     Binding: Global
// CHECK-NEXT:     Type: None
// CHECK-NEXT:     Other: 3
// CHECK-NEXT:     Section: .text
// CHECK-NEXT:   }
// CHECK-NEXT: ]

.global _start
_start:

.global default
default:

.global protected
protected:

.global hidden
hidden:

.global internal
internal:

.global protected_with_hidden
.protected
protected_with_hidden:
