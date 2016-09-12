; NOTE: Assertions have been autogenerated by utils/update_test_checks.py
; RUN: opt < %s -instcombine -S | FileCheck %s

; rdar://11748024

define i32 @a(i1 zeroext %x, i1 zeroext %y) {
; CHECK-LABEL: @a(
; CHECK-NEXT:    [[CONV3_NEG:%.*]] = sext i1 %y to i32
; CHECK-NEXT:    [[SUB:%.*]] = select i1 %x, i32 2, i32 1
; CHECK-NEXT:    [[ADD:%.*]] = add nsw i32 [[SUB]], [[CONV3_NEG]]
; CHECK-NEXT:    ret i32 [[ADD]]
;
  %conv = zext i1 %x to i32
  %conv3 = zext i1 %y to i32
  %conv3.neg = sub i32 0, %conv3
  %sub = add i32 %conv, 1
  %add = add i32 %sub, %conv3.neg
  ret i32 %add
}

define i32 @PR30273(i1 %a, i1 %b) {
; CHECK-LABEL: @PR30273(
; CHECK-NEXT:    [[ZEXT:%.*]] = zext i1 %a to i32
; CHECK-NEXT:    [[SEL1:%.*]] = select i1 %a, i32 2, i32 1
; CHECK-NEXT:    [[SEL2:%.*]] = select i1 %b, i32 [[SEL1]], i32 [[ZEXT]]
; CHECK-NEXT:    ret i32 [[SEL2]]
;
  %zext = zext i1 %a to i32
  %sel1 = select i1 %a, i32 2, i32 1
  %sel2 = select i1 %b, i32 %sel1, i32 %zext
  ret i32 %sel2
}

