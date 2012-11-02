; RUN: opt %loadPolly %defaultOpts -polly-cloog -analyze  -S %s | FileCheck %s

;#define N 20
;#include "limits.h"
;
;int main () {
;  int i;
;  int A[N];
;
;  A[0] = 0;
;
;  __sync_synchronize();
;
;  for (i = 0; i < INT_MAX; i++)
;    A[0] = i;
;
;  __sync_synchronize();
;
;  if (A[0] == INT_MAX - 1)
;    return 0;
;  else
;    return 1;
;}

target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64"
target triple = "x86_64-unknown-linux-gnu"

define i32 @main() nounwind {
entry:
  %A = alloca [20 x i32], align 4                 ; <[20 x i32]*> [#uses=3]
  %arraydecay = getelementptr inbounds [20 x i32]* %A, i32 0, i32 0 ; <i32*> [#uses=1]
  %arrayidx = getelementptr inbounds i32* %arraydecay, i64 0 ; <i32*> [#uses=1]
  store i32 0, i32* %arrayidx
  fence seq_cst
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %0 = phi i32 [ 0, %entry ], [ %inc, %for.inc ]  ; <i32> [#uses=3]
  %exitcond = icmp ne i32 %0, 2147483647          ; <i1> [#uses=1]
  br i1 %exitcond, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  %arraydecay2 = getelementptr inbounds [20 x i32]* %A, i32 0, i32 0 ; <i32*> [#uses=1]
  %arrayidx3 = getelementptr inbounds i32* %arraydecay2, i64 0 ; <i32*> [#uses=1]
  store i32 %0, i32* %arrayidx3
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %inc = add nsw i32 %0, 1                        ; <i32> [#uses=1]
  br label %for.cond

for.end:                                          ; preds = %for.cond
  fence seq_cst
  %arraydecay5 = getelementptr inbounds [20 x i32]* %A, i32 0, i32 0 ; <i32*> [#uses=1]
  %arrayidx6 = getelementptr inbounds i32* %arraydecay5, i64 0 ; <i32*> [#uses=1]
  %tmp7 = load i32* %arrayidx6                    ; <i32> [#uses=1]
  %cmp8 = icmp eq i32 %tmp7, 2147483646           ; <i1> [#uses=1]
  br i1 %cmp8, label %if.then, label %if.else

if.then:                                          ; preds = %for.end
  br label %return

if.else:                                          ; preds = %for.end
  br label %return

return:                                           ; preds = %if.else, %if.then
  %retval.0 = phi i32 [ 0, %if.then ], [ 1, %if.else ] ; <i32> [#uses=1]
  ret i32 %retval.0
}

; CHECK:for (c2=0;c2<=2147483646;c2++) {
