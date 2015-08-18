; RUN: llc < %s -mtriple=x86_64-unknown-unknown -mattr=+sse2 | FileCheck %s --check-prefix=ALL --check-prefix=SSE --check-prefix=SSE2
; RUN: llc < %s -mtriple=x86_64-unknown-unknown -mattr=+sse4.1 | FileCheck %s --check-prefix=ALL --check-prefix=SSE --check-prefix=SSE41
; RUN: llc < %s -mtriple=x86_64-unknown-unknown -mattr=+sse4.2 | FileCheck %s --check-prefix=ALL --check-prefix=SSE --check-prefix=SSE42
; RUN: llc < %s -mtriple=x86_64-unknown-unknown -mattr=+avx | FileCheck %s --check-prefix=ALL --check-prefix=AVX --check-prefix=AVX1
; RUN: llc < %s -mtriple=x86_64-unknown-unknown -mattr=+avx2 | FileCheck %s --check-prefix=ALL --check-prefix=AVX --check-prefix=AVX2
; RUN: llc < %s -mtriple=x86_64-unknown-unknown -mattr=+avx512f | FileCheck %s --check-prefix=ALL --check-prefix=AVX --check-prefix=AVX512 --check-prefix=AVX512F
; RUN: llc < %s -mtriple=x86_64-unknown-unknown -mattr=+avx512bw | FileCheck %s --check-prefix=ALL --check-prefix=AVX --check-prefix=AVX512 --check-prefix=AVX512BW

;
; Unsigned Maximum (GT)
;

