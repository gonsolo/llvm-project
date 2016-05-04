; NOTE: Assertions have been autogenerated by update_llc_test_checks.py
; RUN: llc < %s -mtriple=i386-apple-darwin -mattr=-avx,+sse | FileCheck %s --check-prefix=SSE
; RUN: llc < %s -mtriple=i386-apple-darwin -mcpu=knl | FileCheck %s --check-prefix=KNL

define <4 x float> @test_x86_sse_add_ss(<4 x float> %a0, <4 x float> %a1) {
; SSE-LABEL: test_x86_sse_add_ss:
; SSE:       ## BB#0:
; SSE-NEXT:    addss %xmm1, %xmm0
; SSE-NEXT:    retl
;
; KNL-LABEL: test_x86_sse_add_ss:
; KNL:       ## BB#0:
; KNL-NEXT:    vaddss %xmm1, %xmm0, %xmm0
; KNL-NEXT:    retl
  %res = call <4 x float> @llvm.x86.sse.add.ss(<4 x float> %a0, <4 x float> %a1) ; <<4 x float>> [#uses=1]
  ret <4 x float> %res
}
declare <4 x float> @llvm.x86.sse.add.ss(<4 x float>, <4 x float>) nounwind readnone


define <4 x float> @test_x86_sse_cmp_ps(<4 x float> %a0, <4 x float> %a1) {
; SSE-LABEL: test_x86_sse_cmp_ps:
; SSE:       ## BB#0:
; SSE-NEXT:    cmpordps %xmm1, %xmm0
; SSE-NEXT:    retl
;
; KNL-LABEL: test_x86_sse_cmp_ps:
; KNL:       ## BB#0:
; KNL-NEXT:    vcmpordps %xmm1, %xmm0, %xmm0
; KNL-NEXT:    retl
  %res = call <4 x float> @llvm.x86.sse.cmp.ps(<4 x float> %a0, <4 x float> %a1, i8 7) ; <<4 x float>> [#uses=1]
  ret <4 x float> %res
}
declare <4 x float> @llvm.x86.sse.cmp.ps(<4 x float>, <4 x float>, i8) nounwind readnone


define <4 x float> @test_x86_sse_cmp_ss(<4 x float> %a0, <4 x float> %a1) {
; SSE-LABEL: test_x86_sse_cmp_ss:
; SSE:       ## BB#0:
; SSE-NEXT:    cmpordss %xmm1, %xmm0
; SSE-NEXT:    retl
;
; KNL-LABEL: test_x86_sse_cmp_ss:
; KNL:       ## BB#0:
; KNL-NEXT:    vcmpordss %xmm1, %xmm0, %xmm0
; KNL-NEXT:    retl
  %res = call <4 x float> @llvm.x86.sse.cmp.ss(<4 x float> %a0, <4 x float> %a1, i8 7) ; <<4 x float>> [#uses=1]
  ret <4 x float> %res
}
declare <4 x float> @llvm.x86.sse.cmp.ss(<4 x float>, <4 x float>, i8) nounwind readnone


define i32 @test_x86_sse_comieq_ss(<4 x float> %a0, <4 x float> %a1) {
; SSE-LABEL: test_x86_sse_comieq_ss:
; SSE:       ## BB#0:
; SSE-NEXT:    comiss %xmm1, %xmm0
; SSE-NEXT:    sete %al
; SSE-NEXT:    movzbl %al, %eax
; SSE-NEXT:    retl
;
; KNL-LABEL: test_x86_sse_comieq_ss:
; KNL:       ## BB#0:
; KNL-NEXT:    vcomiss %xmm1, %xmm0
; KNL-NEXT:    sete %al
; KNL-NEXT:    movzbl %al, %eax
; KNL-NEXT:    retl
  %res = call i32 @llvm.x86.sse.comieq.ss(<4 x float> %a0, <4 x float> %a1) ; <i32> [#uses=1]
  ret i32 %res
}
declare i32 @llvm.x86.sse.comieq.ss(<4 x float>, <4 x float>) nounwind readnone


define i32 @test_x86_sse_comige_ss(<4 x float> %a0, <4 x float> %a1) {
; SSE-LABEL: test_x86_sse_comige_ss:
; SSE:       ## BB#0:
; SSE-NEXT:    comiss %xmm1, %xmm0
; SSE-NEXT:    setae %al
; SSE-NEXT:    movzbl %al, %eax
; SSE-NEXT:    retl
;
; KNL-LABEL: test_x86_sse_comige_ss:
; KNL:       ## BB#0:
; KNL-NEXT:    vcomiss %xmm1, %xmm0
; KNL-NEXT:    setae %al
; KNL-NEXT:    movzbl %al, %eax
; KNL-NEXT:    retl
  %res = call i32 @llvm.x86.sse.comige.ss(<4 x float> %a0, <4 x float> %a1) ; <i32> [#uses=1]
  ret i32 %res
}
declare i32 @llvm.x86.sse.comige.ss(<4 x float>, <4 x float>) nounwind readnone


define i32 @test_x86_sse_comigt_ss(<4 x float> %a0, <4 x float> %a1) {
; SSE-LABEL: test_x86_sse_comigt_ss:
; SSE:       ## BB#0:
; SSE-NEXT:    comiss %xmm1, %xmm0
; SSE-NEXT:    seta %al
; SSE-NEXT:    movzbl %al, %eax
; SSE-NEXT:    retl
;
; KNL-LABEL: test_x86_sse_comigt_ss:
; KNL:       ## BB#0:
; KNL-NEXT:    vcomiss %xmm1, %xmm0
; KNL-NEXT:    seta %al
; KNL-NEXT:    movzbl %al, %eax
; KNL-NEXT:    retl
  %res = call i32 @llvm.x86.sse.comigt.ss(<4 x float> %a0, <4 x float> %a1) ; <i32> [#uses=1]
  ret i32 %res
}
declare i32 @llvm.x86.sse.comigt.ss(<4 x float>, <4 x float>) nounwind readnone


define i32 @test_x86_sse_comile_ss(<4 x float> %a0, <4 x float> %a1) {
; SSE-LABEL: test_x86_sse_comile_ss:
; SSE:       ## BB#0:
; SSE-NEXT:    comiss %xmm1, %xmm0
; SSE-NEXT:    setbe %al
; SSE-NEXT:    movzbl %al, %eax
; SSE-NEXT:    retl
;
; KNL-LABEL: test_x86_sse_comile_ss:
; KNL:       ## BB#0:
; KNL-NEXT:    vcomiss %xmm1, %xmm0
; KNL-NEXT:    setbe %al
; KNL-NEXT:    movzbl %al, %eax
; KNL-NEXT:    retl
  %res = call i32 @llvm.x86.sse.comile.ss(<4 x float> %a0, <4 x float> %a1) ; <i32> [#uses=1]
  ret i32 %res
}
declare i32 @llvm.x86.sse.comile.ss(<4 x float>, <4 x float>) nounwind readnone


define i32 @test_x86_sse_comilt_ss(<4 x float> %a0, <4 x float> %a1) {
; SSE-LABEL: test_x86_sse_comilt_ss:
; SSE:       ## BB#0:
; SSE-NEXT:    comiss %xmm1, %xmm0
; SSE-NEXT:    sbbl %eax, %eax
; SSE-NEXT:    andl $1, %eax
; SSE-NEXT:    retl
;
; KNL-LABEL: test_x86_sse_comilt_ss:
; KNL:       ## BB#0:
; KNL-NEXT:    vcomiss %xmm1, %xmm0
; KNL-NEXT:    sbbl %eax, %eax
; KNL-NEXT:    andl $1, %eax
; KNL-NEXT:    retl
  %res = call i32 @llvm.x86.sse.comilt.ss(<4 x float> %a0, <4 x float> %a1) ; <i32> [#uses=1]
  ret i32 %res
}
declare i32 @llvm.x86.sse.comilt.ss(<4 x float>, <4 x float>) nounwind readnone


define i32 @test_x86_sse_comineq_ss(<4 x float> %a0, <4 x float> %a1) {
; SSE-LABEL: test_x86_sse_comineq_ss:
; SSE:       ## BB#0:
; SSE-NEXT:    comiss %xmm1, %xmm0
; SSE-NEXT:    setne %al
; SSE-NEXT:    movzbl %al, %eax
; SSE-NEXT:    retl
;
; KNL-LABEL: test_x86_sse_comineq_ss:
; KNL:       ## BB#0:
; KNL-NEXT:    vcomiss %xmm1, %xmm0
; KNL-NEXT:    setne %al
; KNL-NEXT:    movzbl %al, %eax
; KNL-NEXT:    retl
  %res = call i32 @llvm.x86.sse.comineq.ss(<4 x float> %a0, <4 x float> %a1) ; <i32> [#uses=1]
  ret i32 %res
}
declare i32 @llvm.x86.sse.comineq.ss(<4 x float>, <4 x float>) nounwind readnone


define <4 x float> @test_x86_sse_cvtsi2ss(<4 x float> %a0) {
; SSE-LABEL: test_x86_sse_cvtsi2ss:
; SSE:       ## BB#0:
; SSE-NEXT:    movl $7, %eax
; SSE-NEXT:    cvtsi2ssl %eax, %xmm0
; SSE-NEXT:    retl
;
; KNL-LABEL: test_x86_sse_cvtsi2ss:
; KNL:       ## BB#0:
; KNL-NEXT:    movl $7, %eax
; KNL-NEXT:    vcvtsi2ssl %eax, %xmm0, %xmm0
; KNL-NEXT:    retl
  %res = call <4 x float> @llvm.x86.sse.cvtsi2ss(<4 x float> %a0, i32 7) ; <<4 x float>> [#uses=1]
  ret <4 x float> %res
}
declare <4 x float> @llvm.x86.sse.cvtsi2ss(<4 x float>, i32) nounwind readnone


define i32 @test_x86_sse_cvtss2si(<4 x float> %a0) {
; SSE-LABEL: test_x86_sse_cvtss2si:
; SSE:       ## BB#0:
; SSE-NEXT:    cvtss2si %xmm0, %eax
; SSE-NEXT:    retl
;
; KNL-LABEL: test_x86_sse_cvtss2si:
; KNL:       ## BB#0:
; KNL-NEXT:    vcvtss2si %xmm0, %eax
; KNL-NEXT:    retl
  %res = call i32 @llvm.x86.sse.cvtss2si(<4 x float> %a0) ; <i32> [#uses=1]
  ret i32 %res
}
declare i32 @llvm.x86.sse.cvtss2si(<4 x float>) nounwind readnone


define i32 @test_x86_sse_cvttss2si(<4 x float> %a0) {
; SSE-LABEL: test_x86_sse_cvttss2si:
; SSE:       ## BB#0:
; SSE-NEXT:    cvttss2si %xmm0, %eax
; SSE-NEXT:    retl
;
; KNL-LABEL: test_x86_sse_cvttss2si:
; KNL:       ## BB#0:
; KNL-NEXT:    vcvttss2si %xmm0, %eax
; KNL-NEXT:    retl
  %res = call i32 @llvm.x86.sse.cvttss2si(<4 x float> %a0) ; <i32> [#uses=1]
  ret i32 %res
}
declare i32 @llvm.x86.sse.cvttss2si(<4 x float>) nounwind readnone


define <4 x float> @test_x86_sse_div_ss(<4 x float> %a0, <4 x float> %a1) {
; SSE-LABEL: test_x86_sse_div_ss:
; SSE:       ## BB#0:
; SSE-NEXT:    divss %xmm1, %xmm0
; SSE-NEXT:    retl
;
; KNL-LABEL: test_x86_sse_div_ss:
; KNL:       ## BB#0:
; KNL-NEXT:    vdivss %xmm1, %xmm0, %xmm0
; KNL-NEXT:    retl
  %res = call <4 x float> @llvm.x86.sse.div.ss(<4 x float> %a0, <4 x float> %a1) ; <<4 x float>> [#uses=1]
  ret <4 x float> %res
}
declare <4 x float> @llvm.x86.sse.div.ss(<4 x float>, <4 x float>) nounwind readnone


define void @test_x86_sse_ldmxcsr(i8* %a0) {
; SSE-LABEL: test_x86_sse_ldmxcsr:
; SSE:       ## BB#0:
; SSE-NEXT:    movl {{[0-9]+}}(%esp), %eax
; SSE-NEXT:    ldmxcsr (%eax)
; SSE-NEXT:    retl
;
; KNL-LABEL: test_x86_sse_ldmxcsr:
; KNL:       ## BB#0:
; KNL-NEXT:    movl {{[0-9]+}}(%esp), %eax
; KNL-NEXT:    vldmxcsr (%eax)
; KNL-NEXT:    retl
  call void @llvm.x86.sse.ldmxcsr(i8* %a0)
  ret void
}
declare void @llvm.x86.sse.ldmxcsr(i8*) nounwind



define <4 x float> @test_x86_sse_max_ps(<4 x float> %a0, <4 x float> %a1) {
; SSE-LABEL: test_x86_sse_max_ps:
; SSE:       ## BB#0:
; SSE-NEXT:    maxps %xmm1, %xmm0
; SSE-NEXT:    retl
;
; KNL-LABEL: test_x86_sse_max_ps:
; KNL:       ## BB#0:
; KNL-NEXT:    vmaxps %xmm1, %xmm0, %xmm0
; KNL-NEXT:    retl
  %res = call <4 x float> @llvm.x86.sse.max.ps(<4 x float> %a0, <4 x float> %a1) ; <<4 x float>> [#uses=1]
  ret <4 x float> %res
}
declare <4 x float> @llvm.x86.sse.max.ps(<4 x float>, <4 x float>) nounwind readnone


define <4 x float> @test_x86_sse_max_ss(<4 x float> %a0, <4 x float> %a1) {
; SSE-LABEL: test_x86_sse_max_ss:
; SSE:       ## BB#0:
; SSE-NEXT:    maxss %xmm1, %xmm0
; SSE-NEXT:    retl
;
; KNL-LABEL: test_x86_sse_max_ss:
; KNL:       ## BB#0:
; KNL-NEXT:    vmaxss %xmm1, %xmm0, %xmm0
; KNL-NEXT:    retl
  %res = call <4 x float> @llvm.x86.sse.max.ss(<4 x float> %a0, <4 x float> %a1) ; <<4 x float>> [#uses=1]
  ret <4 x float> %res
}
declare <4 x float> @llvm.x86.sse.max.ss(<4 x float>, <4 x float>) nounwind readnone


define <4 x float> @test_x86_sse_min_ps(<4 x float> %a0, <4 x float> %a1) {
; SSE-LABEL: test_x86_sse_min_ps:
; SSE:       ## BB#0:
; SSE-NEXT:    minps %xmm1, %xmm0
; SSE-NEXT:    retl
;
; KNL-LABEL: test_x86_sse_min_ps:
; KNL:       ## BB#0:
; KNL-NEXT:    vminps %xmm1, %xmm0, %xmm0
; KNL-NEXT:    retl
  %res = call <4 x float> @llvm.x86.sse.min.ps(<4 x float> %a0, <4 x float> %a1) ; <<4 x float>> [#uses=1]
  ret <4 x float> %res
}
declare <4 x float> @llvm.x86.sse.min.ps(<4 x float>, <4 x float>) nounwind readnone


define <4 x float> @test_x86_sse_min_ss(<4 x float> %a0, <4 x float> %a1) {
; SSE-LABEL: test_x86_sse_min_ss:
; SSE:       ## BB#0:
; SSE-NEXT:    minss %xmm1, %xmm0
; SSE-NEXT:    retl
;
; KNL-LABEL: test_x86_sse_min_ss:
; KNL:       ## BB#0:
; KNL-NEXT:    vminss %xmm1, %xmm0, %xmm0
; KNL-NEXT:    retl
  %res = call <4 x float> @llvm.x86.sse.min.ss(<4 x float> %a0, <4 x float> %a1) ; <<4 x float>> [#uses=1]
  ret <4 x float> %res
}
declare <4 x float> @llvm.x86.sse.min.ss(<4 x float>, <4 x float>) nounwind readnone


define i32 @test_x86_sse_movmsk_ps(<4 x float> %a0) {
; SSE-LABEL: test_x86_sse_movmsk_ps:
; SSE:       ## BB#0:
; SSE-NEXT:    movmskps %xmm0, %eax
; SSE-NEXT:    retl
;
; KNL-LABEL: test_x86_sse_movmsk_ps:
; KNL:       ## BB#0:
; KNL-NEXT:    vmovmskps %xmm0, %eax
; KNL-NEXT:    retl
  %res = call i32 @llvm.x86.sse.movmsk.ps(<4 x float> %a0) ; <i32> [#uses=1]
  ret i32 %res
}
declare i32 @llvm.x86.sse.movmsk.ps(<4 x float>) nounwind readnone



define <4 x float> @test_x86_sse_mul_ss(<4 x float> %a0, <4 x float> %a1) {
; SSE-LABEL: test_x86_sse_mul_ss:
; SSE:       ## BB#0:
; SSE-NEXT:    mulss %xmm1, %xmm0
; SSE-NEXT:    retl
;
; KNL-LABEL: test_x86_sse_mul_ss:
; KNL:       ## BB#0:
; KNL-NEXT:    vmulss %xmm1, %xmm0, %xmm0
; KNL-NEXT:    retl
  %res = call <4 x float> @llvm.x86.sse.mul.ss(<4 x float> %a0, <4 x float> %a1) ; <<4 x float>> [#uses=1]
  ret <4 x float> %res
}
declare <4 x float> @llvm.x86.sse.mul.ss(<4 x float>, <4 x float>) nounwind readnone


define <4 x float> @test_x86_sse_rcp_ps(<4 x float> %a0) {
; SSE-LABEL: test_x86_sse_rcp_ps:
; SSE:       ## BB#0:
; SSE-NEXT:    rcpps %xmm0, %xmm0
; SSE-NEXT:    retl
;
; KNL-LABEL: test_x86_sse_rcp_ps:
; KNL:       ## BB#0:
; KNL-NEXT:    vrcpps %xmm0, %xmm0
; KNL-NEXT:    retl
  %res = call <4 x float> @llvm.x86.sse.rcp.ps(<4 x float> %a0) ; <<4 x float>> [#uses=1]
  ret <4 x float> %res
}
declare <4 x float> @llvm.x86.sse.rcp.ps(<4 x float>) nounwind readnone


define <4 x float> @test_x86_sse_rcp_ss(<4 x float> %a0) {
; SSE-LABEL: test_x86_sse_rcp_ss:
; SSE:       ## BB#0:
; SSE-NEXT:    rcpss %xmm0, %xmm0
; SSE-NEXT:    retl
;
; KNL-LABEL: test_x86_sse_rcp_ss:
; KNL:       ## BB#0:
; KNL-NEXT:    vrcpss %xmm0, %xmm0, %xmm0
; KNL-NEXT:    retl
  %res = call <4 x float> @llvm.x86.sse.rcp.ss(<4 x float> %a0) ; <<4 x float>> [#uses=1]
  ret <4 x float> %res
}
declare <4 x float> @llvm.x86.sse.rcp.ss(<4 x float>) nounwind readnone


define <4 x float> @test_x86_sse_rsqrt_ps(<4 x float> %a0) {
; SSE-LABEL: test_x86_sse_rsqrt_ps:
; SSE:       ## BB#0:
; SSE-NEXT:    rsqrtps %xmm0, %xmm0
; SSE-NEXT:    retl
;
; KNL-LABEL: test_x86_sse_rsqrt_ps:
; KNL:       ## BB#0:
; KNL-NEXT:    vrsqrtps %xmm0, %xmm0
; KNL-NEXT:    retl
  %res = call <4 x float> @llvm.x86.sse.rsqrt.ps(<4 x float> %a0) ; <<4 x float>> [#uses=1]
  ret <4 x float> %res
}
declare <4 x float> @llvm.x86.sse.rsqrt.ps(<4 x float>) nounwind readnone


define <4 x float> @test_x86_sse_rsqrt_ss(<4 x float> %a0) {
; SSE-LABEL: test_x86_sse_rsqrt_ss:
; SSE:       ## BB#0:
; SSE-NEXT:    rsqrtss %xmm0, %xmm0
; SSE-NEXT:    retl
;
; KNL-LABEL: test_x86_sse_rsqrt_ss:
; KNL:       ## BB#0:
; KNL-NEXT:    vrsqrtss %xmm0, %xmm0, %xmm0
; KNL-NEXT:    retl
  %res = call <4 x float> @llvm.x86.sse.rsqrt.ss(<4 x float> %a0) ; <<4 x float>> [#uses=1]
  ret <4 x float> %res
}
declare <4 x float> @llvm.x86.sse.rsqrt.ss(<4 x float>) nounwind readnone


define <4 x float> @test_x86_sse_sqrt_ps(<4 x float> %a0) {
; SSE-LABEL: test_x86_sse_sqrt_ps:
; SSE:       ## BB#0:
; SSE-NEXT:    sqrtps %xmm0, %xmm0
; SSE-NEXT:    retl
;
; KNL-LABEL: test_x86_sse_sqrt_ps:
; KNL:       ## BB#0:
; KNL-NEXT:    vsqrtps %xmm0, %xmm0
; KNL-NEXT:    retl
  %res = call <4 x float> @llvm.x86.sse.sqrt.ps(<4 x float> %a0) ; <<4 x float>> [#uses=1]
  ret <4 x float> %res
}
declare <4 x float> @llvm.x86.sse.sqrt.ps(<4 x float>) nounwind readnone


define <4 x float> @test_x86_sse_sqrt_ss(<4 x float> %a0) {
; SSE-LABEL: test_x86_sse_sqrt_ss:
; SSE:       ## BB#0:
; SSE-NEXT:    sqrtss %xmm0, %xmm0
; SSE-NEXT:    retl
;
; KNL-LABEL: test_x86_sse_sqrt_ss:
; KNL:       ## BB#0:
; KNL-NEXT:    vsqrtss %xmm0, %xmm0, %xmm0
; KNL-NEXT:    retl
  %res = call <4 x float> @llvm.x86.sse.sqrt.ss(<4 x float> %a0) ; <<4 x float>> [#uses=1]
  ret <4 x float> %res
}
declare <4 x float> @llvm.x86.sse.sqrt.ss(<4 x float>) nounwind readnone


define void @test_x86_sse_stmxcsr(i8* %a0) {
; SSE-LABEL: test_x86_sse_stmxcsr:
; SSE:       ## BB#0:
; SSE-NEXT:    movl {{[0-9]+}}(%esp), %eax
; SSE-NEXT:    stmxcsr (%eax)
; SSE-NEXT:    retl
;
; KNL-LABEL: test_x86_sse_stmxcsr:
; KNL:       ## BB#0:
; KNL-NEXT:    movl {{[0-9]+}}(%esp), %eax
; KNL-NEXT:    vstmxcsr (%eax)
; KNL-NEXT:    retl
  call void @llvm.x86.sse.stmxcsr(i8* %a0)
  ret void
}
declare void @llvm.x86.sse.stmxcsr(i8*) nounwind


define void @test_x86_sse_storeu_ps(i8* %a0, <4 x float> %a1) {
; SSE-LABEL: test_x86_sse_storeu_ps:
; SSE:       ## BB#0:
; SSE-NEXT:    movl {{[0-9]+}}(%esp), %eax
; SSE-NEXT:    movups %xmm0, (%eax)
; SSE-NEXT:    retl
;
; KNL-LABEL: test_x86_sse_storeu_ps:
; KNL:       ## BB#0:
; KNL-NEXT:    movl {{[0-9]+}}(%esp), %eax
; KNL-NEXT:    vmovups %xmm0, (%eax)
; KNL-NEXT:    retl
  call void @llvm.x86.sse.storeu.ps(i8* %a0, <4 x float> %a1)
  ret void
}
declare void @llvm.x86.sse.storeu.ps(i8*, <4 x float>) nounwind


define <4 x float> @test_x86_sse_sub_ss(<4 x float> %a0, <4 x float> %a1) {
; SSE-LABEL: test_x86_sse_sub_ss:
; SSE:       ## BB#0:
; SSE-NEXT:    subss %xmm1, %xmm0
; SSE-NEXT:    retl
;
; KNL-LABEL: test_x86_sse_sub_ss:
; KNL:       ## BB#0:
; KNL-NEXT:    vsubss %xmm1, %xmm0, %xmm0
; KNL-NEXT:    retl
  %res = call <4 x float> @llvm.x86.sse.sub.ss(<4 x float> %a0, <4 x float> %a1) ; <<4 x float>> [#uses=1]
  ret <4 x float> %res
}
declare <4 x float> @llvm.x86.sse.sub.ss(<4 x float>, <4 x float>) nounwind readnone


define i32 @test_x86_sse_ucomieq_ss(<4 x float> %a0, <4 x float> %a1) {
; SSE-LABEL: test_x86_sse_ucomieq_ss:
; SSE:       ## BB#0:
; SSE-NEXT:    ucomiss %xmm1, %xmm0
; SSE-NEXT:    sete %al
; SSE-NEXT:    movzbl %al, %eax
; SSE-NEXT:    retl
;
; KNL-LABEL: test_x86_sse_ucomieq_ss:
; KNL:       ## BB#0:
; KNL-NEXT:    vucomiss %xmm1, %xmm0
; KNL-NEXT:    sete %al
; KNL-NEXT:    movzbl %al, %eax
; KNL-NEXT:    retl
  %res = call i32 @llvm.x86.sse.ucomieq.ss(<4 x float> %a0, <4 x float> %a1) ; <i32> [#uses=1]
  ret i32 %res
}
declare i32 @llvm.x86.sse.ucomieq.ss(<4 x float>, <4 x float>) nounwind readnone


define i32 @test_x86_sse_ucomige_ss(<4 x float> %a0, <4 x float> %a1) {
; SSE-LABEL: test_x86_sse_ucomige_ss:
; SSE:       ## BB#0:
; SSE-NEXT:    ucomiss %xmm1, %xmm0
; SSE-NEXT:    setae %al
; SSE-NEXT:    movzbl %al, %eax
; SSE-NEXT:    retl
;
; KNL-LABEL: test_x86_sse_ucomige_ss:
; KNL:       ## BB#0:
; KNL-NEXT:    vucomiss %xmm1, %xmm0
; KNL-NEXT:    setae %al
; KNL-NEXT:    movzbl %al, %eax
; KNL-NEXT:    retl
  %res = call i32 @llvm.x86.sse.ucomige.ss(<4 x float> %a0, <4 x float> %a1) ; <i32> [#uses=1]
  ret i32 %res
}
declare i32 @llvm.x86.sse.ucomige.ss(<4 x float>, <4 x float>) nounwind readnone


define i32 @test_x86_sse_ucomigt_ss(<4 x float> %a0, <4 x float> %a1) {
; SSE-LABEL: test_x86_sse_ucomigt_ss:
; SSE:       ## BB#0:
; SSE-NEXT:    ucomiss %xmm1, %xmm0
; SSE-NEXT:    seta %al
; SSE-NEXT:    movzbl %al, %eax
; SSE-NEXT:    retl
;
; KNL-LABEL: test_x86_sse_ucomigt_ss:
; KNL:       ## BB#0:
; KNL-NEXT:    vucomiss %xmm1, %xmm0
; KNL-NEXT:    seta %al
; KNL-NEXT:    movzbl %al, %eax
; KNL-NEXT:    retl
  %res = call i32 @llvm.x86.sse.ucomigt.ss(<4 x float> %a0, <4 x float> %a1) ; <i32> [#uses=1]
  ret i32 %res
}
declare i32 @llvm.x86.sse.ucomigt.ss(<4 x float>, <4 x float>) nounwind readnone


define i32 @test_x86_sse_ucomile_ss(<4 x float> %a0, <4 x float> %a1) {
; SSE-LABEL: test_x86_sse_ucomile_ss:
; SSE:       ## BB#0:
; SSE-NEXT:    ucomiss %xmm1, %xmm0
; SSE-NEXT:    setbe %al
; SSE-NEXT:    movzbl %al, %eax
; SSE-NEXT:    retl
;
; KNL-LABEL: test_x86_sse_ucomile_ss:
; KNL:       ## BB#0:
; KNL-NEXT:    vucomiss %xmm1, %xmm0
; KNL-NEXT:    setbe %al
; KNL-NEXT:    movzbl %al, %eax
; KNL-NEXT:    retl
  %res = call i32 @llvm.x86.sse.ucomile.ss(<4 x float> %a0, <4 x float> %a1) ; <i32> [#uses=1]
  ret i32 %res
}
declare i32 @llvm.x86.sse.ucomile.ss(<4 x float>, <4 x float>) nounwind readnone


define i32 @test_x86_sse_ucomilt_ss(<4 x float> %a0, <4 x float> %a1) {
; SSE-LABEL: test_x86_sse_ucomilt_ss:
; SSE:       ## BB#0:
; SSE-NEXT:    ucomiss %xmm1, %xmm0
; SSE-NEXT:    sbbl %eax, %eax
; SSE-NEXT:    andl $1, %eax
; SSE-NEXT:    retl
;
; KNL-LABEL: test_x86_sse_ucomilt_ss:
; KNL:       ## BB#0:
; KNL-NEXT:    vucomiss %xmm1, %xmm0
; KNL-NEXT:    sbbl %eax, %eax
; KNL-NEXT:    andl $1, %eax
; KNL-NEXT:    retl
  %res = call i32 @llvm.x86.sse.ucomilt.ss(<4 x float> %a0, <4 x float> %a1) ; <i32> [#uses=1]
  ret i32 %res
}
declare i32 @llvm.x86.sse.ucomilt.ss(<4 x float>, <4 x float>) nounwind readnone


define i32 @test_x86_sse_ucomineq_ss(<4 x float> %a0, <4 x float> %a1) {
; SSE-LABEL: test_x86_sse_ucomineq_ss:
; SSE:       ## BB#0:
; SSE-NEXT:    ucomiss %xmm1, %xmm0
; SSE-NEXT:    setne %al
; SSE-NEXT:    movzbl %al, %eax
; SSE-NEXT:    retl
;
; KNL-LABEL: test_x86_sse_ucomineq_ss:
; KNL:       ## BB#0:
; KNL-NEXT:    vucomiss %xmm1, %xmm0
; KNL-NEXT:    setne %al
; KNL-NEXT:    movzbl %al, %eax
; KNL-NEXT:    retl
  %res = call i32 @llvm.x86.sse.ucomineq.ss(<4 x float> %a0, <4 x float> %a1) ; <i32> [#uses=1]
  ret i32 %res
}
declare i32 @llvm.x86.sse.ucomineq.ss(<4 x float>, <4 x float>) nounwind readnone
