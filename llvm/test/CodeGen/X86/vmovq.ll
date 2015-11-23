; RUN: llc < %s -mtriple=x86_64-unknown-unknown -mattr=sse4.1 | FileCheck %s --check-prefix=SSE
; RUN: llc < %s -mtriple=x86_64-unknown-unknown -mattr=avx    | FileCheck %s --check-prefix=AVX

declare i32 @llvm.x86.sse41.ptestz(<2 x i64>, <2 x i64>)

define i1 @PR25554(<2 x i64> %v0, <2 x i64> %v1) {
; SSE-LABEL: PR25554:
; SSE:       # BB#0:
; SSE-NEXT:    movl $1, %eax
; SSE-NEXT:    movd %rax, %xmm2
; SSE-NEXT:    ptest %xmm2, %xmm0
; SSE-NEXT:    sete %cl
; SSE-NEXT:    movzbl %cl, %ecx
; SSE-NEXT:    movd %rax, %xmm0
; SSE-NEXT:    pslldq {{.*#+}} xmm0 = zero,zero,zero,zero,zero,zero,zero,zero,xmm0[0,1,2,3,4,5,6,7]
; SSE-NEXT:    ptest %xmm0, %xmm1
; SSE-NEXT:    sete %al
; SSE-NEXT:    movzbl %al, %eax
; SSE-NEXT:    orl %ecx, %eax
; SSE-NEXT:    sete %al
; SSE-NEXT:    retq
;
; AVX-LABEL: PR25554:
; AVX:       # BB#0:
; AVX-NEXT:    movl $1, %eax
; AVX-NEXT:    vmovq %rax, %xmm2
; AVX-NEXT:    vptest %xmm2, %xmm0
; AVX-NEXT:    sete %cl
; AVX-NEXT:    movzbl %cl, %ecx
; AVX-NEXT:    vmovq %rax, %xmm0
; AVX-NEXT:    vpslldq {{.*#+}} xmm0 = zero,zero,zero,zero,zero,zero,zero,zero,xmm0[0,1,2,3,4,5,6,7]
; AVX-NEXT:    vptest %xmm0, %xmm1
; AVX-NEXT:    sete %al
; AVX-NEXT:    movzbl %al, %eax
; AVX-NEXT:    orl %ecx, %eax
; AVX-NEXT:    sete %al
; AVX-NEXT:    retq

  %c1 = tail call i32 @llvm.x86.sse41.ptestz(<2 x i64> %v0, <2 x i64> <i64 1, i64 0>)
  %b1 = icmp eq i32 %c1, 0
  %c2 = tail call i32 @llvm.x86.sse41.ptestz(<2 x i64> %v1, <2 x i64> <i64 0, i64 1>)
  %b2 = icmp eq i32 %c2, 0
  %and = and i1 %b1, %b2
  ret i1 %and
}