define <2 x i64> @max_gt_v2i64(<2 x i64> %a, <2 x i64> %b) {
; SSE2-LABEL: max_gt_v2i64:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa {{.*#+}} xmm2 = [2147483648,2147483648,2147483648,2147483648]
; SSE2-NEXT:    movdqa %xmm1, %xmm3
; SSE2-NEXT:    pxor %xmm2, %xmm3
; SSE2-NEXT:    pxor %xmm0, %xmm2
; SSE2-NEXT:    movdqa %xmm2, %xmm4
; SSE2-NEXT:    pcmpgtd %xmm3, %xmm4
; SSE2-NEXT:    pshufd {{.*#+}} xmm5 = xmm4[0,0,2,2]
; SSE2-NEXT:    pcmpeqd %xmm3, %xmm2
; SSE2-NEXT:    pshufd {{.*#+}} xmm2 = xmm2[1,1,3,3]
; SSE2-NEXT:    pand %xmm5, %xmm2
; SSE2-NEXT:    pshufd {{.*#+}} xmm3 = xmm4[1,1,3,3]
; SSE2-NEXT:    por %xmm2, %xmm3
; SSE2-NEXT:    pand %xmm3, %xmm0
; SSE2-NEXT:    pandn %xmm1, %xmm3
; SSE2-NEXT:    por %xmm3, %xmm0
; SSE2-NEXT:    retq
;
; SSE41-LABEL: max_gt_v2i64:
; SSE41:       # BB#0:
; SSE41-NEXT:    movdqa %xmm0, %xmm2
; SSE41-NEXT:    movdqa {{.*#+}} xmm0 = [2147483648,2147483648,2147483648,2147483648]
; SSE41-NEXT:    movdqa %xmm1, %xmm3
; SSE41-NEXT:    pxor %xmm0, %xmm3
; SSE41-NEXT:    pxor %xmm2, %xmm0
; SSE41-NEXT:    movdqa %xmm0, %xmm4
; SSE41-NEXT:    pcmpgtd %xmm3, %xmm4
; SSE41-NEXT:    pshufd {{.*#+}} xmm5 = xmm4[0,0,2,2]
; SSE41-NEXT:    pcmpeqd %xmm3, %xmm0
; SSE41-NEXT:    pshufd {{.*#+}} xmm3 = xmm0[1,1,3,3]
; SSE41-NEXT:    pand %xmm5, %xmm3
; SSE41-NEXT:    pshufd {{.*#+}} xmm0 = xmm4[1,1,3,3]
; SSE41-NEXT:    por %xmm3, %xmm0
; SSE41-NEXT:    blendvpd %xmm2, %xmm1
; SSE41-NEXT:    movapd %xmm1, %xmm0
; SSE41-NEXT:    retq
;
; SSE42-LABEL: max_gt_v2i64:
; SSE42:       # BB#0:
; SSE42-NEXT:    movdqa %xmm0, %xmm2
; SSE42-NEXT:    movdqa {{.*#+}} xmm0 = [9223372036854775808,9223372036854775808]
; SSE42-NEXT:    movdqa %xmm1, %xmm3
; SSE42-NEXT:    pxor %xmm0, %xmm3
; SSE42-NEXT:    pxor %xmm2, %xmm0
; SSE42-NEXT:    pcmpgtq %xmm3, %xmm0
; SSE42-NEXT:    blendvpd %xmm2, %xmm1
; SSE42-NEXT:    movapd %xmm1, %xmm0
; SSE42-NEXT:    retq
;
; AVX-LABEL: max_gt_v2i64:
; AVX:       # BB#0:
; AVX-NEXT:    vmovdqa {{.*#+}} xmm2 = [9223372036854775808,9223372036854775808]
; AVX-NEXT:    vpxor %xmm2, %xmm1, %xmm3
; AVX-NEXT:    vpxor %xmm2, %xmm0, %xmm2
; AVX-NEXT:    vpcmpgtq %xmm3, %xmm2, %xmm2
; AVX-NEXT:    vblendvpd %xmm2, %xmm0, %xmm1, %xmm0
; AVX-NEXT:    retq
  %1 = icmp ugt <2 x i64> %a, %b
  %2 = select <2 x i1> %1, <2 x i64> %a, <2 x i64> %b
  ret <2 x i64> %2
}

define <4 x i64> @max_gt_v4i64(<4 x i64> %a, <4 x i64> %b) {
; SSE2-LABEL: max_gt_v4i64:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa {{.*#+}} xmm4 = [2147483648,2147483648,2147483648,2147483648]
; SSE2-NEXT:    movdqa %xmm3, %xmm5
; SSE2-NEXT:    pxor %xmm4, %xmm5
; SSE2-NEXT:    movdqa %xmm1, %xmm6
; SSE2-NEXT:    pxor %xmm4, %xmm6
; SSE2-NEXT:    movdqa %xmm6, %xmm7
; SSE2-NEXT:    pcmpgtd %xmm5, %xmm7
; SSE2-NEXT:    pshufd {{.*#+}} xmm8 = xmm7[0,0,2,2]
; SSE2-NEXT:    pcmpeqd %xmm5, %xmm6
; SSE2-NEXT:    pshufd {{.*#+}} xmm5 = xmm6[1,1,3,3]
; SSE2-NEXT:    pand %xmm8, %xmm5
; SSE2-NEXT:    pshufd {{.*#+}} xmm6 = xmm7[1,1,3,3]
; SSE2-NEXT:    por %xmm5, %xmm6
; SSE2-NEXT:    movdqa %xmm2, %xmm5
; SSE2-NEXT:    pxor %xmm4, %xmm5
; SSE2-NEXT:    pxor %xmm0, %xmm4
; SSE2-NEXT:    movdqa %xmm4, %xmm7
; SSE2-NEXT:    pcmpgtd %xmm5, %xmm7
; SSE2-NEXT:    pshufd {{.*#+}} xmm8 = xmm7[0,0,2,2]
; SSE2-NEXT:    pcmpeqd %xmm5, %xmm4
; SSE2-NEXT:    pshufd {{.*#+}} xmm4 = xmm4[1,1,3,3]
; SSE2-NEXT:    pand %xmm8, %xmm4
; SSE2-NEXT:    pshufd {{.*#+}} xmm5 = xmm7[1,1,3,3]
; SSE2-NEXT:    por %xmm4, %xmm5
; SSE2-NEXT:    pand %xmm5, %xmm0
; SSE2-NEXT:    pandn %xmm2, %xmm5
; SSE2-NEXT:    por %xmm5, %xmm0
; SSE2-NEXT:    pand %xmm6, %xmm1
; SSE2-NEXT:    pandn %xmm3, %xmm6
; SSE2-NEXT:    por %xmm6, %xmm1
; SSE2-NEXT:    retq
;
; SSE41-LABEL: max_gt_v4i64:
; SSE41:       # BB#0:
; SSE41-NEXT:    movdqa %xmm0, %xmm8
; SSE41-NEXT:    movdqa {{.*#+}} xmm0 = [2147483648,2147483648,2147483648,2147483648]
; SSE41-NEXT:    movdqa %xmm3, %xmm5
; SSE41-NEXT:    pxor %xmm0, %xmm5
; SSE41-NEXT:    movdqa %xmm1, %xmm6
; SSE41-NEXT:    pxor %xmm0, %xmm6
; SSE41-NEXT:    movdqa %xmm6, %xmm7
; SSE41-NEXT:    pcmpgtd %xmm5, %xmm7
; SSE41-NEXT:    pshufd {{.*#+}} xmm4 = xmm7[0,0,2,2]
; SSE41-NEXT:    pcmpeqd %xmm5, %xmm6
; SSE41-NEXT:    pshufd {{.*#+}} xmm6 = xmm6[1,1,3,3]
; SSE41-NEXT:    pand %xmm4, %xmm6
; SSE41-NEXT:    pshufd {{.*#+}} xmm5 = xmm7[1,1,3,3]
; SSE41-NEXT:    por %xmm6, %xmm5
; SSE41-NEXT:    movdqa %xmm2, %xmm4
; SSE41-NEXT:    pxor %xmm0, %xmm4
; SSE41-NEXT:    pxor %xmm8, %xmm0
; SSE41-NEXT:    movdqa %xmm0, %xmm6
; SSE41-NEXT:    pcmpgtd %xmm4, %xmm6
; SSE41-NEXT:    pshufd {{.*#+}} xmm7 = xmm6[0,0,2,2]
; SSE41-NEXT:    pcmpeqd %xmm4, %xmm0
; SSE41-NEXT:    pshufd {{.*#+}} xmm4 = xmm0[1,1,3,3]
; SSE41-NEXT:    pand %xmm7, %xmm4
; SSE41-NEXT:    pshufd {{.*#+}} xmm0 = xmm6[1,1,3,3]
; SSE41-NEXT:    por %xmm4, %xmm0
; SSE41-NEXT:    blendvpd %xmm8, %xmm2
; SSE41-NEXT:    movdqa %xmm5, %xmm0
; SSE41-NEXT:    blendvpd %xmm1, %xmm3
; SSE41-NEXT:    movapd %xmm2, %xmm0
; SSE41-NEXT:    movapd %xmm3, %xmm1
; SSE41-NEXT:    retq
;
; SSE42-LABEL: max_gt_v4i64:
; SSE42:       # BB#0:
; SSE42-NEXT:    movdqa %xmm0, %xmm4
; SSE42-NEXT:    movdqa {{.*#+}} xmm0 = [9223372036854775808,9223372036854775808]
; SSE42-NEXT:    movdqa %xmm3, %xmm6
; SSE42-NEXT:    pxor %xmm0, %xmm6
; SSE42-NEXT:    movdqa %xmm1, %xmm5
; SSE42-NEXT:    pxor %xmm0, %xmm5
; SSE42-NEXT:    pcmpgtq %xmm6, %xmm5
; SSE42-NEXT:    movdqa %xmm2, %xmm6
; SSE42-NEXT:    pxor %xmm0, %xmm6
; SSE42-NEXT:    pxor %xmm4, %xmm0
; SSE42-NEXT:    pcmpgtq %xmm6, %xmm0
; SSE42-NEXT:    blendvpd %xmm4, %xmm2
; SSE42-NEXT:    movdqa %xmm5, %xmm0
; SSE42-NEXT:    blendvpd %xmm1, %xmm3
; SSE42-NEXT:    movapd %xmm2, %xmm0
; SSE42-NEXT:    movapd %xmm3, %xmm1
; SSE42-NEXT:    retq
;
; AVX1-LABEL: max_gt_v4i64:
; AVX1:       # BB#0:
; AVX1-NEXT:    vextractf128 $1, %ymm1, %xmm2
; AVX1-NEXT:    vmovaps {{.*#+}} xmm3 = [9223372036854775808,9223372036854775808]
; AVX1-NEXT:    vxorps %xmm3, %xmm2, %xmm2
; AVX1-NEXT:    vextractf128 $1, %ymm0, %xmm4
; AVX1-NEXT:    vxorps %xmm3, %xmm4, %xmm4
; AVX1-NEXT:    vpcmpgtq %xmm2, %xmm4, %xmm2
; AVX1-NEXT:    vxorps %xmm3, %xmm1, %xmm4
; AVX1-NEXT:    vxorps %xmm3, %xmm0, %xmm3
; AVX1-NEXT:    vpcmpgtq %xmm4, %xmm3, %xmm3
; AVX1-NEXT:    vinsertf128 $1, %xmm2, %ymm3, %ymm2
; AVX1-NEXT:    vblendvpd %ymm2, %ymm0, %ymm1, %ymm0
; AVX1-NEXT:    retq
;
; AVX2-LABEL: max_gt_v4i64:
; AVX2:       # BB#0:
; AVX2-NEXT:    vpbroadcastq {{.*}}(%rip), %ymm2
; AVX2-NEXT:    vpxor %ymm2, %ymm1, %ymm3
; AVX2-NEXT:    vpxor %ymm2, %ymm0, %ymm2
; AVX2-NEXT:    vpcmpgtq %ymm3, %ymm2, %ymm2
; AVX2-NEXT:    vblendvpd %ymm2, %ymm0, %ymm1, %ymm0
; AVX2-NEXT:    retq
;
; AVX512-LABEL: max_gt_v4i64:
; AVX512:       # BB#0:
; AVX512-NEXT:    vpbroadcastq {{.*}}(%rip), %ymm2
; AVX512-NEXT:    vpxor %ymm2, %ymm1, %ymm3
; AVX512-NEXT:    vpxor %ymm2, %ymm0, %ymm2
; AVX512-NEXT:    vpcmpgtq %ymm3, %ymm2, %ymm2
; AVX512-NEXT:    vblendvpd %ymm2, %ymm0, %ymm1, %ymm0
; AVX512-NEXT:    retq
  %1 = icmp ugt <4 x i64> %a, %b
  %2 = select <4 x i1> %1, <4 x i64> %a, <4 x i64> %b
  ret <4 x i64> %2
}

define <4 x i32> @max_gt_v4i32(<4 x i32> %a, <4 x i32> %b) {
; SSE2-LABEL: max_gt_v4i32:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa {{.*#+}} xmm2 = [2147483648,2147483648,2147483648,2147483648]
; SSE2-NEXT:    movdqa %xmm1, %xmm3
; SSE2-NEXT:    pxor %xmm2, %xmm3
; SSE2-NEXT:    pxor %xmm0, %xmm2
; SSE2-NEXT:    pcmpgtd %xmm3, %xmm2
; SSE2-NEXT:    pand %xmm2, %xmm0
; SSE2-NEXT:    pandn %xmm1, %xmm2
; SSE2-NEXT:    por %xmm2, %xmm0
; SSE2-NEXT:    retq
;
; SSE41-LABEL: max_gt_v4i32:
; SSE41:       # BB#0:
; SSE41-NEXT:    pmaxud %xmm1, %xmm0
; SSE41-NEXT:    retq
;
; SSE42-LABEL: max_gt_v4i32:
; SSE42:       # BB#0:
; SSE42-NEXT:    pmaxud %xmm1, %xmm0
; SSE42-NEXT:    retq
;
; AVX-LABEL: max_gt_v4i32:
; AVX:       # BB#0:
; AVX-NEXT:    vpmaxud %xmm1, %xmm0, %xmm0
; AVX-NEXT:    retq
  %1 = icmp ugt <4 x i32> %a, %b
  %2 = select <4 x i1> %1, <4 x i32> %a, <4 x i32> %b
  ret <4 x i32> %2
}

define <8 x i32> @max_gt_v8i32(<8 x i32> %a, <8 x i32> %b) {
; SSE2-LABEL: max_gt_v8i32:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa {{.*#+}} xmm5 = [2147483648,2147483648,2147483648,2147483648]
; SSE2-NEXT:    movdqa %xmm3, %xmm6
; SSE2-NEXT:    pxor %xmm5, %xmm6
; SSE2-NEXT:    movdqa %xmm1, %xmm4
; SSE2-NEXT:    pxor %xmm5, %xmm4
; SSE2-NEXT:    pcmpgtd %xmm6, %xmm4
; SSE2-NEXT:    movdqa %xmm2, %xmm6
; SSE2-NEXT:    pxor %xmm5, %xmm6
; SSE2-NEXT:    pxor %xmm0, %xmm5
; SSE2-NEXT:    pcmpgtd %xmm6, %xmm5
; SSE2-NEXT:    pand %xmm5, %xmm0
; SSE2-NEXT:    pandn %xmm2, %xmm5
; SSE2-NEXT:    por %xmm5, %xmm0
; SSE2-NEXT:    pand %xmm4, %xmm1
; SSE2-NEXT:    pandn %xmm3, %xmm4
; SSE2-NEXT:    por %xmm1, %xmm4
; SSE2-NEXT:    movdqa %xmm4, %xmm1
; SSE2-NEXT:    retq
;
; SSE41-LABEL: max_gt_v8i32:
; SSE41:       # BB#0:
; SSE41-NEXT:    pmaxud %xmm2, %xmm0
; SSE41-NEXT:    pmaxud %xmm3, %xmm1
; SSE41-NEXT:    retq
;
; SSE42-LABEL: max_gt_v8i32:
; SSE42:       # BB#0:
; SSE42-NEXT:    pmaxud %xmm2, %xmm0
; SSE42-NEXT:    pmaxud %xmm3, %xmm1
; SSE42-NEXT:    retq
;
; AVX1-LABEL: max_gt_v8i32:
; AVX1:       # BB#0:
; AVX1-NEXT:    vextractf128 $1, %ymm1, %xmm2
; AVX1-NEXT:    vextractf128 $1, %ymm0, %xmm3
; AVX1-NEXT:    vpmaxud %xmm2, %xmm3, %xmm2
; AVX1-NEXT:    vpmaxud %xmm1, %xmm0, %xmm0
; AVX1-NEXT:    vinsertf128 $1, %xmm2, %ymm0, %ymm0
; AVX1-NEXT:    retq
;
; AVX2-LABEL: max_gt_v8i32:
; AVX2:       # BB#0:
; AVX2-NEXT:    vpmaxud %ymm1, %ymm0, %ymm0
; AVX2-NEXT:    retq
;
; AVX512-LABEL: max_gt_v8i32:
; AVX512:       # BB#0:
; AVX512-NEXT:    vpmaxud %ymm1, %ymm0, %ymm0
; AVX512-NEXT:    retq
  %1 = icmp ugt <8 x i32> %a, %b
  %2 = select <8 x i1> %1, <8 x i32> %a, <8 x i32> %b
  ret <8 x i32> %2
}

define <8 x i16> @max_gt_v8i16(<8 x i16> %a, <8 x i16> %b) {
; SSE2-LABEL: max_gt_v8i16:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa {{.*#+}} xmm2 = [32768,32768,32768,32768,32768,32768,32768,32768]
; SSE2-NEXT:    movdqa %xmm1, %xmm3
; SSE2-NEXT:    pxor %xmm2, %xmm3
; SSE2-NEXT:    pxor %xmm0, %xmm2
; SSE2-NEXT:    pcmpgtw %xmm3, %xmm2
; SSE2-NEXT:    pand %xmm2, %xmm0
; SSE2-NEXT:    pandn %xmm1, %xmm2
; SSE2-NEXT:    por %xmm2, %xmm0
; SSE2-NEXT:    retq
;
; SSE41-LABEL: max_gt_v8i16:
; SSE41:       # BB#0:
; SSE41-NEXT:    pmaxuw %xmm1, %xmm0
; SSE41-NEXT:    retq
;
; SSE42-LABEL: max_gt_v8i16:
; SSE42:       # BB#0:
; SSE42-NEXT:    pmaxuw %xmm1, %xmm0
; SSE42-NEXT:    retq
;
; AVX-LABEL: max_gt_v8i16:
; AVX:       # BB#0:
; AVX-NEXT:    vpmaxuw %xmm1, %xmm0, %xmm0
; AVX-NEXT:    retq
  %1 = icmp ugt <8 x i16> %a, %b
  %2 = select <8 x i1> %1, <8 x i16> %a, <8 x i16> %b
  ret <8 x i16> %2
}

define <16 x i16> @max_gt_v16i16(<16 x i16> %a, <16 x i16> %b) {
; SSE2-LABEL: max_gt_v16i16:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa {{.*#+}} xmm5 = [32768,32768,32768,32768,32768,32768,32768,32768]
; SSE2-NEXT:    movdqa %xmm3, %xmm6
; SSE2-NEXT:    pxor %xmm5, %xmm6
; SSE2-NEXT:    movdqa %xmm1, %xmm4
; SSE2-NEXT:    pxor %xmm5, %xmm4
; SSE2-NEXT:    pcmpgtw %xmm6, %xmm4
; SSE2-NEXT:    movdqa %xmm2, %xmm6
; SSE2-NEXT:    pxor %xmm5, %xmm6
; SSE2-NEXT:    pxor %xmm0, %xmm5
; SSE2-NEXT:    pcmpgtw %xmm6, %xmm5
; SSE2-NEXT:    pand %xmm5, %xmm0
; SSE2-NEXT:    pandn %xmm2, %xmm5
; SSE2-NEXT:    por %xmm5, %xmm0
; SSE2-NEXT:    pand %xmm4, %xmm1
; SSE2-NEXT:    pandn %xmm3, %xmm4
; SSE2-NEXT:    por %xmm1, %xmm4
; SSE2-NEXT:    movdqa %xmm4, %xmm1
; SSE2-NEXT:    retq
;
; SSE41-LABEL: max_gt_v16i16:
; SSE41:       # BB#0:
; SSE41-NEXT:    pmaxuw %xmm2, %xmm0
; SSE41-NEXT:    pmaxuw %xmm3, %xmm1
; SSE41-NEXT:    retq
;
; SSE42-LABEL: max_gt_v16i16:
; SSE42:       # BB#0:
; SSE42-NEXT:    pmaxuw %xmm2, %xmm0
; SSE42-NEXT:    pmaxuw %xmm3, %xmm1
; SSE42-NEXT:    retq
;
; AVX1-LABEL: max_gt_v16i16:
; AVX1:       # BB#0:
; AVX1-NEXT:    vextractf128 $1, %ymm1, %xmm2
; AVX1-NEXT:    vextractf128 $1, %ymm0, %xmm3
; AVX1-NEXT:    vpmaxuw %xmm2, %xmm3, %xmm2
; AVX1-NEXT:    vpmaxuw %xmm1, %xmm0, %xmm0
; AVX1-NEXT:    vinsertf128 $1, %xmm2, %ymm0, %ymm0
; AVX1-NEXT:    retq
;
; AVX2-LABEL: max_gt_v16i16:
; AVX2:       # BB#0:
; AVX2-NEXT:    vpmaxuw %ymm1, %ymm0, %ymm0
; AVX2-NEXT:    retq
;
; AVX512-LABEL: max_gt_v16i16:
; AVX512:       # BB#0:
; AVX512-NEXT:    vpmaxuw %ymm1, %ymm0, %ymm0
; AVX512-NEXT:    retq
  %1 = icmp ugt <16 x i16> %a, %b
  %2 = select <16 x i1> %1, <16 x i16> %a, <16 x i16> %b
  ret <16 x i16> %2
}

define <16 x i8> @max_gt_v16i8(<16 x i8> %a, <16 x i8> %b) {
; SSE-LABEL: max_gt_v16i8:
; SSE:       # BB#0:
; SSE-NEXT:    pmaxub %xmm1, %xmm0
; SSE-NEXT:    retq
;
; AVX-LABEL: max_gt_v16i8:
; AVX:       # BB#0:
; AVX-NEXT:    vpmaxub %xmm1, %xmm0, %xmm0
; AVX-NEXT:    retq
  %1 = icmp ugt <16 x i8> %a, %b
  %2 = select <16 x i1> %1, <16 x i8> %a, <16 x i8> %b
  ret <16 x i8> %2
}

define <32 x i8> @max_gt_v32i8(<32 x i8> %a, <32 x i8> %b) {
; SSE-LABEL: max_gt_v32i8:
; SSE:       # BB#0:
; SSE-NEXT:    pmaxub %xmm2, %xmm0
; SSE-NEXT:    pmaxub %xmm3, %xmm1
; SSE-NEXT:    retq
;
; AVX1-LABEL: max_gt_v32i8:
; AVX1:       # BB#0:
; AVX1-NEXT:    vextractf128 $1, %ymm1, %xmm2
; AVX1-NEXT:    vextractf128 $1, %ymm0, %xmm3
; AVX1-NEXT:    vpmaxub %xmm2, %xmm3, %xmm2
; AVX1-NEXT:    vpmaxub %xmm1, %xmm0, %xmm0
; AVX1-NEXT:    vinsertf128 $1, %xmm2, %ymm0, %ymm0
; AVX1-NEXT:    retq
;
; AVX2-LABEL: max_gt_v32i8:
; AVX2:       # BB#0:
; AVX2-NEXT:    vpmaxub %ymm1, %ymm0, %ymm0
; AVX2-NEXT:    retq
;
; AVX512-LABEL: max_gt_v32i8:
; AVX512:       # BB#0:
; AVX512-NEXT:    vpmaxub %ymm1, %ymm0, %ymm0
; AVX512-NEXT:    retq
  %1 = icmp ugt <32 x i8> %a, %b
  %2 = select <32 x i1> %1, <32 x i8> %a, <32 x i8> %b
  ret <32 x i8> %2
}

;
; Unsigned Maximum (GE)
;

define <2 x i64> @max_ge_v2i64(<2 x i64> %a, <2 x i64> %b) {
; SSE2-LABEL: max_ge_v2i64:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa {{.*#+}} xmm2 = [2147483648,2147483648,2147483648,2147483648]
; SSE2-NEXT:    movdqa %xmm0, %xmm3
; SSE2-NEXT:    pxor %xmm2, %xmm3
; SSE2-NEXT:    pxor %xmm1, %xmm2
; SSE2-NEXT:    movdqa %xmm2, %xmm4
; SSE2-NEXT:    pcmpgtd %xmm3, %xmm4
; SSE2-NEXT:    pshufd {{.*#+}} xmm5 = xmm4[0,0,2,2]
; SSE2-NEXT:    pcmpeqd %xmm3, %xmm2
; SSE2-NEXT:    pshufd {{.*#+}} xmm2 = xmm2[1,1,3,3]
; SSE2-NEXT:    pand %xmm5, %xmm2
; SSE2-NEXT:    pshufd {{.*#+}} xmm3 = xmm4[1,1,3,3]
; SSE2-NEXT:    por %xmm2, %xmm3
; SSE2-NEXT:    pcmpeqd %xmm2, %xmm2
; SSE2-NEXT:    pxor %xmm3, %xmm2
; SSE2-NEXT:    pandn %xmm0, %xmm3
; SSE2-NEXT:    pandn %xmm1, %xmm2
; SSE2-NEXT:    por %xmm3, %xmm2
; SSE2-NEXT:    movdqa %xmm2, %xmm0
; SSE2-NEXT:    retq
;
; SSE41-LABEL: max_ge_v2i64:
; SSE41:       # BB#0:
; SSE41-NEXT:    movdqa %xmm0, %xmm2
; SSE41-NEXT:    movdqa {{.*#+}} xmm0 = [2147483648,2147483648,2147483648,2147483648]
; SSE41-NEXT:    movdqa %xmm2, %xmm3
; SSE41-NEXT:    pxor %xmm0, %xmm3
; SSE41-NEXT:    pxor %xmm1, %xmm0
; SSE41-NEXT:    movdqa %xmm0, %xmm4
; SSE41-NEXT:    pcmpgtd %xmm3, %xmm4
; SSE41-NEXT:    pshufd {{.*#+}} xmm5 = xmm4[0,0,2,2]
; SSE41-NEXT:    pcmpeqd %xmm3, %xmm0
; SSE41-NEXT:    pshufd {{.*#+}} xmm0 = xmm0[1,1,3,3]
; SSE41-NEXT:    pand %xmm5, %xmm0
; SSE41-NEXT:    pshufd {{.*#+}} xmm3 = xmm4[1,1,3,3]
; SSE41-NEXT:    por %xmm0, %xmm3
; SSE41-NEXT:    pcmpeqd %xmm0, %xmm0
; SSE41-NEXT:    pxor %xmm3, %xmm0
; SSE41-NEXT:    blendvpd %xmm2, %xmm1
; SSE41-NEXT:    movapd %xmm1, %xmm0
; SSE41-NEXT:    retq
;
; SSE42-LABEL: max_ge_v2i64:
; SSE42:       # BB#0:
; SSE42-NEXT:    movdqa %xmm0, %xmm2
; SSE42-NEXT:    movdqa {{.*#+}} xmm3 = [9223372036854775808,9223372036854775808]
; SSE42-NEXT:    pxor %xmm3, %xmm0
; SSE42-NEXT:    pxor %xmm1, %xmm3
; SSE42-NEXT:    pcmpgtq %xmm0, %xmm3
; SSE42-NEXT:    pcmpeqd %xmm0, %xmm0
; SSE42-NEXT:    pxor %xmm3, %xmm0
; SSE42-NEXT:    blendvpd %xmm2, %xmm1
; SSE42-NEXT:    movapd %xmm1, %xmm0
; SSE42-NEXT:    retq
;
; AVX-LABEL: max_ge_v2i64:
; AVX:       # BB#0:
; AVX-NEXT:    vmovdqa {{.*#+}} xmm2 = [9223372036854775808,9223372036854775808]
; AVX-NEXT:    vpxor %xmm2, %xmm0, %xmm3
; AVX-NEXT:    vpxor %xmm2, %xmm1, %xmm2
; AVX-NEXT:    vpcmpgtq %xmm3, %xmm2, %xmm2
; AVX-NEXT:    vpcmpeqd %xmm3, %xmm3, %xmm3
; AVX-NEXT:    vpxor %xmm3, %xmm2, %xmm2
; AVX-NEXT:    vblendvpd %xmm2, %xmm0, %xmm1, %xmm0
; AVX-NEXT:    retq
  %1 = icmp uge <2 x i64> %a, %b
  %2 = select <2 x i1> %1, <2 x i64> %a, <2 x i64> %b
  ret <2 x i64> %2
}

define <4 x i64> @max_ge_v4i64(<4 x i64> %a, <4 x i64> %b) {
; SSE2-LABEL: max_ge_v4i64:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa {{.*#+}} xmm7 = [2147483648,2147483648,2147483648,2147483648]
; SSE2-NEXT:    movdqa %xmm1, %xmm4
; SSE2-NEXT:    pxor %xmm7, %xmm4
; SSE2-NEXT:    movdqa %xmm3, %xmm5
; SSE2-NEXT:    pxor %xmm7, %xmm5
; SSE2-NEXT:    movdqa %xmm5, %xmm6
; SSE2-NEXT:    pcmpgtd %xmm4, %xmm6
; SSE2-NEXT:    pshufd {{.*#+}} xmm8 = xmm6[0,0,2,2]
; SSE2-NEXT:    pcmpeqd %xmm4, %xmm5
; SSE2-NEXT:    pshufd {{.*#+}} xmm4 = xmm5[1,1,3,3]
; SSE2-NEXT:    pand %xmm8, %xmm4
; SSE2-NEXT:    pshufd {{.*#+}} xmm8 = xmm6[1,1,3,3]
; SSE2-NEXT:    por %xmm4, %xmm8
; SSE2-NEXT:    pcmpeqd %xmm4, %xmm4
; SSE2-NEXT:    movdqa %xmm8, %xmm9
; SSE2-NEXT:    pxor %xmm4, %xmm9
; SSE2-NEXT:    movdqa %xmm0, %xmm6
; SSE2-NEXT:    pxor %xmm7, %xmm6
; SSE2-NEXT:    pxor %xmm2, %xmm7
; SSE2-NEXT:    movdqa %xmm7, %xmm5
; SSE2-NEXT:    pcmpgtd %xmm6, %xmm5
; SSE2-NEXT:    pshufd {{.*#+}} xmm10 = xmm5[0,0,2,2]
; SSE2-NEXT:    pcmpeqd %xmm6, %xmm7
; SSE2-NEXT:    pshufd {{.*#+}} xmm6 = xmm7[1,1,3,3]
; SSE2-NEXT:    pand %xmm10, %xmm6
; SSE2-NEXT:    pshufd {{.*#+}} xmm5 = xmm5[1,1,3,3]
; SSE2-NEXT:    por %xmm6, %xmm5
; SSE2-NEXT:    pxor %xmm5, %xmm4
; SSE2-NEXT:    pandn %xmm0, %xmm5
; SSE2-NEXT:    pandn %xmm2, %xmm4
; SSE2-NEXT:    por %xmm5, %xmm4
; SSE2-NEXT:    pandn %xmm1, %xmm8
; SSE2-NEXT:    pandn %xmm3, %xmm9
; SSE2-NEXT:    por %xmm8, %xmm9
; SSE2-NEXT:    movdqa %xmm4, %xmm0
; SSE2-NEXT:    movdqa %xmm9, %xmm1
; SSE2-NEXT:    retq
;
; SSE41-LABEL: max_ge_v4i64:
; SSE41:       # BB#0:
; SSE41-NEXT:    movdqa %xmm0, %xmm8
; SSE41-NEXT:    movdqa {{.*#+}} xmm0 = [2147483648,2147483648,2147483648,2147483648]
; SSE41-NEXT:    movdqa %xmm1, %xmm5
; SSE41-NEXT:    pxor %xmm0, %xmm5
; SSE41-NEXT:    movdqa %xmm3, %xmm6
; SSE41-NEXT:    pxor %xmm0, %xmm6
; SSE41-NEXT:    movdqa %xmm6, %xmm7
; SSE41-NEXT:    pcmpgtd %xmm5, %xmm7
; SSE41-NEXT:    pshufd {{.*#+}} xmm4 = xmm7[0,0,2,2]
; SSE41-NEXT:    pcmpeqd %xmm5, %xmm6
; SSE41-NEXT:    pshufd {{.*#+}} xmm6 = xmm6[1,1,3,3]
; SSE41-NEXT:    pand %xmm4, %xmm6
; SSE41-NEXT:    pshufd {{.*#+}} xmm5 = xmm7[1,1,3,3]
; SSE41-NEXT:    por %xmm6, %xmm5
; SSE41-NEXT:    pcmpeqd %xmm9, %xmm9
; SSE41-NEXT:    pxor %xmm9, %xmm5
; SSE41-NEXT:    movdqa %xmm8, %xmm6
; SSE41-NEXT:    pxor %xmm0, %xmm6
; SSE41-NEXT:    pxor %xmm2, %xmm0
; SSE41-NEXT:    movdqa %xmm0, %xmm7
; SSE41-NEXT:    pcmpgtd %xmm6, %xmm7
; SSE41-NEXT:    pshufd {{.*#+}} xmm4 = xmm7[0,0,2,2]
; SSE41-NEXT:    pcmpeqd %xmm6, %xmm0
; SSE41-NEXT:    pshufd {{.*#+}} xmm6 = xmm0[1,1,3,3]
; SSE41-NEXT:    pand %xmm4, %xmm6
; SSE41-NEXT:    pshufd {{.*#+}} xmm0 = xmm7[1,1,3,3]
; SSE41-NEXT:    por %xmm6, %xmm0
; SSE41-NEXT:    pxor %xmm9, %xmm0
; SSE41-NEXT:    blendvpd %xmm8, %xmm2
; SSE41-NEXT:    movdqa %xmm5, %xmm0
; SSE41-NEXT:    blendvpd %xmm1, %xmm3
; SSE41-NEXT:    movapd %xmm2, %xmm0
; SSE41-NEXT:    movapd %xmm3, %xmm1
; SSE41-NEXT:    retq
;
; SSE42-LABEL: max_ge_v4i64:
; SSE42:       # BB#0:
; SSE42-NEXT:    movdqa %xmm0, %xmm4
; SSE42-NEXT:    movdqa {{.*#+}} xmm0 = [9223372036854775808,9223372036854775808]
; SSE42-NEXT:    movdqa %xmm1, %xmm6
; SSE42-NEXT:    pxor %xmm0, %xmm6
; SSE42-NEXT:    movdqa %xmm3, %xmm5
; SSE42-NEXT:    pxor %xmm0, %xmm5
; SSE42-NEXT:    pcmpgtq %xmm6, %xmm5
; SSE42-NEXT:    pcmpeqd %xmm6, %xmm6
; SSE42-NEXT:    pxor %xmm6, %xmm5
; SSE42-NEXT:    movdqa %xmm4, %xmm7
; SSE42-NEXT:    pxor %xmm0, %xmm7
; SSE42-NEXT:    pxor %xmm2, %xmm0
; SSE42-NEXT:    pcmpgtq %xmm7, %xmm0
; SSE42-NEXT:    pxor %xmm6, %xmm0
; SSE42-NEXT:    blendvpd %xmm4, %xmm2
; SSE42-NEXT:    movdqa %xmm5, %xmm0
; SSE42-NEXT:    blendvpd %xmm1, %xmm3
; SSE42-NEXT:    movapd %xmm2, %xmm0
; SSE42-NEXT:    movapd %xmm3, %xmm1
; SSE42-NEXT:    retq
;
; AVX1-LABEL: max_ge_v4i64:
; AVX1:       # BB#0:
; AVX1-NEXT:    vextractf128 $1, %ymm0, %xmm2
; AVX1-NEXT:    vmovaps {{.*#+}} xmm3 = [9223372036854775808,9223372036854775808]
; AVX1-NEXT:    vxorps %xmm3, %xmm2, %xmm2
; AVX1-NEXT:    vextractf128 $1, %ymm1, %xmm4
; AVX1-NEXT:    vxorps %xmm3, %xmm4, %xmm4
; AVX1-NEXT:    vpcmpgtq %xmm2, %xmm4, %xmm2
; AVX1-NEXT:    vpcmpeqd %xmm4, %xmm4, %xmm4
; AVX1-NEXT:    vpxor %xmm4, %xmm2, %xmm2
; AVX1-NEXT:    vxorps %xmm3, %xmm0, %xmm5
; AVX1-NEXT:    vxorps %xmm3, %xmm1, %xmm3
; AVX1-NEXT:    vpcmpgtq %xmm5, %xmm3, %xmm3
; AVX1-NEXT:    vpxor %xmm4, %xmm3, %xmm3
; AVX1-NEXT:    vinsertf128 $1, %xmm2, %ymm3, %ymm2
; AVX1-NEXT:    vblendvpd %ymm2, %ymm0, %ymm1, %ymm0
; AVX1-NEXT:    retq
;
; AVX2-LABEL: max_ge_v4i64:
; AVX2:       # BB#0:
; AVX2-NEXT:    vpbroadcastq {{.*}}(%rip), %ymm2
; AVX2-NEXT:    vpxor %ymm2, %ymm0, %ymm3
; AVX2-NEXT:    vpxor %ymm2, %ymm1, %ymm2
; AVX2-NEXT:    vpcmpgtq %ymm3, %ymm2, %ymm2
; AVX2-NEXT:    vpcmpeqd %ymm3, %ymm3, %ymm3
; AVX2-NEXT:    vpxor %ymm3, %ymm2, %ymm2
; AVX2-NEXT:    vblendvpd %ymm2, %ymm0, %ymm1, %ymm0
; AVX2-NEXT:    retq
;
; AVX512-LABEL: max_ge_v4i64:
; AVX512:       # BB#0:
; AVX512-NEXT:    vpbroadcastq {{.*}}(%rip), %ymm2
; AVX512-NEXT:    vpxor %ymm2, %ymm0, %ymm3
; AVX512-NEXT:    vpxor %ymm2, %ymm1, %ymm2
; AVX512-NEXT:    vpcmpgtq %ymm3, %ymm2, %ymm2
; AVX512-NEXT:    vpcmpeqd %ymm3, %ymm3, %ymm3
; AVX512-NEXT:    vpxor %ymm3, %ymm2, %ymm2
; AVX512-NEXT:    vblendvpd %ymm2, %ymm0, %ymm1, %ymm0
; AVX512-NEXT:    retq
  %1 = icmp uge <4 x i64> %a, %b
  %2 = select <4 x i1> %1, <4 x i64> %a, <4 x i64> %b
  ret <4 x i64> %2
}

define <4 x i32> @max_ge_v4i32(<4 x i32> %a, <4 x i32> %b) {
; SSE2-LABEL: max_ge_v4i32:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa {{.*#+}} xmm3 = [2147483648,2147483648,2147483648,2147483648]
; SSE2-NEXT:    movdqa %xmm0, %xmm2
; SSE2-NEXT:    pxor %xmm3, %xmm2
; SSE2-NEXT:    pxor %xmm1, %xmm3
; SSE2-NEXT:    pcmpgtd %xmm2, %xmm3
; SSE2-NEXT:    pcmpeqd %xmm2, %xmm2
; SSE2-NEXT:    pxor %xmm3, %xmm2
; SSE2-NEXT:    pandn %xmm0, %xmm3
; SSE2-NEXT:    pandn %xmm1, %xmm2
; SSE2-NEXT:    por %xmm3, %xmm2
; SSE2-NEXT:    movdqa %xmm2, %xmm0
; SSE2-NEXT:    retq
;
; SSE41-LABEL: max_ge_v4i32:
; SSE41:       # BB#0:
; SSE41-NEXT:    pmaxud %xmm1, %xmm0
; SSE41-NEXT:    retq
;
; SSE42-LABEL: max_ge_v4i32:
; SSE42:       # BB#0:
; SSE42-NEXT:    pmaxud %xmm1, %xmm0
; SSE42-NEXT:    retq
;
; AVX-LABEL: max_ge_v4i32:
; AVX:       # BB#0:
; AVX-NEXT:    vpmaxud %xmm1, %xmm0, %xmm0
; AVX-NEXT:    retq
  %1 = icmp uge <4 x i32> %a, %b
  %2 = select <4 x i1> %1, <4 x i32> %a, <4 x i32> %b
  ret <4 x i32> %2
}

define <8 x i32> @max_ge_v8i32(<8 x i32> %a, <8 x i32> %b) {
; SSE2-LABEL: max_ge_v8i32:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa {{.*#+}} xmm6 = [2147483648,2147483648,2147483648,2147483648]
; SSE2-NEXT:    movdqa %xmm1, %xmm4
; SSE2-NEXT:    pxor %xmm6, %xmm4
; SSE2-NEXT:    movdqa %xmm3, %xmm7
; SSE2-NEXT:    pxor %xmm6, %xmm7
; SSE2-NEXT:    pcmpgtd %xmm4, %xmm7
; SSE2-NEXT:    pcmpeqd %xmm4, %xmm4
; SSE2-NEXT:    movdqa %xmm7, %xmm5
; SSE2-NEXT:    pxor %xmm4, %xmm5
; SSE2-NEXT:    movdqa %xmm0, %xmm8
; SSE2-NEXT:    pxor %xmm6, %xmm8
; SSE2-NEXT:    pxor %xmm2, %xmm6
; SSE2-NEXT:    pcmpgtd %xmm8, %xmm6
; SSE2-NEXT:    pxor %xmm6, %xmm4
; SSE2-NEXT:    pandn %xmm0, %xmm6
; SSE2-NEXT:    pandn %xmm2, %xmm4
; SSE2-NEXT:    por %xmm6, %xmm4
; SSE2-NEXT:    pandn %xmm1, %xmm7
; SSE2-NEXT:    pandn %xmm3, %xmm5
; SSE2-NEXT:    por %xmm7, %xmm5
; SSE2-NEXT:    movdqa %xmm4, %xmm0
; SSE2-NEXT:    movdqa %xmm5, %xmm1
; SSE2-NEXT:    retq
;
; SSE41-LABEL: max_ge_v8i32:
; SSE41:       # BB#0:
; SSE41-NEXT:    pmaxud %xmm2, %xmm0
; SSE41-NEXT:    pmaxud %xmm3, %xmm1
; SSE41-NEXT:    retq
;
; SSE42-LABEL: max_ge_v8i32:
; SSE42:       # BB#0:
; SSE42-NEXT:    pmaxud %xmm2, %xmm0
; SSE42-NEXT:    pmaxud %xmm3, %xmm1
; SSE42-NEXT:    retq
;
; AVX1-LABEL: max_ge_v8i32:
; AVX1:       # BB#0:
; AVX1-NEXT:    vextractf128 $1, %ymm1, %xmm2
; AVX1-NEXT:    vextractf128 $1, %ymm0, %xmm3
; AVX1-NEXT:    vpmaxud %xmm2, %xmm3, %xmm2
; AVX1-NEXT:    vpmaxud %xmm1, %xmm0, %xmm0
; AVX1-NEXT:    vinsertf128 $1, %xmm2, %ymm0, %ymm0
; AVX1-NEXT:    retq
;
; AVX2-LABEL: max_ge_v8i32:
; AVX2:       # BB#0:
; AVX2-NEXT:    vpmaxud %ymm1, %ymm0, %ymm0
; AVX2-NEXT:    retq
;
; AVX512-LABEL: max_ge_v8i32:
; AVX512:       # BB#0:
; AVX512-NEXT:    vpmaxud %ymm1, %ymm0, %ymm0
; AVX512-NEXT:    retq
  %1 = icmp uge <8 x i32> %a, %b
  %2 = select <8 x i1> %1, <8 x i32> %a, <8 x i32> %b
  ret <8 x i32> %2
}

define <8 x i16> @max_ge_v8i16(<8 x i16> %a, <8 x i16> %b) {
; SSE2-LABEL: max_ge_v8i16:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa %xmm1, %xmm2
; SSE2-NEXT:    psubusw %xmm0, %xmm2
; SSE2-NEXT:    pxor %xmm3, %xmm3
; SSE2-NEXT:    pcmpeqw %xmm2, %xmm3
; SSE2-NEXT:    pand %xmm3, %xmm0
; SSE2-NEXT:    pandn %xmm1, %xmm3
; SSE2-NEXT:    por %xmm3, %xmm0
; SSE2-NEXT:    retq
;
; SSE41-LABEL: max_ge_v8i16:
; SSE41:       # BB#0:
; SSE41-NEXT:    pmaxuw %xmm1, %xmm0
; SSE41-NEXT:    retq
;
; SSE42-LABEL: max_ge_v8i16:
; SSE42:       # BB#0:
; SSE42-NEXT:    pmaxuw %xmm1, %xmm0
; SSE42-NEXT:    retq
;
; AVX-LABEL: max_ge_v8i16:
; AVX:       # BB#0:
; AVX-NEXT:    vpmaxuw %xmm1, %xmm0, %xmm0
; AVX-NEXT:    retq
  %1 = icmp uge <8 x i16> %a, %b
  %2 = select <8 x i1> %1, <8 x i16> %a, <8 x i16> %b
  ret <8 x i16> %2
}

define <16 x i16> @max_ge_v16i16(<16 x i16> %a, <16 x i16> %b) {
; SSE2-LABEL: max_ge_v16i16:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa %xmm3, %xmm4
; SSE2-NEXT:    psubusw %xmm1, %xmm4
; SSE2-NEXT:    pxor %xmm5, %xmm5
; SSE2-NEXT:    pcmpeqw %xmm5, %xmm4
; SSE2-NEXT:    movdqa %xmm2, %xmm6
; SSE2-NEXT:    psubusw %xmm0, %xmm6
; SSE2-NEXT:    pcmpeqw %xmm5, %xmm6
; SSE2-NEXT:    pand %xmm6, %xmm0
; SSE2-NEXT:    pandn %xmm2, %xmm6
; SSE2-NEXT:    por %xmm6, %xmm0
; SSE2-NEXT:    pand %xmm4, %xmm1
; SSE2-NEXT:    pandn %xmm3, %xmm4
; SSE2-NEXT:    por %xmm4, %xmm1
; SSE2-NEXT:    retq
;
; SSE41-LABEL: max_ge_v16i16:
; SSE41:       # BB#0:
; SSE41-NEXT:    pmaxuw %xmm2, %xmm0
; SSE41-NEXT:    pmaxuw %xmm3, %xmm1
; SSE41-NEXT:    retq
;
; SSE42-LABEL: max_ge_v16i16:
; SSE42:       # BB#0:
; SSE42-NEXT:    pmaxuw %xmm2, %xmm0
; SSE42-NEXT:    pmaxuw %xmm3, %xmm1
; SSE42-NEXT:    retq
;
; AVX1-LABEL: max_ge_v16i16:
; AVX1:       # BB#0:
; AVX1-NEXT:    vextractf128 $1, %ymm1, %xmm2
; AVX1-NEXT:    vextractf128 $1, %ymm0, %xmm3
; AVX1-NEXT:    vpmaxuw %xmm2, %xmm3, %xmm2
; AVX1-NEXT:    vpmaxuw %xmm1, %xmm0, %xmm0
; AVX1-NEXT:    vinsertf128 $1, %xmm2, %ymm0, %ymm0
; AVX1-NEXT:    retq
;
; AVX2-LABEL: max_ge_v16i16:
; AVX2:       # BB#0:
; AVX2-NEXT:    vpmaxuw %ymm1, %ymm0, %ymm0
; AVX2-NEXT:    retq
;
; AVX512-LABEL: max_ge_v16i16:
; AVX512:       # BB#0:
; AVX512-NEXT:    vpmaxuw %ymm1, %ymm0, %ymm0
; AVX512-NEXT:    retq
  %1 = icmp uge <16 x i16> %a, %b
  %2 = select <16 x i1> %1, <16 x i16> %a, <16 x i16> %b
  ret <16 x i16> %2
}

define <16 x i8> @max_ge_v16i8(<16 x i8> %a, <16 x i8> %b) {
; SSE-LABEL: max_ge_v16i8:
; SSE:       # BB#0:
; SSE-NEXT:    pmaxub %xmm1, %xmm0
; SSE-NEXT:    retq
;
; AVX-LABEL: max_ge_v16i8:
; AVX:       # BB#0:
; AVX-NEXT:    vpmaxub %xmm1, %xmm0, %xmm0
; AVX-NEXT:    retq
  %1 = icmp uge <16 x i8> %a, %b
  %2 = select <16 x i1> %1, <16 x i8> %a, <16 x i8> %b
  ret <16 x i8> %2
}

define <32 x i8> @max_ge_v32i8(<32 x i8> %a, <32 x i8> %b) {
; SSE-LABEL: max_ge_v32i8:
; SSE:       # BB#0:
; SSE-NEXT:    pmaxub %xmm2, %xmm0
; SSE-NEXT:    pmaxub %xmm3, %xmm1
; SSE-NEXT:    retq
;
; AVX1-LABEL: max_ge_v32i8:
; AVX1:       # BB#0:
; AVX1-NEXT:    vextractf128 $1, %ymm1, %xmm2
; AVX1-NEXT:    vextractf128 $1, %ymm0, %xmm3
; AVX1-NEXT:    vpmaxub %xmm2, %xmm3, %xmm2
; AVX1-NEXT:    vpmaxub %xmm1, %xmm0, %xmm0
; AVX1-NEXT:    vinsertf128 $1, %xmm2, %ymm0, %ymm0
; AVX1-NEXT:    retq
;
; AVX2-LABEL: max_ge_v32i8:
; AVX2:       # BB#0:
; AVX2-NEXT:    vpmaxub %ymm1, %ymm0, %ymm0
; AVX2-NEXT:    retq
;
; AVX512-LABEL: max_ge_v32i8:
; AVX512:       # BB#0:
; AVX512-NEXT:    vpmaxub %ymm1, %ymm0, %ymm0
; AVX512-NEXT:    retq
  %1 = icmp uge <32 x i8> %a, %b
  %2 = select <32 x i1> %1, <32 x i8> %a, <32 x i8> %b
  ret <32 x i8> %2
}

;
; Unsigned Minimum (LT)
;

define <2 x i64> @min_lt_v2i64(<2 x i64> %a, <2 x i64> %b) {
; SSE2-LABEL: min_lt_v2i64:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa {{.*#+}} xmm2 = [2147483648,2147483648,2147483648,2147483648]
; SSE2-NEXT:    movdqa %xmm0, %xmm3
; SSE2-NEXT:    pxor %xmm2, %xmm3
; SSE2-NEXT:    pxor %xmm1, %xmm2
; SSE2-NEXT:    movdqa %xmm2, %xmm4
; SSE2-NEXT:    pcmpgtd %xmm3, %xmm4
; SSE2-NEXT:    pshufd {{.*#+}} xmm5 = xmm4[0,0,2,2]
; SSE2-NEXT:    pcmpeqd %xmm3, %xmm2
; SSE2-NEXT:    pshufd {{.*#+}} xmm2 = xmm2[1,1,3,3]
; SSE2-NEXT:    pand %xmm5, %xmm2
; SSE2-NEXT:    pshufd {{.*#+}} xmm3 = xmm4[1,1,3,3]
; SSE2-NEXT:    por %xmm2, %xmm3
; SSE2-NEXT:    pand %xmm3, %xmm0
; SSE2-NEXT:    pandn %xmm1, %xmm3
; SSE2-NEXT:    por %xmm3, %xmm0
; SSE2-NEXT:    retq
;
; SSE41-LABEL: min_lt_v2i64:
; SSE41:       # BB#0:
; SSE41-NEXT:    movdqa %xmm0, %xmm2
; SSE41-NEXT:    movdqa {{.*#+}} xmm0 = [2147483648,2147483648,2147483648,2147483648]
; SSE41-NEXT:    movdqa %xmm2, %xmm3
; SSE41-NEXT:    pxor %xmm0, %xmm3
; SSE41-NEXT:    pxor %xmm1, %xmm0
; SSE41-NEXT:    movdqa %xmm0, %xmm4
; SSE41-NEXT:    pcmpgtd %xmm3, %xmm4
; SSE41-NEXT:    pshufd {{.*#+}} xmm5 = xmm4[0,0,2,2]
; SSE41-NEXT:    pcmpeqd %xmm3, %xmm0
; SSE41-NEXT:    pshufd {{.*#+}} xmm3 = xmm0[1,1,3,3]
; SSE41-NEXT:    pand %xmm5, %xmm3
; SSE41-NEXT:    pshufd {{.*#+}} xmm0 = xmm4[1,1,3,3]
; SSE41-NEXT:    por %xmm3, %xmm0
; SSE41-NEXT:    blendvpd %xmm2, %xmm1
; SSE41-NEXT:    movapd %xmm1, %xmm0
; SSE41-NEXT:    retq
;
; SSE42-LABEL: min_lt_v2i64:
; SSE42:       # BB#0:
; SSE42-NEXT:    movdqa %xmm0, %xmm2
; SSE42-NEXT:    movdqa {{.*#+}} xmm0 = [9223372036854775808,9223372036854775808]
; SSE42-NEXT:    movdqa %xmm2, %xmm3
; SSE42-NEXT:    pxor %xmm0, %xmm3
; SSE42-NEXT:    pxor %xmm1, %xmm0
; SSE42-NEXT:    pcmpgtq %xmm3, %xmm0
; SSE42-NEXT:    blendvpd %xmm2, %xmm1
; SSE42-NEXT:    movapd %xmm1, %xmm0
; SSE42-NEXT:    retq
;
; AVX-LABEL: min_lt_v2i64:
; AVX:       # BB#0:
; AVX-NEXT:    vmovdqa {{.*#+}} xmm2 = [9223372036854775808,9223372036854775808]
; AVX-NEXT:    vpxor %xmm2, %xmm0, %xmm3
; AVX-NEXT:    vpxor %xmm2, %xmm1, %xmm2
; AVX-NEXT:    vpcmpgtq %xmm3, %xmm2, %xmm2
; AVX-NEXT:    vblendvpd %xmm2, %xmm0, %xmm1, %xmm0
; AVX-NEXT:    retq
  %1 = icmp ult <2 x i64> %a, %b
  %2 = select <2 x i1> %1, <2 x i64> %a, <2 x i64> %b
  ret <2 x i64> %2
}

define <4 x i64> @min_lt_v4i64(<4 x i64> %a, <4 x i64> %b) {
; SSE2-LABEL: min_lt_v4i64:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa {{.*#+}} xmm4 = [2147483648,2147483648,2147483648,2147483648]
; SSE2-NEXT:    movdqa %xmm1, %xmm5
; SSE2-NEXT:    pxor %xmm4, %xmm5
; SSE2-NEXT:    movdqa %xmm3, %xmm6
; SSE2-NEXT:    pxor %xmm4, %xmm6
; SSE2-NEXT:    movdqa %xmm6, %xmm7
; SSE2-NEXT:    pcmpgtd %xmm5, %xmm7
; SSE2-NEXT:    pshufd {{.*#+}} xmm8 = xmm7[0,0,2,2]
; SSE2-NEXT:    pcmpeqd %xmm5, %xmm6
; SSE2-NEXT:    pshufd {{.*#+}} xmm5 = xmm6[1,1,3,3]
; SSE2-NEXT:    pand %xmm8, %xmm5
; SSE2-NEXT:    pshufd {{.*#+}} xmm6 = xmm7[1,1,3,3]
; SSE2-NEXT:    por %xmm5, %xmm6
; SSE2-NEXT:    movdqa %xmm0, %xmm5
; SSE2-NEXT:    pxor %xmm4, %xmm5
; SSE2-NEXT:    pxor %xmm2, %xmm4
; SSE2-NEXT:    movdqa %xmm4, %xmm7
; SSE2-NEXT:    pcmpgtd %xmm5, %xmm7
; SSE2-NEXT:    pshufd {{.*#+}} xmm8 = xmm7[0,0,2,2]
; SSE2-NEXT:    pcmpeqd %xmm5, %xmm4
; SSE2-NEXT:    pshufd {{.*#+}} xmm4 = xmm4[1,1,3,3]
; SSE2-NEXT:    pand %xmm8, %xmm4
; SSE2-NEXT:    pshufd {{.*#+}} xmm5 = xmm7[1,1,3,3]
; SSE2-NEXT:    por %xmm4, %xmm5
; SSE2-NEXT:    pand %xmm5, %xmm0
; SSE2-NEXT:    pandn %xmm2, %xmm5
; SSE2-NEXT:    por %xmm5, %xmm0
; SSE2-NEXT:    pand %xmm6, %xmm1
; SSE2-NEXT:    pandn %xmm3, %xmm6
; SSE2-NEXT:    por %xmm6, %xmm1
; SSE2-NEXT:    retq
;
; SSE41-LABEL: min_lt_v4i64:
; SSE41:       # BB#0:
; SSE41-NEXT:    movdqa %xmm0, %xmm8
; SSE41-NEXT:    movdqa {{.*#+}} xmm0 = [2147483648,2147483648,2147483648,2147483648]
; SSE41-NEXT:    movdqa %xmm1, %xmm5
; SSE41-NEXT:    pxor %xmm0, %xmm5
; SSE41-NEXT:    movdqa %xmm3, %xmm6
; SSE41-NEXT:    pxor %xmm0, %xmm6
; SSE41-NEXT:    movdqa %xmm6, %xmm7
; SSE41-NEXT:    pcmpgtd %xmm5, %xmm7
; SSE41-NEXT:    pshufd {{.*#+}} xmm4 = xmm7[0,0,2,2]
; SSE41-NEXT:    pcmpeqd %xmm5, %xmm6
; SSE41-NEXT:    pshufd {{.*#+}} xmm6 = xmm6[1,1,3,3]
; SSE41-NEXT:    pand %xmm4, %xmm6
; SSE41-NEXT:    pshufd {{.*#+}} xmm5 = xmm7[1,1,3,3]
; SSE41-NEXT:    por %xmm6, %xmm5
; SSE41-NEXT:    movdqa %xmm8, %xmm4
; SSE41-NEXT:    pxor %xmm0, %xmm4
; SSE41-NEXT:    pxor %xmm2, %xmm0
; SSE41-NEXT:    movdqa %xmm0, %xmm6
; SSE41-NEXT:    pcmpgtd %xmm4, %xmm6
; SSE41-NEXT:    pshufd {{.*#+}} xmm7 = xmm6[0,0,2,2]
; SSE41-NEXT:    pcmpeqd %xmm4, %xmm0
; SSE41-NEXT:    pshufd {{.*#+}} xmm4 = xmm0[1,1,3,3]
; SSE41-NEXT:    pand %xmm7, %xmm4
; SSE41-NEXT:    pshufd {{.*#+}} xmm0 = xmm6[1,1,3,3]
; SSE41-NEXT:    por %xmm4, %xmm0
; SSE41-NEXT:    blendvpd %xmm8, %xmm2
; SSE41-NEXT:    movdqa %xmm5, %xmm0
; SSE41-NEXT:    blendvpd %xmm1, %xmm3
; SSE41-NEXT:    movapd %xmm2, %xmm0
; SSE41-NEXT:    movapd %xmm3, %xmm1
; SSE41-NEXT:    retq
;
; SSE42-LABEL: min_lt_v4i64:
; SSE42:       # BB#0:
; SSE42-NEXT:    movdqa %xmm0, %xmm4
; SSE42-NEXT:    movdqa {{.*#+}} xmm0 = [9223372036854775808,9223372036854775808]
; SSE42-NEXT:    movdqa %xmm1, %xmm6
; SSE42-NEXT:    pxor %xmm0, %xmm6
; SSE42-NEXT:    movdqa %xmm3, %xmm5
; SSE42-NEXT:    pxor %xmm0, %xmm5
; SSE42-NEXT:    pcmpgtq %xmm6, %xmm5
; SSE42-NEXT:    movdqa %xmm4, %xmm6
; SSE42-NEXT:    pxor %xmm0, %xmm6
; SSE42-NEXT:    pxor %xmm2, %xmm0
; SSE42-NEXT:    pcmpgtq %xmm6, %xmm0
; SSE42-NEXT:    blendvpd %xmm4, %xmm2
; SSE42-NEXT:    movdqa %xmm5, %xmm0
; SSE42-NEXT:    blendvpd %xmm1, %xmm3
; SSE42-NEXT:    movapd %xmm2, %xmm0
; SSE42-NEXT:    movapd %xmm3, %xmm1
; SSE42-NEXT:    retq
;
; AVX1-LABEL: min_lt_v4i64:
; AVX1:       # BB#0:
; AVX1-NEXT:    vextractf128 $1, %ymm0, %xmm2
; AVX1-NEXT:    vmovaps {{.*#+}} xmm3 = [9223372036854775808,9223372036854775808]
; AVX1-NEXT:    vxorps %xmm3, %xmm2, %xmm2
; AVX1-NEXT:    vextractf128 $1, %ymm1, %xmm4
; AVX1-NEXT:    vxorps %xmm3, %xmm4, %xmm4
; AVX1-NEXT:    vpcmpgtq %xmm2, %xmm4, %xmm2
; AVX1-NEXT:    vxorps %xmm3, %xmm0, %xmm4
; AVX1-NEXT:    vxorps %xmm3, %xmm1, %xmm3
; AVX1-NEXT:    vpcmpgtq %xmm4, %xmm3, %xmm3
; AVX1-NEXT:    vinsertf128 $1, %xmm2, %ymm3, %ymm2
; AVX1-NEXT:    vblendvpd %ymm2, %ymm0, %ymm1, %ymm0
; AVX1-NEXT:    retq
;
; AVX2-LABEL: min_lt_v4i64:
; AVX2:       # BB#0:
; AVX2-NEXT:    vpbroadcastq {{.*}}(%rip), %ymm2
; AVX2-NEXT:    vpxor %ymm2, %ymm0, %ymm3
; AVX2-NEXT:    vpxor %ymm2, %ymm1, %ymm2
; AVX2-NEXT:    vpcmpgtq %ymm3, %ymm2, %ymm2
; AVX2-NEXT:    vblendvpd %ymm2, %ymm0, %ymm1, %ymm0
; AVX2-NEXT:    retq
;
; AVX512-LABEL: min_lt_v4i64:
; AVX512:       # BB#0:
; AVX512-NEXT:    vpbroadcastq {{.*}}(%rip), %ymm2
; AVX512-NEXT:    vpxor %ymm2, %ymm0, %ymm3
; AVX512-NEXT:    vpxor %ymm2, %ymm1, %ymm2
; AVX512-NEXT:    vpcmpgtq %ymm3, %ymm2, %ymm2
; AVX512-NEXT:    vblendvpd %ymm2, %ymm0, %ymm1, %ymm0
; AVX512-NEXT:    retq
  %1 = icmp ult <4 x i64> %a, %b
  %2 = select <4 x i1> %1, <4 x i64> %a, <4 x i64> %b
  ret <4 x i64> %2
}

define <4 x i32> @min_lt_v4i32(<4 x i32> %a, <4 x i32> %b) {
; SSE2-LABEL: min_lt_v4i32:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa {{.*#+}} xmm2 = [2147483648,2147483648,2147483648,2147483648]
; SSE2-NEXT:    movdqa %xmm0, %xmm3
; SSE2-NEXT:    pxor %xmm2, %xmm3
; SSE2-NEXT:    pxor %xmm1, %xmm2
; SSE2-NEXT:    pcmpgtd %xmm3, %xmm2
; SSE2-NEXT:    pand %xmm2, %xmm0
; SSE2-NEXT:    pandn %xmm1, %xmm2
; SSE2-NEXT:    por %xmm2, %xmm0
; SSE2-NEXT:    retq
;
; SSE41-LABEL: min_lt_v4i32:
; SSE41:       # BB#0:
; SSE41-NEXT:    pminud %xmm1, %xmm0
; SSE41-NEXT:    retq
;
; SSE42-LABEL: min_lt_v4i32:
; SSE42:       # BB#0:
; SSE42-NEXT:    pminud %xmm1, %xmm0
; SSE42-NEXT:    retq
;
; AVX-LABEL: min_lt_v4i32:
; AVX:       # BB#0:
; AVX-NEXT:    vpminud %xmm1, %xmm0, %xmm0
; AVX-NEXT:    retq
  %1 = icmp ult <4 x i32> %a, %b
  %2 = select <4 x i1> %1, <4 x i32> %a, <4 x i32> %b
  ret <4 x i32> %2
}

define <8 x i32> @min_lt_v8i32(<8 x i32> %a, <8 x i32> %b) {
; SSE2-LABEL: min_lt_v8i32:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa {{.*#+}} xmm4 = [2147483648,2147483648,2147483648,2147483648]
; SSE2-NEXT:    movdqa %xmm1, %xmm5
; SSE2-NEXT:    pxor %xmm4, %xmm5
; SSE2-NEXT:    movdqa %xmm3, %xmm6
; SSE2-NEXT:    pxor %xmm4, %xmm6
; SSE2-NEXT:    pcmpgtd %xmm5, %xmm6
; SSE2-NEXT:    movdqa %xmm0, %xmm5
; SSE2-NEXT:    pxor %xmm4, %xmm5
; SSE2-NEXT:    pxor %xmm2, %xmm4
; SSE2-NEXT:    pcmpgtd %xmm5, %xmm4
; SSE2-NEXT:    pand %xmm4, %xmm0
; SSE2-NEXT:    pandn %xmm2, %xmm4
; SSE2-NEXT:    por %xmm4, %xmm0
; SSE2-NEXT:    pand %xmm6, %xmm1
; SSE2-NEXT:    pandn %xmm3, %xmm6
; SSE2-NEXT:    por %xmm6, %xmm1
; SSE2-NEXT:    retq
;
; SSE41-LABEL: min_lt_v8i32:
; SSE41:       # BB#0:
; SSE41-NEXT:    pminud %xmm2, %xmm0
; SSE41-NEXT:    pminud %xmm3, %xmm1
; SSE41-NEXT:    retq
;
; SSE42-LABEL: min_lt_v8i32:
; SSE42:       # BB#0:
; SSE42-NEXT:    pminud %xmm2, %xmm0
; SSE42-NEXT:    pminud %xmm3, %xmm1
; SSE42-NEXT:    retq
;
; AVX1-LABEL: min_lt_v8i32:
; AVX1:       # BB#0:
; AVX1-NEXT:    vextractf128 $1, %ymm1, %xmm2
; AVX1-NEXT:    vextractf128 $1, %ymm0, %xmm3
; AVX1-NEXT:    vpminud %xmm2, %xmm3, %xmm2
; AVX1-NEXT:    vpminud %xmm1, %xmm0, %xmm0
; AVX1-NEXT:    vinsertf128 $1, %xmm2, %ymm0, %ymm0
; AVX1-NEXT:    retq
;
; AVX2-LABEL: min_lt_v8i32:
; AVX2:       # BB#0:
; AVX2-NEXT:    vpminud %ymm1, %ymm0, %ymm0
; AVX2-NEXT:    retq
;
; AVX512-LABEL: min_lt_v8i32:
; AVX512:       # BB#0:
; AVX512-NEXT:    vpminud %ymm1, %ymm0, %ymm0
; AVX512-NEXT:    retq
  %1 = icmp ult <8 x i32> %a, %b
  %2 = select <8 x i1> %1, <8 x i32> %a, <8 x i32> %b
  ret <8 x i32> %2
}

define <8 x i16> @min_lt_v8i16(<8 x i16> %a, <8 x i16> %b) {
; SSE2-LABEL: min_lt_v8i16:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa {{.*#+}} xmm2 = [32768,32768,32768,32768,32768,32768,32768,32768]
; SSE2-NEXT:    movdqa %xmm0, %xmm3
; SSE2-NEXT:    pxor %xmm2, %xmm3
; SSE2-NEXT:    pxor %xmm1, %xmm2
; SSE2-NEXT:    pcmpgtw %xmm3, %xmm2
; SSE2-NEXT:    pand %xmm2, %xmm0
; SSE2-NEXT:    pandn %xmm1, %xmm2
; SSE2-NEXT:    por %xmm2, %xmm0
; SSE2-NEXT:    retq
;
; SSE41-LABEL: min_lt_v8i16:
; SSE41:       # BB#0:
; SSE41-NEXT:    pminuw %xmm1, %xmm0
; SSE41-NEXT:    retq
;
; SSE42-LABEL: min_lt_v8i16:
; SSE42:       # BB#0:
; SSE42-NEXT:    pminuw %xmm1, %xmm0
; SSE42-NEXT:    retq
;
; AVX-LABEL: min_lt_v8i16:
; AVX:       # BB#0:
; AVX-NEXT:    vpminuw %xmm1, %xmm0, %xmm0
; AVX-NEXT:    retq
  %1 = icmp ult <8 x i16> %a, %b
  %2 = select <8 x i1> %1, <8 x i16> %a, <8 x i16> %b
  ret <8 x i16> %2
}

define <16 x i16> @min_lt_v16i16(<16 x i16> %a, <16 x i16> %b) {
; SSE2-LABEL: min_lt_v16i16:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa {{.*#+}} xmm4 = [32768,32768,32768,32768,32768,32768,32768,32768]
; SSE2-NEXT:    movdqa %xmm1, %xmm5
; SSE2-NEXT:    pxor %xmm4, %xmm5
; SSE2-NEXT:    movdqa %xmm3, %xmm6
; SSE2-NEXT:    pxor %xmm4, %xmm6
; SSE2-NEXT:    pcmpgtw %xmm5, %xmm6
; SSE2-NEXT:    movdqa %xmm0, %xmm5
; SSE2-NEXT:    pxor %xmm4, %xmm5
; SSE2-NEXT:    pxor %xmm2, %xmm4
; SSE2-NEXT:    pcmpgtw %xmm5, %xmm4
; SSE2-NEXT:    pand %xmm4, %xmm0
; SSE2-NEXT:    pandn %xmm2, %xmm4
; SSE2-NEXT:    por %xmm4, %xmm0
; SSE2-NEXT:    pand %xmm6, %xmm1
; SSE2-NEXT:    pandn %xmm3, %xmm6
; SSE2-NEXT:    por %xmm6, %xmm1
; SSE2-NEXT:    retq
;
; SSE41-LABEL: min_lt_v16i16:
; SSE41:       # BB#0:
; SSE41-NEXT:    pminuw %xmm2, %xmm0
; SSE41-NEXT:    pminuw %xmm3, %xmm1
; SSE41-NEXT:    retq
;
; SSE42-LABEL: min_lt_v16i16:
; SSE42:       # BB#0:
; SSE42-NEXT:    pminuw %xmm2, %xmm0
; SSE42-NEXT:    pminuw %xmm3, %xmm1
; SSE42-NEXT:    retq
;
; AVX1-LABEL: min_lt_v16i16:
; AVX1:       # BB#0:
; AVX1-NEXT:    vextractf128 $1, %ymm1, %xmm2
; AVX1-NEXT:    vextractf128 $1, %ymm0, %xmm3
; AVX1-NEXT:    vpminuw %xmm2, %xmm3, %xmm2
; AVX1-NEXT:    vpminuw %xmm1, %xmm0, %xmm0
; AVX1-NEXT:    vinsertf128 $1, %xmm2, %ymm0, %ymm0
; AVX1-NEXT:    retq
;
; AVX2-LABEL: min_lt_v16i16:
; AVX2:       # BB#0:
; AVX2-NEXT:    vpminuw %ymm1, %ymm0, %ymm0
; AVX2-NEXT:    retq
;
; AVX512-LABEL: min_lt_v16i16:
; AVX512:       # BB#0:
; AVX512-NEXT:    vpminuw %ymm1, %ymm0, %ymm0
; AVX512-NEXT:    retq
  %1 = icmp ult <16 x i16> %a, %b
  %2 = select <16 x i1> %1, <16 x i16> %a, <16 x i16> %b
  ret <16 x i16> %2
}

define <16 x i8> @min_lt_v16i8(<16 x i8> %a, <16 x i8> %b) {
; SSE-LABEL: min_lt_v16i8:
; SSE:       # BB#0:
; SSE-NEXT:    pminub %xmm1, %xmm0
; SSE-NEXT:    retq
;
; AVX-LABEL: min_lt_v16i8:
; AVX:       # BB#0:
; AVX-NEXT:    vpminub %xmm1, %xmm0, %xmm0
; AVX-NEXT:    retq
  %1 = icmp ult <16 x i8> %a, %b
  %2 = select <16 x i1> %1, <16 x i8> %a, <16 x i8> %b
  ret <16 x i8> %2
}

define <32 x i8> @min_lt_v32i8(<32 x i8> %a, <32 x i8> %b) {
; SSE-LABEL: min_lt_v32i8:
; SSE:       # BB#0:
; SSE-NEXT:    pminub %xmm2, %xmm0
; SSE-NEXT:    pminub %xmm3, %xmm1
; SSE-NEXT:    retq
;
; AVX1-LABEL: min_lt_v32i8:
; AVX1:       # BB#0:
; AVX1-NEXT:    vextractf128 $1, %ymm1, %xmm2
; AVX1-NEXT:    vextractf128 $1, %ymm0, %xmm3
; AVX1-NEXT:    vpminub %xmm2, %xmm3, %xmm2
; AVX1-NEXT:    vpminub %xmm1, %xmm0, %xmm0
; AVX1-NEXT:    vinsertf128 $1, %xmm2, %ymm0, %ymm0
; AVX1-NEXT:    retq
;
; AVX2-LABEL: min_lt_v32i8:
; AVX2:       # BB#0:
; AVX2-NEXT:    vpminub %ymm1, %ymm0, %ymm0
; AVX2-NEXT:    retq
;
; AVX512-LABEL: min_lt_v32i8:
; AVX512:       # BB#0:
; AVX512-NEXT:    vpminub %ymm1, %ymm0, %ymm0
; AVX512-NEXT:    retq
  %1 = icmp ult <32 x i8> %a, %b
  %2 = select <32 x i1> %1, <32 x i8> %a, <32 x i8> %b
  ret <32 x i8> %2
}

;
; Unsigned Minimum (LE)
;

define <2 x i64> @min_le_v2i64(<2 x i64> %a, <2 x i64> %b) {
; SSE2-LABEL: min_le_v2i64:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa {{.*#+}} xmm2 = [2147483648,2147483648,2147483648,2147483648]
; SSE2-NEXT:    movdqa %xmm1, %xmm3
; SSE2-NEXT:    pxor %xmm2, %xmm3
; SSE2-NEXT:    pxor %xmm0, %xmm2
; SSE2-NEXT:    movdqa %xmm2, %xmm4
; SSE2-NEXT:    pcmpgtd %xmm3, %xmm4
; SSE2-NEXT:    pshufd {{.*#+}} xmm5 = xmm4[0,0,2,2]
; SSE2-NEXT:    pcmpeqd %xmm3, %xmm2
; SSE2-NEXT:    pshufd {{.*#+}} xmm2 = xmm2[1,1,3,3]
; SSE2-NEXT:    pand %xmm5, %xmm2
; SSE2-NEXT:    pshufd {{.*#+}} xmm3 = xmm4[1,1,3,3]
; SSE2-NEXT:    por %xmm2, %xmm3
; SSE2-NEXT:    pcmpeqd %xmm2, %xmm2
; SSE2-NEXT:    pxor %xmm3, %xmm2
; SSE2-NEXT:    pandn %xmm0, %xmm3
; SSE2-NEXT:    pandn %xmm1, %xmm2
; SSE2-NEXT:    por %xmm3, %xmm2
; SSE2-NEXT:    movdqa %xmm2, %xmm0
; SSE2-NEXT:    retq
;
; SSE41-LABEL: min_le_v2i64:
; SSE41:       # BB#0:
; SSE41-NEXT:    movdqa %xmm0, %xmm2
; SSE41-NEXT:    movdqa {{.*#+}} xmm0 = [2147483648,2147483648,2147483648,2147483648]
; SSE41-NEXT:    movdqa %xmm1, %xmm3
; SSE41-NEXT:    pxor %xmm0, %xmm3
; SSE41-NEXT:    pxor %xmm2, %xmm0
; SSE41-NEXT:    movdqa %xmm0, %xmm4
; SSE41-NEXT:    pcmpgtd %xmm3, %xmm4
; SSE41-NEXT:    pshufd {{.*#+}} xmm5 = xmm4[0,0,2,2]
; SSE41-NEXT:    pcmpeqd %xmm3, %xmm0
; SSE41-NEXT:    pshufd {{.*#+}} xmm0 = xmm0[1,1,3,3]
; SSE41-NEXT:    pand %xmm5, %xmm0
; SSE41-NEXT:    pshufd {{.*#+}} xmm3 = xmm4[1,1,3,3]
; SSE41-NEXT:    por %xmm0, %xmm3
; SSE41-NEXT:    pcmpeqd %xmm0, %xmm0
; SSE41-NEXT:    pxor %xmm3, %xmm0
; SSE41-NEXT:    blendvpd %xmm2, %xmm1
; SSE41-NEXT:    movapd %xmm1, %xmm0
; SSE41-NEXT:    retq
;
; SSE42-LABEL: min_le_v2i64:
; SSE42:       # BB#0:
; SSE42-NEXT:    movdqa %xmm0, %xmm2
; SSE42-NEXT:    movdqa {{.*#+}} xmm3 = [9223372036854775808,9223372036854775808]
; SSE42-NEXT:    movdqa %xmm1, %xmm0
; SSE42-NEXT:    pxor %xmm3, %xmm0
; SSE42-NEXT:    pxor %xmm2, %xmm3
; SSE42-NEXT:    pcmpgtq %xmm0, %xmm3
; SSE42-NEXT:    pcmpeqd %xmm0, %xmm0
; SSE42-NEXT:    pxor %xmm3, %xmm0
; SSE42-NEXT:    blendvpd %xmm2, %xmm1
; SSE42-NEXT:    movapd %xmm1, %xmm0
; SSE42-NEXT:    retq
;
; AVX-LABEL: min_le_v2i64:
; AVX:       # BB#0:
; AVX-NEXT:    vmovdqa {{.*#+}} xmm2 = [9223372036854775808,9223372036854775808]
; AVX-NEXT:    vpxor %xmm2, %xmm1, %xmm3
; AVX-NEXT:    vpxor %xmm2, %xmm0, %xmm2
; AVX-NEXT:    vpcmpgtq %xmm3, %xmm2, %xmm2
; AVX-NEXT:    vpcmpeqd %xmm3, %xmm3, %xmm3
; AVX-NEXT:    vpxor %xmm3, %xmm2, %xmm2
; AVX-NEXT:    vblendvpd %xmm2, %xmm0, %xmm1, %xmm0
; AVX-NEXT:    retq
  %1 = icmp ule <2 x i64> %a, %b
  %2 = select <2 x i1> %1, <2 x i64> %a, <2 x i64> %b
  ret <2 x i64> %2
}

define <4 x i64> @min_le_v4i64(<4 x i64> %a, <4 x i64> %b) {
; SSE2-LABEL: min_le_v4i64:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa {{.*#+}} xmm7 = [2147483648,2147483648,2147483648,2147483648]
; SSE2-NEXT:    movdqa %xmm3, %xmm4
; SSE2-NEXT:    pxor %xmm7, %xmm4
; SSE2-NEXT:    movdqa %xmm1, %xmm5
; SSE2-NEXT:    pxor %xmm7, %xmm5
; SSE2-NEXT:    movdqa %xmm5, %xmm6
; SSE2-NEXT:    pcmpgtd %xmm4, %xmm6
; SSE2-NEXT:    pshufd {{.*#+}} xmm8 = xmm6[0,0,2,2]
; SSE2-NEXT:    pcmpeqd %xmm4, %xmm5
; SSE2-NEXT:    pshufd {{.*#+}} xmm4 = xmm5[1,1,3,3]
; SSE2-NEXT:    pand %xmm8, %xmm4
; SSE2-NEXT:    pshufd {{.*#+}} xmm8 = xmm6[1,1,3,3]
; SSE2-NEXT:    por %xmm4, %xmm8
; SSE2-NEXT:    pcmpeqd %xmm4, %xmm4
; SSE2-NEXT:    movdqa %xmm8, %xmm9
; SSE2-NEXT:    pxor %xmm4, %xmm9
; SSE2-NEXT:    movdqa %xmm2, %xmm6
; SSE2-NEXT:    pxor %xmm7, %xmm6
; SSE2-NEXT:    pxor %xmm0, %xmm7
; SSE2-NEXT:    movdqa %xmm7, %xmm5
; SSE2-NEXT:    pcmpgtd %xmm6, %xmm5
; SSE2-NEXT:    pshufd {{.*#+}} xmm10 = xmm5[0,0,2,2]
; SSE2-NEXT:    pcmpeqd %xmm6, %xmm7
; SSE2-NEXT:    pshufd {{.*#+}} xmm6 = xmm7[1,1,3,3]
; SSE2-NEXT:    pand %xmm10, %xmm6
; SSE2-NEXT:    pshufd {{.*#+}} xmm5 = xmm5[1,1,3,3]
; SSE2-NEXT:    por %xmm6, %xmm5
; SSE2-NEXT:    pxor %xmm5, %xmm4
; SSE2-NEXT:    pandn %xmm0, %xmm5
; SSE2-NEXT:    pandn %xmm2, %xmm4
; SSE2-NEXT:    por %xmm5, %xmm4
; SSE2-NEXT:    pandn %xmm1, %xmm8
; SSE2-NEXT:    pandn %xmm3, %xmm9
; SSE2-NEXT:    por %xmm8, %xmm9
; SSE2-NEXT:    movdqa %xmm4, %xmm0
; SSE2-NEXT:    movdqa %xmm9, %xmm1
; SSE2-NEXT:    retq
;
; SSE41-LABEL: min_le_v4i64:
; SSE41:       # BB#0:
; SSE41-NEXT:    movdqa %xmm0, %xmm8
; SSE41-NEXT:    movdqa {{.*#+}} xmm0 = [2147483648,2147483648,2147483648,2147483648]
; SSE41-NEXT:    movdqa %xmm3, %xmm5
; SSE41-NEXT:    pxor %xmm0, %xmm5
; SSE41-NEXT:    movdqa %xmm1, %xmm6
; SSE41-NEXT:    pxor %xmm0, %xmm6
; SSE41-NEXT:    movdqa %xmm6, %xmm7
; SSE41-NEXT:    pcmpgtd %xmm5, %xmm7
; SSE41-NEXT:    pshufd {{.*#+}} xmm4 = xmm7[0,0,2,2]
; SSE41-NEXT:    pcmpeqd %xmm5, %xmm6
; SSE41-NEXT:    pshufd {{.*#+}} xmm6 = xmm6[1,1,3,3]
; SSE41-NEXT:    pand %xmm4, %xmm6
; SSE41-NEXT:    pshufd {{.*#+}} xmm5 = xmm7[1,1,3,3]
; SSE41-NEXT:    por %xmm6, %xmm5
; SSE41-NEXT:    pcmpeqd %xmm9, %xmm9
; SSE41-NEXT:    pxor %xmm9, %xmm5
; SSE41-NEXT:    movdqa %xmm2, %xmm6
; SSE41-NEXT:    pxor %xmm0, %xmm6
; SSE41-NEXT:    pxor %xmm8, %xmm0
; SSE41-NEXT:    movdqa %xmm0, %xmm7
; SSE41-NEXT:    pcmpgtd %xmm6, %xmm7
; SSE41-NEXT:    pshufd {{.*#+}} xmm4 = xmm7[0,0,2,2]
; SSE41-NEXT:    pcmpeqd %xmm6, %xmm0
; SSE41-NEXT:    pshufd {{.*#+}} xmm6 = xmm0[1,1,3,3]
; SSE41-NEXT:    pand %xmm4, %xmm6
; SSE41-NEXT:    pshufd {{.*#+}} xmm0 = xmm7[1,1,3,3]
; SSE41-NEXT:    por %xmm6, %xmm0
; SSE41-NEXT:    pxor %xmm9, %xmm0
; SSE41-NEXT:    blendvpd %xmm8, %xmm2
; SSE41-NEXT:    movdqa %xmm5, %xmm0
; SSE41-NEXT:    blendvpd %xmm1, %xmm3
; SSE41-NEXT:    movapd %xmm2, %xmm0
; SSE41-NEXT:    movapd %xmm3, %xmm1
; SSE41-NEXT:    retq
;
; SSE42-LABEL: min_le_v4i64:
; SSE42:       # BB#0:
; SSE42-NEXT:    movdqa %xmm0, %xmm4
; SSE42-NEXT:    movdqa {{.*#+}} xmm0 = [9223372036854775808,9223372036854775808]
; SSE42-NEXT:    movdqa %xmm3, %xmm6
; SSE42-NEXT:    pxor %xmm0, %xmm6
; SSE42-NEXT:    movdqa %xmm1, %xmm5
; SSE42-NEXT:    pxor %xmm0, %xmm5
; SSE42-NEXT:    pcmpgtq %xmm6, %xmm5
; SSE42-NEXT:    pcmpeqd %xmm6, %xmm6
; SSE42-NEXT:    pxor %xmm6, %xmm5
; SSE42-NEXT:    movdqa %xmm2, %xmm7
; SSE42-NEXT:    pxor %xmm0, %xmm7
; SSE42-NEXT:    pxor %xmm4, %xmm0
; SSE42-NEXT:    pcmpgtq %xmm7, %xmm0
; SSE42-NEXT:    pxor %xmm6, %xmm0
; SSE42-NEXT:    blendvpd %xmm4, %xmm2
; SSE42-NEXT:    movdqa %xmm5, %xmm0
; SSE42-NEXT:    blendvpd %xmm1, %xmm3
; SSE42-NEXT:    movapd %xmm2, %xmm0
; SSE42-NEXT:    movapd %xmm3, %xmm1
; SSE42-NEXT:    retq
;
; AVX1-LABEL: min_le_v4i64:
; AVX1:       # BB#0:
; AVX1-NEXT:    vextractf128 $1, %ymm1, %xmm2
; AVX1-NEXT:    vmovaps {{.*#+}} xmm3 = [9223372036854775808,9223372036854775808]
; AVX1-NEXT:    vxorps %xmm3, %xmm2, %xmm2
; AVX1-NEXT:    vextractf128 $1, %ymm0, %xmm4
; AVX1-NEXT:    vxorps %xmm3, %xmm4, %xmm4
; AVX1-NEXT:    vpcmpgtq %xmm2, %xmm4, %xmm2
; AVX1-NEXT:    vpcmpeqd %xmm4, %xmm4, %xmm4
; AVX1-NEXT:    vpxor %xmm4, %xmm2, %xmm2
; AVX1-NEXT:    vxorps %xmm3, %xmm1, %xmm5
; AVX1-NEXT:    vxorps %xmm3, %xmm0, %xmm3
; AVX1-NEXT:    vpcmpgtq %xmm5, %xmm3, %xmm3
; AVX1-NEXT:    vpxor %xmm4, %xmm3, %xmm3
; AVX1-NEXT:    vinsertf128 $1, %xmm2, %ymm3, %ymm2
; AVX1-NEXT:    vblendvpd %ymm2, %ymm0, %ymm1, %ymm0
; AVX1-NEXT:    retq
;
; AVX2-LABEL: min_le_v4i64:
; AVX2:       # BB#0:
; AVX2-NEXT:    vpbroadcastq {{.*}}(%rip), %ymm2
; AVX2-NEXT:    vpxor %ymm2, %ymm1, %ymm3
; AVX2-NEXT:    vpxor %ymm2, %ymm0, %ymm2
; AVX2-NEXT:    vpcmpgtq %ymm3, %ymm2, %ymm2
; AVX2-NEXT:    vpcmpeqd %ymm3, %ymm3, %ymm3
; AVX2-NEXT:    vpxor %ymm3, %ymm2, %ymm2
; AVX2-NEXT:    vblendvpd %ymm2, %ymm0, %ymm1, %ymm0
; AVX2-NEXT:    retq
;
; AVX512-LABEL: min_le_v4i64:
; AVX512:       # BB#0:
; AVX512-NEXT:    vpbroadcastq {{.*}}(%rip), %ymm2
; AVX512-NEXT:    vpxor %ymm2, %ymm1, %ymm3
; AVX512-NEXT:    vpxor %ymm2, %ymm0, %ymm2
; AVX512-NEXT:    vpcmpgtq %ymm3, %ymm2, %ymm2
; AVX512-NEXT:    vpcmpeqd %ymm3, %ymm3, %ymm3
; AVX512-NEXT:    vpxor %ymm3, %ymm2, %ymm2
; AVX512-NEXT:    vblendvpd %ymm2, %ymm0, %ymm1, %ymm0
; AVX512-NEXT:    retq
  %1 = icmp ule <4 x i64> %a, %b
  %2 = select <4 x i1> %1, <4 x i64> %a, <4 x i64> %b
  ret <4 x i64> %2
}

define <4 x i32> @min_le_v4i32(<4 x i32> %a, <4 x i32> %b) {
; SSE2-LABEL: min_le_v4i32:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa {{.*#+}} xmm3 = [2147483648,2147483648,2147483648,2147483648]
; SSE2-NEXT:    movdqa %xmm1, %xmm2
; SSE2-NEXT:    pxor %xmm3, %xmm2
; SSE2-NEXT:    pxor %xmm0, %xmm3
; SSE2-NEXT:    pcmpgtd %xmm2, %xmm3
; SSE2-NEXT:    pcmpeqd %xmm2, %xmm2
; SSE2-NEXT:    pxor %xmm3, %xmm2
; SSE2-NEXT:    pandn %xmm0, %xmm3
; SSE2-NEXT:    pandn %xmm1, %xmm2
; SSE2-NEXT:    por %xmm3, %xmm2
; SSE2-NEXT:    movdqa %xmm2, %xmm0
; SSE2-NEXT:    retq
;
; SSE41-LABEL: min_le_v4i32:
; SSE41:       # BB#0:
; SSE41-NEXT:    pminud %xmm1, %xmm0
; SSE41-NEXT:    retq
;
; SSE42-LABEL: min_le_v4i32:
; SSE42:       # BB#0:
; SSE42-NEXT:    pminud %xmm1, %xmm0
; SSE42-NEXT:    retq
;
; AVX-LABEL: min_le_v4i32:
; AVX:       # BB#0:
; AVX-NEXT:    vpminud %xmm1, %xmm0, %xmm0
; AVX-NEXT:    retq
  %1 = icmp ule <4 x i32> %a, %b
  %2 = select <4 x i1> %1, <4 x i32> %a, <4 x i32> %b
  ret <4 x i32> %2
}

define <8 x i32> @min_le_v8i32(<8 x i32> %a, <8 x i32> %b) {
; SSE2-LABEL: min_le_v8i32:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa {{.*#+}} xmm6 = [2147483648,2147483648,2147483648,2147483648]
; SSE2-NEXT:    movdqa %xmm3, %xmm4
; SSE2-NEXT:    pxor %xmm6, %xmm4
; SSE2-NEXT:    movdqa %xmm1, %xmm7
; SSE2-NEXT:    pxor %xmm6, %xmm7
; SSE2-NEXT:    pcmpgtd %xmm4, %xmm7
; SSE2-NEXT:    pcmpeqd %xmm4, %xmm4
; SSE2-NEXT:    movdqa %xmm7, %xmm5
; SSE2-NEXT:    pxor %xmm4, %xmm5
; SSE2-NEXT:    movdqa %xmm2, %xmm8
; SSE2-NEXT:    pxor %xmm6, %xmm8
; SSE2-NEXT:    pxor %xmm0, %xmm6
; SSE2-NEXT:    pcmpgtd %xmm8, %xmm6
; SSE2-NEXT:    pxor %xmm6, %xmm4
; SSE2-NEXT:    pandn %xmm0, %xmm6
; SSE2-NEXT:    pandn %xmm2, %xmm4
; SSE2-NEXT:    por %xmm6, %xmm4
; SSE2-NEXT:    pandn %xmm1, %xmm7
; SSE2-NEXT:    pandn %xmm3, %xmm5
; SSE2-NEXT:    por %xmm7, %xmm5
; SSE2-NEXT:    movdqa %xmm4, %xmm0
; SSE2-NEXT:    movdqa %xmm5, %xmm1
; SSE2-NEXT:    retq
;
; SSE41-LABEL: min_le_v8i32:
; SSE41:       # BB#0:
; SSE41-NEXT:    pminud %xmm2, %xmm0
; SSE41-NEXT:    pminud %xmm3, %xmm1
; SSE41-NEXT:    retq
;
; SSE42-LABEL: min_le_v8i32:
; SSE42:       # BB#0:
; SSE42-NEXT:    pminud %xmm2, %xmm0
; SSE42-NEXT:    pminud %xmm3, %xmm1
; SSE42-NEXT:    retq
;
; AVX1-LABEL: min_le_v8i32:
; AVX1:       # BB#0:
; AVX1-NEXT:    vextractf128 $1, %ymm1, %xmm2
; AVX1-NEXT:    vextractf128 $1, %ymm0, %xmm3
; AVX1-NEXT:    vpminud %xmm2, %xmm3, %xmm2
; AVX1-NEXT:    vpminud %xmm1, %xmm0, %xmm0
; AVX1-NEXT:    vinsertf128 $1, %xmm2, %ymm0, %ymm0
; AVX1-NEXT:    retq
;
; AVX2-LABEL: min_le_v8i32:
; AVX2:       # BB#0:
; AVX2-NEXT:    vpminud %ymm1, %ymm0, %ymm0
; AVX2-NEXT:    retq
;
; AVX512-LABEL: min_le_v8i32:
; AVX512:       # BB#0:
; AVX512-NEXT:    vpminud %ymm1, %ymm0, %ymm0
; AVX512-NEXT:    retq
  %1 = icmp ule <8 x i32> %a, %b
  %2 = select <8 x i1> %1, <8 x i32> %a, <8 x i32> %b
  ret <8 x i32> %2
}

define <8 x i16> @min_le_v8i16(<8 x i16> %a, <8 x i16> %b) {
; SSE2-LABEL: min_le_v8i16:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa %xmm0, %xmm2
; SSE2-NEXT:    psubusw %xmm1, %xmm2
; SSE2-NEXT:    pxor %xmm3, %xmm3
; SSE2-NEXT:    pcmpeqw %xmm2, %xmm3
; SSE2-NEXT:    pand %xmm3, %xmm0
; SSE2-NEXT:    pandn %xmm1, %xmm3
; SSE2-NEXT:    por %xmm3, %xmm0
; SSE2-NEXT:    retq
;
; SSE41-LABEL: min_le_v8i16:
; SSE41:       # BB#0:
; SSE41-NEXT:    pminuw %xmm1, %xmm0
; SSE41-NEXT:    retq
;
; SSE42-LABEL: min_le_v8i16:
; SSE42:       # BB#0:
; SSE42-NEXT:    pminuw %xmm1, %xmm0
; SSE42-NEXT:    retq
;
; AVX-LABEL: min_le_v8i16:
; AVX:       # BB#0:
; AVX-NEXT:    vpminuw %xmm1, %xmm0, %xmm0
; AVX-NEXT:    retq
  %1 = icmp ule <8 x i16> %a, %b
  %2 = select <8 x i1> %1, <8 x i16> %a, <8 x i16> %b
  ret <8 x i16> %2
}

define <16 x i16> @min_le_v16i16(<16 x i16> %a, <16 x i16> %b) {
; SSE2-LABEL: min_le_v16i16:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa %xmm1, %xmm4
; SSE2-NEXT:    psubusw %xmm3, %xmm4
; SSE2-NEXT:    pxor %xmm6, %xmm6
; SSE2-NEXT:    pcmpeqw %xmm6, %xmm4
; SSE2-NEXT:    movdqa %xmm0, %xmm5
; SSE2-NEXT:    psubusw %xmm2, %xmm5
; SSE2-NEXT:    pcmpeqw %xmm6, %xmm5
; SSE2-NEXT:    pand %xmm5, %xmm0
; SSE2-NEXT:    pandn %xmm2, %xmm5
; SSE2-NEXT:    por %xmm0, %xmm5
; SSE2-NEXT:    pand %xmm4, %xmm1
; SSE2-NEXT:    pandn %xmm3, %xmm4
; SSE2-NEXT:    por %xmm1, %xmm4
; SSE2-NEXT:    movdqa %xmm5, %xmm0
; SSE2-NEXT:    movdqa %xmm4, %xmm1
; SSE2-NEXT:    retq
;
; SSE41-LABEL: min_le_v16i16:
; SSE41:       # BB#0:
; SSE41-NEXT:    pminuw %xmm2, %xmm0
; SSE41-NEXT:    pminuw %xmm3, %xmm1
; SSE41-NEXT:    retq
;
; SSE42-LABEL: min_le_v16i16:
; SSE42:       # BB#0:
; SSE42-NEXT:    pminuw %xmm2, %xmm0
; SSE42-NEXT:    pminuw %xmm3, %xmm1
; SSE42-NEXT:    retq
;
; AVX1-LABEL: min_le_v16i16:
; AVX1:       # BB#0:
; AVX1-NEXT:    vextractf128 $1, %ymm1, %xmm2
; AVX1-NEXT:    vextractf128 $1, %ymm0, %xmm3
; AVX1-NEXT:    vpminuw %xmm2, %xmm3, %xmm2
; AVX1-NEXT:    vpminuw %xmm1, %xmm0, %xmm0
; AVX1-NEXT:    vinsertf128 $1, %xmm2, %ymm0, %ymm0
; AVX1-NEXT:    retq
;
; AVX2-LABEL: min_le_v16i16:
; AVX2:       # BB#0:
; AVX2-NEXT:    vpminuw %ymm1, %ymm0, %ymm0
; AVX2-NEXT:    retq
;
; AVX512-LABEL: min_le_v16i16:
; AVX512:       # BB#0:
; AVX512-NEXT:    vpminuw %ymm1, %ymm0, %ymm0
; AVX512-NEXT:    retq
  %1 = icmp ule <16 x i16> %a, %b
  %2 = select <16 x i1> %1, <16 x i16> %a, <16 x i16> %b
  ret <16 x i16> %2
}

define <16 x i8> @min_le_v16i8(<16 x i8> %a, <16 x i8> %b) {
; SSE-LABEL: min_le_v16i8:
; SSE:       # BB#0:
; SSE-NEXT:    pminub %xmm1, %xmm0
; SSE-NEXT:    retq
;
; AVX-LABEL: min_le_v16i8:
; AVX:       # BB#0:
; AVX-NEXT:    vpminub %xmm1, %xmm0, %xmm0
; AVX-NEXT:    retq
  %1 = icmp ule <16 x i8> %a, %b
  %2 = select <16 x i1> %1, <16 x i8> %a, <16 x i8> %b
  ret <16 x i8> %2
}

define <32 x i8> @min_le_v32i8(<32 x i8> %a, <32 x i8> %b) {
; SSE-LABEL: min_le_v32i8:
; SSE:       # BB#0:
; SSE-NEXT:    pminub %xmm2, %xmm0
; SSE-NEXT:    pminub %xmm3, %xmm1
; SSE-NEXT:    retq
;
; AVX1-LABEL: min_le_v32i8:
; AVX1:       # BB#0:
; AVX1-NEXT:    vextractf128 $1, %ymm1, %xmm2
; AVX1-NEXT:    vextractf128 $1, %ymm0, %xmm3
; AVX1-NEXT:    vpminub %xmm2, %xmm3, %xmm2
; AVX1-NEXT:    vpminub %xmm1, %xmm0, %xmm0
; AVX1-NEXT:    vinsertf128 $1, %xmm2, %ymm0, %ymm0
; AVX1-NEXT:    retq
;
; AVX2-LABEL: min_le_v32i8:
; AVX2:       # BB#0:
; AVX2-NEXT:    vpminub %ymm1, %ymm0, %ymm0
; AVX2-NEXT:    retq
;
; AVX512-LABEL: min_le_v32i8:
; AVX512:       # BB#0:
; AVX512-NEXT:    vpminub %ymm1, %ymm0, %ymm0
; AVX512-NEXT:    retq
  %1 = icmp ule <32 x i8> %a, %b
  %2 = select <32 x i1> %1, <32 x i8> %a, <32 x i8> %b
  ret <32 x i8> %2
}

;
; Constant Folding
;

define <2 x i64> @max_gt_v2i64c() {
; SSE2-LABEL: max_gt_v2i64c:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa {{.*#+}} xmm1 = [18446744073709551609,7]
; SSE2-NEXT:    movdqa {{.*#+}} xmm2 = [18446744073709551615,1]
; SSE2-NEXT:    movdqa {{.*#+}} xmm0 = [2147483648,2147483648,2147483648,2147483648]
; SSE2-NEXT:    movdqa %xmm0, %xmm3
; SSE2-NEXT:    pxor %xmm2, %xmm3
; SSE2-NEXT:    pxor %xmm1, %xmm0
; SSE2-NEXT:    movdqa %xmm0, %xmm4
; SSE2-NEXT:    pcmpgtd %xmm3, %xmm4
; SSE2-NEXT:    pshufd {{.*#+}} xmm5 = xmm4[0,0,2,2]
; SSE2-NEXT:    pcmpeqd %xmm3, %xmm0
; SSE2-NEXT:    pshufd {{.*#+}} xmm3 = xmm0[1,1,3,3]
; SSE2-NEXT:    pand %xmm5, %xmm3
; SSE2-NEXT:    pshufd {{.*#+}} xmm0 = xmm4[1,1,3,3]
; SSE2-NEXT:    por %xmm3, %xmm0
; SSE2-NEXT:    movdqa %xmm0, %xmm3
; SSE2-NEXT:    pandn %xmm2, %xmm3
; SSE2-NEXT:    pand %xmm1, %xmm0
; SSE2-NEXT:    por %xmm3, %xmm0
; SSE2-NEXT:    retq
;
; SSE41-LABEL: max_gt_v2i64c:
; SSE41:       # BB#0:
; SSE41-NEXT:    movdqa {{.*#+}} xmm2 = [18446744073709551609,7]
; SSE41-NEXT:    movdqa {{.*#+}} xmm1 = [18446744073709551615,1]
; SSE41-NEXT:    movdqa {{.*#+}} xmm0 = [2147483648,2147483648,2147483648,2147483648]
; SSE41-NEXT:    movdqa %xmm0, %xmm3
; SSE41-NEXT:    pxor %xmm1, %xmm3
; SSE41-NEXT:    pxor %xmm2, %xmm0
; SSE41-NEXT:    movdqa %xmm0, %xmm4
; SSE41-NEXT:    pcmpgtd %xmm3, %xmm4
; SSE41-NEXT:    pshufd {{.*#+}} xmm5 = xmm4[0,0,2,2]
; SSE41-NEXT:    pcmpeqd %xmm3, %xmm0
; SSE41-NEXT:    pshufd {{.*#+}} xmm3 = xmm0[1,1,3,3]
; SSE41-NEXT:    pand %xmm5, %xmm3
; SSE41-NEXT:    pshufd {{.*#+}} xmm0 = xmm4[1,1,3,3]
; SSE41-NEXT:    por %xmm3, %xmm0
; SSE41-NEXT:    blendvpd %xmm2, %xmm1
; SSE41-NEXT:    movapd %xmm1, %xmm0
; SSE41-NEXT:    retq
;
; SSE42-LABEL: max_gt_v2i64c:
; SSE42:       # BB#0:
; SSE42-NEXT:    movapd {{.*#+}} xmm1 = [18446744073709551615,1]
; SSE42-NEXT:    movdqa {{.*#+}} xmm0 = [9223372036854775801,9223372036854775815]
; SSE42-NEXT:    pcmpgtq {{.*}}(%rip), %xmm0
; SSE42-NEXT:    blendvpd {{.*}}(%rip), %xmm1
; SSE42-NEXT:    movapd %xmm1, %xmm0
; SSE42-NEXT:    retq
;
; AVX-LABEL: max_gt_v2i64c:
; AVX:       # BB#0:
; AVX-NEXT:    vmovapd {{.*#+}} xmm0 = [18446744073709551615,1]
; AVX-NEXT:    vmovdqa {{.*#+}} xmm1 = [9223372036854775801,9223372036854775815]
; AVX-NEXT:    vpcmpgtq {{.*}}(%rip), %xmm1, %xmm1
; AVX-NEXT:    vblendvpd %xmm1, {{.*}}(%rip), %xmm0, %xmm0
; AVX-NEXT:    retq
  %1 = insertelement <2 x i64> <i64 -7, i64 7>, i64 -7, i32 0
  %2 = insertelement <2 x i64> <i64 -1, i64 1>, i64 -1, i32 0
  %3 = icmp ugt <2 x i64> %1, %2
  %4 = select <2 x i1> %3, <2 x i64> %1, <2 x i64> %2
  ret <2 x i64> %4
}

define <4 x i64> @max_gt_v4i64c() {
; SSE2-LABEL: max_gt_v4i64c:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa {{.*#+}} xmm4 = [18446744073709551609,18446744073709551615]
; SSE2-NEXT:    movdqa {{.*#+}} xmm8 = [1,7]
; SSE2-NEXT:    movdqa {{.*#+}} xmm5 = [18446744073709551615,18446744073709551609]
; SSE2-NEXT:    movdqa {{.*#+}} xmm3 = [7,1]
; SSE2-NEXT:    movdqa {{.*#+}} xmm0 = [2147483648,2147483648,2147483648,2147483648]
; SSE2-NEXT:    movdqa %xmm0, %xmm1
; SSE2-NEXT:    pxor %xmm3, %xmm1
; SSE2-NEXT:    movdqa %xmm0, %xmm6
; SSE2-NEXT:    pxor %xmm8, %xmm6
; SSE2-NEXT:    movdqa %xmm6, %xmm7
; SSE2-NEXT:    pcmpgtd %xmm1, %xmm7
; SSE2-NEXT:    pshufd {{.*#+}} xmm2 = xmm7[0,0,2,2]
; SSE2-NEXT:    pcmpeqd %xmm1, %xmm6
; SSE2-NEXT:    pshufd {{.*#+}} xmm6 = xmm6[1,1,3,3]
; SSE2-NEXT:    pand %xmm2, %xmm6
; SSE2-NEXT:    pshufd {{.*#+}} xmm1 = xmm7[1,1,3,3]
; SSE2-NEXT:    por %xmm6, %xmm1
; SSE2-NEXT:    movdqa %xmm0, %xmm2
; SSE2-NEXT:    pxor %xmm5, %xmm2
; SSE2-NEXT:    pxor %xmm4, %xmm0
; SSE2-NEXT:    movdqa %xmm0, %xmm6
; SSE2-NEXT:    pcmpgtd %xmm2, %xmm6
; SSE2-NEXT:    pshufd {{.*#+}} xmm7 = xmm6[0,0,2,2]
; SSE2-NEXT:    pcmpeqd %xmm2, %xmm0
; SSE2-NEXT:    pshufd {{.*#+}} xmm2 = xmm0[1,1,3,3]
; SSE2-NEXT:    pand %xmm7, %xmm2
; SSE2-NEXT:    pshufd {{.*#+}} xmm0 = xmm6[1,1,3,3]
; SSE2-NEXT:    por %xmm2, %xmm0
; SSE2-NEXT:    movdqa %xmm0, %xmm2
; SSE2-NEXT:    pandn %xmm5, %xmm2
; SSE2-NEXT:    pand %xmm4, %xmm0
; SSE2-NEXT:    por %xmm2, %xmm0
; SSE2-NEXT:    movdqa %xmm1, %xmm2
; SSE2-NEXT:    pandn %xmm3, %xmm2
; SSE2-NEXT:    pand %xmm8, %xmm1
; SSE2-NEXT:    por %xmm2, %xmm1
; SSE2-NEXT:    retq
;
; SSE41-LABEL: max_gt_v4i64c:
; SSE41:       # BB#0:
; SSE41-NEXT:    movdqa {{.*#+}} xmm5 = [18446744073709551609,18446744073709551615]
; SSE41-NEXT:    movdqa {{.*#+}} xmm8 = [1,7]
; SSE41-NEXT:    movdqa {{.*#+}} xmm2 = [18446744073709551615,18446744073709551609]
; SSE41-NEXT:    movdqa {{.*#+}} xmm1 = [7,1]
; SSE41-NEXT:    movdqa {{.*#+}} xmm0 = [2147483648,2147483648,2147483648,2147483648]
; SSE41-NEXT:    movdqa %xmm0, %xmm3
; SSE41-NEXT:    pxor %xmm1, %xmm3
; SSE41-NEXT:    movdqa %xmm0, %xmm6
; SSE41-NEXT:    pxor %xmm8, %xmm6
; SSE41-NEXT:    movdqa %xmm6, %xmm7
; SSE41-NEXT:    pcmpgtd %xmm3, %xmm7
; SSE41-NEXT:    pshufd {{.*#+}} xmm4 = xmm7[0,0,2,2]
; SSE41-NEXT:    pcmpeqd %xmm3, %xmm6
; SSE41-NEXT:    pshufd {{.*#+}} xmm6 = xmm6[1,1,3,3]
; SSE41-NEXT:    pand %xmm4, %xmm6
; SSE41-NEXT:    pshufd {{.*#+}} xmm3 = xmm7[1,1,3,3]
; SSE41-NEXT:    por %xmm6, %xmm3
; SSE41-NEXT:    movdqa %xmm0, %xmm4
; SSE41-NEXT:    pxor %xmm2, %xmm4
; SSE41-NEXT:    pxor %xmm5, %xmm0
; SSE41-NEXT:    movdqa %xmm0, %xmm6
; SSE41-NEXT:    pcmpgtd %xmm4, %xmm6
; SSE41-NEXT:    pshufd {{.*#+}} xmm7 = xmm6[0,0,2,2]
; SSE41-NEXT:    pcmpeqd %xmm4, %xmm0
; SSE41-NEXT:    pshufd {{.*#+}} xmm4 = xmm0[1,1,3,3]
; SSE41-NEXT:    pand %xmm7, %xmm4
; SSE41-NEXT:    pshufd {{.*#+}} xmm0 = xmm6[1,1,3,3]
; SSE41-NEXT:    por %xmm4, %xmm0
; SSE41-NEXT:    blendvpd %xmm5, %xmm2
; SSE41-NEXT:    movdqa %xmm3, %xmm0
; SSE41-NEXT:    blendvpd %xmm8, %xmm1
; SSE41-NEXT:    movapd %xmm2, %xmm0
; SSE41-NEXT:    retq
;
; SSE42-LABEL: max_gt_v4i64c:
; SSE42:       # BB#0:
; SSE42-NEXT:    movapd {{.*#+}} xmm1 = [7,1]
; SSE42-NEXT:    movapd {{.*#+}} xmm2 = [18446744073709551615,18446744073709551609]
; SSE42-NEXT:    movdqa {{.*#+}} xmm3 = [9223372036854775809,9223372036854775815]
; SSE42-NEXT:    pcmpgtq {{.*}}(%rip), %xmm3
; SSE42-NEXT:    movdqa {{.*#+}} xmm0 = [9223372036854775801,9223372036854775807]
; SSE42-NEXT:    pcmpgtq {{.*}}(%rip), %xmm0
; SSE42-NEXT:    blendvpd {{.*}}(%rip), %xmm2
; SSE42-NEXT:    movdqa %xmm3, %xmm0
; SSE42-NEXT:    blendvpd {{.*}}(%rip), %xmm1
; SSE42-NEXT:    movapd %xmm2, %xmm0
; SSE42-NEXT:    retq
;
; AVX1-LABEL: max_gt_v4i64c:
; AVX1:       # BB#0:
; AVX1-NEXT:    vmovapd {{.*#+}} ymm0 = [18446744073709551615,18446744073709551609,7,1]
; AVX1-NEXT:    vmovdqa {{.*#+}} xmm1 = [9223372036854775801,9223372036854775807]
; AVX1-NEXT:    vpcmpgtq {{.*}}(%rip), %xmm1, %xmm1
; AVX1-NEXT:    vmovdqa {{.*#+}} xmm2 = [9223372036854775809,9223372036854775815]
; AVX1-NEXT:    vpcmpgtq {{.*}}(%rip), %xmm2, %xmm2
; AVX1-NEXT:    vinsertf128 $1, %xmm2, %ymm1, %ymm1
; AVX1-NEXT:    vblendvpd %ymm1, {{.*}}(%rip), %ymm0, %ymm0
; AVX1-NEXT:    retq
;
; AVX2-LABEL: max_gt_v4i64c:
; AVX2:       # BB#0:
; AVX2-NEXT:    vmovapd {{.*#+}} ymm0 = [18446744073709551615,18446744073709551609,7,1]
; AVX2-NEXT:    vmovdqa {{.*#+}} ymm1 = [9223372036854775801,9223372036854775807,9223372036854775809,9223372036854775815]
; AVX2-NEXT:    vpcmpgtq {{.*}}(%rip), %ymm1, %ymm1
; AVX2-NEXT:    vblendvpd %ymm1, {{.*}}(%rip), %ymm0, %ymm0
; AVX2-NEXT:    retq
;
; AVX512-LABEL: max_gt_v4i64c:
; AVX512:       # BB#0:
; AVX512-NEXT:    vmovapd {{.*#+}} ymm0 = [18446744073709551615,18446744073709551609,7,1]
; AVX512-NEXT:    vmovdqa {{.*#+}} ymm1 = [9223372036854775801,9223372036854775807,9223372036854775809,9223372036854775815]
; AVX512-NEXT:    vpcmpgtq {{.*}}(%rip), %ymm1, %ymm1
; AVX512-NEXT:    vblendvpd %ymm1, {{.*}}(%rip), %ymm0, %ymm0
; AVX512-NEXT:    retq
  %1 = insertelement <4 x i64> <i64 -7, i64 -1, i64 1, i64 7>, i64 -7, i32 0
  %2 = insertelement <4 x i64> <i64 -1, i64 -7, i64 7, i64 1>, i64 -1, i32 0
  %3 = icmp ugt <4 x i64> %1, %2
  %4 = select <4 x i1> %3, <4 x i64> %1, <4 x i64> %2
  ret <4 x i64> %4
}

define <4 x i32> @max_gt_v4i32c() {
; SSE2-LABEL: max_gt_v4i32c:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa {{.*#+}} xmm0 = [2147483641,2147483647,2147483649,2147483655]
; SSE2-NEXT:    pcmpgtd {{.*}}(%rip), %xmm0
; SSE2-NEXT:    movdqa %xmm0, %xmm1
; SSE2-NEXT:    pandn {{.*}}(%rip), %xmm1
; SSE2-NEXT:    pand {{.*}}(%rip), %xmm0
; SSE2-NEXT:    por %xmm1, %xmm0
; SSE2-NEXT:    retq
;
; SSE41-LABEL: max_gt_v4i32c:
; SSE41:       # BB#0:
; SSE41-NEXT:    movdqa {{.*#+}} xmm0 = [4294967289,4294967295,1,7]
; SSE41-NEXT:    pmaxud {{.*}}(%rip), %xmm0
; SSE41-NEXT:    retq
;
; SSE42-LABEL: max_gt_v4i32c:
; SSE42:       # BB#0:
; SSE42-NEXT:    movdqa {{.*#+}} xmm0 = [4294967289,4294967295,1,7]
; SSE42-NEXT:    pmaxud {{.*}}(%rip), %xmm0
; SSE42-NEXT:    retq
;
; AVX-LABEL: max_gt_v4i32c:
; AVX:       # BB#0:
; AVX-NEXT:    vmovdqa {{.*#+}} xmm0 = [4294967289,4294967295,1,7]
; AVX-NEXT:    vpmaxud {{.*}}(%rip), %xmm0, %xmm0
; AVX-NEXT:    retq
  %1 = insertelement <4 x i32> <i32 -7, i32 -1, i32 1, i32 7>, i32 -7, i32 0
  %2 = insertelement <4 x i32> <i32 -1, i32 -7, i32 7, i32 1>, i32  1, i32 0
  %3 = icmp ugt <4 x i32> %1, %2
  %4 = select <4 x i1> %3, <4 x i32> %1, <4 x i32> %2
  ret <4 x i32> %4
}

define <8 x i32> @max_gt_v8i32c() {
; SSE2-LABEL: max_gt_v8i32c:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa {{.*#+}} xmm1 = [2147483649,2147483651,2147483653,2147483655]
; SSE2-NEXT:    pcmpgtd {{.*}}(%rip), %xmm1
; SSE2-NEXT:    movdqa {{.*#+}} xmm0 = [2147483641,2147483643,2147483645,2147483647]
; SSE2-NEXT:    pcmpgtd {{.*}}(%rip), %xmm0
; SSE2-NEXT:    movdqa %xmm0, %xmm2
; SSE2-NEXT:    pandn {{.*}}(%rip), %xmm2
; SSE2-NEXT:    pand {{.*}}(%rip), %xmm0
; SSE2-NEXT:    por %xmm2, %xmm0
; SSE2-NEXT:    movdqa %xmm1, %xmm2
; SSE2-NEXT:    pandn {{.*}}(%rip), %xmm2
; SSE2-NEXT:    pand {{.*}}(%rip), %xmm1
; SSE2-NEXT:    por %xmm2, %xmm1
; SSE2-NEXT:    retq
;
; SSE41-LABEL: max_gt_v8i32c:
; SSE41:       # BB#0:
; SSE41-NEXT:    movdqa {{.*#+}} xmm1 = [1,3,5,7]
; SSE41-NEXT:    movdqa {{.*#+}} xmm0 = [4294967289,4294967291,4294967293,4294967295]
; SSE41-NEXT:    pmaxud {{.*}}(%rip), %xmm0
; SSE41-NEXT:    pmaxud {{.*}}(%rip), %xmm1
; SSE41-NEXT:    retq
;
; SSE42-LABEL: max_gt_v8i32c:
; SSE42:       # BB#0:
; SSE42-NEXT:    movdqa {{.*#+}} xmm1 = [1,3,5,7]
; SSE42-NEXT:    movdqa {{.*#+}} xmm0 = [4294967289,4294967291,4294967293,4294967295]
; SSE42-NEXT:    pmaxud {{.*}}(%rip), %xmm0
; SSE42-NEXT:    pmaxud {{.*}}(%rip), %xmm1
; SSE42-NEXT:    retq
;
; AVX1-LABEL: max_gt_v8i32c:
; AVX1:       # BB#0:
; AVX1-NEXT:    vmovdqa {{.*#+}} xmm0 = [4294967289,4294967291,4294967293,4294967295]
; AVX1-NEXT:    vpmaxud {{.*}}(%rip), %xmm0, %xmm0
; AVX1-NEXT:    vmovdqa {{.*#+}} xmm1 = [1,3,5,7]
; AVX1-NEXT:    vpmaxud {{.*}}(%rip), %xmm1, %xmm1
; AVX1-NEXT:    vinsertf128 $1, %xmm1, %ymm0, %ymm0
; AVX1-NEXT:    retq
;
; AVX2-LABEL: max_gt_v8i32c:
; AVX2:       # BB#0:
; AVX2-NEXT:    vmovdqa {{.*#+}} ymm0 = [4294967289,4294967291,4294967293,4294967295,1,3,5,7]
; AVX2-NEXT:    vpmaxud {{.*}}(%rip), %ymm0, %ymm0
; AVX2-NEXT:    retq
;
; AVX512-LABEL: max_gt_v8i32c:
; AVX512:       # BB#0:
; AVX512-NEXT:    vmovdqa {{.*#+}} ymm0 = [4294967289,4294967291,4294967293,4294967295,1,3,5,7]
; AVX512-NEXT:    vpmaxud {{.*}}(%rip), %ymm0, %ymm0
; AVX512-NEXT:    retq
  %1 = insertelement <8 x i32> <i32 -7, i32 -5, i32 -3, i32 -1, i32 1, i32 3, i32 5, i32 7>, i32 -7, i32 0
  %2 = insertelement <8 x i32> <i32 -1, i32 -3, i32 -5, i32 -7, i32 7, i32 5, i32 3, i32 1>, i32  1, i32 0
  %3 = icmp ugt <8 x i32> %1, %2
  %4 = select <8 x i1> %3, <8 x i32> %1, <8 x i32> %2
  ret <8 x i32> %4
}

define <8 x i16> @max_gt_v8i16c() {
; SSE2-LABEL: max_gt_v8i16c:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa {{.*#+}} xmm0 = [32761,32763,32765,32767,32769,32771,32773,32775]
; SSE2-NEXT:    pcmpgtw {{.*}}(%rip), %xmm0
; SSE2-NEXT:    movdqa %xmm0, %xmm1
; SSE2-NEXT:    pandn {{.*}}(%rip), %xmm1
; SSE2-NEXT:    pand {{.*}}(%rip), %xmm0
; SSE2-NEXT:    por %xmm1, %xmm0
; SSE2-NEXT:    retq
;
; SSE41-LABEL: max_gt_v8i16c:
; SSE41:       # BB#0:
; SSE41-NEXT:    movdqa {{.*#+}} xmm0 = [65529,65531,65533,65535,1,3,5,7]
; SSE41-NEXT:    pmaxuw {{.*}}(%rip), %xmm0
; SSE41-NEXT:    retq
;
; SSE42-LABEL: max_gt_v8i16c:
; SSE42:       # BB#0:
; SSE42-NEXT:    movdqa {{.*#+}} xmm0 = [65529,65531,65533,65535,1,3,5,7]
; SSE42-NEXT:    pmaxuw {{.*}}(%rip), %xmm0
; SSE42-NEXT:    retq
;
; AVX-LABEL: max_gt_v8i16c:
; AVX:       # BB#0:
; AVX-NEXT:    vmovdqa {{.*#+}} xmm0 = [65529,65531,65533,65535,1,3,5,7]
; AVX-NEXT:    vpmaxuw {{.*}}(%rip), %xmm0, %xmm0
; AVX-NEXT:    retq
  %1 = insertelement <8 x i16> <i16 -7, i16 -5, i16 -3, i16 -1, i16 1, i16 3, i16 5, i16 7>, i16 -7, i16 0
  %2 = insertelement <8 x i16> <i16 -1, i16 -3, i16 -5, i16 -7, i16 7, i16 5, i16 3, i16 1>, i16  1, i16 0
  %3 = icmp ugt <8 x i16> %1, %2
  %4 = select <8 x i1> %3, <8 x i16> %1, <8 x i16> %2
  ret <8 x i16> %4
}

define <16 x i16> @max_gt_v16i16c() {
; SSE2-LABEL: max_gt_v16i16c:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa {{.*#+}} xmm1 = [32769,32770,32771,32772,32773,32774,32775,32776]
; SSE2-NEXT:    pcmpgtw {{.*}}(%rip), %xmm1
; SSE2-NEXT:    movdqa {{.*#+}} xmm0 = [32761,32762,32763,32764,32765,32766,32767,32768]
; SSE2-NEXT:    pcmpgtw {{.*}}(%rip), %xmm0
; SSE2-NEXT:    movdqa %xmm0, %xmm2
; SSE2-NEXT:    pandn {{.*}}(%rip), %xmm2
; SSE2-NEXT:    pand {{.*}}(%rip), %xmm0
; SSE2-NEXT:    por %xmm2, %xmm0
; SSE2-NEXT:    movdqa %xmm1, %xmm2
; SSE2-NEXT:    pandn {{.*}}(%rip), %xmm2
; SSE2-NEXT:    pand {{.*}}(%rip), %xmm1
; SSE2-NEXT:    por %xmm2, %xmm1
; SSE2-NEXT:    retq
;
; SSE41-LABEL: max_gt_v16i16c:
; SSE41:       # BB#0:
; SSE41-NEXT:    movdqa {{.*#+}} xmm1 = [1,2,3,4,5,6,7,8]
; SSE41-NEXT:    movdqa {{.*#+}} xmm0 = [65529,65530,65531,65532,65533,65534,65535,0]
; SSE41-NEXT:    pmaxuw {{.*}}(%rip), %xmm0
; SSE41-NEXT:    pmaxuw {{.*}}(%rip), %xmm1
; SSE41-NEXT:    retq
;
; SSE42-LABEL: max_gt_v16i16c:
; SSE42:       # BB#0:
; SSE42-NEXT:    movdqa {{.*#+}} xmm1 = [1,2,3,4,5,6,7,8]
; SSE42-NEXT:    movdqa {{.*#+}} xmm0 = [65529,65530,65531,65532,65533,65534,65535,0]
; SSE42-NEXT:    pmaxuw {{.*}}(%rip), %xmm0
; SSE42-NEXT:    pmaxuw {{.*}}(%rip), %xmm1
; SSE42-NEXT:    retq
;
; AVX1-LABEL: max_gt_v16i16c:
; AVX1:       # BB#0:
; AVX1-NEXT:    vmovdqa {{.*#+}} xmm0 = [65529,65530,65531,65532,65533,65534,65535,0]
; AVX1-NEXT:    vpmaxuw {{.*}}(%rip), %xmm0, %xmm0
; AVX1-NEXT:    vmovdqa {{.*#+}} xmm1 = [1,2,3,4,5,6,7,8]
; AVX1-NEXT:    vpmaxuw {{.*}}(%rip), %xmm1, %xmm1
; AVX1-NEXT:    vinsertf128 $1, %xmm1, %ymm0, %ymm0
; AVX1-NEXT:    retq
;
; AVX2-LABEL: max_gt_v16i16c:
; AVX2:       # BB#0:
; AVX2-NEXT:    vmovdqa {{.*#+}} ymm0 = [65529,65530,65531,65532,65533,65534,65535,0,1,2,3,4,5,6,7,8]
; AVX2-NEXT:    vpmaxuw {{.*}}(%rip), %ymm0, %ymm0
; AVX2-NEXT:    retq
;
; AVX512-LABEL: max_gt_v16i16c:
; AVX512:       # BB#0:
; AVX512-NEXT:    vmovdqa {{.*#+}} ymm0 = [65529,65530,65531,65532,65533,65534,65535,0,1,2,3,4,5,6,7,8]
; AVX512-NEXT:    vpmaxuw {{.*}}(%rip), %ymm0, %ymm0
; AVX512-NEXT:    retq
  %1 = insertelement <16 x i16> <i16 -7, i16 -6, i16 -5, i16 -4, i16 -3, i16 -2, i16 -1, i16 0, i16 1, i16 2, i16 3, i16 4, i16 5, i16 6, i16 7, i16 8>, i16 -7, i16 0
  %2 = insertelement <16 x i16> <i16 -1, i16 -2, i16 -3, i16 -4, i16 -5, i16 -6, i16 -7, i16 0, i16 7, i16 6, i16 5, i16 4, i16 3, i16 2, i16 1, i16 0>, i16  1, i16 0
  %3 = icmp ugt <16 x i16> %1, %2
  %4 = select <16 x i1> %3, <16 x i16> %1, <16 x i16> %2
  ret <16 x i16> %4
}

define <16 x i8> @max_gt_v16i8c() {
; SSE-LABEL: max_gt_v16i8c:
; SSE:       # BB#0:
; SSE-NEXT:    movdqa {{.*#+}} xmm0 = [249,250,251,252,253,254,255,0,1,2,3,4,5,6,7,8]
; SSE-NEXT:    pmaxub {{.*}}(%rip), %xmm0
; SSE-NEXT:    retq
;
; AVX-LABEL: max_gt_v16i8c:
; AVX:       # BB#0:
; AVX-NEXT:    vmovdqa {{.*#+}} xmm0 = [249,250,251,252,253,254,255,0,1,2,3,4,5,6,7,8]
; AVX-NEXT:    vpmaxub {{.*}}(%rip), %xmm0, %xmm0
; AVX-NEXT:    retq
  %1 = insertelement <16 x i8> <i8 -7, i8 -6, i8 -5, i8 -4, i8 -3, i8 -2, i8 -1, i8 0, i8 1, i8 2, i8 3, i8 4, i8 5, i8 6, i8 7, i8 8>, i8 -7, i8 0
  %2 = insertelement <16 x i8> <i8 -1, i8 -2, i8 -3, i8 -4, i8 -5, i8 -6, i8 -7, i8 0, i8 7, i8 6, i8 5, i8 4, i8 3, i8 2, i8 1, i8 0>, i8  1, i8 0
  %3 = icmp ugt <16 x i8> %1, %2
  %4 = select <16 x i1> %3, <16 x i8> %1, <16 x i8> %2
  ret <16 x i8> %4
}

define <2 x i64> @max_ge_v2i64c() {
; SSE2-LABEL: max_ge_v2i64c:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa {{.*#+}} xmm1 = [18446744073709551609,7]
; SSE2-NEXT:    movdqa {{.*#+}} xmm2 = [18446744073709551615,1]
; SSE2-NEXT:    movdqa {{.*#+}} xmm0 = [2147483648,2147483648,2147483648,2147483648]
; SSE2-NEXT:    movdqa %xmm0, %xmm3
; SSE2-NEXT:    pxor %xmm1, %xmm3
; SSE2-NEXT:    pxor %xmm2, %xmm0
; SSE2-NEXT:    movdqa %xmm0, %xmm4
; SSE2-NEXT:    pcmpgtd %xmm3, %xmm4
; SSE2-NEXT:    pshufd {{.*#+}} xmm5 = xmm4[0,0,2,2]
; SSE2-NEXT:    pcmpeqd %xmm3, %xmm0
; SSE2-NEXT:    pshufd {{.*#+}} xmm0 = xmm0[1,1,3,3]
; SSE2-NEXT:    pand %xmm5, %xmm0
; SSE2-NEXT:    pshufd {{.*#+}} xmm3 = xmm4[1,1,3,3]
; SSE2-NEXT:    por %xmm0, %xmm3
; SSE2-NEXT:    pcmpeqd %xmm0, %xmm0
; SSE2-NEXT:    pxor %xmm3, %xmm0
; SSE2-NEXT:    pandn %xmm1, %xmm3
; SSE2-NEXT:    pandn %xmm2, %xmm0
; SSE2-NEXT:    por %xmm3, %xmm0
; SSE2-NEXT:    retq
;
; SSE41-LABEL: max_ge_v2i64c:
; SSE41:       # BB#0:
; SSE41-NEXT:    movdqa {{.*#+}} xmm2 = [18446744073709551609,7]
; SSE41-NEXT:    movdqa {{.*#+}} xmm1 = [18446744073709551615,1]
; SSE41-NEXT:    movdqa {{.*#+}} xmm0 = [2147483648,2147483648,2147483648,2147483648]
; SSE41-NEXT:    movdqa %xmm0, %xmm3
; SSE41-NEXT:    pxor %xmm2, %xmm3
; SSE41-NEXT:    pxor %xmm1, %xmm0
; SSE41-NEXT:    movdqa %xmm0, %xmm4
; SSE41-NEXT:    pcmpgtd %xmm3, %xmm4
; SSE41-NEXT:    pshufd {{.*#+}} xmm5 = xmm4[0,0,2,2]
; SSE41-NEXT:    pcmpeqd %xmm3, %xmm0
; SSE41-NEXT:    pshufd {{.*#+}} xmm0 = xmm0[1,1,3,3]
; SSE41-NEXT:    pand %xmm5, %xmm0
; SSE41-NEXT:    pshufd {{.*#+}} xmm3 = xmm4[1,1,3,3]
; SSE41-NEXT:    por %xmm0, %xmm3
; SSE41-NEXT:    pcmpeqd %xmm0, %xmm0
; SSE41-NEXT:    pxor %xmm3, %xmm0
; SSE41-NEXT:    blendvpd %xmm2, %xmm1
; SSE41-NEXT:    movapd %xmm1, %xmm0
; SSE41-NEXT:    retq
;
; SSE42-LABEL: max_ge_v2i64c:
; SSE42:       # BB#0:
; SSE42-NEXT:    movapd {{.*#+}} xmm1 = [18446744073709551615,1]
; SSE42-NEXT:    movdqa {{.*#+}} xmm2 = [9223372036854775807,9223372036854775809]
; SSE42-NEXT:    pcmpgtq {{.*}}(%rip), %xmm2
; SSE42-NEXT:    pcmpeqd %xmm0, %xmm0
; SSE42-NEXT:    pxor %xmm2, %xmm0
; SSE42-NEXT:    blendvpd {{.*}}(%rip), %xmm1
; SSE42-NEXT:    movapd %xmm1, %xmm0
; SSE42-NEXT:    retq
;
; AVX-LABEL: max_ge_v2i64c:
; AVX:       # BB#0:
; AVX-NEXT:    vmovapd {{.*#+}} xmm0 = [18446744073709551615,1]
; AVX-NEXT:    vmovdqa {{.*#+}} xmm1 = [9223372036854775807,9223372036854775809]
; AVX-NEXT:    vpcmpgtq {{.*}}(%rip), %xmm1, %xmm1
; AVX-NEXT:    vpcmpeqd %xmm2, %xmm2, %xmm2
; AVX-NEXT:    vpxor %xmm2, %xmm1, %xmm1
; AVX-NEXT:    vblendvpd %xmm1, {{.*}}(%rip), %xmm0, %xmm0
; AVX-NEXT:    retq
  %1 = insertelement <2 x i64> <i64 -7, i64 7>, i64 -7, i32 0
  %2 = insertelement <2 x i64> <i64 -1, i64 1>, i64 -1, i32 0
  %3 = icmp uge <2 x i64> %1, %2
  %4 = select <2 x i1> %3, <2 x i64> %1, <2 x i64> %2
  ret <2 x i64> %4
}

define <4 x i64> @max_ge_v4i64c() {
; SSE2-LABEL: max_ge_v4i64c:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa {{.*#+}} xmm10 = [18446744073709551609,18446744073709551615]
; SSE2-NEXT:    movdqa {{.*#+}} xmm8 = [1,7]
; SSE2-NEXT:    movdqa {{.*#+}} xmm5 = [18446744073709551615,18446744073709551609]
; SSE2-NEXT:    movdqa {{.*#+}} xmm9 = [7,1]
; SSE2-NEXT:    movdqa {{.*#+}} xmm7 = [2147483648,2147483648,2147483648,2147483648]
; SSE2-NEXT:    movdqa %xmm7, %xmm0
; SSE2-NEXT:    pxor %xmm8, %xmm0
; SSE2-NEXT:    movdqa %xmm7, %xmm1
; SSE2-NEXT:    pxor %xmm9, %xmm1
; SSE2-NEXT:    movdqa %xmm1, %xmm6
; SSE2-NEXT:    pcmpgtd %xmm0, %xmm6
; SSE2-NEXT:    pshufd {{.*#+}} xmm2 = xmm6[0,0,2,2]
; SSE2-NEXT:    pcmpeqd %xmm0, %xmm1
; SSE2-NEXT:    pshufd {{.*#+}} xmm0 = xmm1[1,1,3,3]
; SSE2-NEXT:    pand %xmm2, %xmm0
; SSE2-NEXT:    pshufd {{.*#+}} xmm6 = xmm6[1,1,3,3]
; SSE2-NEXT:    por %xmm0, %xmm6
; SSE2-NEXT:    pcmpeqd %xmm0, %xmm0
; SSE2-NEXT:    movdqa %xmm6, %xmm1
; SSE2-NEXT:    pxor %xmm0, %xmm1
; SSE2-NEXT:    movdqa %xmm7, %xmm2
; SSE2-NEXT:    pxor %xmm10, %xmm2
; SSE2-NEXT:    pxor %xmm5, %xmm7
; SSE2-NEXT:    movdqa %xmm7, %xmm3
; SSE2-NEXT:    pcmpgtd %xmm2, %xmm3
; SSE2-NEXT:    pshufd {{.*#+}} xmm4 = xmm3[0,0,2,2]
; SSE2-NEXT:    pcmpeqd %xmm2, %xmm7
; SSE2-NEXT:    pshufd {{.*#+}} xmm2 = xmm7[1,1,3,3]
; SSE2-NEXT:    pand %xmm4, %xmm2
; SSE2-NEXT:    pshufd {{.*#+}} xmm3 = xmm3[1,1,3,3]
; SSE2-NEXT:    por %xmm2, %xmm3
; SSE2-NEXT:    pxor %xmm3, %xmm0
; SSE2-NEXT:    pandn %xmm10, %xmm3
; SSE2-NEXT:    pandn %xmm5, %xmm0
; SSE2-NEXT:    por %xmm3, %xmm0
; SSE2-NEXT:    pandn %xmm8, %xmm6
; SSE2-NEXT:    pandn %xmm9, %xmm1
; SSE2-NEXT:    por %xmm6, %xmm1
; SSE2-NEXT:    retq
;
; SSE41-LABEL: max_ge_v4i64c:
; SSE41:       # BB#0:
; SSE41-NEXT:    movdqa {{.*#+}} xmm9 = [18446744073709551609,18446744073709551615]
; SSE41-NEXT:    movdqa {{.*#+}} xmm8 = [1,7]
; SSE41-NEXT:    movdqa {{.*#+}} xmm2 = [18446744073709551615,18446744073709551609]
; SSE41-NEXT:    movdqa {{.*#+}} xmm1 = [7,1]
; SSE41-NEXT:    movdqa {{.*#+}} xmm0 = [2147483648,2147483648,2147483648,2147483648]
; SSE41-NEXT:    movdqa %xmm0, %xmm3
; SSE41-NEXT:    pxor %xmm8, %xmm3
; SSE41-NEXT:    movdqa %xmm0, %xmm6
; SSE41-NEXT:    pxor %xmm1, %xmm6
; SSE41-NEXT:    movdqa %xmm6, %xmm7
; SSE41-NEXT:    pcmpgtd %xmm3, %xmm7
; SSE41-NEXT:    pshufd {{.*#+}} xmm4 = xmm7[0,0,2,2]
; SSE41-NEXT:    pcmpeqd %xmm3, %xmm6
; SSE41-NEXT:    pshufd {{.*#+}} xmm6 = xmm6[1,1,3,3]
; SSE41-NEXT:    pand %xmm4, %xmm6
; SSE41-NEXT:    pshufd {{.*#+}} xmm3 = xmm7[1,1,3,3]
; SSE41-NEXT:    por %xmm6, %xmm3
; SSE41-NEXT:    pcmpeqd %xmm4, %xmm4
; SSE41-NEXT:    pxor %xmm4, %xmm3
; SSE41-NEXT:    movdqa %xmm0, %xmm6
; SSE41-NEXT:    pxor %xmm9, %xmm6
; SSE41-NEXT:    pxor %xmm2, %xmm0
; SSE41-NEXT:    movdqa %xmm0, %xmm7
; SSE41-NEXT:    pcmpgtd %xmm6, %xmm7
; SSE41-NEXT:    pshufd {{.*#+}} xmm5 = xmm7[0,0,2,2]
; SSE41-NEXT:    pcmpeqd %xmm6, %xmm0
; SSE41-NEXT:    pshufd {{.*#+}} xmm6 = xmm0[1,1,3,3]
; SSE41-NEXT:    pand %xmm5, %xmm6
; SSE41-NEXT:    pshufd {{.*#+}} xmm0 = xmm7[1,1,3,3]
; SSE41-NEXT:    por %xmm6, %xmm0
; SSE41-NEXT:    pxor %xmm4, %xmm0
; SSE41-NEXT:    blendvpd %xmm9, %xmm2
; SSE41-NEXT:    movdqa %xmm3, %xmm0
; SSE41-NEXT:    blendvpd %xmm8, %xmm1
; SSE41-NEXT:    movapd %xmm2, %xmm0
; SSE41-NEXT:    retq
;
; SSE42-LABEL: max_ge_v4i64c:
; SSE42:       # BB#0:
; SSE42-NEXT:    movapd {{.*#+}} xmm1 = [7,1]
; SSE42-NEXT:    movapd {{.*#+}} xmm2 = [18446744073709551615,18446744073709551609]
; SSE42-NEXT:    movdqa {{.*#+}} xmm3 = [9223372036854775815,9223372036854775809]
; SSE42-NEXT:    pcmpgtq {{.*}}(%rip), %xmm3
; SSE42-NEXT:    pcmpeqd %xmm4, %xmm4
; SSE42-NEXT:    pxor %xmm4, %xmm3
; SSE42-NEXT:    movdqa {{.*#+}} xmm0 = [9223372036854775807,9223372036854775801]
; SSE42-NEXT:    pcmpgtq {{.*}}(%rip), %xmm0
; SSE42-NEXT:    pxor %xmm4, %xmm0
; SSE42-NEXT:    blendvpd {{.*}}(%rip), %xmm2
; SSE42-NEXT:    movdqa %xmm3, %xmm0
; SSE42-NEXT:    blendvpd {{.*}}(%rip), %xmm1
; SSE42-NEXT:    movapd %xmm2, %xmm0
; SSE42-NEXT:    retq
;
; AVX1-LABEL: max_ge_v4i64c:
; AVX1:       # BB#0:
; AVX1-NEXT:    vmovapd {{.*#+}} ymm0 = [18446744073709551615,18446744073709551609,7,1]
; AVX1-NEXT:    vmovdqa {{.*#+}} xmm1 = [9223372036854775815,9223372036854775809]
; AVX1-NEXT:    vpcmpgtq {{.*}}(%rip), %xmm1, %xmm1
; AVX1-NEXT:    vpcmpeqd %xmm2, %xmm2, %xmm2
; AVX1-NEXT:    vpxor %xmm2, %xmm1, %xmm1
; AVX1-NEXT:    vmovdqa {{.*#+}} xmm3 = [9223372036854775807,9223372036854775801]
; AVX1-NEXT:    vpcmpgtq {{.*}}(%rip), %xmm3, %xmm3
; AVX1-NEXT:    vpxor %xmm2, %xmm3, %xmm2
; AVX1-NEXT:    vinsertf128 $1, %xmm1, %ymm2, %ymm1
; AVX1-NEXT:    vblendvpd %ymm1, {{.*}}(%rip), %ymm0, %ymm0
; AVX1-NEXT:    retq
;
; AVX2-LABEL: max_ge_v4i64c:
; AVX2:       # BB#0:
; AVX2-NEXT:    vmovapd {{.*#+}} ymm0 = [18446744073709551615,18446744073709551609,7,1]
; AVX2-NEXT:    vmovdqa {{.*#+}} ymm1 = [9223372036854775807,9223372036854775801,9223372036854775815,9223372036854775809]
; AVX2-NEXT:    vpcmpgtq {{.*}}(%rip), %ymm1, %ymm1
; AVX2-NEXT:    vpcmpeqd %ymm2, %ymm2, %ymm2
; AVX2-NEXT:    vpxor %ymm2, %ymm1, %ymm1
; AVX2-NEXT:    vblendvpd %ymm1, {{.*}}(%rip), %ymm0, %ymm0
; AVX2-NEXT:    retq
;
; AVX512-LABEL: max_ge_v4i64c:
; AVX512:       # BB#0:
; AVX512-NEXT:    vmovapd {{.*#+}} ymm0 = [18446744073709551615,18446744073709551609,7,1]
; AVX512-NEXT:    vmovdqa {{.*#+}} ymm1 = [9223372036854775807,9223372036854775801,9223372036854775815,9223372036854775809]
; AVX512-NEXT:    vpcmpgtq {{.*}}(%rip), %ymm1, %ymm1
; AVX512-NEXT:    vpcmpeqd %ymm2, %ymm2, %ymm2
; AVX512-NEXT:    vpxor %ymm2, %ymm1, %ymm1
; AVX512-NEXT:    vblendvpd %ymm1, {{.*}}(%rip), %ymm0, %ymm0
; AVX512-NEXT:    retq
  %1 = insertelement <4 x i64> <i64 -7, i64 -1, i64 1, i64 7>, i64 -7, i32 0
  %2 = insertelement <4 x i64> <i64 -1, i64 -7, i64 7, i64 1>, i64 -1, i32 0
  %3 = icmp uge <4 x i64> %1, %2
  %4 = select <4 x i1> %3, <4 x i64> %1, <4 x i64> %2
  ret <4 x i64> %4
}

define <4 x i32> @max_ge_v4i32c() {
; SSE2-LABEL: max_ge_v4i32c:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa {{.*#+}} xmm0 = [2147483649,2147483641,2147483655,2147483649]
; SSE2-NEXT:    pcmpgtd {{.*}}(%rip), %xmm0
; SSE2-NEXT:    pcmpeqd %xmm1, %xmm1
; SSE2-NEXT:    pxor %xmm0, %xmm1
; SSE2-NEXT:    pandn {{.*}}(%rip), %xmm1
; SSE2-NEXT:    pandn {{.*}}(%rip), %xmm0
; SSE2-NEXT:    por %xmm1, %xmm0
; SSE2-NEXT:    retq
;
; SSE41-LABEL: max_ge_v4i32c:
; SSE41:       # BB#0:
; SSE41-NEXT:    movdqa {{.*#+}} xmm0 = [4294967289,4294967295,1,7]
; SSE41-NEXT:    pmaxud {{.*}}(%rip), %xmm0
; SSE41-NEXT:    retq
;
; SSE42-LABEL: max_ge_v4i32c:
; SSE42:       # BB#0:
; SSE42-NEXT:    movdqa {{.*#+}} xmm0 = [4294967289,4294967295,1,7]
; SSE42-NEXT:    pmaxud {{.*}}(%rip), %xmm0
; SSE42-NEXT:    retq
;
; AVX-LABEL: max_ge_v4i32c:
; AVX:       # BB#0:
; AVX-NEXT:    vmovdqa {{.*#+}} xmm0 = [4294967289,4294967295,1,7]
; AVX-NEXT:    vpmaxud {{.*}}(%rip), %xmm0, %xmm0
; AVX-NEXT:    retq
  %1 = insertelement <4 x i32> <i32 -7, i32 -1, i32 1, i32 7>, i32 -7, i32 0
  %2 = insertelement <4 x i32> <i32 -1, i32 -7, i32 7, i32 1>, i32  1, i32 0
  %3 = icmp uge <4 x i32> %1, %2
  %4 = select <4 x i1> %3, <4 x i32> %1, <4 x i32> %2
  ret <4 x i32> %4
}

define <8 x i32> @max_ge_v8i32c() {
; SSE2-LABEL: max_ge_v8i32c:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa {{.*#+}} xmm1 = [2147483655,2147483653,2147483651,2147483649]
; SSE2-NEXT:    pcmpgtd {{.*}}(%rip), %xmm1
; SSE2-NEXT:    pcmpeqd %xmm2, %xmm2
; SSE2-NEXT:    movdqa %xmm1, %xmm3
; SSE2-NEXT:    pxor %xmm2, %xmm3
; SSE2-NEXT:    movdqa {{.*#+}} xmm0 = [2147483649,2147483645,2147483643,2147483641]
; SSE2-NEXT:    pcmpgtd {{.*}}(%rip), %xmm0
; SSE2-NEXT:    pxor %xmm0, %xmm2
; SSE2-NEXT:    pandn {{.*}}(%rip), %xmm2
; SSE2-NEXT:    pandn {{.*}}(%rip), %xmm0
; SSE2-NEXT:    por %xmm2, %xmm0
; SSE2-NEXT:    pandn {{.*}}(%rip), %xmm3
; SSE2-NEXT:    pandn {{.*}}(%rip), %xmm1
; SSE2-NEXT:    por %xmm3, %xmm1
; SSE2-NEXT:    retq
;
; SSE41-LABEL: max_ge_v8i32c:
; SSE41:       # BB#0:
; SSE41-NEXT:    movdqa {{.*#+}} xmm1 = [1,3,5,7]
; SSE41-NEXT:    movdqa {{.*#+}} xmm0 = [4294967289,4294967291,4294967293,4294967295]
; SSE41-NEXT:    pmaxud {{.*}}(%rip), %xmm0
; SSE41-NEXT:    pmaxud {{.*}}(%rip), %xmm1
; SSE41-NEXT:    retq
;
; SSE42-LABEL: max_ge_v8i32c:
; SSE42:       # BB#0:
; SSE42-NEXT:    movdqa {{.*#+}} xmm1 = [1,3,5,7]
; SSE42-NEXT:    movdqa {{.*#+}} xmm0 = [4294967289,4294967291,4294967293,4294967295]
; SSE42-NEXT:    pmaxud {{.*}}(%rip), %xmm0
; SSE42-NEXT:    pmaxud {{.*}}(%rip), %xmm1
; SSE42-NEXT:    retq
;
; AVX1-LABEL: max_ge_v8i32c:
; AVX1:       # BB#0:
; AVX1-NEXT:    vmovdqa {{.*#+}} xmm0 = [4294967289,4294967291,4294967293,4294967295]
; AVX1-NEXT:    vpmaxud {{.*}}(%rip), %xmm0, %xmm0
; AVX1-NEXT:    vmovdqa {{.*#+}} xmm1 = [1,3,5,7]
; AVX1-NEXT:    vpmaxud {{.*}}(%rip), %xmm1, %xmm1
; AVX1-NEXT:    vinsertf128 $1, %xmm1, %ymm0, %ymm0
; AVX1-NEXT:    retq
;
; AVX2-LABEL: max_ge_v8i32c:
; AVX2:       # BB#0:
; AVX2-NEXT:    vmovdqa {{.*#+}} ymm0 = [4294967289,4294967291,4294967293,4294967295,1,3,5,7]
; AVX2-NEXT:    vpmaxud {{.*}}(%rip), %ymm0, %ymm0
; AVX2-NEXT:    retq
;
; AVX512-LABEL: max_ge_v8i32c:
; AVX512:       # BB#0:
; AVX512-NEXT:    vmovdqa {{.*#+}} ymm0 = [4294967289,4294967291,4294967293,4294967295,1,3,5,7]
; AVX512-NEXT:    vpmaxud {{.*}}(%rip), %ymm0, %ymm0
; AVX512-NEXT:    retq
  %1 = insertelement <8 x i32> <i32 -7, i32 -5, i32 -3, i32 -1, i32 1, i32 3, i32 5, i32 7>, i32 -7, i32 0
  %2 = insertelement <8 x i32> <i32 -1, i32 -3, i32 -5, i32 -7, i32 7, i32 5, i32 3, i32 1>, i32  1, i32 0
  %3 = icmp uge <8 x i32> %1, %2
  %4 = select <8 x i1> %3, <8 x i32> %1, <8 x i32> %2
  ret <8 x i32> %4
}

define <8 x i16> @max_ge_v8i16c() {
; SSE2-LABEL: max_ge_v8i16c:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa {{.*#+}} xmm1 = [65529,65531,65533,65535,1,3,5,7]
; SSE2-NEXT:    movdqa {{.*#+}} xmm2 = [1,65533,65531,65529,7,5,3,1]
; SSE2-NEXT:    movdqa %xmm2, %xmm3
; SSE2-NEXT:    psubusw %xmm1, %xmm3
; SSE2-NEXT:    pxor %xmm0, %xmm0
; SSE2-NEXT:    pcmpeqw %xmm3, %xmm0
; SSE2-NEXT:    pand %xmm0, %xmm1
; SSE2-NEXT:    pandn %xmm2, %xmm0
; SSE2-NEXT:    por %xmm1, %xmm0
; SSE2-NEXT:    retq
;
; SSE41-LABEL: max_ge_v8i16c:
; SSE41:       # BB#0:
; SSE41-NEXT:    movdqa {{.*#+}} xmm0 = [65529,65531,65533,65535,1,3,5,7]
; SSE41-NEXT:    pmaxuw {{.*}}(%rip), %xmm0
; SSE41-NEXT:    retq
;
; SSE42-LABEL: max_ge_v8i16c:
; SSE42:       # BB#0:
; SSE42-NEXT:    movdqa {{.*#+}} xmm0 = [65529,65531,65533,65535,1,3,5,7]
; SSE42-NEXT:    pmaxuw {{.*}}(%rip), %xmm0
; SSE42-NEXT:    retq
;
; AVX-LABEL: max_ge_v8i16c:
; AVX:       # BB#0:
; AVX-NEXT:    vmovdqa {{.*#+}} xmm0 = [65529,65531,65533,65535,1,3,5,7]
; AVX-NEXT:    vpmaxuw {{.*}}(%rip), %xmm0, %xmm0
; AVX-NEXT:    retq
  %1 = insertelement <8 x i16> <i16 -7, i16 -5, i16 -3, i16 -1, i16 1, i16 3, i16 5, i16 7>, i16 -7, i16 0
  %2 = insertelement <8 x i16> <i16 -1, i16 -3, i16 -5, i16 -7, i16 7, i16 5, i16 3, i16 1>, i16  1, i16 0
  %3 = icmp uge <8 x i16> %1, %2
  %4 = select <8 x i1> %3, <8 x i16> %1, <8 x i16> %2
  ret <8 x i16> %4
}

define <16 x i16> @max_ge_v16i16c() {
; SSE2-LABEL: max_ge_v16i16c:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa {{.*#+}} xmm2 = [65529,65530,65531,65532,65533,65534,65535,0]
; SSE2-NEXT:    movdqa {{.*#+}} xmm3 = [1,2,3,4,5,6,7,8]
; SSE2-NEXT:    movdqa {{.*#+}} xmm4 = [1,65534,65533,65532,65531,65530,65529,0]
; SSE2-NEXT:    movdqa {{.*#+}} xmm5 = [7,6,5,4,3,2,1,0]
; SSE2-NEXT:    movdqa %xmm5, %xmm1
; SSE2-NEXT:    psubusw %xmm3, %xmm1
; SSE2-NEXT:    pxor %xmm6, %xmm6
; SSE2-NEXT:    pcmpeqw %xmm6, %xmm1
; SSE2-NEXT:    movdqa %xmm4, %xmm0
; SSE2-NEXT:    psubusw %xmm2, %xmm0
; SSE2-NEXT:    pcmpeqw %xmm6, %xmm0
; SSE2-NEXT:    pand %xmm0, %xmm2
; SSE2-NEXT:    pandn %xmm4, %xmm0
; SSE2-NEXT:    por %xmm2, %xmm0
; SSE2-NEXT:    pand %xmm1, %xmm3
; SSE2-NEXT:    pandn %xmm5, %xmm1
; SSE2-NEXT:    por %xmm3, %xmm1
; SSE2-NEXT:    retq
;
; SSE41-LABEL: max_ge_v16i16c:
; SSE41:       # BB#0:
; SSE41-NEXT:    movdqa {{.*#+}} xmm1 = [1,2,3,4,5,6,7,8]
; SSE41-NEXT:    movdqa {{.*#+}} xmm0 = [65529,65530,65531,65532,65533,65534,65535,0]
; SSE41-NEXT:    pmaxuw {{.*}}(%rip), %xmm0
; SSE41-NEXT:    pmaxuw {{.*}}(%rip), %xmm1
; SSE41-NEXT:    retq
;
; SSE42-LABEL: max_ge_v16i16c:
; SSE42:       # BB#0:
; SSE42-NEXT:    movdqa {{.*#+}} xmm1 = [1,2,3,4,5,6,7,8]
; SSE42-NEXT:    movdqa {{.*#+}} xmm0 = [65529,65530,65531,65532,65533,65534,65535,0]
; SSE42-NEXT:    pmaxuw {{.*}}(%rip), %xmm0
; SSE42-NEXT:    pmaxuw {{.*}}(%rip), %xmm1
; SSE42-NEXT:    retq
;
; AVX1-LABEL: max_ge_v16i16c:
; AVX1:       # BB#0:
; AVX1-NEXT:    vmovdqa {{.*#+}} xmm0 = [65529,65530,65531,65532,65533,65534,65535,0]
; AVX1-NEXT:    vpmaxuw {{.*}}(%rip), %xmm0, %xmm0
; AVX1-NEXT:    vmovdqa {{.*#+}} xmm1 = [1,2,3,4,5,6,7,8]
; AVX1-NEXT:    vpmaxuw {{.*}}(%rip), %xmm1, %xmm1
; AVX1-NEXT:    vinsertf128 $1, %xmm1, %ymm0, %ymm0
; AVX1-NEXT:    retq
;
; AVX2-LABEL: max_ge_v16i16c:
; AVX2:       # BB#0:
; AVX2-NEXT:    vmovdqa {{.*#+}} ymm0 = [65529,65530,65531,65532,65533,65534,65535,0,1,2,3,4,5,6,7,8]
; AVX2-NEXT:    vpmaxuw {{.*}}(%rip), %ymm0, %ymm0
; AVX2-NEXT:    retq
;
; AVX512-LABEL: max_ge_v16i16c:
; AVX512:       # BB#0:
; AVX512-NEXT:    vmovdqa {{.*#+}} ymm0 = [65529,65530,65531,65532,65533,65534,65535,0,1,2,3,4,5,6,7,8]
; AVX512-NEXT:    vpmaxuw {{.*}}(%rip), %ymm0, %ymm0
; AVX512-NEXT:    retq
  %1 = insertelement <16 x i16> <i16 -7, i16 -6, i16 -5, i16 -4, i16 -3, i16 -2, i16 -1, i16 0, i16 1, i16 2, i16 3, i16 4, i16 5, i16 6, i16 7, i16 8>, i16 -7, i16 0
  %2 = insertelement <16 x i16> <i16 -1, i16 -2, i16 -3, i16 -4, i16 -5, i16 -6, i16 -7, i16 0, i16 7, i16 6, i16 5, i16 4, i16 3, i16 2, i16 1, i16 0>, i16  1, i16 0
  %3 = icmp uge <16 x i16> %1, %2
  %4 = select <16 x i1> %3, <16 x i16> %1, <16 x i16> %2
  ret <16 x i16> %4
}

define <16 x i8> @max_ge_v16i8c() {
; SSE-LABEL: max_ge_v16i8c:
; SSE:       # BB#0:
; SSE-NEXT:    movdqa {{.*#+}} xmm0 = [249,250,251,252,253,254,255,0,1,2,3,4,5,6,7,8]
; SSE-NEXT:    pmaxub {{.*}}(%rip), %xmm0
; SSE-NEXT:    retq
;
; AVX-LABEL: max_ge_v16i8c:
; AVX:       # BB#0:
; AVX-NEXT:    vmovdqa {{.*#+}} xmm0 = [249,250,251,252,253,254,255,0,1,2,3,4,5,6,7,8]
; AVX-NEXT:    vpmaxub {{.*}}(%rip), %xmm0, %xmm0
; AVX-NEXT:    retq
  %1 = insertelement <16 x i8> <i8 -7, i8 -6, i8 -5, i8 -4, i8 -3, i8 -2, i8 -1, i8 0, i8 1, i8 2, i8 3, i8 4, i8 5, i8 6, i8 7, i8 8>, i8 -7, i8 0
  %2 = insertelement <16 x i8> <i8 -1, i8 -2, i8 -3, i8 -4, i8 -5, i8 -6, i8 -7, i8 0, i8 7, i8 6, i8 5, i8 4, i8 3, i8 2, i8 1, i8 0>, i8  1, i8 0
  %3 = icmp uge <16 x i8> %1, %2
  %4 = select <16 x i1> %3, <16 x i8> %1, <16 x i8> %2
  ret <16 x i8> %4
}

define <2 x i64> @min_lt_v2i64c() {
; SSE2-LABEL: min_lt_v2i64c:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa {{.*#+}} xmm1 = [18446744073709551609,7]
; SSE2-NEXT:    movdqa {{.*#+}} xmm2 = [18446744073709551615,1]
; SSE2-NEXT:    movdqa {{.*#+}} xmm0 = [2147483648,2147483648,2147483648,2147483648]
; SSE2-NEXT:    movdqa %xmm0, %xmm3
; SSE2-NEXT:    pxor %xmm1, %xmm3
; SSE2-NEXT:    pxor %xmm2, %xmm0
; SSE2-NEXT:    movdqa %xmm0, %xmm4
; SSE2-NEXT:    pcmpgtd %xmm3, %xmm4
; SSE2-NEXT:    pshufd {{.*#+}} xmm5 = xmm4[0,0,2,2]
; SSE2-NEXT:    pcmpeqd %xmm3, %xmm0
; SSE2-NEXT:    pshufd {{.*#+}} xmm3 = xmm0[1,1,3,3]
; SSE2-NEXT:    pand %xmm5, %xmm3
; SSE2-NEXT:    pshufd {{.*#+}} xmm0 = xmm4[1,1,3,3]
; SSE2-NEXT:    por %xmm3, %xmm0
; SSE2-NEXT:    movdqa %xmm0, %xmm3
; SSE2-NEXT:    pandn %xmm2, %xmm3
; SSE2-NEXT:    pand %xmm1, %xmm0
; SSE2-NEXT:    por %xmm3, %xmm0
; SSE2-NEXT:    retq
;
; SSE41-LABEL: min_lt_v2i64c:
; SSE41:       # BB#0:
; SSE41-NEXT:    movdqa {{.*#+}} xmm2 = [18446744073709551609,7]
; SSE41-NEXT:    movdqa {{.*#+}} xmm1 = [18446744073709551615,1]
; SSE41-NEXT:    movdqa {{.*#+}} xmm0 = [2147483648,2147483648,2147483648,2147483648]
; SSE41-NEXT:    movdqa %xmm0, %xmm3
; SSE41-NEXT:    pxor %xmm2, %xmm3
; SSE41-NEXT:    pxor %xmm1, %xmm0
; SSE41-NEXT:    movdqa %xmm0, %xmm4
; SSE41-NEXT:    pcmpgtd %xmm3, %xmm4
; SSE41-NEXT:    pshufd {{.*#+}} xmm5 = xmm4[0,0,2,2]
; SSE41-NEXT:    pcmpeqd %xmm3, %xmm0
; SSE41-NEXT:    pshufd {{.*#+}} xmm3 = xmm0[1,1,3,3]
; SSE41-NEXT:    pand %xmm5, %xmm3
; SSE41-NEXT:    pshufd {{.*#+}} xmm0 = xmm4[1,1,3,3]
; SSE41-NEXT:    por %xmm3, %xmm0
; SSE41-NEXT:    blendvpd %xmm2, %xmm1
; SSE41-NEXT:    movapd %xmm1, %xmm0
; SSE41-NEXT:    retq
;
; SSE42-LABEL: min_lt_v2i64c:
; SSE42:       # BB#0:
; SSE42-NEXT:    movapd {{.*#+}} xmm1 = [18446744073709551615,1]
; SSE42-NEXT:    movdqa {{.*#+}} xmm0 = [9223372036854775807,9223372036854775809]
; SSE42-NEXT:    pcmpgtq {{.*}}(%rip), %xmm0
; SSE42-NEXT:    blendvpd {{.*}}(%rip), %xmm1
; SSE42-NEXT:    movapd %xmm1, %xmm0
; SSE42-NEXT:    retq
;
; AVX-LABEL: min_lt_v2i64c:
; AVX:       # BB#0:
; AVX-NEXT:    vmovapd {{.*#+}} xmm0 = [18446744073709551615,1]
; AVX-NEXT:    vmovdqa {{.*#+}} xmm1 = [9223372036854775807,9223372036854775809]
; AVX-NEXT:    vpcmpgtq {{.*}}(%rip), %xmm1, %xmm1
; AVX-NEXT:    vblendvpd %xmm1, {{.*}}(%rip), %xmm0, %xmm0
; AVX-NEXT:    retq
  %1 = insertelement <2 x i64> <i64 -7, i64 7>, i64 -7, i32 0
  %2 = insertelement <2 x i64> <i64 -1, i64 1>, i64 -1, i32 0
  %3 = icmp ult <2 x i64> %1, %2
  %4 = select <2 x i1> %3, <2 x i64> %1, <2 x i64> %2
  ret <2 x i64> %4
}

define <4 x i64> @min_lt_v4i64c() {
; SSE2-LABEL: min_lt_v4i64c:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa {{.*#+}} xmm4 = [18446744073709551609,18446744073709551615]
; SSE2-NEXT:    movdqa {{.*#+}} xmm8 = [1,7]
; SSE2-NEXT:    movdqa {{.*#+}} xmm5 = [18446744073709551615,18446744073709551609]
; SSE2-NEXT:    movdqa {{.*#+}} xmm3 = [7,1]
; SSE2-NEXT:    movdqa {{.*#+}} xmm0 = [2147483648,2147483648,2147483648,2147483648]
; SSE2-NEXT:    movdqa %xmm0, %xmm1
; SSE2-NEXT:    pxor %xmm8, %xmm1
; SSE2-NEXT:    movdqa %xmm0, %xmm6
; SSE2-NEXT:    pxor %xmm3, %xmm6
; SSE2-NEXT:    movdqa %xmm6, %xmm7
; SSE2-NEXT:    pcmpgtd %xmm1, %xmm7
; SSE2-NEXT:    pshufd {{.*#+}} xmm2 = xmm7[0,0,2,2]
; SSE2-NEXT:    pcmpeqd %xmm1, %xmm6
; SSE2-NEXT:    pshufd {{.*#+}} xmm6 = xmm6[1,1,3,3]
; SSE2-NEXT:    pand %xmm2, %xmm6
; SSE2-NEXT:    pshufd {{.*#+}} xmm1 = xmm7[1,1,3,3]
; SSE2-NEXT:    por %xmm6, %xmm1
; SSE2-NEXT:    movdqa %xmm0, %xmm2
; SSE2-NEXT:    pxor %xmm4, %xmm2
; SSE2-NEXT:    pxor %xmm5, %xmm0
; SSE2-NEXT:    movdqa %xmm0, %xmm6
; SSE2-NEXT:    pcmpgtd %xmm2, %xmm6
; SSE2-NEXT:    pshufd {{.*#+}} xmm7 = xmm6[0,0,2,2]
; SSE2-NEXT:    pcmpeqd %xmm2, %xmm0
; SSE2-NEXT:    pshufd {{.*#+}} xmm2 = xmm0[1,1,3,3]
; SSE2-NEXT:    pand %xmm7, %xmm2
; SSE2-NEXT:    pshufd {{.*#+}} xmm0 = xmm6[1,1,3,3]
; SSE2-NEXT:    por %xmm2, %xmm0
; SSE2-NEXT:    movdqa %xmm0, %xmm2
; SSE2-NEXT:    pandn %xmm5, %xmm2
; SSE2-NEXT:    pand %xmm4, %xmm0
; SSE2-NEXT:    por %xmm2, %xmm0
; SSE2-NEXT:    movdqa %xmm1, %xmm2
; SSE2-NEXT:    pandn %xmm3, %xmm2
; SSE2-NEXT:    pand %xmm8, %xmm1
; SSE2-NEXT:    por %xmm2, %xmm1
; SSE2-NEXT:    retq
;
; SSE41-LABEL: min_lt_v4i64c:
; SSE41:       # BB#0:
; SSE41-NEXT:    movdqa {{.*#+}} xmm5 = [18446744073709551609,18446744073709551615]
; SSE41-NEXT:    movdqa {{.*#+}} xmm8 = [1,7]
; SSE41-NEXT:    movdqa {{.*#+}} xmm2 = [18446744073709551615,18446744073709551609]
; SSE41-NEXT:    movdqa {{.*#+}} xmm1 = [7,1]
; SSE41-NEXT:    movdqa {{.*#+}} xmm0 = [2147483648,2147483648,2147483648,2147483648]
; SSE41-NEXT:    movdqa %xmm0, %xmm3
; SSE41-NEXT:    pxor %xmm8, %xmm3
; SSE41-NEXT:    movdqa %xmm0, %xmm6
; SSE41-NEXT:    pxor %xmm1, %xmm6
; SSE41-NEXT:    movdqa %xmm6, %xmm7
; SSE41-NEXT:    pcmpgtd %xmm3, %xmm7
; SSE41-NEXT:    pshufd {{.*#+}} xmm4 = xmm7[0,0,2,2]
; SSE41-NEXT:    pcmpeqd %xmm3, %xmm6
; SSE41-NEXT:    pshufd {{.*#+}} xmm6 = xmm6[1,1,3,3]
; SSE41-NEXT:    pand %xmm4, %xmm6
; SSE41-NEXT:    pshufd {{.*#+}} xmm3 = xmm7[1,1,3,3]
; SSE41-NEXT:    por %xmm6, %xmm3
; SSE41-NEXT:    movdqa %xmm0, %xmm4
; SSE41-NEXT:    pxor %xmm5, %xmm4
; SSE41-NEXT:    pxor %xmm2, %xmm0
; SSE41-NEXT:    movdqa %xmm0, %xmm6
; SSE41-NEXT:    pcmpgtd %xmm4, %xmm6
; SSE41-NEXT:    pshufd {{.*#+}} xmm7 = xmm6[0,0,2,2]
; SSE41-NEXT:    pcmpeqd %xmm4, %xmm0
; SSE41-NEXT:    pshufd {{.*#+}} xmm4 = xmm0[1,1,3,3]
; SSE41-NEXT:    pand %xmm7, %xmm4
; SSE41-NEXT:    pshufd {{.*#+}} xmm0 = xmm6[1,1,3,3]
; SSE41-NEXT:    por %xmm4, %xmm0
; SSE41-NEXT:    blendvpd %xmm5, %xmm2
; SSE41-NEXT:    movdqa %xmm3, %xmm0
; SSE41-NEXT:    blendvpd %xmm8, %xmm1
; SSE41-NEXT:    movapd %xmm2, %xmm0
; SSE41-NEXT:    retq
;
; SSE42-LABEL: min_lt_v4i64c:
; SSE42:       # BB#0:
; SSE42-NEXT:    movapd {{.*#+}} xmm1 = [7,1]
; SSE42-NEXT:    movapd {{.*#+}} xmm2 = [18446744073709551615,18446744073709551609]
; SSE42-NEXT:    movdqa {{.*#+}} xmm3 = [9223372036854775815,9223372036854775809]
; SSE42-NEXT:    pcmpgtq {{.*}}(%rip), %xmm3
; SSE42-NEXT:    movdqa {{.*#+}} xmm0 = [9223372036854775807,9223372036854775801]
; SSE42-NEXT:    pcmpgtq {{.*}}(%rip), %xmm0
; SSE42-NEXT:    blendvpd {{.*}}(%rip), %xmm2
; SSE42-NEXT:    movdqa %xmm3, %xmm0
; SSE42-NEXT:    blendvpd {{.*}}(%rip), %xmm1
; SSE42-NEXT:    movapd %xmm2, %xmm0
; SSE42-NEXT:    retq
;
; AVX1-LABEL: min_lt_v4i64c:
; AVX1:       # BB#0:
; AVX1-NEXT:    vmovapd {{.*#+}} ymm0 = [18446744073709551615,18446744073709551609,7,1]
; AVX1-NEXT:    vmovdqa {{.*#+}} xmm1 = [9223372036854775807,9223372036854775801]
; AVX1-NEXT:    vpcmpgtq {{.*}}(%rip), %xmm1, %xmm1
; AVX1-NEXT:    vmovdqa {{.*#+}} xmm2 = [9223372036854775815,9223372036854775809]
; AVX1-NEXT:    vpcmpgtq {{.*}}(%rip), %xmm2, %xmm2
; AVX1-NEXT:    vinsertf128 $1, %xmm2, %ymm1, %ymm1
; AVX1-NEXT:    vblendvpd %ymm1, {{.*}}(%rip), %ymm0, %ymm0
; AVX1-NEXT:    retq
;
; AVX2-LABEL: min_lt_v4i64c:
; AVX2:       # BB#0:
; AVX2-NEXT:    vmovapd {{.*#+}} ymm0 = [18446744073709551615,18446744073709551609,7,1]
; AVX2-NEXT:    vmovdqa {{.*#+}} ymm1 = [9223372036854775807,9223372036854775801,9223372036854775815,9223372036854775809]
; AVX2-NEXT:    vpcmpgtq {{.*}}(%rip), %ymm1, %ymm1
; AVX2-NEXT:    vblendvpd %ymm1, {{.*}}(%rip), %ymm0, %ymm0
; AVX2-NEXT:    retq
;
; AVX512-LABEL: min_lt_v4i64c:
; AVX512:       # BB#0:
; AVX512-NEXT:    vmovapd {{.*#+}} ymm0 = [18446744073709551615,18446744073709551609,7,1]
; AVX512-NEXT:    vmovdqa {{.*#+}} ymm1 = [9223372036854775807,9223372036854775801,9223372036854775815,9223372036854775809]
; AVX512-NEXT:    vpcmpgtq {{.*}}(%rip), %ymm1, %ymm1
; AVX512-NEXT:    vblendvpd %ymm1, {{.*}}(%rip), %ymm0, %ymm0
; AVX512-NEXT:    retq
  %1 = insertelement <4 x i64> <i64 -7, i64 -1, i64 1, i64 7>, i64 -7, i32 0
  %2 = insertelement <4 x i64> <i64 -1, i64 -7, i64 7, i64 1>, i64 -1, i32 0
  %3 = icmp ult <4 x i64> %1, %2
  %4 = select <4 x i1> %3, <4 x i64> %1, <4 x i64> %2
  ret <4 x i64> %4
}

define <4 x i32> @min_lt_v4i32c() {
; SSE2-LABEL: min_lt_v4i32c:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa {{.*#+}} xmm0 = [2147483649,2147483641,2147483655,2147483649]
; SSE2-NEXT:    pcmpgtd {{.*}}(%rip), %xmm0
; SSE2-NEXT:    movdqa %xmm0, %xmm1
; SSE2-NEXT:    pandn {{.*}}(%rip), %xmm1
; SSE2-NEXT:    pand {{.*}}(%rip), %xmm0
; SSE2-NEXT:    por %xmm1, %xmm0
; SSE2-NEXT:    retq
;
; SSE41-LABEL: min_lt_v4i32c:
; SSE41:       # BB#0:
; SSE41-NEXT:    movdqa {{.*#+}} xmm0 = [4294967289,4294967295,1,7]
; SSE41-NEXT:    pminud {{.*}}(%rip), %xmm0
; SSE41-NEXT:    retq
;
; SSE42-LABEL: min_lt_v4i32c:
; SSE42:       # BB#0:
; SSE42-NEXT:    movdqa {{.*#+}} xmm0 = [4294967289,4294967295,1,7]
; SSE42-NEXT:    pminud {{.*}}(%rip), %xmm0
; SSE42-NEXT:    retq
;
; AVX-LABEL: min_lt_v4i32c:
; AVX:       # BB#0:
; AVX-NEXT:    vmovdqa {{.*#+}} xmm0 = [4294967289,4294967295,1,7]
; AVX-NEXT:    vpminud {{.*}}(%rip), %xmm0, %xmm0
; AVX-NEXT:    retq
  %1 = insertelement <4 x i32> <i32 -7, i32 -1, i32 1, i32 7>, i32 -7, i32 0
  %2 = insertelement <4 x i32> <i32 -1, i32 -7, i32 7, i32 1>, i32  1, i32 0
  %3 = icmp ult <4 x i32> %1, %2
  %4 = select <4 x i1> %3, <4 x i32> %1, <4 x i32> %2
  ret <4 x i32> %4
}

define <8 x i32> @min_lt_v8i32c() {
; SSE2-LABEL: min_lt_v8i32c:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa {{.*#+}} xmm1 = [2147483655,2147483653,2147483651,2147483649]
; SSE2-NEXT:    pcmpgtd {{.*}}(%rip), %xmm1
; SSE2-NEXT:    movdqa {{.*#+}} xmm0 = [2147483649,2147483645,2147483643,2147483641]
; SSE2-NEXT:    pcmpgtd {{.*}}(%rip), %xmm0
; SSE2-NEXT:    movdqa %xmm0, %xmm2
; SSE2-NEXT:    pandn {{.*}}(%rip), %xmm2
; SSE2-NEXT:    pand {{.*}}(%rip), %xmm0
; SSE2-NEXT:    por %xmm2, %xmm0
; SSE2-NEXT:    movdqa %xmm1, %xmm2
; SSE2-NEXT:    pandn {{.*}}(%rip), %xmm2
; SSE2-NEXT:    pand {{.*}}(%rip), %xmm1
; SSE2-NEXT:    por %xmm2, %xmm1
; SSE2-NEXT:    retq
;
; SSE41-LABEL: min_lt_v8i32c:
; SSE41:       # BB#0:
; SSE41-NEXT:    movdqa {{.*#+}} xmm1 = [1,3,5,7]
; SSE41-NEXT:    movdqa {{.*#+}} xmm0 = [4294967289,4294967291,4294967293,4294967295]
; SSE41-NEXT:    pminud {{.*}}(%rip), %xmm0
; SSE41-NEXT:    pminud {{.*}}(%rip), %xmm1
; SSE41-NEXT:    retq
;
; SSE42-LABEL: min_lt_v8i32c:
; SSE42:       # BB#0:
; SSE42-NEXT:    movdqa {{.*#+}} xmm1 = [1,3,5,7]
; SSE42-NEXT:    movdqa {{.*#+}} xmm0 = [4294967289,4294967291,4294967293,4294967295]
; SSE42-NEXT:    pminud {{.*}}(%rip), %xmm0
; SSE42-NEXT:    pminud {{.*}}(%rip), %xmm1
; SSE42-NEXT:    retq
;
; AVX1-LABEL: min_lt_v8i32c:
; AVX1:       # BB#0:
; AVX1-NEXT:    vmovdqa {{.*#+}} xmm0 = [4294967289,4294967291,4294967293,4294967295]
; AVX1-NEXT:    vpminud {{.*}}(%rip), %xmm0, %xmm0
; AVX1-NEXT:    vmovdqa {{.*#+}} xmm1 = [1,3,5,7]
; AVX1-NEXT:    vpminud {{.*}}(%rip), %xmm1, %xmm1
; AVX1-NEXT:    vinsertf128 $1, %xmm1, %ymm0, %ymm0
; AVX1-NEXT:    retq
;
; AVX2-LABEL: min_lt_v8i32c:
; AVX2:       # BB#0:
; AVX2-NEXT:    vmovdqa {{.*#+}} ymm0 = [4294967289,4294967291,4294967293,4294967295,1,3,5,7]
; AVX2-NEXT:    vpminud {{.*}}(%rip), %ymm0, %ymm0
; AVX2-NEXT:    retq
;
; AVX512-LABEL: min_lt_v8i32c:
; AVX512:       # BB#0:
; AVX512-NEXT:    vmovdqa {{.*#+}} ymm0 = [4294967289,4294967291,4294967293,4294967295,1,3,5,7]
; AVX512-NEXT:    vpminud {{.*}}(%rip), %ymm0, %ymm0
; AVX512-NEXT:    retq
  %1 = insertelement <8 x i32> <i32 -7, i32 -5, i32 -3, i32 -1, i32 1, i32 3, i32 5, i32 7>, i32 -7, i32 0
  %2 = insertelement <8 x i32> <i32 -1, i32 -3, i32 -5, i32 -7, i32 7, i32 5, i32 3, i32 1>, i32  1, i32 0
  %3 = icmp ult <8 x i32> %1, %2
  %4 = select <8 x i1> %3, <8 x i32> %1, <8 x i32> %2
  ret <8 x i32> %4
}

define <8 x i16> @min_lt_v8i16c() {
; SSE2-LABEL: min_lt_v8i16c:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa {{.*#+}} xmm1 = [65529,65531,65533,65535,1,3,5,7]
; SSE2-NEXT:    movdqa %xmm1, %xmm2
; SSE2-NEXT:    psubusw {{.*}}(%rip), %xmm2
; SSE2-NEXT:    pxor %xmm0, %xmm0
; SSE2-NEXT:    pcmpeqw %xmm2, %xmm0
; SSE2-NEXT:    pand %xmm0, %xmm1
; SSE2-NEXT:    pandn {{.*}}(%rip), %xmm0
; SSE2-NEXT:    por %xmm1, %xmm0
; SSE2-NEXT:    retq
;
; SSE41-LABEL: min_lt_v8i16c:
; SSE41:       # BB#0:
; SSE41-NEXT:    movdqa {{.*#+}} xmm0 = [65529,65531,65533,65535,1,3,5,7]
; SSE41-NEXT:    pminuw {{.*}}(%rip), %xmm0
; SSE41-NEXT:    retq
;
; SSE42-LABEL: min_lt_v8i16c:
; SSE42:       # BB#0:
; SSE42-NEXT:    movdqa {{.*#+}} xmm0 = [65529,65531,65533,65535,1,3,5,7]
; SSE42-NEXT:    pminuw {{.*}}(%rip), %xmm0
; SSE42-NEXT:    retq
;
; AVX-LABEL: min_lt_v8i16c:
; AVX:       # BB#0:
; AVX-NEXT:    vmovdqa {{.*#+}} xmm0 = [65529,65531,65533,65535,1,3,5,7]
; AVX-NEXT:    vpminuw {{.*}}(%rip), %xmm0, %xmm0
; AVX-NEXT:    retq
  %1 = insertelement <8 x i16> <i16 -7, i16 -5, i16 -3, i16 -1, i16 1, i16 3, i16 5, i16 7>, i16 -7, i16 0
  %2 = insertelement <8 x i16> <i16 -1, i16 -3, i16 -5, i16 -7, i16 7, i16 5, i16 3, i16 1>, i16  1, i16 0
  %3 = icmp ult <8 x i16> %1, %2
  %4 = select <8 x i1> %3, <8 x i16> %1, <8 x i16> %2
  ret <8 x i16> %4
}

define <16 x i16> @min_lt_v16i16c() {
; SSE2-LABEL: min_lt_v16i16c:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa {{.*#+}} xmm1 = [32775,32774,32773,32772,32771,32770,32769,32768]
; SSE2-NEXT:    pcmpgtw {{.*}}(%rip), %xmm1
; SSE2-NEXT:    movdqa {{.*#+}} xmm0 = [32769,32766,32765,32764,32763,32762,32761,32768]
; SSE2-NEXT:    pcmpgtw {{.*}}(%rip), %xmm0
; SSE2-NEXT:    movdqa %xmm0, %xmm2
; SSE2-NEXT:    pandn {{.*}}(%rip), %xmm2
; SSE2-NEXT:    pand {{.*}}(%rip), %xmm0
; SSE2-NEXT:    por %xmm2, %xmm0
; SSE2-NEXT:    movdqa %xmm1, %xmm2
; SSE2-NEXT:    pandn {{.*}}(%rip), %xmm2
; SSE2-NEXT:    pand {{.*}}(%rip), %xmm1
; SSE2-NEXT:    por %xmm2, %xmm1
; SSE2-NEXT:    retq
;
; SSE41-LABEL: min_lt_v16i16c:
; SSE41:       # BB#0:
; SSE41-NEXT:    movdqa {{.*#+}} xmm1 = [1,2,3,4,5,6,7,8]
; SSE41-NEXT:    movdqa {{.*#+}} xmm0 = [65529,65530,65531,65532,65533,65534,65535,0]
; SSE41-NEXT:    pminuw {{.*}}(%rip), %xmm0
; SSE41-NEXT:    pminuw {{.*}}(%rip), %xmm1
; SSE41-NEXT:    retq
;
; SSE42-LABEL: min_lt_v16i16c:
; SSE42:       # BB#0:
; SSE42-NEXT:    movdqa {{.*#+}} xmm1 = [1,2,3,4,5,6,7,8]
; SSE42-NEXT:    movdqa {{.*#+}} xmm0 = [65529,65530,65531,65532,65533,65534,65535,0]
; SSE42-NEXT:    pminuw {{.*}}(%rip), %xmm0
; SSE42-NEXT:    pminuw {{.*}}(%rip), %xmm1
; SSE42-NEXT:    retq
;
; AVX1-LABEL: min_lt_v16i16c:
; AVX1:       # BB#0:
; AVX1-NEXT:    vmovdqa {{.*#+}} xmm0 = [65529,65530,65531,65532,65533,65534,65535,0]
; AVX1-NEXT:    vpminuw {{.*}}(%rip), %xmm0, %xmm0
; AVX1-NEXT:    vmovdqa {{.*#+}} xmm1 = [1,2,3,4,5,6,7,8]
; AVX1-NEXT:    vpminuw {{.*}}(%rip), %xmm1, %xmm1
; AVX1-NEXT:    vinsertf128 $1, %xmm1, %ymm0, %ymm0
; AVX1-NEXT:    retq
;
; AVX2-LABEL: min_lt_v16i16c:
; AVX2:       # BB#0:
; AVX2-NEXT:    vmovdqa {{.*#+}} ymm0 = [65529,65530,65531,65532,65533,65534,65535,0,1,2,3,4,5,6,7,8]
; AVX2-NEXT:    vpminuw {{.*}}(%rip), %ymm0, %ymm0
; AVX2-NEXT:    retq
;
; AVX512-LABEL: min_lt_v16i16c:
; AVX512:       # BB#0:
; AVX512-NEXT:    vmovdqa {{.*#+}} ymm0 = [65529,65530,65531,65532,65533,65534,65535,0,1,2,3,4,5,6,7,8]
; AVX512-NEXT:    vpminuw {{.*}}(%rip), %ymm0, %ymm0
; AVX512-NEXT:    retq
  %1 = insertelement <16 x i16> <i16 -7, i16 -6, i16 -5, i16 -4, i16 -3, i16 -2, i16 -1, i16 0, i16 1, i16 2, i16 3, i16 4, i16 5, i16 6, i16 7, i16 8>, i16 -7, i16 0
  %2 = insertelement <16 x i16> <i16 -1, i16 -2, i16 -3, i16 -4, i16 -5, i16 -6, i16 -7, i16 0, i16 7, i16 6, i16 5, i16 4, i16 3, i16 2, i16 1, i16 0>, i16  1, i16 0
  %3 = icmp ult <16 x i16> %1, %2
  %4 = select <16 x i1> %3, <16 x i16> %1, <16 x i16> %2
  ret <16 x i16> %4
}

define <16 x i8> @min_lt_v16i8c() {
; SSE-LABEL: min_lt_v16i8c:
; SSE:       # BB#0:
; SSE-NEXT:    movdqa {{.*#+}} xmm0 = [249,250,251,252,253,254,255,0,1,2,3,4,5,6,7,8]
; SSE-NEXT:    pminub {{.*}}(%rip), %xmm0
; SSE-NEXT:    retq
;
; AVX-LABEL: min_lt_v16i8c:
; AVX:       # BB#0:
; AVX-NEXT:    vmovdqa {{.*#+}} xmm0 = [249,250,251,252,253,254,255,0,1,2,3,4,5,6,7,8]
; AVX-NEXT:    vpminub {{.*}}(%rip), %xmm0, %xmm0
; AVX-NEXT:    retq
  %1 = insertelement <16 x i8> <i8 -7, i8 -6, i8 -5, i8 -4, i8 -3, i8 -2, i8 -1, i8 0, i8 1, i8 2, i8 3, i8 4, i8 5, i8 6, i8 7, i8 8>, i8 -7, i8 0
  %2 = insertelement <16 x i8> <i8 -1, i8 -2, i8 -3, i8 -4, i8 -5, i8 -6, i8 -7, i8 0, i8 7, i8 6, i8 5, i8 4, i8 3, i8 2, i8 1, i8 0>, i8  1, i8 0
  %3 = icmp ult <16 x i8> %1, %2
  %4 = select <16 x i1> %3, <16 x i8> %1, <16 x i8> %2
  ret <16 x i8> %4
}

define <2 x i64> @min_le_v2i64c() {
; SSE2-LABEL: min_le_v2i64c:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa {{.*#+}} xmm1 = [18446744073709551609,7]
; SSE2-NEXT:    movdqa {{.*#+}} xmm2 = [18446744073709551615,1]
; SSE2-NEXT:    movdqa {{.*#+}} xmm0 = [2147483648,2147483648,2147483648,2147483648]
; SSE2-NEXT:    movdqa %xmm0, %xmm3
; SSE2-NEXT:    pxor %xmm2, %xmm3
; SSE2-NEXT:    pxor %xmm1, %xmm0
; SSE2-NEXT:    movdqa %xmm0, %xmm4
; SSE2-NEXT:    pcmpgtd %xmm3, %xmm4
; SSE2-NEXT:    pshufd {{.*#+}} xmm5 = xmm4[0,0,2,2]
; SSE2-NEXT:    pcmpeqd %xmm3, %xmm0
; SSE2-NEXT:    pshufd {{.*#+}} xmm0 = xmm0[1,1,3,3]
; SSE2-NEXT:    pand %xmm5, %xmm0
; SSE2-NEXT:    pshufd {{.*#+}} xmm3 = xmm4[1,1,3,3]
; SSE2-NEXT:    por %xmm0, %xmm3
; SSE2-NEXT:    pcmpeqd %xmm0, %xmm0
; SSE2-NEXT:    pxor %xmm3, %xmm0
; SSE2-NEXT:    pandn %xmm1, %xmm3
; SSE2-NEXT:    pandn %xmm2, %xmm0
; SSE2-NEXT:    por %xmm3, %xmm0
; SSE2-NEXT:    retq
;
; SSE41-LABEL: min_le_v2i64c:
; SSE41:       # BB#0:
; SSE41-NEXT:    movdqa {{.*#+}} xmm2 = [18446744073709551609,7]
; SSE41-NEXT:    movdqa {{.*#+}} xmm1 = [18446744073709551615,1]
; SSE41-NEXT:    movdqa {{.*#+}} xmm0 = [2147483648,2147483648,2147483648,2147483648]
; SSE41-NEXT:    movdqa %xmm0, %xmm3
; SSE41-NEXT:    pxor %xmm1, %xmm3
; SSE41-NEXT:    pxor %xmm2, %xmm0
; SSE41-NEXT:    movdqa %xmm0, %xmm4
; SSE41-NEXT:    pcmpgtd %xmm3, %xmm4
; SSE41-NEXT:    pshufd {{.*#+}} xmm5 = xmm4[0,0,2,2]
; SSE41-NEXT:    pcmpeqd %xmm3, %xmm0
; SSE41-NEXT:    pshufd {{.*#+}} xmm0 = xmm0[1,1,3,3]
; SSE41-NEXT:    pand %xmm5, %xmm0
; SSE41-NEXT:    pshufd {{.*#+}} xmm3 = xmm4[1,1,3,3]
; SSE41-NEXT:    por %xmm0, %xmm3
; SSE41-NEXT:    pcmpeqd %xmm0, %xmm0
; SSE41-NEXT:    pxor %xmm3, %xmm0
; SSE41-NEXT:    blendvpd %xmm2, %xmm1
; SSE41-NEXT:    movapd %xmm1, %xmm0
; SSE41-NEXT:    retq
;
; SSE42-LABEL: min_le_v2i64c:
; SSE42:       # BB#0:
; SSE42-NEXT:    movapd {{.*#+}} xmm1 = [18446744073709551615,1]
; SSE42-NEXT:    movdqa {{.*#+}} xmm2 = [9223372036854775801,9223372036854775815]
; SSE42-NEXT:    pcmpgtq {{.*}}(%rip), %xmm2
; SSE42-NEXT:    pcmpeqd %xmm0, %xmm0
; SSE42-NEXT:    pxor %xmm2, %xmm0
; SSE42-NEXT:    blendvpd {{.*}}(%rip), %xmm1
; SSE42-NEXT:    movapd %xmm1, %xmm0
; SSE42-NEXT:    retq
;
; AVX-LABEL: min_le_v2i64c:
; AVX:       # BB#0:
; AVX-NEXT:    vmovapd {{.*#+}} xmm0 = [18446744073709551615,1]
; AVX-NEXT:    vmovdqa {{.*#+}} xmm1 = [9223372036854775801,9223372036854775815]
; AVX-NEXT:    vpcmpgtq {{.*}}(%rip), %xmm1, %xmm1
; AVX-NEXT:    vpcmpeqd %xmm2, %xmm2, %xmm2
; AVX-NEXT:    vpxor %xmm2, %xmm1, %xmm1
; AVX-NEXT:    vblendvpd %xmm1, {{.*}}(%rip), %xmm0, %xmm0
; AVX-NEXT:    retq
  %1 = insertelement <2 x i64> <i64 -7, i64 7>, i64 -7, i32 0
  %2 = insertelement <2 x i64> <i64 -1, i64 1>, i64 -1, i32 0
  %3 = icmp ule <2 x i64> %1, %2
  %4 = select <2 x i1> %3, <2 x i64> %1, <2 x i64> %2
  ret <2 x i64> %4
}

define <4 x i64> @min_le_v4i64c() {
; SSE2-LABEL: min_le_v4i64c:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa {{.*#+}} xmm10 = [18446744073709551609,18446744073709551615]
; SSE2-NEXT:    movdqa {{.*#+}} xmm8 = [1,7]
; SSE2-NEXT:    movdqa {{.*#+}} xmm5 = [18446744073709551615,18446744073709551609]
; SSE2-NEXT:    movdqa {{.*#+}} xmm9 = [7,1]
; SSE2-NEXT:    movdqa {{.*#+}} xmm7 = [2147483648,2147483648,2147483648,2147483648]
; SSE2-NEXT:    movdqa %xmm7, %xmm0
; SSE2-NEXT:    pxor %xmm9, %xmm0
; SSE2-NEXT:    movdqa %xmm7, %xmm1
; SSE2-NEXT:    pxor %xmm8, %xmm1
; SSE2-NEXT:    movdqa %xmm1, %xmm6
; SSE2-NEXT:    pcmpgtd %xmm0, %xmm6
; SSE2-NEXT:    pshufd {{.*#+}} xmm2 = xmm6[0,0,2,2]
; SSE2-NEXT:    pcmpeqd %xmm0, %xmm1
; SSE2-NEXT:    pshufd {{.*#+}} xmm0 = xmm1[1,1,3,3]
; SSE2-NEXT:    pand %xmm2, %xmm0
; SSE2-NEXT:    pshufd {{.*#+}} xmm6 = xmm6[1,1,3,3]
; SSE2-NEXT:    por %xmm0, %xmm6
; SSE2-NEXT:    pcmpeqd %xmm0, %xmm0
; SSE2-NEXT:    movdqa %xmm6, %xmm1
; SSE2-NEXT:    pxor %xmm0, %xmm1
; SSE2-NEXT:    movdqa %xmm7, %xmm2
; SSE2-NEXT:    pxor %xmm5, %xmm2
; SSE2-NEXT:    pxor %xmm10, %xmm7
; SSE2-NEXT:    movdqa %xmm7, %xmm3
; SSE2-NEXT:    pcmpgtd %xmm2, %xmm3
; SSE2-NEXT:    pshufd {{.*#+}} xmm4 = xmm3[0,0,2,2]
; SSE2-NEXT:    pcmpeqd %xmm2, %xmm7
; SSE2-NEXT:    pshufd {{.*#+}} xmm2 = xmm7[1,1,3,3]
; SSE2-NEXT:    pand %xmm4, %xmm2
; SSE2-NEXT:    pshufd {{.*#+}} xmm3 = xmm3[1,1,3,3]
; SSE2-NEXT:    por %xmm2, %xmm3
; SSE2-NEXT:    pxor %xmm3, %xmm0
; SSE2-NEXT:    pandn %xmm10, %xmm3
; SSE2-NEXT:    pandn %xmm5, %xmm0
; SSE2-NEXT:    por %xmm3, %xmm0
; SSE2-NEXT:    pandn %xmm8, %xmm6
; SSE2-NEXT:    pandn %xmm9, %xmm1
; SSE2-NEXT:    por %xmm6, %xmm1
; SSE2-NEXT:    retq
;
; SSE41-LABEL: min_le_v4i64c:
; SSE41:       # BB#0:
; SSE41-NEXT:    movdqa {{.*#+}} xmm9 = [18446744073709551609,18446744073709551615]
; SSE41-NEXT:    movdqa {{.*#+}} xmm8 = [1,7]
; SSE41-NEXT:    movdqa {{.*#+}} xmm2 = [18446744073709551615,18446744073709551609]
; SSE41-NEXT:    movdqa {{.*#+}} xmm1 = [7,1]
; SSE41-NEXT:    movdqa {{.*#+}} xmm0 = [2147483648,2147483648,2147483648,2147483648]
; SSE41-NEXT:    movdqa %xmm0, %xmm3
; SSE41-NEXT:    pxor %xmm1, %xmm3
; SSE41-NEXT:    movdqa %xmm0, %xmm6
; SSE41-NEXT:    pxor %xmm8, %xmm6
; SSE41-NEXT:    movdqa %xmm6, %xmm7
; SSE41-NEXT:    pcmpgtd %xmm3, %xmm7
; SSE41-NEXT:    pshufd {{.*#+}} xmm4 = xmm7[0,0,2,2]
; SSE41-NEXT:    pcmpeqd %xmm3, %xmm6
; SSE41-NEXT:    pshufd {{.*#+}} xmm6 = xmm6[1,1,3,3]
; SSE41-NEXT:    pand %xmm4, %xmm6
; SSE41-NEXT:    pshufd {{.*#+}} xmm3 = xmm7[1,1,3,3]
; SSE41-NEXT:    por %xmm6, %xmm3
; SSE41-NEXT:    pcmpeqd %xmm4, %xmm4
; SSE41-NEXT:    pxor %xmm4, %xmm3
; SSE41-NEXT:    movdqa %xmm0, %xmm6
; SSE41-NEXT:    pxor %xmm2, %xmm6
; SSE41-NEXT:    pxor %xmm9, %xmm0
; SSE41-NEXT:    movdqa %xmm0, %xmm7
; SSE41-NEXT:    pcmpgtd %xmm6, %xmm7
; SSE41-NEXT:    pshufd {{.*#+}} xmm5 = xmm7[0,0,2,2]
; SSE41-NEXT:    pcmpeqd %xmm6, %xmm0
; SSE41-NEXT:    pshufd {{.*#+}} xmm6 = xmm0[1,1,3,3]
; SSE41-NEXT:    pand %xmm5, %xmm6
; SSE41-NEXT:    pshufd {{.*#+}} xmm0 = xmm7[1,1,3,3]
; SSE41-NEXT:    por %xmm6, %xmm0
; SSE41-NEXT:    pxor %xmm4, %xmm0
; SSE41-NEXT:    blendvpd %xmm9, %xmm2
; SSE41-NEXT:    movdqa %xmm3, %xmm0
; SSE41-NEXT:    blendvpd %xmm8, %xmm1
; SSE41-NEXT:    movapd %xmm2, %xmm0
; SSE41-NEXT:    retq
;
; SSE42-LABEL: min_le_v4i64c:
; SSE42:       # BB#0:
; SSE42-NEXT:    movapd {{.*#+}} xmm1 = [7,1]
; SSE42-NEXT:    movapd {{.*#+}} xmm2 = [18446744073709551615,18446744073709551609]
; SSE42-NEXT:    movdqa {{.*#+}} xmm3 = [9223372036854775809,9223372036854775815]
; SSE42-NEXT:    pcmpgtq {{.*}}(%rip), %xmm3
; SSE42-NEXT:    pcmpeqd %xmm4, %xmm4
; SSE42-NEXT:    pxor %xmm4, %xmm3
; SSE42-NEXT:    movdqa {{.*#+}} xmm0 = [9223372036854775801,9223372036854775807]
; SSE42-NEXT:    pcmpgtq {{.*}}(%rip), %xmm0
; SSE42-NEXT:    pxor %xmm4, %xmm0
; SSE42-NEXT:    blendvpd {{.*}}(%rip), %xmm2
; SSE42-NEXT:    movdqa %xmm3, %xmm0
; SSE42-NEXT:    blendvpd {{.*}}(%rip), %xmm1
; SSE42-NEXT:    movapd %xmm2, %xmm0
; SSE42-NEXT:    retq
;
; AVX1-LABEL: min_le_v4i64c:
; AVX1:       # BB#0:
; AVX1-NEXT:    vmovapd {{.*#+}} ymm0 = [18446744073709551615,18446744073709551609,7,1]
; AVX1-NEXT:    vmovdqa {{.*#+}} xmm1 = [9223372036854775809,9223372036854775815]
; AVX1-NEXT:    vpcmpgtq {{.*}}(%rip), %xmm1, %xmm1
; AVX1-NEXT:    vpcmpeqd %xmm2, %xmm2, %xmm2
; AVX1-NEXT:    vpxor %xmm2, %xmm1, %xmm1
; AVX1-NEXT:    vmovdqa {{.*#+}} xmm3 = [9223372036854775801,9223372036854775807]
; AVX1-NEXT:    vpcmpgtq {{.*}}(%rip), %xmm3, %xmm3
; AVX1-NEXT:    vpxor %xmm2, %xmm3, %xmm2
; AVX1-NEXT:    vinsertf128 $1, %xmm1, %ymm2, %ymm1
; AVX1-NEXT:    vblendvpd %ymm1, {{.*}}(%rip), %ymm0, %ymm0
; AVX1-NEXT:    retq
;
; AVX2-LABEL: min_le_v4i64c:
; AVX2:       # BB#0:
; AVX2-NEXT:    vmovapd {{.*#+}} ymm0 = [18446744073709551615,18446744073709551609,7,1]
; AVX2-NEXT:    vmovdqa {{.*#+}} ymm1 = [9223372036854775801,9223372036854775807,9223372036854775809,9223372036854775815]
; AVX2-NEXT:    vpcmpgtq {{.*}}(%rip), %ymm1, %ymm1
; AVX2-NEXT:    vpcmpeqd %ymm2, %ymm2, %ymm2
; AVX2-NEXT:    vpxor %ymm2, %ymm1, %ymm1
; AVX2-NEXT:    vblendvpd %ymm1, {{.*}}(%rip), %ymm0, %ymm0
; AVX2-NEXT:    retq
;
; AVX512-LABEL: min_le_v4i64c:
; AVX512:       # BB#0:
; AVX512-NEXT:    vmovapd {{.*#+}} ymm0 = [18446744073709551615,18446744073709551609,7,1]
; AVX512-NEXT:    vmovdqa {{.*#+}} ymm1 = [9223372036854775801,9223372036854775807,9223372036854775809,9223372036854775815]
; AVX512-NEXT:    vpcmpgtq {{.*}}(%rip), %ymm1, %ymm1
; AVX512-NEXT:    vpcmpeqd %ymm2, %ymm2, %ymm2
; AVX512-NEXT:    vpxor %ymm2, %ymm1, %ymm1
; AVX512-NEXT:    vblendvpd %ymm1, {{.*}}(%rip), %ymm0, %ymm0
; AVX512-NEXT:    retq
  %1 = insertelement <4 x i64> <i64 -7, i64 -1, i64 1, i64 7>, i64 -7, i32 0
  %2 = insertelement <4 x i64> <i64 -1, i64 -7, i64 7, i64 1>, i64 -1, i32 0
  %3 = icmp ule <4 x i64> %1, %2
  %4 = select <4 x i1> %3, <4 x i64> %1, <4 x i64> %2
  ret <4 x i64> %4
}

define <4 x i32> @min_le_v4i32c() {
; SSE2-LABEL: min_le_v4i32c:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa {{.*#+}} xmm0 = [2147483641,2147483647,2147483649,2147483655]
; SSE2-NEXT:    pcmpgtd {{.*}}(%rip), %xmm0
; SSE2-NEXT:    pcmpeqd %xmm1, %xmm1
; SSE2-NEXT:    pxor %xmm0, %xmm1
; SSE2-NEXT:    pandn {{.*}}(%rip), %xmm1
; SSE2-NEXT:    pandn {{.*}}(%rip), %xmm0
; SSE2-NEXT:    por %xmm1, %xmm0
; SSE2-NEXT:    retq
;
; SSE41-LABEL: min_le_v4i32c:
; SSE41:       # BB#0:
; SSE41-NEXT:    movdqa {{.*#+}} xmm0 = [4294967289,4294967295,1,7]
; SSE41-NEXT:    pminud {{.*}}(%rip), %xmm0
; SSE41-NEXT:    retq
;
; SSE42-LABEL: min_le_v4i32c:
; SSE42:       # BB#0:
; SSE42-NEXT:    movdqa {{.*#+}} xmm0 = [4294967289,4294967295,1,7]
; SSE42-NEXT:    pminud {{.*}}(%rip), %xmm0
; SSE42-NEXT:    retq
;
; AVX-LABEL: min_le_v4i32c:
; AVX:       # BB#0:
; AVX-NEXT:    vmovdqa {{.*#+}} xmm0 = [4294967289,4294967295,1,7]
; AVX-NEXT:    vpminud {{.*}}(%rip), %xmm0, %xmm0
; AVX-NEXT:    retq
  %1 = insertelement <4 x i32> <i32 -7, i32 -1, i32 1, i32 7>, i32 -7, i32 0
  %2 = insertelement <4 x i32> <i32 -1, i32 -7, i32 7, i32 1>, i32  1, i32 0
  %3 = icmp ule <4 x i32> %1, %2
  %4 = select <4 x i1> %3, <4 x i32> %1, <4 x i32> %2
  ret <4 x i32> %4
}

define <8 x i32> @min_le_v8i32c() {
; SSE2-LABEL: min_le_v8i32c:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa {{.*#+}} xmm1 = [2147483649,2147483651,2147483653,2147483655]
; SSE2-NEXT:    pcmpgtd {{.*}}(%rip), %xmm1
; SSE2-NEXT:    pcmpeqd %xmm2, %xmm2
; SSE2-NEXT:    movdqa %xmm1, %xmm3
; SSE2-NEXT:    pxor %xmm2, %xmm3
; SSE2-NEXT:    movdqa {{.*#+}} xmm0 = [2147483641,2147483643,2147483645,2147483647]
; SSE2-NEXT:    pcmpgtd {{.*}}(%rip), %xmm0
; SSE2-NEXT:    pxor %xmm0, %xmm2
; SSE2-NEXT:    pandn {{.*}}(%rip), %xmm2
; SSE2-NEXT:    pandn {{.*}}(%rip), %xmm0
; SSE2-NEXT:    por %xmm2, %xmm0
; SSE2-NEXT:    pandn {{.*}}(%rip), %xmm3
; SSE2-NEXT:    pandn {{.*}}(%rip), %xmm1
; SSE2-NEXT:    por %xmm3, %xmm1
; SSE2-NEXT:    retq
;
; SSE41-LABEL: min_le_v8i32c:
; SSE41:       # BB#0:
; SSE41-NEXT:    movdqa {{.*#+}} xmm1 = [1,3,5,7]
; SSE41-NEXT:    movdqa {{.*#+}} xmm0 = [4294967289,4294967291,4294967293,4294967295]
; SSE41-NEXT:    pminud {{.*}}(%rip), %xmm0
; SSE41-NEXT:    pminud {{.*}}(%rip), %xmm1
; SSE41-NEXT:    retq
;
; SSE42-LABEL: min_le_v8i32c:
; SSE42:       # BB#0:
; SSE42-NEXT:    movdqa {{.*#+}} xmm1 = [1,3,5,7]
; SSE42-NEXT:    movdqa {{.*#+}} xmm0 = [4294967289,4294967291,4294967293,4294967295]
; SSE42-NEXT:    pminud {{.*}}(%rip), %xmm0
; SSE42-NEXT:    pminud {{.*}}(%rip), %xmm1
; SSE42-NEXT:    retq
;
; AVX1-LABEL: min_le_v8i32c:
; AVX1:       # BB#0:
; AVX1-NEXT:    vmovdqa {{.*#+}} xmm0 = [4294967289,4294967291,4294967293,4294967295]
; AVX1-NEXT:    vpminud {{.*}}(%rip), %xmm0, %xmm0
; AVX1-NEXT:    vmovdqa {{.*#+}} xmm1 = [1,3,5,7]
; AVX1-NEXT:    vpminud {{.*}}(%rip), %xmm1, %xmm1
; AVX1-NEXT:    vinsertf128 $1, %xmm1, %ymm0, %ymm0
; AVX1-NEXT:    retq
;
; AVX2-LABEL: min_le_v8i32c:
; AVX2:       # BB#0:
; AVX2-NEXT:    vmovdqa {{.*#+}} ymm0 = [4294967289,4294967291,4294967293,4294967295,1,3,5,7]
; AVX2-NEXT:    vpminud {{.*}}(%rip), %ymm0, %ymm0
; AVX2-NEXT:    retq
;
; AVX512-LABEL: min_le_v8i32c:
; AVX512:       # BB#0:
; AVX512-NEXT:    vmovdqa {{.*#+}} ymm0 = [4294967289,4294967291,4294967293,4294967295,1,3,5,7]
; AVX512-NEXT:    vpminud {{.*}}(%rip), %ymm0, %ymm0
; AVX512-NEXT:    retq
  %1 = insertelement <8 x i32> <i32 -7, i32 -5, i32 -3, i32 -1, i32 1, i32 3, i32 5, i32 7>, i32 -7, i32 0
  %2 = insertelement <8 x i32> <i32 -1, i32 -3, i32 -5, i32 -7, i32 7, i32 5, i32 3, i32 1>, i32  1, i32 0
  %3 = icmp ule <8 x i32> %1, %2
  %4 = select <8 x i1> %3, <8 x i32> %1, <8 x i32> %2
  ret <8 x i32> %4
}

define <8 x i16> @min_le_v8i16c() {
; SSE2-LABEL: min_le_v8i16c:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa {{.*#+}} xmm1 = [65529,65531,65533,65535,1,3,5,7]
; SSE2-NEXT:    movdqa {{.*#+}} xmm2 = [1,65533,65531,65529,7,5,3,1]
; SSE2-NEXT:    movdqa %xmm1, %xmm3
; SSE2-NEXT:    psubusw %xmm2, %xmm3
; SSE2-NEXT:    pxor %xmm0, %xmm0
; SSE2-NEXT:    pcmpeqw %xmm3, %xmm0
; SSE2-NEXT:    pand %xmm0, %xmm1
; SSE2-NEXT:    pandn %xmm2, %xmm0
; SSE2-NEXT:    por %xmm1, %xmm0
; SSE2-NEXT:    retq
;
; SSE41-LABEL: min_le_v8i16c:
; SSE41:       # BB#0:
; SSE41-NEXT:    movdqa {{.*#+}} xmm0 = [65529,65531,65533,65535,1,3,5,7]
; SSE41-NEXT:    pminuw {{.*}}(%rip), %xmm0
; SSE41-NEXT:    retq
;
; SSE42-LABEL: min_le_v8i16c:
; SSE42:       # BB#0:
; SSE42-NEXT:    movdqa {{.*#+}} xmm0 = [65529,65531,65533,65535,1,3,5,7]
; SSE42-NEXT:    pminuw {{.*}}(%rip), %xmm0
; SSE42-NEXT:    retq
;
; AVX-LABEL: min_le_v8i16c:
; AVX:       # BB#0:
; AVX-NEXT:    vmovdqa {{.*#+}} xmm0 = [65529,65531,65533,65535,1,3,5,7]
; AVX-NEXT:    vpminuw {{.*}}(%rip), %xmm0, %xmm0
; AVX-NEXT:    retq
  %1 = insertelement <8 x i16> <i16 -7, i16 -5, i16 -3, i16 -1, i16 1, i16 3, i16 5, i16 7>, i16 -7, i16 0
  %2 = insertelement <8 x i16> <i16 -1, i16 -3, i16 -5, i16 -7, i16 7, i16 5, i16 3, i16 1>, i16  1, i16 0
  %3 = icmp ule <8 x i16> %1, %2
  %4 = select <8 x i1> %3, <8 x i16> %1, <8 x i16> %2
  ret <8 x i16> %4
}

define <16 x i16> @min_le_v16i16c() {
; SSE2-LABEL: min_le_v16i16c:
; SSE2:       # BB#0:
; SSE2-NEXT:    movdqa {{.*#+}} xmm2 = [65529,65530,65531,65532,65533,65534,65535,0]
; SSE2-NEXT:    movdqa {{.*#+}} xmm3 = [1,2,3,4,5,6,7,8]
; SSE2-NEXT:    movdqa {{.*#+}} xmm4 = [1,65534,65533,65532,65531,65530,65529,0]
; SSE2-NEXT:    movdqa {{.*#+}} xmm5 = [7,6,5,4,3,2,1,0]
; SSE2-NEXT:    movdqa %xmm3, %xmm1
; SSE2-NEXT:    psubusw %xmm5, %xmm1
; SSE2-NEXT:    pxor %xmm6, %xmm6
; SSE2-NEXT:    pcmpeqw %xmm6, %xmm1
; SSE2-NEXT:    movdqa %xmm2, %xmm0
; SSE2-NEXT:    psubusw %xmm4, %xmm0
; SSE2-NEXT:    pcmpeqw %xmm6, %xmm0
; SSE2-NEXT:    pand %xmm0, %xmm2
; SSE2-NEXT:    pandn %xmm4, %xmm0
; SSE2-NEXT:    por %xmm2, %xmm0
; SSE2-NEXT:    pand %xmm1, %xmm3
; SSE2-NEXT:    pandn %xmm5, %xmm1
; SSE2-NEXT:    por %xmm3, %xmm1
; SSE2-NEXT:    retq
;
; SSE41-LABEL: min_le_v16i16c:
; SSE41:       # BB#0:
; SSE41-NEXT:    movdqa {{.*#+}} xmm1 = [1,2,3,4,5,6,7,8]
; SSE41-NEXT:    movdqa {{.*#+}} xmm0 = [65529,65530,65531,65532,65533,65534,65535,0]
; SSE41-NEXT:    pminuw {{.*}}(%rip), %xmm0
; SSE41-NEXT:    pminuw {{.*}}(%rip), %xmm1
; SSE41-NEXT:    retq
;
; SSE42-LABEL: min_le_v16i16c:
; SSE42:       # BB#0:
; SSE42-NEXT:    movdqa {{.*#+}} xmm1 = [1,2,3,4,5,6,7,8]
; SSE42-NEXT:    movdqa {{.*#+}} xmm0 = [65529,65530,65531,65532,65533,65534,65535,0]
; SSE42-NEXT:    pminuw {{.*}}(%rip), %xmm0
; SSE42-NEXT:    pminuw {{.*}}(%rip), %xmm1
; SSE42-NEXT:    retq
;
; AVX1-LABEL: min_le_v16i16c:
; AVX1:       # BB#0:
; AVX1-NEXT:    vmovdqa {{.*#+}} xmm0 = [65529,65530,65531,65532,65533,65534,65535,0]
; AVX1-NEXT:    vpminuw {{.*}}(%rip), %xmm0, %xmm0
; AVX1-NEXT:    vmovdqa {{.*#+}} xmm1 = [1,2,3,4,5,6,7,8]
; AVX1-NEXT:    vpminuw {{.*}}(%rip), %xmm1, %xmm1
; AVX1-NEXT:    vinsertf128 $1, %xmm1, %ymm0, %ymm0
; AVX1-NEXT:    retq
;
; AVX2-LABEL: min_le_v16i16c:
; AVX2:       # BB#0:
; AVX2-NEXT:    vmovdqa {{.*#+}} ymm0 = [65529,65530,65531,65532,65533,65534,65535,0,1,2,3,4,5,6,7,8]
; AVX2-NEXT:    vpminuw {{.*}}(%rip), %ymm0, %ymm0
; AVX2-NEXT:    retq
;
; AVX512-LABEL: min_le_v16i16c:
; AVX512:       # BB#0:
; AVX512-NEXT:    vmovdqa {{.*#+}} ymm0 = [65529,65530,65531,65532,65533,65534,65535,0,1,2,3,4,5,6,7,8]
; AVX512-NEXT:    vpminuw {{.*}}(%rip), %ymm0, %ymm0
; AVX512-NEXT:    retq
  %1 = insertelement <16 x i16> <i16 -7, i16 -6, i16 -5, i16 -4, i16 -3, i16 -2, i16 -1, i16 0, i16 1, i16 2, i16 3, i16 4, i16 5, i16 6, i16 7, i16 8>, i16 -7, i16 0
  %2 = insertelement <16 x i16> <i16 -1, i16 -2, i16 -3, i16 -4, i16 -5, i16 -6, i16 -7, i16 0, i16 7, i16 6, i16 5, i16 4, i16 3, i16 2, i16 1, i16 0>, i16  1, i16 0
  %3 = icmp ule <16 x i16> %1, %2
  %4 = select <16 x i1> %3, <16 x i16> %1, <16 x i16> %2
  ret <16 x i16> %4
}

define <16 x i8> @min_le_v16i8c() {
; SSE-LABEL: min_le_v16i8c:
; SSE:       # BB#0:
; SSE-NEXT:    movdqa {{.*#+}} xmm0 = [249,250,251,252,253,254,255,0,1,2,3,4,5,6,7,8]
; SSE-NEXT:    pminub {{.*}}(%rip), %xmm0
; SSE-NEXT:    retq
;
; AVX-LABEL: min_le_v16i8c:
; AVX:       # BB#0:
; AVX-NEXT:    vmovdqa {{.*#+}} xmm0 = [249,250,251,252,253,254,255,0,1,2,3,4,5,6,7,8]
; AVX-NEXT:    vpminub {{.*}}(%rip), %xmm0, %xmm0
; AVX-NEXT:    retq
  %1 = insertelement <16 x i8> <i8 -7, i8 -6, i8 -5, i8 -4, i8 -3, i8 -2, i8 -1, i8 0, i8 1, i8 2, i8 3, i8 4, i8 5, i8 6, i8 7, i8 8>, i8 -7, i8 0
  %2 = insertelement <16 x i8> <i8 -1, i8 -2, i8 -3, i8 -4, i8 -5, i8 -6, i8 -7, i8 0, i8 7, i8 6, i8 5, i8 4, i8 3, i8 2, i8 1, i8 0>, i8  1, i8 0
  %3 = icmp ule <16 x i8> %1, %2
  %4 = select <16 x i1> %3, <16 x i8> %1, <16 x i8> %2
  ret <16 x i8> %4
}
