; Test function attributes
; RUN: llvm-as < %s | llvm-dis | FileCheck %s
; RUN: verify-uselistorder %s -preserve-bc-use-list-order

; CHECK: define void @fn1() #0
define void @fn1() alwaysinline {
  ret void
}

; CHECK: define void @fn2() #1
define void @fn2() noinline {
  ret void
}

; CHECK: define void @fn3()
; CHECK-NOT: define void @fn3() #{{.*}}
define void @fn3() {
  ret void
}

; CHECK: attributes #0 = { alwaysinline }
; CHECK: attributes #1 = { noinline }
